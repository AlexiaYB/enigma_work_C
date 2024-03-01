#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <cmath> 
#include <chrono>
#include <map>
using namespace std;
using namespace std::chrono;

const int qlDatasetLen = 40629;
const string alphabet = "abcdefghijklmnopqrstuvwxyz";
map<string,double> quadgrams;

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

string plugboardEncryptor(string text, string swaps){
    int i;
    string newText = text;
    for(i=0;i<text.length();i++){
        newText[i] = swaps[text[i] - 'a'];
    }
    return newText;
}

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
            if((alphabet[i+j] != swaps[i]) && ((alphabet[i+j] != swaps[i+j]) or (alphabet[i] != swaps[i]))){
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
    // decryptionsCounted = timeCount;
    return bestSwaps;
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

double equivalenceRelationship(string actualSwaps, string bestSwaps, string actualText){
    int i;
    double errorSum = 0.;
    if (actualSwaps == bestSwaps){
        return 0.;

    }else{
        map<char, double> percentageFrequencies;
        frequenciesCountPercentages(percentageFrequencies,actualText);
        for(i=0;i<26;i++){
            if(actualSwaps[i] != bestSwaps[i]){
                errorSum += percentageFrequencies[bestSwaps[i]];
            }
        }
        return errorSum;
    }
}

int main() {
    srand((unsigned) time(NULL));
    ifstream qFile("short_quadgram_log_frequency.csv"); 
    readQuadgramsCSV();
    int i;
    string plaintext = "dearmrrogersyouwillneverknowhowrelievediamtohearthatthelibrarywillremainintactandhereatthemanoriknowthatyouwerekeentosupportourcityinitsambitiontorivalnewyorkwithitsastorandlennoxlibrariesbuttogetherwehavespentconsiderableeffortbuildingauniquecollectionofrareandimportantworksandiwouldhavebeendeeplysaddenedtoloseitiwastouchedbyyouroffertomakeitaconditionofthedonationthatishouldcontinuetobeemployedasthecuratorbutthemanorismyhomeaswellasmyworkplaceanditfeelsrightthatthisimportantcollectionofrarebooksshouldremainhereinprivatehandswhereicancareforitproperlyiamsorrytohearthatyouwillbewithdrawingfromtheraceforgovernorasiknowthatwasacherishedambitioniamsureyouwouldhavefilledtherolewithdistinctionbuttherewillbeotherwaystoserveandithinkyouarerightthatthestrangedeathofstanleyisleswouldhaveunderminedyourcampaignweknowwhatgossipcandointhesmallworldofrhodeislandsocietyandyouropponentswouldhavetakengreatpleasureinspeculatingontheunexplaineddeathinyourhomeasyourequestedihavecompletedmyauditofthelibrarycontentsandicanassureyouthatnothingismissingandnobookshavebeenharmediwillofcoursecontinuetoprotectthemandihopeyouwillagreethatitisworthcontinuingtoinvestinthecollectiontogetherwecanbuilditintooneofthefinestarchivesofamericanliteratureontheeastcoastiunderstandthatyourniecenancywillbevisitingnextweekandilookforwardtoseeingheragainwhileshedoesnotshareourloveoffinebooksshecertainlylovesthishouseandiamsureshewillhaveherownideasabouthowtofreshenitupiremainaloyalservanttoyouandtothelibrarymaisie";
    string bestKey = "aosduvpqiwrlmnbghkcxefjtzy";
    string ciphertext = plugboardEncryptor(plaintext,bestKey);
    map<double,int> ERcounts;
    double result;
    for(i=0;i<1000;i++){
        result = equivalenceRelationship(bestKey, plugboardHC(bestKey,ciphertext), plaintext);
        if (ERcounts[result] == 0){
            ERcounts[result] =1;
        }
        else{
            ERcounts[result] += 1;
        }
        if(i %100 == 0){
        cout << i<< " done" << "\n";
        }
    }


    map<double,int>::iterator itr;
        for (itr = ERcounts.begin(); itr != ERcounts.end(); itr++) {
        cout << itr->first << "," << itr ->second << "\n";
    }
    // auto start = high_resolution_clock::now();
    // cout << equivalenceRelationship(bestKey, plugboardHC(bestKey,ciphertext), plaintext)<< "\n";

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // cout << duration.count();

    qFile.close();
    return 1;
}


