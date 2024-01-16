#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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

void plugboardEncryptor(char* text, char* newText, char* swaps){
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

void randomisePlugboard(char* swaps){
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

void notInSwaps(char* notIn, char* swaps){
    int i, counter = 0;
    for(i=0; i<26; i++){
        if (position(swaps,alphabet[i]) == -1){
            notIn[counter] = alphabet[i];
            counter ++;
        }
    }
    return;
}

void neighbours_plugboard(char* swaps, char neighbours[121][21]){
    char notIn[6],newSwaps[20], tempChar;
    int i,j, counter = 0;

    notInSwaps(notIn, swaps);
    for(i=0;i<20;i++){
        // swapping already included letters
        if(i%2 ==0){
            for(j=(i+2);j<20;j++){
                strcpy(newSwaps,swaps);
                newSwaps[i] = swaps[j];
                newSwaps[j] = swaps[i];
                strcpy(neighbours[counter],newSwaps);
                counter ++;
            }
        }

        // not included letters
        strcpy(newSwaps,swaps);
        for(j=0;j<6;j++){
            newSwaps[i] = notIn[j];
            strcpy(neighbours[counter],newSwaps);
            counter ++;
        }
    }
    return;
}

int main(){
    srand((unsigned) time(NULL));
    // variables
    // char plaintext[] = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristic";
    // char ciphertext[strlen(plaintext)];
    char swaps[] = "adehgyimknlrozqvtxuw";
    char notIn[7];
    char neighbours[211][21];

    // plugboard_encryptor(plaintext,ciphertext, swaps);
    // printf("%s",plaintext);
    // printf("\n%s",ciphertext);
    //randomisePlugboard(swaps);
    neighbours_plugboard(swaps, neighbours);
    int i,j, counter =0;
    for(i=0;i<210;i++){
        counter ++;
        printf("\n%s",neighbours[i]);
    }
    printf("\n%d", counter);
    return 0;
}





