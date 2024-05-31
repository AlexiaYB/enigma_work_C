#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <cmath> 
#include <chrono>
#include <map>
using namespace std;
using namespace std::chrono;

const string alphabet = "abcdefghijklmnopqrstuvwxyz";
map<string,double> quadgrams;

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
// rotor variables
multimap<double, enigmaKey> fullRotorKeys;

// file handling 
void readQuadgramsCSV(){
    ifstream qFile("short_quadgram_log_frequency.csv"); 
    string text, qString;
    int column= 0, row = 0;
    while (getline(qFile, text, '\n')) {
        if (column != 0){
            stringstream ss(text); 
            while (getline(ss, text, ',')){
                if (row==0){
                    qString = text;
                }
                else{
                    quadgrams.emplace(make_pair(qString, stod(text)));
                }
                row ++;
            }
        }
        column ++;
        row = 0;
    }
    return;
}

// tools 
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

// fitness functions
double quadgramFitness(string text){
    int i,j;
    double sum = 0., value = 0.;
    string currentQuadgram = "aaaa";
    for(i=0;i<(text.length()-3);i++){
        for(j=0;j<4;j++){
            currentQuadgram[j] = text[i+j];
        }
        value = quadgrams[currentQuadgram];
        if(value != 0){
            sum += value;
        }
        else {
            sum += -4.000000000000196;
        }
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
                // abs??? % not working 
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

string enigma(string text, enigmaKey &currentKey){
    return plugboardEncryptor(rotors(plugboardEncryptor(text, currentKey.plugs), currentKey), currentKey.plugs);
}

// plugboard HC
string randomPlugboard(){
    // a new random plugboard each second
    int i,randomValue1,randomValue2,In1,In2;
    string new_swaps = "abcdefghijklmnopqrstuvwxyz";
    for(i=0;i<10;i++){
        In1 = In2 = 0;
        while ((In1 + In2) != 2){
            if (In1 != 1){
                randomValue1 = rand()%26;
            }
            if (In2 != 1){
                randomValue2 = rand()%26;
            }
            if(randomValue1 != randomValue2){
                if((new_swaps[randomValue1] - 'a') ==  randomValue1){
                    In1 = 1;
                }
                if((new_swaps[randomValue2] - 'a') ==  randomValue2){
                    In2 = 1;
                }
            }
        }
        new_swaps[randomValue1] = alphabet[randomValue2];
        new_swaps[randomValue2] = alphabet[randomValue1];
    }
    return new_swaps;
}

void neighbours_plugboard(string swaps, set<string>& neighbours){
    int i,j, counter = 0;
    string newSwaps;
    for(i=0;i<25;i++){
        for(j=1;j<(25-i);j++){
            if((alphabet[i+j] != swaps[i]) && ((alphabet[i+j] != swaps[i+j]) || (alphabet[i] != swaps[i]))){
                // either in pair or duplicate doubled  case
                newSwaps = swaps;
                if(swaps[i] == alphabet[i]){
                    // current letter doubled case
                    newSwaps[i] = alphabet[i+j];
                    newSwaps[i+j] = alphabet[i];
                    newSwaps[swaps[i+j] - 'a'] = swaps[i+j];
                }else if (swaps[i+j] == alphabet[i+j]){
                    // new letter doubled case
                    newSwaps[i] = alphabet[i+j];
                    newSwaps[i+j] = alphabet[i];
                    newSwaps[swaps[i] - 'a'] = swaps[i];
                }else{
                    // two pairs case
                    newSwaps[i] = swaps[i+j];
                    newSwaps[i+j] = swaps[i];
                    newSwaps[swaps[i+j] - 'a'] = alphabet[i];
                    newSwaps[swaps[i] - 'a'] = alphabet[i+j];
                    
                }
                neighbours.insert(newSwaps);
            }

        }
    }
    return;
}

string plugboardHC(string bestSwaps,string ciphertext){
    string testPlaintext = ciphertext;
    double bestFitness, currentFitness;
    set<string> neighbours;
    string currentSwaps = bestSwaps;
    int i, stuck = 0;

    bestSwaps = randomPlugboard();
    testPlaintext = plugboardEncryptor(ciphertext,bestSwaps);
    bestFitness = quadgramFitness(testPlaintext);

    while (stuck == 0){
        stuck = 1;
        neighbours.clear();
        neighbours_plugboard(bestSwaps, neighbours);
        set<string>::iterator itr;
        for (itr = neighbours.begin(); itr != neighbours.end(); itr++) {
            currentSwaps = *itr;
            testPlaintext = plugboardEncryptor(ciphertext,currentSwaps);
            currentFitness = quadgramFitness(testPlaintext);
            if(bestFitness < currentFitness){
                bestSwaps = currentSwaps;
                bestFitness = currentFitness;
                stuck = 0;
                break;
            }
        }
    }
    return bestSwaps;
}

// rotor brute forcing 
void rotorBruteForce(string ciphertext){
    // wheel order
    int i,j,k,l,m,n, counter = 0;
    double fitness;
    enigmaKey currentKey;

    // wheel order
    for(i=0;i<3;i++){
        for (j=0;j<3;j++){
            if (i != j){
                for (k=0;k<3;k++){
                    if (i != k && j != k){
                        currentKey.wheelOrder = to_string(i + 1) + to_string(j + 1) + to_string(k  + 1);
                        for(l=0;l<13;l++){for(m=0;m<13;m++){for (n=0;n<13;n++){
                                    currentKey.initalSetting = string() + alphabet[l] + alphabet[m] + alphabet[n];
                                    currentKey.ringSetting = "gto";
                                    currentKey.plugs = "";
                                    fitness = entropy(rotors(ciphertext,currentKey));
                                    fullRotorKeys.emplace(make_pair(fitness, currentKey));
                                }}}
                        cout << "wheel order "<< currentKey.wheelOrder << " complete" << "\n";
                    }}}}}
}

// overal algorithm
// parameters 
const int N = 1000;
const int M = 100;
const int X = 20;

bool successCriterion(multimap<double, enigmaKey>& shortlistedKeys, int depthSize){
    double TopDifference = 0, otherMaxDifference, currentDifference, previous;
    multimap<double, enigmaKey>::iterator itr;
    itr = shortlistedKeys.begin();
    previous = itr -> first;
    itr ++;
    // cout << " "<< previous;
    while(itr != shortlistedKeys.end()) {
        if (TopDifference == 0){
            TopDifference = itr -> first - previous;
        }
        else {
            currentDifference = itr -> first - previous;
            if(currentDifference > otherMaxDifference){
                otherMaxDifference = currentDifference;
            }
        }
        previous = itr -> first;
        itr ++;
    }
    cout << (TopDifference / otherMaxDifference) << " " << (TopDifference - otherMaxDifference)  << " ";
    if(TopDifference > (otherMaxDifference * (depthSize /4))){
        return true;
    }else{
        return false;
    }

}



const int depthLetters[] = {4, 0, 19};
const int depthSizeParameters[] = {1000,100,20};
void recursivePKeyTest(int depth, string baseKey, string ciphertext, multimap<double, enigmaKey>& priorKeyList){
    if (depth == 2){
        // cout << baseKey << " ";
        // if (successCriterion(priorKeyList, depthSizeParameters[depth]) == true){
        //     enigmaKey currentKey = priorKeyList.begin() -> second;
        //     cout << "wheel order:" << currentKey.wheelOrder << "\n";
        //     cout << "inital setting:" << currentKey.initalSetting << "\n";
        //     cout << "ring setting:" << currentKey.ringSetting << "\n";
        //     cout << "starter plugboard:" << currentKey.plugs << "\n";
        // }
        cout << baseKey << " ";
        successCriterion(priorKeyList, depthSizeParameters[depth]);

        enigmaKey bestKey = priorKeyList.begin() -> second;
        double entropy1 = priorKeyList.begin() -> first;
        bestKey.plugs = alphabet;
        double entropy2 = entropy(enigma(ciphertext,bestKey));
        cout << entropy1  << " ";
        cout << entropy2  << " ";
        cout << entropy2 - entropy1 << " ";
        cout << entropy2 / entropy1 << "\n";

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
                    shortKeyList.clear();
                    newKey = baseKey;
                    newKey[depthLetters[depth]] = alphabet[i];
                    newKey[i] = alphabet[depthLetters[depth]];
                    // take top X and try new plugboard
                    itr = priorKeyList.begin();
                    for (mapIterator=0;mapIterator<depthSizeParameters[depth];mapIterator++) {
                        currentKey = itr ->second;
                        currentKey.plugs = newKey;
                        shortKeyList.emplace(make_pair(entropy(enigma(ciphertext,currentKey)), currentKey));
                        itr++;
                    }
                    recursivePKeyTest(depth+1, newKey, ciphertext, shortKeyList);
                }
            }
        }
    }

}

void newOveralAlgorithm(string ciphertext){
    rotorBruteForce(ciphertext);
    recursivePKeyTest(0,alphabet,ciphertext,fullRotorKeys);
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

int main(){
    srand((unsigned) time(NULL));
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristicsofthetypecanbeusedtoidentifythesourceasthetypewriterweholdinthemansionseemstometobedivinationnotscienceandwithoutmuchstrongerevidenceofalinkbetweensomeoneinmyhouseholdandtheyoungmanislesiwillnotentertainorcooperatewithanyfurtherinvestigationsihavealreadyexpressedmyconsternationattheprospectofascandalderailingmycampaignforstategovernorandithoughtthatyouunderstoodandagreedwithmewhyyouwouldwanttoreopenthecasewhenihavesomuchtoloseandwehavenothingtogainfromiticannotunderstandiknowthatmaisiehasbeenpressingforfurtherinvestigationanditisclearthatshewasdeeplydistressedbythewholeincidentshelovesthelibraryandwoulddoanythingtoprotectithoweversheismerelyaservantoftheestateandsurelymystatusaspresentownergivesgreaterweighttomyviewsandwisheswhileislesclearlytrespassedheseemsnottohavecausedanydamageandevenifhewasintendingtostealfrommehisuntimelydeathforestalledthatplanhehasalreadypaidthepriceforhismisdeedsandidonotseeanyreasontocontinuetopokeandprobeatwhatmustsurelybearoutinecaseimustinsistthatyoubringafinalhalttoyourinvestigationsandlethimrestinpeaceletusnotinvitegossipatthiscriticaltimeyoursinfriendshiprogers";
    enigmaKey testingKey;
    testingKey.wheelOrder = "321";
    testingKey.ringSetting = "gto";
    testingKey.initalSetting ="aeh";
    testingKey.plugs = "ceapbythojurmvidqlsgknwzfx";


    // string testCiphertext = enigma(testPlaintext, testingKey);
    // newOveralAlgorithm(testCiphertext);
    cout << randomPlugboard();

    // string key = alphabet;
    // while (key[1] != 'e' || key[0] != 'n' || key[19] != 'm'){
    //     key = randomPlugboard();
    // }
    // cout << key;

    // string key = randomPlugboard();
    // while (key[2] != 'a' || key[0] != 'c' || key[1] != 'e' || key[4] != 'b'){
    //     key = randomPlugboard();
    // }
    // cout << key;
    
    // auto start = high_resolution_clock::now();
    // cout << entropy(rotors(testCiphertext,testingKey)) << "\n";
    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // cout << duration.count();

    return 1;
}


