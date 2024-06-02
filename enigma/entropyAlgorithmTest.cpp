#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <cmath> 
#include <chrono>
#include <map>
#include <vector> 
#include <thread>
#include <mutex>
using namespace std;
using namespace std::chrono;

// threading 
mutex mx;

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

multimap<double, enigmaKey> finalPlugboardResults;

// tools 
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

// fitness functioned 
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

// base encryptors 
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

// rotors
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

vector<string> lookupTableCreator(enigmaKey &currentKey){
    vector<string> lookuptablestore;
    int i,j;
    for(i=0;i<26;i++){
        string baseText = "";
        for(j=0;j<16900;j++){
            baseText += alphabet[i];
        }
        lookuptablestore.push_back(rotors(baseText, currentKey));
    }
    return lookuptablestore;
}

string fasterRotors(string text, enigmaKey &currentKey, vector<string> currentLookupTable){
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
        outputText += currentLookupTable[text[j] - 'a'][mod(counter, 16900)];
        counter ++;
    }
    return outputText;
}

// overal encryptor
string enigma(string text, enigmaKey &currentKey){
    return plugboardEncryptor(rotors(plugboardEncryptor(text, currentKey.plugs), currentKey), currentKey.plugs);
}

string fasterEngima(string text, enigmaKey &currentKey, vector<string> currentLookupTable){
    return plugboardEncryptor(fasterRotors(plugboardEncryptor(text, currentKey.plugs), currentKey, currentLookupTable), currentKey.plugs);
}

// rotor brute forcing 
multimap<string,vector<string>> rotorBruteForce(string ciphertext, string rings, multimap<double, enigmaKey> &rotorKeysOut){
    // wheel order
    int i,j,k,l,m,n, counter = 0;
    double fitness;
    enigmaKey currentKey;
    multimap<string,vector<string>> usedLookupTable;
    vector<string> currentLookupTable;

    // wheel order
    for(i=0;i<5;i++){
        for (j=0;j<5;j++){
            if (i != j){
                for (k=0;k<5;k++){
                    if (i != k && j != k){
                        currentKey.wheelOrder = to_string(i + 1) + to_string(j + 1) + to_string(k  + 1);
                        currentKey.ringSetting = rings;
                        currentKey.plugs = "";
                        currentKey.initalSetting = "aaa";
                        currentLookupTable =  lookupTableCreator(currentKey);
                        usedLookupTable.emplace(make_pair(currentKey.wheelOrder + currentKey.ringSetting, currentLookupTable));
                        for(l=0;l<26;l++){for(m=0;m<26;m++){for(n=0;n<26;n++){
                                    currentKey.initalSetting = string() + alphabet[l] + alphabet[m] + alphabet[n];
                                    fitness = entropy(fasterRotors(ciphertext,currentKey, currentLookupTable));
                                    rotorKeysOut.emplace(make_pair(fitness, currentKey));
                                }}}
                        // cout << currentKey.wheelOrder << " done\n";
                    }}}}
                    cout << currentKey.wheelOrder[0] << " done\n";
                    }
    return usedLookupTable;
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

// overal algorithm
multimap<string,vector<string>> cullOveralLookupTable(multimap<string,vector<string>> overalLookupTable, multimap<double, enigmaKey> referenceTable, int newLength){
    int i;
    enigmaKey currentKey;
    string searchKey;
    multimap<double, enigmaKey>::iterator itr;
    multimap<string,vector<string>> shortLookupTable;
    itr = referenceTable.begin();
    for (i=0;i<newLength;i++){
        currentKey = itr ->second;
        searchKey = currentKey.wheelOrder + currentKey.ringSetting;
        if (shortLookupTable.count(searchKey) == 0){
        shortLookupTable.emplace(make_pair(searchKey, overalLookupTable.find(searchKey) -> second));
        }
        itr++;
    }
    return shortLookupTable;
}

// parameters 
const int E = 5000;
const int A = 1000;

const int depthLetters[] = {4,0};
const int depthSizeParameters[] = {E,A};
void recursivePKeyTest(int depth, string baseKey, string ciphertext, multimap<double, enigmaKey> &priorKeyList, multimap<string,vector<string>> overalLookupTable){
    if (depth == 2){
        finalPlugboardResults.merge(priorKeyList);
    }
    else if(alphabet[depthLetters[depth]] != baseKey[depthLetters[depth]]){
        recursivePKeyTest(depth+1, baseKey, ciphertext, priorKeyList, cullOveralLookupTable(overalLookupTable,priorKeyList,depthSizeParameters[depth]));
    }
    else{
        int i, j, mapIterator, selfSteckered = 0;
        string newKey, searchKey;
        enigmaKey currentKey;
        multimap<double, enigmaKey>::iterator itr;
        multimap<double, enigmaKey> shortKeyList;
        vector<string> currentLookupTable;
        multimap<string,vector<string>> shortLookupTable;
        for (i=0;i<26;i++){ 
            shortKeyList.clear();        
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
                        currentLookupTable.clear();
                        currentKey = itr ->second;
                        currentKey.plugs = newKey;
                        searchKey = currentKey.wheelOrder + currentKey.ringSetting;
                        currentLookupTable = overalLookupTable.find(searchKey)-> second;
                        shortLookupTable.emplace(make_pair(searchKey, currentLookupTable));
                        shortKeyList.emplace(make_pair(entropy(fasterEngima(ciphertext,currentKey, currentLookupTable)), currentKey));
                        itr++;
                    }
                    recursivePKeyTest(depth+1, newKey, ciphertext, shortKeyList, shortLookupTable);
                    if(depth == 0){
                        cout << newKey << " done\n";
                    }
                    shortLookupTable.clear();
                }
            }
        }
    }
}

void newOveralAlgorithm(string ciphertext, string middleRing){
    string fastRings[] = {"a","h","o","v"};
    multimap<double, enigmaKey> fullRotorKeys;
    int i;
    multimap<string,vector<string>> overalLookupTable;
    for(i=0;i<4;i++){
        overalLookupTable.merge(rotorBruteForce(ciphertext, "a"+ middleRing + fastRings[i], fullRotorKeys));
         cout << "rotorbrute force " << "a"+ middleRing + fastRings[i] << " done" << "\n";
    }
    recursivePKeyTest(0,alphabet,ciphertext,fullRotorKeys, cullOveralLookupTable(overalLookupTable,fullRotorKeys,E));
}

// void overheadThreads(string ciphertext, string baseRings){
//     string fastRings[] = {"a","h","o","v"};
//     thread t[8];
//     int i;
//     // spawn
//     for(i=0;i<4;i++){
//         t[i] = thread(&rotorBruteForce, ciphertext,"ar" + fastRings[i]);
//         t[4+i] = thread(&rotorBruteForce, ciphertext,"ag" + fastRings[i]);
//     }
//     // join
//     for(i=0;i<8;i++){
//         t[i].join();
//     }
//     return;
// }

int main () {
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristicsofthetypecanbeusedtoidentifythesourceasthetypewriterweholdinthemansionseemstometobedivinationnotscienceandwithoutmuchstrongerevidenceofalinkbetweensomeoneinmyhouseholdandtheyoungmanislesiwillnotentertainorcooperatewithanyfurtherinvestigationsihavealreadyexpressedmyconsternationattheprospectofascandalderailingmycampaignforstategovernorandithoughtthatyouunderstoodandagreedwithmewhyyouwouldwanttoreopenthecasewhenihavesomuchtoloseandwehavenothingtogainfromiticannotunderstandiknowthatmaisiehasbeenpressingforfurtherinvestigationanditisclearthatshewasdeeplydistressedbythewholeincidentshelovesthelibraryandwoulddoanythingtoprotectithoweversheismerelyaservantoftheestateandsurelymystatusaspresentownergivesgreaterweighttomyviewsandwisheswhileislesclearlytrespassedheseemsnottohavecausedanydamageandevenifhewasintendingtostealfrommehisuntimelydeathforestalledthatplanhehasalreadypaidthepriceforhismisdeedsandidonotseeanyreasontocontinuetopokeandprobeatwhatmustsurelybearoutinecaseimustinsistthatyoubringafinalhalttoyourinvestigationsandlethimrestinpeaceletusnotinvitegossipatthiscriticaltimeyoursinfriendshiprogers";
    enigmaKey testingKey;
    testingKey.wheelOrder = "321";
    testingKey.ringSetting = "nqy";
    testingKey.initalSetting ="etf";
    testingKey.plugs = "abodrtzkujhlyqcwnexfivpsmg";

    string testCiphertext = enigma(testPlaintext,testingKey);

    multimap<double, enigmaKey> fullRotorKeys;
    
    auto start = high_resolution_clock::now();
    // newOveralAlgorithm(testCiphertext,"q");
    rotorBruteForce(testCiphertext,"aqy",fullRotorKeys);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count();

    // outputFitnessMultimapCSV("output.csv", finalPlugboardResults);
    outputFitnessMultimapCSV("output.csv", fullRotorKeys);

    cout << "done";
    return 1;
}



