/* -*- coding: utf-8 -*- */
/****************************************************************************
 * FileName     [ mydisambig.cpp ]
 * Synopsis     [ Implementation of a viterbi-based decoding process of the language model ]
 * Author       [ Ting-Wei Liu (Andi611) ]
 * Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
 * Reference    [ https://github.com/orbxball/DSP/blob/master/hw3/mydisambig.cpp ]
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;
/***************************************************************************/
#include "File.h"
#include "Prob.h"           // About Probability
#include "LM.h"             // Basic interface of Language Model
#include "Ngram.h"          // Inherit from LM.h
#include "Vocab.h"          // Vocabulary: Here is for set of Chinese Character
#include "VocabMap.h"       // Vocabulary map: two vocabulary mapping
#include "VocabMultiMap.h"
/******************************************************************************************************
 * Available Constants in .h
 * --------------------------------
 * maxWordsPerLine = 50000 (File.h)
 * maxWordLength = 1024 (Vocab.h)
 * Log_Zero = -inf (Prob.h)
 * Log_Inf = inf (Prob.h)
 * Log_One = 0 (Prob.h)
 * Vocab_None: A special VocabIndex used to denote no vocabulary item and to terminate VocabIndex arrays
*******************************************************************************************************/


/**************************************/
/*          Macro Definition          */
/**************************************/
#ifndef CANDIDATE
#define CANDIDATE 1000
#endif

#ifndef LEN
#define LEN 200
#endif

#ifndef SMALL
#define SMALL -100
#endif


/***************************************/
/*           Program Backoff           */
/***************************************/
void program_backoff(const string s, bool keyword_error) {
    if (keyword_error) cerr << "Error >>> Invalid Argument: " << s << endl;
    else cerr << "Error >>> Invalid number of argument: " << s << endl;
    cerr << "Usage: ./mydisambig -text [testing file] -map [mapping file] -lm [language model] -order [order of ngram language model] > [output file path]\n" << endl;
    exit(1);
}


void lm_decode(Ngram lm, File& text_file, VocabMap map, Vocab voc, Vocab ZhuYin, Vocab Big5) {
    /******* Go through the text file *******/
    char* buf;
    while(buf = text_file.getline()) {

        // Need to parse to words, otherwise char* will not work
        VocabString line[maxWordsPerLine];
        unsigned int count = Vocab::parseWords(buf, &(line[1]), maxWordsPerLine);
        line[0] = "<s>";
        line[count+1] = "</s>";
        count += 2;

        // Parameter
        LogP Proba[LEN][CANDIDATE] = {{0.0}};
        VocabIndex VidxGraph[LEN][CANDIDATE]; // Record the (i, j) VocalIndex of Big5
        int Backtrack[LEN][CANDIDATE];
        int CandiNum[LEN];

        Prob p;
        VocabIndex v_idx; //Be mapped by ZhuYin, which is a VocabIndex of Big5
        VocabIndex empty_context[] = {Vocab_None};
        VocabIndex bi_context[] = {Vocab_None, Vocab_None};

        // Initialization for Viterbi
        VocabMapIter iter(map, ZhuYin.getIndex(line[0]));
        iter.init();
        int size = 0;
        while (iter.next(v_idx, p)) {
            VocabIndex candi = voc.getIndex(Big5.getWord(v_idx));

            // if we cannot find the word in voc, we should mark it Unknown rather than discard it
            candi = (candi == Vocab_None)? voc.getIndex(Vocab_Unknown): candi;

            LogP logp = lm.wordProb(candi, empty_context); //unigram for start <s>
            Proba[0][size] = (logp == LogP_Zero)? SMALL: logp;
            VidxGraph[0][size] = v_idx;
            Backtrack[0][size] = -1; // start: -1
            size++;
        }
        CandiNum[0] = size;

        
        // Recursion for Viterbi
        for (int i = 1; i < count; i++) {
            VocabMapIter iter(map, ZhuYin.getIndex(line[i]));
            iter.init();
            size = 0;
            while (iter.next(v_idx, p)) {
                VocabIndex candi = voc.getIndex(Big5.getWord(v_idx));
                candi = (candi == Vocab_None)? voc.getIndex(Vocab_Unknown): candi;

                // See last column which has the highest probability
                LogP maxp = LogP_Zero;
                for (int j = 0; j < CandiNum[i-1]; j++) {
                    VocabIndex last = voc.getIndex(Big5.getWord(VidxGraph[i-1][j]));
                    last = (last == Vocab_None)? voc.getIndex(Vocab_Unknown): last;
                    bi_context[0] = last;

                    LogP logp = lm.wordProb(candi, bi_context);
                    // Check backoff!!! VERY IMPORTANT!!! 
                    LogP backoff = lm.wordProb(candi, empty_context);
                    if (logp == LogP_Zero && backoff == LogP_Zero) logp == SMALL;

                    logp += Proba[i-1][j];
                    if (logp > maxp) {
                        maxp = logp;
                        Backtrack[i][size] = j;
                    }
                }
                Proba[i][size] = maxp;
                VidxGraph[i][size] = v_idx;
                size++;
            }
            CandiNum[i] = size;
        }

        // Find the Max Probability path
        LogP maxp = LogP_Zero;
        int max_col = -1, j;
        for (j = 0; j < CandiNum[count-1]; j++) {
            if (Proba[count-1][j] > maxp) {
                maxp = Proba[count-1][j];
                max_col = j;
            }
        }

        VocabString AnsPath[maxWordLength];
        AnsPath[0] = "<s>";
        AnsPath[count-1] = "</s>";
        for (int i = count-1; i > 0; i--) {
            AnsPath[i] = Big5.getWord(VidxGraph[i][max_col]);
            max_col = Backtrack[i][max_col];
        }

        // Print the Answer Path
        for (int i = 0; i < count; i++)
            printf("%s%s", AnsPath[i], (i == count-1)? "\n": " ");
    }
    
}


/*************************************/
/*           Main Function           */
/*************************************/
int main (int argc, char* argv[]) {

    /******* Parse arguments *******/
    const string arg_text("text");
    const string arg_map("map");
    const string arg_lm("lm");
    const string arg_order("order");
    if (argc != 9) program_backoff(to_string(argc), false);

    const string argv_1(argv[1]);
    const string argv_3(argv[3]);
    const string argv_5(argv[5]);
    const string argv_7(argv[7]);
    if (argv_1.substr(argv_1.find(arg_text), 4) != arg_text) program_backoff(argv_1, true);
    if (argv_3.substr(argv_3.find(arg_map), 3) != arg_map) program_backoff(argv_3, true);
    if (argv_5.substr(argv_5.find(arg_lm), 2) != arg_lm) program_backoff(argv_5, true);
    if (argv_7.substr(argv_7.find(arg_order), 5) != arg_order) program_backoff(argv_7, true);

    
    const char* arg_text_value = argv[2];
    const char* arg_map_value = argv[4];
    const char* arg_lm_value = argv[6];
    const int arg_order_value = atoi(argv[8]);

    /******* Initialize extern parameter *******/
    Vocab voc;
    Vocab ZhuYin, Big5;

    /******* Read Map *******/
    VocabMap map(ZhuYin, Big5);
    File map_file(arg_map_value, "r");
    map.read(map_file);
    map_file.close();

    /******* Read Language Model *******/
    Ngram lm(voc, arg_order_value);
    File lm_file(arg_lm_value, "r");
    lm.read(lm_file);
    lm_file.close();
    
    /******* Read the testdata *******/
    File text_file(arg_text_value, "r");

    /******* Language model decoding *******/
    lm_decode(lm, text_file, map, voc, ZhuYin, Big5);
    text_file.close();

    return 0;
}

