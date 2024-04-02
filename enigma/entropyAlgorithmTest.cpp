# include <iostream>
using namespace std;
const string alphabet = "abcdefghijklmnopqrstuvwxyz";

string generatePKeyTest(){
    int i, j, k;
    string previousPlugboardList[] = {alphabet,alphabet};
    // all E swaps
    for(i=0;i<26;i++){
        string newPlugboard = alphabet;
        // change swap
        newPlugboard[4] = alphabet[i];
        newPlugboard[i] = 'e';
        previousPlugboardList[0] = newPlugboard;
        // all A swaps
        for (j=0;j<26;j++){
            if(j!= 4 && j != i){
                newPlugboard = previousPlugboardList[0];
                if (i != 0){
                    newPlugboard[0] = alphabet[j];
                    newPlugboard[j] = 'a';
                    
                }
                previousPlugboardList[1] = newPlugboard;
                // all T swaps
                for (k=0;k<26;k++){
                    if (k != 4 && k != 0 && k != i && k!= j){
                        newPlugboard = previousPlugboardList[1];
                    // change swap
                        if (i != 19 && j != 19){
                            newPlugboard[19] = alphabet[k];
                            newPlugboard[k] = 't';
                        }
                        // if (newPlugboard == "ebcdafghijklmnopqrstuvwxyz"){
                        //     return newPlugboard;
                        // }
                        cout << newPlugboard;
                        cout << "\n";
                    }
                }
            }

        }
    }
    return "failed";
}


const int depthLetters[] = {4,0, 19};
void recursivePKeyTest(int depth, string baseKey){
    if (depth == 3){
        cout << baseKey;
        cout << "\n";
    }
    else if(alphabet[depthLetters[depth]] != baseKey[depthLetters[depth]]){
        recursivePKeyTest(depth+1, baseKey);
    }
    else{
        int i;
        string newKey;
        for (i=0;i<26;i++){
            if (alphabet[i] == baseKey[i]){
                newKey = baseKey;
                newKey[depthLetters[depth]] = alphabet[i];
                newKey[i] = alphabet[depthLetters[depth]];
                recursivePKeyTest(depth+1, newKey);
            }
        }
    }

}


int main () {
    recursivePKeyTest(0,alphabet);
    return 1;
}



