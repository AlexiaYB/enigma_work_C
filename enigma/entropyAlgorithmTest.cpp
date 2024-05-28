# include <iostream>
#include <set>
using namespace std;
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


string slowEquivalentKeys(char slowIntialSetting, char slowRingSetting){
    return ""s + alphabet[mod(26 - (slowRingSetting- slowIntialSetting))] + 'a';
}


const int depthLetters[] = {4, 0, 19};
void recursivePKeyTest(int depth, string baseKey){
    if (depth == 3){
        cout << baseKey;
        cout << "\n";
    }
    else if(alphabet[depthLetters[depth]] != baseKey[depthLetters[depth]]){
        recursivePKeyTest(depth+1, baseKey);
    }
    else{
        int i, j;
        string newKey;
        int selfSteckered = 0;
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
                    recursivePKeyTest(depth+1, newKey);
                }
            }
        }
    }

}



int main () {
    // recursivePKeyTest(0,alphabet);
    string testPlaintext = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristicsofthetypecanbeusedtoidentifythesourceasthetypewriterweholdinthemansionseemstometobedivinationnotscienceandwithoutmuchstrongerevidenceofalinkbetweensomeoneinmyhouseholdandtheyoungmanislesiwillnotentertainorcooperatewithanyfurtherinvestigationsihavealreadyexpressedmyconsternationattheprospectofascandalderailingmycampaignforstategovernorandithoughtthatyouunderstoodandagreedwithmewhyyouwouldwanttoreopenthecasewhenihavesomuchtoloseandwehavenothingtogainfromiticannotunderstandiknowthatmaisiehasbeenpressingforfurtherinvestigationanditisclearthatshewasdeeplydistressedbythewholeincidentshelovesthelibraryandwoulddoanythingtoprotectithoweversheismerelyaservantoftheestateandsurelymystatusaspresentownergivesgreaterweighttomyviewsandwisheswhileislesclearlytrespassedheseemsnottohavecausedanydamageandevenifhewasintendingtostealfrommehisuntimelydeathforestalledthatplanhehasalreadypaidthepriceforhismisdeedsandidonotseeanyreasontocontinuetopokeandprobeatwhatmustsurelybearoutinecaseimustinsistthatyoubringafinalhalttoyourinvestigationsandlethimrestinpeaceletusnotinvitegossipatthiscriticaltimeyoursinfriendshiprogers";
    enigmaKey testingKey;
    testingKey.wheelOrder = "321";
    testingKey.initalSetting ="beh";
    testingKey.ringSetting = "ito";
    string testCiphertext = rotors(testPlaintext,testingKey);
    testingKey.initalSetting ="teh";
    testingKey.ringSetting = "ato";
    cout << rotors(testCiphertext,testingKey);
    cout << slowEquivalentKeys('b','i');
    return 1;
}



