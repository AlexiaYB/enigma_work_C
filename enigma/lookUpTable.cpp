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
const int wheels[5][26] = {{4, 9, 10, 2, 7, 1, 23, 9, 13, 16, 3, 8, 2, 9, 10, 18, 7, 3, 0, 22, 6, 13, 5, 20, 4, 10}, 
                        {0, 8, 1, 7, 14, 3, 11, 13, 15, 18, 1, 22, 10, 6, 24, 13, 0, 15, 7, 20, 21, 3, 9, 24, 16, 5},
                        {1, 2, 3, 4, 5, 6, 22, 8, 9, 10, 13, 10, 13, 0, 10, 15, 18, 5, 14, 7, 16, 17, 24, 21, 18, 15},
                        {4, 17, 12, 18, 11, 20, 3, 19, 16, 7, 10, 23, 5, 20, 9, 22, 23, 14, 1, 13, 16, 8, 6, 15, 24, 2},
                        {21, 24, 25, 14, 2, 3, 13, 17, 12, 6, 8, 18, 1, 20, 23, 8, 10, 5, 20, 16, 22, 19, 9, 7, 4, 11}};
const string wheelsAlphabet[] = {"ekmflgdqvzntowyhxuspaibrcj","ajdksiruxblhwtmcqgznpyfvoe", "bdfhjlcprtxvznyeiwgakmusqo", "esovpzjayquirhxlnftgkdcmwb", "vzbrgityupsdnhlxawmjqofeck"};

string currentLookuptable [26];

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

// encryptors 
int reflectorUKWB(int letterNum){
    string key = "yruhqsldpxngokmiebfzcwvjat";
    // well isn't that more efficient!!!!
    return (key[letterNum] - 'a');
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
    int i,j,k, tempInitial, tempRing, doubleStep, counter=0;
    string outputText;
    set<int> ringAffected;

    // set initial position of counter
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
        // then add the adjustments (for rotor position)
        tempInitial *= pow(26, 2-i);
        tempRing *= pow(26, 2-i);
        counter += tempInitial + tempRing;
    }
    counter = mod(counter,17576);

    // figure out things affected by ring settings
    for (j=0;j<(currentKey.ringSetting[2] - 'a');j++){
        ringAffected.insert(mod((turnovers[currentKey.wheelOrder[2] - '1'] - currentKey.initalSetting[2])+ j,26));
    }

    // figure out double step location
    doubleStep = mod((turnovers[currentKey.wheelOrder[2] - '1'] -'a') - (currentKey.initalSetting[2]- 'a'));
    doubleStep += mod((turnovers[currentKey.wheelOrder[1] - '1'] -'a') - (currentKey.initalSetting[1]- 'a')- 1) * 26;
    doubleStep = mod(doubleStep + counter, 17576);


    for (k=0;k<text.length();k++){
        counter = mod(counter, 17576);
        // rings adjustment fix
        if (ringAffected.count(mod(k,26)) == 1){
           outputText += currentLookuptable[text[k] - 'a'][mod(counter + 26,17576)]; 
        }else{
            outputText += currentLookuptable[text[k] - 'a'][counter];
        }
        // double stepping fix 
        if(counter  == doubleStep){
            doubleStep = mod((doubleStep + 676), 17576);
            counter += 26;
        }
        counter ++;
    }
    return outputText;
}

string extendedRotors(string text, enigmaKey &currentKey){
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
        if ((alphabet[currentMessageSettings[2]] == turnovers[(int)currentKey.wheelOrder[2]-49])&&(alphabet[currentMessageSettings[1]] == turnovers[(int)currentKey.wheelOrder[1]-49])){
            currentMessageSettings[0] = mod(currentMessageSettings[0] + 1);
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



int main(){
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristicsofthetypecanbeusedtoidentifythesourceasthetypewriterweholdinthemansionseemstometobedivinationnotscienceandwithoutmuchstrongerevidenceofalinkbetweensomeoneinmyhouseholdandtheyoungmanislesiwillnotentertainorcooperatewithanyfurtherinvestigationsihavealreadyexpressedmyconsternationattheprospectofascandalderailingmycampaignforstategovernorandithoughtthatyouunderstoodandagreedwithmewhyyouwouldwanttoreopenthecasewhenihavesomuchtoloseandwehavenothingtogainfromiticannotunderstandiknowthatmaisiehasbeenpressingforfurtherinvestigationanditisclearthatshewasdeeplydistressedbythewholeincidentshelovesthelibraryandwoulddoanythingtoprotectithoweversheismerelyaservantoftheestateandsurelymystatusaspresentownergivesgreaterweighttomyviewsandwisheswhileislesclearlytrespassedheseemsnottohavecausedanydamageandevenifhewasintendingtostealfrommehisuntimelydeathforestalledthatplanhehasalreadypaidthepriceforhismisdeedsandidonotseeanyreasontocontinuetopokeandprobeatwhatmustsurelybearoutinecaseimustinsistthatyoubringafinalhalttoyourinvestigationsandlethimrestinpeaceletusnotinvitegossipatthiscriticaltimeyoursinfriendshiprogers";
    enigmaKey testingKey;
    testingKey.wheelOrder = "123";
    testingKey.ringSetting = "aau";
    testingKey.initalSetting = "aaa";
    
    string testCiphertext = rotors(testPlaintext, testingKey);
    lookupTableReader("123");
    cout << fasterRotors(testCiphertext,testingKey);
    // cout << testCiphertext;


    auto start = high_resolution_clock::now();
    // lookupTableReader("123");
    fasterRotors(testCiphertext,testingKey);
    // rotors(testCiphertext,testingKey);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count();

    // string baseText = "";
    // int i;
    // for(i=0;i<17576;i++){
    //     baseText += 'a';
    // }
    // cout << extendedRotors(baseText,testingKey);

    return 1;
}


