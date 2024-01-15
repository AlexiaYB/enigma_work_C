#include <stdio.h>
#include <string.h>


int position(char* referenceString, char currentharacter){
    // returns first position in string
    int i,length;
    length = strlen(referenceString);
    for(i=0; i<length;i++){
        if (referenceString[i] == currentharacter){
            return i;
        }
    }
    return -1;
}

void plugboard_encryptor(char* text, char* newText, char* swaps){
    // iterate through the text
    int i, index, length;
    length = strlen(text);
    newText[length] = '\0';
    for(i = 0; i < length; i++){
        index = position(swaps,text[i]);
        if(index == -1){
            // no?
            // newText[currentIndex] = text[currentIndex]
            newText[i] = text[i];
        }
        else {
            // yes?
            // newText[currentIndex] = text[currentIndex +/- 1] (depends on even/odd)
            if (index % 2 == 0){
                newText[i] = swaps[index + 1];
            }
            else {
                newText[i] = swaps[index - 1];
            }
        }
    }   
    return;
}


int main(){
    // variables
    char plaintext[] = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristic";
    char ciphertext[strlen(plaintext)];
    char swaps[] = "adehgyimknlrozqvtxuw";
    plugboard_encryptor(plaintext,ciphertext, swaps);
    printf("%s",plaintext);
    printf("\n%s",ciphertext);
    return 0;
}





