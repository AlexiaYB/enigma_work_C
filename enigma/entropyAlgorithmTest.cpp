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
multimap<double, enigmaKey> fullRotorKeys;
string currentLookuptable [26];

int mod(int currentNum, int modulus = 26){
    if (currentNum > 0){
        return currentNum % modulus;
    }
    else{
        return (currentNum % modulus + modulus) % modulus;
    }
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

void lookupTableReader(string wheelOrder){
    ifstream dFile("lookup/"+ wheelOrder + ".txt");
    string changedText;
    int counter = 0;
    while (getline(dFile, changedText, '\n')){
        currentLookuptable[counter] = changedText;
        counter ++;
    }
    return;
}

string fasterRotors(string text, enigmaKey &currentKey){
    int i,j, tempInitial, tempRing, counter=0;
    string outputText;

    for(i=0;i<3;i++){
        // check for initial settings over turnover
        if(((currentKey.initalSetting[i] - 'a')) > (turnovers[currentKey.wheelOrder[i] - '1'] - 'a')){
            tempInitial = -(26 - (currentKey.initalSetting[i]- 'a'));
        }else{
            tempInitial = (currentKey.initalSetting[i]- 'a');
        }
        // check for ring settings over turnover
        if(((currentKey.ringSetting[i] - 'a')) > (turnovers[currentKey.wheelOrder[i] - '1'] - 'a')){
            tempRing = (26 - (currentKey.ringSetting[i]- 'a'));
        }else{
            tempRing = -(currentKey.ringSetting[i]- 'a');
        }
        // then add the adjustments (for rotor position & double stepping)
        if(i == 0){
            tempInitial *= 650;
            tempRing *= 650;
        }else {
            tempInitial *= pow(26, 2-i);
            tempRing *= pow(26, 2-i);
        }

        counter += tempInitial + tempRing;
    }
    counter = mod(counter,16900);

    for (j=0;j<text.length();j++){
        outputText += currentLookuptable[text[j] - 'a'][mod(counter, 16900)];
        counter ++;
    }
    return outputText;
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
                        lookupTableReader(currentKey.wheelOrder);
                        for(l=0;l<26;l++){for(m=0;m<26;m++){for (n=0;n<26;n++){
                                    currentKey.initalSetting = string() + alphabet[l] + alphabet[m] + alphabet[n];
                                    currentKey.ringSetting = "aaa";
                                    currentKey.plugs = "";
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

int main () {
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristicsofthetypecanbeusedtoidentifythesourceasthetypewriterweholdinthemansionseemstometobedivinationnotscienceandwithoutmuchstrongerevidenceofalinkbetweensomeoneinmyhouseholdandtheyoungmanislesiwillnotentertainorcooperatewithanyfurtherinvestigationsihavealreadyexpressedmyconsternationattheprospectofascandalderailingmycampaignforstategovernorandithoughtthatyouunderstoodandagreedwithmewhyyouwouldwanttoreopenthecasewhenihavesomuchtoloseandwehavenothingtogainfromiticannotunderstandiknowthatmaisiehasbeenpressingforfurtherinvestigationanditisclearthatshewasdeeplydistressedbythewholeincidentshelovesthelibraryandwoulddoanythingtoprotectithoweversheismerelyaservantoftheestateandsurelymystatusaspresentownergivesgreaterweighttomyviewsandwisheswhileislesclearlytrespassedheseemsnottohavecausedanydamageandevenifhewasintendingtostealfrommehisuntimelydeathforestalledthatplanhehasalreadypaidthepriceforhismisdeedsandidonotseeanyreasontocontinuetopokeandprobeatwhatmustsurelybearoutinecaseimustinsistthatyoubringafinalhalttoyourinvestigationsandlethimrestinpeaceletusnotinvitegossipatthiscriticaltimeyoursinfriendshiprogers";
    enigmaKey testingKey;
    testingKey.wheelOrder = "321";
    testingKey.ringSetting = "aaa";
    testingKey.initalSetting ="gsv";

    lookupTableReader(testingKey.wheelOrder);
    string testCiphertext = fasterRotors(testPlaintext, testingKey);
    auto start = high_resolution_clock::now();
    rotorBruteForce(testCiphertext);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count();
    outputFitnessMultimapCSV("output.csv", fullRotorKeys);
    return 1;
}



