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
string qStrings[qlDatasetLen];
double qFrequencies[qlDatasetLen];
ifstream qFile("quadgram_log_values.csv"); 
const int qLineLength = 20;

double quadgramLogResult(int quadgramValue){
    string data;
    char currentCharacter;
    qFile.seekg(qLineLength * quadgramValue, ios::beg);
    getline(qFile, data);
    return stod(data);
}

double quadgramFitness(string text){
    int i,j, qSum = 0;
    double sum = 0.;
    for(i=0;i<(text.length()-3);i++){
        for(j=0;j<4;j++){
            qSum += (text[i+j] - 'a') * (pow(26,(3-j)));
        }
        sum += quadgramLogResult(qSum);
        qSum = 0;
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



int main() {
    srand((unsigned) time(NULL));
    int i;
    string plaintext = "dearmrrogersyouwillneverknowhowrelievediamtohearthatthelibrarywillremainintactandhereatthemanoriknowthatyouwerekeentosupportourcityinitsambitiontorivalnewyorkwithitsastorandlennoxlibrariesbuttogetherwehavespentconsiderableeffortbuildingauniquecollectionofrareandimportantworksandiwouldhavebeendeeplysaddenedtoloseitiwastouchedbyyouroffertomakeitaconditionofthedonationthatishouldcontinuetobeemployedasthecuratorbutthemanorismyhomeaswellasmyworkplaceanditfeelsrightthatthisimportantcollectionofrarebooksshouldremainhereinprivatehandswhereicancareforitproperlyiamsorrytohearthatyouwillbewithdrawingfromtheraceforgovernorasiknowthatwasacherishedambitioniamsureyouwouldhavefilledtherolewithdistinctionbuttherewillbeotherwaystoserveandithinkyouarerightthatthestrangedeathofstanleyisleswouldhaveunderminedyourcampaignweknowwhatgossipcandointhesmallworldofrhodeislandsocietyandyouropponentswouldhavetakengreatpleasureinspeculatingontheunexplaineddeathinyourhomeasyourequestedihavecompletedmyauditofthelibrarycontentsandicanassureyouthatnothingismissingandnobookshavebeenharmediwillofcoursecontinuetoprotectthemandihopeyouwillagreethatitisworthcontinuingtoinvestinthecollectiontogetherwecanbuilditintooneofthefinestarchivesofamericanliteratureontheeastcoastiunderstandthatyourniecenancywillbevisitingnextweekandilookforwardtoseeingheragainwhileshedoesnotshareourloveoffinebooksshecertainlylovesthishouseandiamsureshewillhaveherownideasabouthowtofreshenitupiremainaloyalservanttoyouandtothelibrarymaisie";
    string bestKey = "aosduvpqiwrlmnbghkcxefjtzy";
    string ciphertext = plugboardEncryptor(plaintext,bestKey);


    auto start = high_resolution_clock::now();
    quadgramFitness(plaintext);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count();

    qFile.close();
    return 1;
}


