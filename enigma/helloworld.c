#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
char alphabet[] = "abcdefghijklmnopqrstuvwxyz";



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
    int i, index, length;
    length = strlen(text);
    newText[length] = '\0';
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
    return;
}

void randomise_plugboard(char* swaps){
    // a new random plugboard each second
    int i,indexIn,randomValue;
    char character;
    char new_swaps[strlen(swaps)];
    for(i=0;i<20;i++){
        while (indexIn != -1){
            randomValue = rand()%26;
            character = alphabet[randomValue];
            indexIn = position(new_swaps,character);
        }
        new_swaps[i] = character;
        indexIn = 0;
    }
    strcpy(swaps,new_swaps);
    return;
}

int main(){
    srand((unsigned) time(NULL));
    // variables
    // char plaintext[] = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristic";
    // char ciphertext[strlen(plaintext)];
    char swaps[] = "adehgyimknlrozqvtxuw";

    // plugboard_encryptor(plaintext,ciphertext, swaps);
    // printf("%s",plaintext);
    // printf("\n%s",ciphertext);
    printf("%s", swaps);
    randomise_plugboard(swaps);
    printf("\n%s", swaps);
    return 0;
}





