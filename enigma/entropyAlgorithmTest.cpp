#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <cmath> 
#include <chrono>
#include <map>
#include <vector> 
using namespace std;
using namespace std::chrono;

const string alphabet = "abcdefghijklmnopqrstuvwxyz";
// struct for keys
struct enigmaKey { 
  string wheelOrder;
  string ringSetting;
  string initalSetting;
  string plugs;
};

// rotor constants
// add/minus 1
const char turnovers[] = {'q','e','v','j','z'};
const int wheels[5][26] = {{4, 9, 10, 2, 7, 1, 23, 9, 13, 16, 3, 8, 2, 9, 10, 18, 7, 3, 0, 22, 6, 13, 5, 20, 4, 10}, 
                        {0, 8, 1, 7, 14, 3, 11, 13, 15, 18, 1, 22, 10, 6, 24, 13, 0, 15, 7, 20, 21, 3, 9, 24, 16, 5},
                        {1, 2, 3, 4, 5, 6, 22, 8, 9, 10, 13, 10, 13, 0, 10, 15, 18, 5, 14, 7, 16, 17, 24, 21, 18, 15},
                        {4, 17, 12, 18, 11, 20, 3, 19, 16, 7, 10, 23, 5, 20, 9, 22, 23, 14, 1, 13, 16, 8, 6, 15, 24, 2},
                        {21, 24, 25, 14, 2, 3, 13, 17, 12, 6, 8, 18, 1, 20, 23, 8, 10, 5, 20, 16, 22, 19, 9, 7, 4, 11}};
const string wheelsAlphabet[] = {"ekmflgdqvzntowyhxuspaibrcj","ajdksiruxblhwtmcqgznpyfvoe", "bdfhjlcprtxvznyeiwgakmusqo", "esovpzjayquirhxlnftgkdcmwb", "vzbrgityupsdnhlxawmjqofeck"};

multimap<double, enigmaKey> fullRotorKeys;
multimap<double, enigmaKey> finalPlugboardResults;
vector<string> currentLookuptable;
multimap<string,vector<string>> usedLookuptable;

int mod(int currentNum, int modulus = 26){
    if (currentNum > 0){
        return currentNum % modulus;
    }
    else{
        return (currentNum % modulus + modulus) % modulus;
    }
}

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

string slowEquivalentKeys(char slowIntialSetting, char slowRingSetting){
    return ""s + alphabet[mod(26 - (slowRingSetting- slowIntialSetting))] + 'a';
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

string fasterRotors(string text, enigmaKey &currentKey){
    int i,j, tempInitial, counter=0;
    string outputText;

    for(i=0;i<3;i++){
        // check for initial settings over turnover
        if(((currentKey.initalSetting[i] - 'a')) > (turnovers[currentKey.wheelOrder[i] - '1'] - 'a')){
            tempInitial = -(26 - (currentKey.initalSetting[i]- 'a'));
        }else{
            tempInitial = (currentKey.initalSetting[i]- 'a');
        }
        // then add the adjustments (for rotor position & double stepping)
        if(i == 0){
            tempInitial *= 650;
        }else {
            tempInitial *= pow(26, 2-i);
        }
        counter += tempInitial;
    }
    counter = mod(counter,16900);

    for (j=0;j<text.length();j++){
        outputText += currentLookuptable[text[j] - 'a'][mod(counter, 16900)];
        counter ++;
    }
    return outputText;
}

// encryptors 
int reflectorUKWB(int letterNum){
    string key = "yruhqsldpxngokmiebfzcwvjat";
    // well isn't that more efficient!!!!
    return (key[letterNum] - 'a');
}

string plugboardEncryptor(string text, string swaps){
    int i;
    string newText = text;
    for(i=0;i<text.length();i++){
        newText[i] = swaps[text[i] - 'a'];
    }
    return newText;
}

// rotor keys go slow --> fast (left to right)
string rotors(string text, enigmaKey &currentKey){
    int i,j;
    int currentCharacter;
    int currentMessageSettings[] = {(int) (currentKey.initalSetting[0] - 'a'),
                                (int) (currentKey.initalSetting[1] - 'a'),
                                (int) (currentKey.initalSetting[2] - 'a')};
    const int numberRingSettings[] = {(int) (currentKey.ringSetting[0] - 'a'),
                                (int) (currentKey.ringSetting[1] - 'a'),
                                (int) (currentKey.ringSetting[2] - 'a')};
    string finalText = text;

    for (i=0;i<text.length();i++){
        currentCharacter = text[i] - 'a';
        // slow
        if (alphabet[currentMessageSettings[1]] == turnovers[(int)currentKey.wheelOrder[1]-49]){
            currentMessageSettings[0] = mod(currentMessageSettings[0] + 1);
            // double step
            currentMessageSettings[1] = mod(currentMessageSettings[1] + 1);
        }
        // middle
        if (alphabet[currentMessageSettings[2]] == turnovers[(int)currentKey.wheelOrder[2] - 49]){
            currentMessageSettings[1] = mod(currentMessageSettings[1] + 1);
        }
        // fast
        currentMessageSettings[2] = mod(currentMessageSettings[2] + 1);

        // first rotor pass
        for(j=2;j>-1;j+=-1){
            currentCharacter = mod(currentCharacter + currentMessageSettings[j]);
            currentCharacter = mod(currentCharacter + wheels[(int)currentKey.wheelOrder[j] - 49][
                mod(currentCharacter - numberRingSettings[j])]);
            currentCharacter = mod(currentCharacter - currentMessageSettings[j]);
        }
        // reflector
        currentCharacter = reflectorUKWB(currentCharacter);
        // second rotor pass
        for(j=0;j<3;j++){
            currentCharacter = mod(currentCharacter + currentMessageSettings[j]);
            currentCharacter = mod(currentCharacter - wheels[(int)currentKey.wheelOrder[j] - 49][
                position(wheelsAlphabet[(int)currentKey.wheelOrder[j] - 49], alphabet[mod(currentCharacter - numberRingSettings[j])])]);
            currentCharacter = mod(currentCharacter - currentMessageSettings[j]);
        }
        finalText[i] = alphabet[currentCharacter];
    }
    return finalText;
}

void lookupTableCreator(enigmaKey &currentKey){
    currentLookuptable.clear();
    int i,j;
    for(i=0;i<26;i++){
        string baseText = "";
        for(j=0;j<16900;j++){
            baseText += alphabet[i];
        }
        currentLookuptable.push_back(rotors(baseText, currentKey));
    }
}

// rotor brute forcing 
void rotorBruteForce(string ciphertext){
    // wheel order
    int i,j,k,l,m,n, counter = 0;
    double fitness;
    enigmaKey currentKey;

    // wheel order
    for(i=0;i<5;i++){
        for (j=0;j<5;j++){
            if (i != j){
                for (k=0;k<5;k++){
                    if (i != k && j != k){
                        currentKey.wheelOrder = to_string(i + 1) + to_string(j + 1) + to_string(k  + 1);
                        currentKey.ringSetting = "ayk";
                        currentKey.plugs = "";
                        currentKey.initalSetting = "aaa";
                        lookupTableCreator(currentKey);
                        usedLookuptable.emplace(make_pair(currentKey.wheelOrder, currentLookuptable));
                        for(l=0;l<26;l++){for(m=0;m<26;m++){for(n=0;n<26;n++){
                                    currentKey.initalSetting = string() + alphabet[l] + alphabet[m] + alphabet[n];
                                    fitness = entropy(fasterRotors(ciphertext,currentKey));
                                    fullRotorKeys.emplace(make_pair(fitness, currentKey));
                                }}}
                        cout << "wheel order "<< currentKey.wheelOrder << " complete" << "\n";
                    }}}}}
}

// // output results for analysis 
void outputFitnessMultimapCSV(string outputFile, multimap<double, enigmaKey>& fitnessMultimap){
    ofstream fout(outputFile);
    multimap<double, enigmaKey>::iterator itr = fitnessMultimap.begin();
    fout << "wheelOrder" <<","<< "initialSetting" << ","<< "ring setting" << ","<< "plugboard"<<","<<"evaluationStatistic"<< "\n";
    while (itr != fitnessMultimap.end()) {
        fout << itr->second.wheelOrder<< "," << itr -> second.initalSetting<<  "," << itr -> second.ringSetting<<"," << itr -> second.plugs << "," << itr->first << "\n";
        itr++;
    }
    fout.close();
}

string enigma(string text, enigmaKey &currentKey){
    return plugboardEncryptor(rotors(plugboardEncryptor(text, currentKey.plugs), currentKey), currentKey.plugs);
}

string fasterEngima(string text, enigmaKey &currentKey){
    return plugboardEncryptor(fasterRotors(plugboardEncryptor(text, currentKey.plugs), currentKey), currentKey.plugs);
}

// overal algorithm
// parameters 
const int E = 5000;
const int A = 1000;
const int X = 20;
multimap<double, enigmaKey> ARotorKeys;

// bool successCriterion(multimap<double, enigmaKey>& shortlistedKeys, int depthSize){
//     double TopDifference = 0, otherMaxDifference, currentDifference, previous;
//     multimap<double, enigmaKey>::iterator itr;
//     itr = shortlistedKeys.begin();
//     previous = itr -> first;
//     itr ++;
//     // cout << " "<< previous;
//     while(itr != shortlistedKeys.end()) {
//         if (TopDifference == 0){
//             TopDifference = itr -> first - previous;
//         }
//         else {
//             currentDifference = itr -> first - previous;
//             if(currentDifference > otherMaxDifference){
//                 otherMaxDifference = currentDifference;
//             }
//         }
//         previous = itr -> first;
//         itr ++;
//     }
//     cout << (TopDifference / otherMaxDifference) << " " << (TopDifference - otherMaxDifference)  << " ";
//     if(TopDifference > (otherMaxDifference * (depthSize /4))){
//         return true;
//     }else{
//         return false;
//     }
// }

// void generateUsedLookupTable(multimap <double, enigmaKey> allKeys, int sizeOfShortlist){
//     usedLookuptable.clear();
//     enigmaKey currentKey;
//     multimap<double, enigmaKey>::iterator itr;
//     int i;
//     string temp[26];
//     itr = allKeys.begin();
//     for (i=0;i<sizeOfShortlist;i++) {
//         currentKey = itr ->second;
//         if (usedLookuptable.count(currentKey.wheelOrder) == 0){
//             currentKey.initalSetting = "aaa";
//             lookupTableCreator(currentKey);
//             usedLookuptable.emplace(make_pair(currentKey.wheelOrder, currentLookuptable));
//         }
//         itr++;
//     }
//     return;
// }

const int depthLetters[] = {4, 0, 19};
const int depthSizeParameters[] = {E,A,X};
void recursivePKeyTest(int depth, string baseKey, string ciphertext, multimap<double, enigmaKey> &priorKeyList){
    if (depth == 2){
        finalPlugboardResults.merge(priorKeyList);
    }
    else if(alphabet[depthLetters[depth]] != baseKey[depthLetters[depth]]){
        recursivePKeyTest(depth+1, baseKey, ciphertext, priorKeyList);
    }
    else{
        int i, j, mapIterator, selfSteckered = 0;
        string newKey;
        enigmaKey currentKey;
        multimap<double, enigmaKey>::iterator itr;
        multimap<double, enigmaKey> shortKeyList;
        string temp[26];
        shortKeyList.clear();
        for (i=0;i<26;i++){         
            selfSteckered = 0;
            if (alphabet[i] == baseKey[i]){
                for(j=0;j<depth;j++){
                    if (i == depthLetters[j]){
                        selfSteckered = 1;
                        break;
                    }
                }
                if (selfSteckered == 0){
                    newKey = baseKey;
                    newKey[depthLetters[depth]] = alphabet[i];
                    newKey[i] = alphabet[depthLetters[depth]];
                    // take top X and try new plugboard
                    itr = priorKeyList.begin();
                    for (mapIterator=0;mapIterator<depthSizeParameters[depth];mapIterator++) {
                        currentLookuptable.clear();
                        currentKey = itr ->second;
                        currentKey.plugs = newKey;
                        currentLookuptable = usedLookuptable.find(currentKey.wheelOrder)-> second;
                        shortKeyList.emplace(make_pair(entropy(fasterEngima(ciphertext,currentKey)), currentKey));
                        itr++;
                    }
                    recursivePKeyTest(depth+1, newKey, ciphertext, shortKeyList);
                    if(depth == 0){
                        cout << newKey << " done\n";
                    }
                }
            }
        }
    }
}

void newOveralAlgorithm(string ciphertext){
    rotorBruteForce(ciphertext);
    cout << "rotorbrute force done" << "\n";
    // generateUsedLookupTable(fullRotorKeys,E);
    // cout << "lookup table generated" << "\n";
    recursivePKeyTest(0,alphabet,ciphertext,fullRotorKeys);
}


int main () {
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristicsofthetypecanbeusedtoidentifythesourceasthetypewriterweholdinthemansionseemstometobedivinationnotscienceandwithoutmuchstrongerevidenceofalinkbetweensomeoneinmyhouseholdandtheyoungmanislesiwillnotentertainorcooperatewithanyfurtherinvestigationsihavealreadyexpressedmyconsternationattheprospectofascandalderailingmycampaignforstategovernorandithoughtthatyouunderstoodandagreedwithmewhyyouwouldwanttoreopenthecasewhenihavesomuchtoloseandwehavenothingtogainfromiticannotunderstandiknowthatmaisiehasbeenpressingforfurtherinvestigationanditisclearthatshewasdeeplydistressedbythewholeincidentshelovesthelibraryandwoulddoanythingtoprotectithoweversheismerelyaservantoftheestateandsurelymystatusaspresentownergivesgreaterweighttomyviewsandwisheswhileislesclearlytrespassedheseemsnottohavecausedanydamageandevenifhewasintendingtostealfrommehisuntimelydeathforestalledthatplanhehasalreadypaidthepriceforhismisdeedsandidonotseeanyreasontocontinuetopokeandprobeatwhatmustsurelybearoutinecaseimustinsistthatyoubringafinalhalttoyourinvestigationsandlethimrestinpeaceletusnotinvitegossipatthiscriticaltimeyoursinfriendshiprogers";
    enigmaKey testingKey;
    testingKey.wheelOrder = "145";
    testingKey.ringSetting = "ryk";
    testingKey.initalSetting ="iym";
    testingKey.plugs = "omtsexhgupkrbnajvldciqwfyz";
    
    // lookupTableCreator(testingKey);
    // testingKey.initalSetting ="iym";
    string testCiphertext = enigma(testPlaintext,testingKey);
    // cout << entropy(fasterEngima(testCiphertext,testingKey));


    auto start = high_resolution_clock::now();
    newOveralAlgorithm(testCiphertext);
    // rotorBruteForce(testCiphertext);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count();

    outputFitnessMultimapCSV("output.csv", finalPlugboardResults);
    cout << "done";
    return 1;
}



