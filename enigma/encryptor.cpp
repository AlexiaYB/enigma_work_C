// imports
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <chrono>
#include <map>
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
const string alphabet = "abcdefghijklmnopqrstuvwxyz";
const int qlDatasetLen = 40629;
// file data
string qStrings[qlDatasetLen];
double qFrequencies[qlDatasetLen];
string mStrings[26];
double mFrequencies[26];


// rotor constants
// add/minus 1 please
const char turnovers[] = {'q','e','v','j','z'};
const int wheels[5][26] = {{4, 9, 10, 2, 7, 1, 23, 9, 13, 16, 3, 8, 2, 9, 10, 18, 7, 3, 0, 22, 6, 13, 5, 20, 4, 10}, 
                        {0, 8, 1, 7, 14, 3, 11, 13, 15, 18, 1, 22, 10, 6, 24, 13, 0, 15, 7, 20, 21, 3, 9, 24, 16, 5},
                        {1, 2, 3, 4, 5, 6, 22, 8, 9, 10, 13, 10, 13, 0, 10, 15, 18, 5, 14, 7, 16, 17, 24, 21, 18, 15},
                        {4, 17, 12, 18, 11, 20, 3, 19, 16, 7, 10, 23, 5, 20, 9, 22, 23, 14, 1, 13, 16, 8, 6, 15, 24, 2},
                        {21, 24, 25, 14, 2, 3, 13, 17, 12, 6, 8, 18, 1, 20, 23, 8, 10, 5, 20, 16, 22, 19, 9, 7, 4, 11}};
const string wheelsAlphabet[] = {"ekmflgdqvzntowyhxuspaibrcj","ajdksiruxblhwtmcqgznpyfvoe", "bdfhjlcprtxvznyeiwgakmusqo", "esovpzjayquirhxlnftgkdcmwb", "vzbrgityupsdnhlxawmjqofeck"};
multimap<double, string> rotorKeys;

int position(string referenceString, char currentCharacter){
    // returns first position in string
    int i,length;
    length = referenceString.length();
    for(i=0; i<length;i++){
        if (referenceString[i] == currentCharacter){
            return i;
        }
    }
    return -1;
}

int mod(int currentNum, int modulus = 26){
    if (currentNum > 0){
        return currentNum % modulus;
    }
    else{
        return (currentNum % modulus + modulus) % modulus;
    }
}

void readMonogramsCSV(){
    ifstream qFile("monogram_frequency.csv"); 
    string text;
    int column= 0, row = 0;
    while (getline(qFile, text, '\n')) {
        if (column != 0){
            stringstream ss(text); 
            while (getline(ss, text, ',')){
                if (row==0){
                    mStrings[column -1 ] = text;
                }
                else{
                    mFrequencies[column - 1] = stod(text);
                }
                row ++;
            }
        }
        column ++;
        row = 0;
    }

    qFile.close();
    return;
}

void readQuadgramsCSV(){
    ifstream qFile("short_quadgram_log_frequency.csv"); 
    string text;
    int column= 0, row = 0;
    while (getline(qFile, text, '\n')) {
        if (column != 0){
            stringstream ss(text); 
            while (getline(ss, text, ',')){
                if (row==0){
                    qStrings[column -1 ] = text;
                }
                else{
                    qFrequencies[column - 1] = stod(text);
                }
                row ++;
            }
        }
        column ++;
        row = 0;
    }

    qFile.close();
    return;
}

int quadgramComparison(string actualQ, string comparisonQ){
    int i;
    for(i=0;i<4;i++){
        if (actualQ[i] == comparisonQ[i]){
            continue;
        }
        else if(position(alphabet,actualQ[i]) > position(alphabet,comparisonQ[i])){
            // infront
            return 1;
        }
        else{
            // behind
            return -1;
        }
    }
    return 0;
}

int inQuadgramsSearch(string quadgram){
    int difference, first = 0, last = qlDatasetLen-1;
    int middle = last/2;
    while(first <= last){
        difference = quadgramComparison(qStrings[middle], quadgram);
        if(difference ==0){
            return middle;
        }
        else if (difference == 1){
            last = middle - 1;
        }
        else {
            first = middle + 1;
        }
        middle = (first + last)/2;
    }
    return -1;
}

double quadgram_fitness(string text){
    int i,j, index = 0;
    double sum = 0.;
    string currentQuadgram = "aaaa";
    for(i=0;i<(text.length()-3);i++){
        for(j=0;j<4;j++){
            currentQuadgram[j] = text[i+j];
        }
        index = inQuadgramsSearch(currentQuadgram);
        if(index != -1){
            sum += qFrequencies[index];
        }
        else {
            sum += -4.000000000000196;
        }
    }
    
    return sum;
}

void frequenciesCount (map<char, double> &frequenciesMap, string textList){
    int i;
    char currentChar;
    for(i=0;i<textList.length();i++){
        currentChar = textList[i];
        if(frequenciesMap.find(currentChar) != frequenciesMap.end()){
            frequenciesMap[currentChar] += 1;
        }
        else{
            frequenciesMap[currentChar] = 1;
        }
    }
}

void frequenciesCountPercentages(map<char, double> &frequenciesPMap, string textList){
    int textLen = textList.length();
    frequenciesCount(frequenciesPMap,textList);
    map<char, double>::iterator itr = frequenciesPMap.begin();
    while (itr != frequenciesPMap.end()) {
        frequenciesPMap[itr-> first] = (itr -> second / textLen) * 100;
        itr++;
    }
}

void fullAlphabetCounts(multimap<double, char> &alphabetMap, string textList){
    int i,textLen = textList.length();
    char currentChar;
    map<char, double> tempMap;
    frequenciesCount(tempMap,textList);
    for(i=0;i<26;i++){
        currentChar = alphabet[i];
        if(tempMap.find(currentChar) != tempMap.end()){
            alphabetMap.insert({(tempMap[currentChar] / textLen) * 100, currentChar});
        }
        else{
            alphabetMap.insert({0, currentChar});
        }
    }
}

double IOC(string textList){
    map<char, double> counts;
    frequenciesCount(counts, textList); 
    int i;
    double sum =0;
    map<char, double>::iterator itr = counts.begin();
    while (itr != counts.end()) {
        sum += (double)(itr -> second * (itr -> second-1)) / (textList.length() * (textList.length() -1));
        itr++;
    }
    return sum;
}

double entropy(string text){
    double current, total = 0;
    map<char, double> Pcounts;
    frequenciesCountPercentages(Pcounts, text);
    map<char, double>::iterator itr = Pcounts.begin();
    while (itr != Pcounts.end()) {
        current = itr -> second / 100;
        total += (double)(current) * (log(current)/log(2));
        itr++;
    }
    return -total;
}

double twist(string text){
    double total = 0;
    int counter = 0;
    multimap<double,char> alphabetCounts;
    fullAlphabetCounts(alphabetCounts, text);
    multimap<double, char>::iterator itr = alphabetCounts.begin();
    while (itr != alphabetCounts.end()) {
        if(counter<14){
            total -= itr -> first ;
        }
        else{
            total += itr -> first;
        }
        itr++;
        counter++;
    }
    return abs(total);
}

double languageTwist(string text){
    double total = 0;
    int i, counter = 0;
    multimap<double,char> alphabetCounts;
    fullAlphabetCounts(alphabetCounts, text);
    multimap<double, char>::iterator itr = alphabetCounts.begin();

    multiset<double> sortedAlphabet;
    for(i=0;i<26;i++){
        sortedAlphabet.insert(mFrequencies[i]);
    }
    multiset<double>::iterator itr2 = sortedAlphabet.begin();

    while (itr != alphabetCounts.end()) {
        if(counter<14){
            total += *itr2 - itr -> first;
        }
        else{
            total +=  itr -> first - *itr2;
        }
        itr++;
        itr2++;
        counter++;
        
    }
    return abs(total);
}

string plugboardEncryptor(string text, string swaps){
    int i, index, length = text.length();
    string newText = text;
    for(i = 0; i < length; i++){
        index = position(swaps,text[i]);
        if(index == -1){
            newText[i] = text[i];
        }
        else {
            if (index % 2 == 0){
                newText[i] = swaps[index + 1];
            }
            else {
                newText[i] = swaps[index - 1];
            }
        }
    }   
    return newText;
}

int reflectorUKWB(int letterNum){
    string key = "yruhqsldpxngokmiebfzcwvjat";
    // well isn't that more efficient!!!!
    return (key[letterNum] - 'a');
}

// rotor keys go slow --> fast (left to right)
string rotors(string text, string wheelOrder, string ringSetting, string initialSetting){
    int i,j;
    int currentCharacter;
    int currentMessageSettings[] = {(int) (initialSetting[0] - 'a'),
                                (int) (initialSetting[1] - 'a'),
                                (int) (initialSetting[2] - 'a')};
    const int numberRingSettings[] = {(int) (ringSetting[0] - 'a'),
                                (int) (ringSetting[1] - 'a'),
                                (int) (ringSetting[2] - 'a')};
    string finalText = text;

    for (i=0;i<text.length();i++){
        currentCharacter = text[i] - 'a';
        // slow
        if (alphabet[currentMessageSettings[1]] == turnovers[(int)wheelOrder[1]-49]){
            currentMessageSettings[0] = mod(currentMessageSettings[0] + 1);
            // double step
            currentMessageSettings[1] = mod(currentMessageSettings[1] + 1);
        }
        // middle
        if (alphabet[currentMessageSettings[2]] == turnovers[(int)wheelOrder[2] - 49]){
            currentMessageSettings[1] = mod(currentMessageSettings[1] + 1);
        }
        // fast
        currentMessageSettings[2] = mod(currentMessageSettings[2] + 1);

        // first rotor pass
        for(j=2;j>-1;j+=-1){
            currentCharacter = mod(currentCharacter + currentMessageSettings[j]);
            currentCharacter = mod(currentCharacter + wheels[(int)wheelOrder[j] - 49][
                mod(currentCharacter - numberRingSettings[j])]);
                // abs??? % not working 
            currentCharacter = mod(currentCharacter - currentMessageSettings[j]);
        }
        // reflector
        currentCharacter = reflectorUKWB(currentCharacter);
        // second rotor pass
        for(j=0;j<3;j++){
            currentCharacter = mod(currentCharacter + currentMessageSettings[j]);
            currentCharacter = mod(currentCharacter - wheels[(int)wheelOrder[j] - 49][
                position(wheelsAlphabet[(int)wheelOrder[j] - 49], alphabet[mod(currentCharacter - numberRingSettings[j])])]);
            currentCharacter = mod(currentCharacter - currentMessageSettings[j]);
        }
        finalText[i] = alphabet[currentCharacter];
    }
    return finalText;
}

string enigma(string text, string wheelOrder, string ringSetting, string initialSetting, string plugs){
    return plugboardEncryptor(rotors(plugboardEncryptor(text, plugs), wheelOrder, ringSetting, initialSetting), plugs);
}

void rotorBruteForce(string ciphertext){
    // wheel order
    int holder;
    // because it refuses to intialise the top line for some reason..
    int i,j,k,l,m,n, counter = 0;
    double fitness;
    string strWheelKey = "111";
    string strInitalKey = "111";

    // wheel order
    for(i=0;i<5;i++){for (j=0;j<5;j++){if (i != j){for (k=0;k<5;k++){if (i != k && j != k){
                        strWheelKey = to_string(i + 1) + to_string(j + 1) + to_string(k  + 1);
                        for(l=0;l<26;l++){for(m=0;m<26;m++){for (n=0;n<26;n++){
                                    strInitalKey = string() + alphabet[l] + alphabet[m] + alphabet[n];
                                    fitness = quadgram_fitness(enigma(ciphertext,strWheelKey, "gto", strInitalKey, ""));
                                    rotorKeys.emplace(make_pair(fitness, strWheelKey + strInitalKey));
                                }}}
                    }}}}}
}

void outputRotorsCSV(){
    ofstream fout("output.csv");
    multimap<double, string>::iterator itr = rotorKeys.end();
    fout << "rotorKey" << ", "<<"evaluationStatistic"<< "\n";
    while (itr != rotorKeys.begin()) {
        --itr;
        fout << itr->second << ", " << itr->first << "\n";
    }
    fout.close();

}


int main(){
    readQuadgramsCSV();
    readMonogramsCSV();
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristic";
    string testCiphertext = enigma(testPlaintext, "423", "gto", "sdv","adehgyimknlrozqvtxuw");
    
    // auto start = high_resolution_clock::now();
    // rotorBruteForce(testCiphertext);
    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // multimap<double, string>::iterator itr = rotorKeys.end();
    // while (itr != rotorKeys.begin()) {
    //     --itr;
    //     cout << '\t' << itr->second << '\t' << itr->first << '\n';
    // }
    // cout << duration.count();

}


