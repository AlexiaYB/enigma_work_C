// imports 
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// constants
const int qlDatasetLen = 40629;
const string alphabet = "abcdefghijklmnopqrstuvwxyz";
// file data
string qStrings[qlDatasetLen];
double qFrequencies[qlDatasetLen];
// decryption counts
int decryptions[] = {19, 24, 26, 23, 18, 17, 21, 16, 27, 20, 22, 31, 14, 25, 28, 32, 34, 13, 30, 12, 15, 29, 10, 11, 33, 35, 38, 36, 39, 8, 40, 45, 41, 37, 9, 43, 5, 42, 7, 44, 48, 54, 4};
double dProbability[] = {0.05582922824302135, 0.054187192118226604, 0.052545155993431854, 0.052545155993431854, 0.04926108374384237, 0.04597701149425287, 0.04433497536945813, 0.04433497536945813, 0.042692939244663386, 0.042692939244663386, 0.041050903119868636, 0.041050903119868636, 0.03776683087027915, 0.034482758620689655, 0.034482758620689655, 0.031198686371100168, 0.027914614121510674, 0.027914614121510674, 0.026272577996715927, 0.022988505747126436, 0.021346469622331693, 0.0180623973727422, 0.016420361247947456, 0.014778325123152709, 0.014778325123152709, 0.013136288998357963, 0.011494252873563218, 0.009852216748768473, 0.008210180623973728, 0.008210180623973728, 0.008210180623973728, 0.006568144499178982, 0.006568144499178982, 0.0049261083743842365, 0.0049261083743842365, 0.0049261083743842365, 0.0049261083743842365, 0.003284072249589491, 0.003284072249589491, 0.0016420361247947454, 0.0016420361247947454, 0.0016420361247947454, 0.0016420361247947454};
int decryptionsCounted;

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

int position(int intArray[], int currentInt){
    // returns first position in string
    int i,length = 43;
    for(i=0; i<length;i++){
        if (intArray[i] == currentInt){
            return i;
        }
    }

    return -1;
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

string randomPlugboard(){
    // a new random plugboard each second
    int i,indexIn,randomValue;
    char character;
    string new_swaps = "abcdefghijklmnopqrst";
    for(i=0;i<20;i++){
        while (indexIn != -1){
            randomValue = rand()%26;
            character = alphabet[randomValue];
            indexIn = position(new_swaps,character);
        }
        new_swaps[i] = character;
        indexIn = 0;
    }
    return new_swaps;
}

string notInSwaps(string swaps){
    int i, counterNIS = 0;
    string notIn = "abcdef";
    for(i=0; i<26; i++){
        if (position(swaps,alphabet[i]) == -1){
            notIn[counterNIS] = alphabet[i];
            counterNIS ++;
        }
    }
    return notIn;
}

void neighbours_plugboard(string swaps, string neighbours[210]){
    int i,j, counterNP =0;
    char tempChar;
    string notIn = notInSwaps(swaps);
    string newSwaps = swaps;

    for(i=0;i<20;i++){
        // swapping already included letters
        if(i%2 ==0){
            for(j=(i+2);j<20;j++){
                newSwaps = swaps;
                newSwaps[i] = swaps[j];
                newSwaps[j] = swaps[i];
                neighbours[counterNP] = newSwaps;
                counterNP ++;
            }
        }

        // not included letters
        for(j=0;j<6;j++){
            newSwaps = swaps;
            newSwaps[i] = notIn[j];
            neighbours[counterNP] = newSwaps;
            counterNP ++;
        }
    }
    return;
}

string plugboardHC(string bestSwaps,string ciphertext){
    int timeCount = 0;
    string testPlaintext = ciphertext;
    double bestFitness, currentFitness;
    string neighbours[210];
    string currentSwaps = bestSwaps;
    int i, stuck = 0;
    readQuadgramsCSV();

    bestSwaps = randomPlugboard();
    testPlaintext = plugboardEncryptor(ciphertext,bestSwaps);
    bestFitness = quadgram_fitness(testPlaintext);

    while (stuck == 0){
        stuck = 1;
        neighbours_plugboard(bestSwaps, neighbours);
        for(i=0;i<210;i++){
            currentSwaps = neighbours[i];
            testPlaintext = plugboardEncryptor(ciphertext,currentSwaps);
            timeCount++;
            currentFitness = quadgram_fitness(testPlaintext);
            if(bestFitness < currentFitness){
                bestSwaps = currentSwaps;
                bestFitness = currentFitness;
                stuck = 0;
                break;
            }
        }
    }
    decryptionsCounted = timeCount;
    return bestSwaps;
}

int plugboardComparison(string actualSwaps, string HCswaps){
    int i, pos;
    for(i=0;i<20;i+= 2){
        pos = position(HCswaps, actualSwaps[i]);
        if(pos == -1){
            return -1;
        }else if (pos % 2 == 0){
            if (HCswaps[pos+1] != actualSwaps[i+1]){
                return -1;
            }
        }else{
            if (HCswaps[pos-1] != actualSwaps[i+1]){
                return -1;
            }

        }
    }
    return 1;
}

double determineDProbability(int measuredDecryptions){
    int pos;
    pos = position(decryptions, measuredDecryptions/100);
    if (pos != -1){
        return dProbability[pos];
    }
    else{
        return 0;
    }

}


int main() {
    int i;
    srand((unsigned) time(NULL));
    string plaintext = "dearmrrogersyouwillneverknowhowrelievediamtohearthatthelibrarywillremainintactandhereatthemanoriknowthatyouwerekeentosupportourcityinitsambitiontorivalnewyorkwithitsastorandlennoxlibrariesbuttogetherwehavespentconsiderableeffortbuildingauniquecollectionofrareandimportantworksandiwouldhavebeendeeplysaddenedtoloseitiwastouchedbyyouroffertomakeitaconditionofthedonationthatishouldcontinuetobeemployedasthecuratorbutthemanorismyhomeaswellasmyworkplaceanditfeelsrightthatthisimportantcollectionofrarebooksshouldremainhereinprivatehandswhereicancareforitproperlyiamsorrytohearthatyouwillbewithdrawingfromtheraceforgovernorasiknowthatwasacherishedambitioniamsureyouwouldhavefilledtherolewithdistinctionbuttherewillbeotherwaystoserveandithinkyouarerightthatthestrangedeathofstanleyisleswouldhaveunderminedyourcampaignweknowwhatgossipcandointhesmallworldofrhodeislandsocietyandyouropponentswouldhavetakengreatpleasureinspeculatingontheunexplaineddeathinyourhomeasyourequestedihavecompletedmyauditofthelibrarycontentsandicanassureyouthatnothingismissingandnobookshavebeenharmediwillofcoursecontinuetoprotectthemandihopeyouwillagreethatitisworthcontinuingtoinvestinthecollectiontogetherwecanbuilditintooneofthefinestarchivesofamericanliteratureontheeastcoastiunderstandthatyourniecenancywillbevisitingnextweekandilookforwardtoseeingheragainwhileshedoesnotshareourloveoffinebooksshecertainlylovesthishouseandiamsureshewillhaveherownideasabouthowtofreshenitupiremainaloyalservanttoyouandtothelibrarymaisie";
    string bestKey = "abcdefghijklmnopqrst";
    for (i=0;i<100;i++){
        bestKey = plugboardHC("abcdefghijklmnopqrst", plugboardEncryptor(plaintext, "adehgyimknlrozqvtxuw"));
        if (plugboardComparison("adehgyimknlrozqvtxuw", bestKey) == 1){
            cout << "correct "<< decryptionsCounted<< " "<< determineDProbability(decryptionsCounted) << ", ";
        }
        else{
            cout << "incorrect "<< decryptionsCounted<< " "<< determineDProbability(decryptionsCounted) << ", ";
        }
    }

    return 0;
}

