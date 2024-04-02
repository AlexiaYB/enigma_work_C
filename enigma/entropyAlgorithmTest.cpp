# include <iostream>
#include <set>
using namespace std;
const string alphabet = "abcdefghijklmnopqrstuvwxyz";
set<string> resultKeys;
int counter = 0;

const int depthLetters[] = {4,0, 19};
void recursivePKeyTest(int depth, string baseKey){
    if (depth == 3){
        // cout << baseKey;
        // cout << "\n";
        resultKeys.insert(baseKey);
        counter ++;
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
    recursivePKeyTest(0,alphabet);
    cout << resultKeys.size() << "\n";
    cout << counter;
    return 1;
}



