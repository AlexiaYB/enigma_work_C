#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
int qldatasetLen = 40629;


void readQuadgramsCSV(FILE *fp, char quadgrams[qldatasetLen][5], double frequencies[qldatasetLen]){
    char buffer[100];
    int row = 0;
    int column = 0, numLen = 0;

 
        while (fgets(buffer,100, fp)) {
            column = 0;
            char* value = strtok(buffer, ", ");
            if (row==0){
                row++;
                continue;
            }
            while (value) {
                // quadgram
                if (column == 0) {
                    strcpy(quadgrams[row -1], value);
                }
 
                // frequencies
                if (column == 1) {
                    char *uptr;
                    double numValue = strtod(value, &uptr);
                    frequencies[row-1] = numValue;
                }
                value = strtok(NULL, ", ");
                column++;
            }
            row++;
        }
 
 
    return; 
}

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
    int i, counterNIS = 0;
    for(i=0; i<26; i++){
        if (position(swaps,alphabet[i]) == -1){
            notIn[counterNIS] = alphabet[i];
            counterNIS ++;
        }
    }
    return;
}

void neighbours_plugboard(char* swaps, char neighbours[121][21]){
    char notIn[6],newSwaps[20], tempChar;
    notInSwaps(notIn, swaps);
    int i,j, counterNP =0;

    for(i=0;i<20;i++){
        // swapping already included letters
        if(i%2 ==0){
            for(j=(i+2);j<20;j++){
                strcpy(newSwaps,swaps);
                newSwaps[i] = swaps[j];
                newSwaps[j] = swaps[i];
                strcpy(neighbours[counterNP],newSwaps);
                counterNP ++;
            }
        }

        // not included letters
        strcpy(newSwaps,swaps);
        for(j=0;j<6;j++){
            newSwaps[i] = notIn[j];
            strcpy(neighbours[counterNP],newSwaps);
            counterNP ++;
        }
    }
    return;
}

int quadgramComparison(char actualQ[5], char comparisonQ[5]){
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

int inQuadgramsSearch(char quadgrams[qldatasetLen][5], char quadgram[5]){
    int difference, first = 0, last = qldatasetLen-1;
    int middle = last/2;
    while(first <= last){
        difference = quadgramComparison(quadgrams[middle], quadgram);
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
    if (first > last){
        return -1;
    }

}

double quadgram_fitness(char* text, double frequencies[qldatasetLen], char quadgrams[qldatasetLen][5]){
    int i,j, index = 0;
    double sum = 0.;
    char currentQuadgram[5];
    for(i=0;i<(strlen(text)-3);i++){
        for(j=0;j<4;j++){
            currentQuadgram[j] = text[i+j];
        }
        index = inQuadgramsSearch(quadgrams,currentQuadgram);
        if(index != -1){
            sum += frequencies[index];
        }
        else {
            sum += -4.000000000000196;
        }
    }
    
    return sum;
}


void plugboardHC(char* bestSwaps,char* ciphertext){
    char testPlaintext[strlen(ciphertext)];
    double bestFitness, currentFitness;
    double frequencies[qldatasetLen];
    char quadgrams[qldatasetLen][5];
    char neighbours[210][21];
    char currentSwaps[21];
    int i, stuck = 0;
    FILE* qp = fopen("short_quadgram_log_frequency.csv", "r");
    readQuadgramsCSV(qp,quadgrams,frequencies);
    fclose(qp);


    randomisePlugboard(bestSwaps);
    plugboardEncryptor(ciphertext,testPlaintext,bestSwaps);
    bestFitness = quadgram_fitness(testPlaintext,frequencies,quadgrams);

    while (stuck == 0){
        stuck = 1;
        neighbours_plugboard(bestSwaps, neighbours);
        for(i=0;i<210;i++){
            strcpy(currentSwaps,neighbours[i]);
            plugboardEncryptor(ciphertext,testPlaintext,currentSwaps);
            currentFitness = quadgram_fitness(testPlaintext,frequencies,quadgrams);
            if(bestFitness < currentFitness){
                strcpy(bestSwaps,currentSwaps);
                bestFitness = currentFitness;
                stuck = 0;
                break;
            }
        }
    }
    return;
}

int main(){
    srand((unsigned) time(NULL));

    // variables
    int i;
    char plaintext[] = "tommydearfriendiwassomewhatsurprisedandconfusedtoreceiveyourletteraskingmeaboutthenotethatdrwhitediscoveredinthebindingofthebookfoundbyislessbodyicannotseehowyoucouldthinkiwouldknowanythingaboutitandifinditextremelyunlikelythatitcouldhavebeenwrittenbyanyoneinmyhouseholdyourclaimthatthecharacteristic";
    char ciphertext[strlen(plaintext)];
    char bestSwaps[] = "abcdefghijklmnopqrst";

    // encrypt
    plugboardEncryptor(plaintext,ciphertext,"adehgyimknlrozqvtxuw");

    // hill climb
    plugboardHC(bestSwaps,ciphertext);
    printf("\n%s", bestSwaps);

 
    return 0;
}





