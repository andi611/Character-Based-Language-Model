/* -*- coding: utf-8 -*- */
/*******************************************************************************************
 * FileName     [ mydisambig.cpp ]
 * Synopsis     [ Implementation of a viterbi-based decoding process of the language model ]
 * Author       [ Ting-Wei Liu (Andi611) ]
 * Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
 * Reference    [ https://github.com/orbxball/DSP/blob/master/hw3/mydisambig.cpp ]
*******************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
/*****************************************************************************************************/
#include "LM.h"
#include "File.h"
#include "Prob.h"
#include "Ngram.h"
#include "Vocab.h"
#include "VocabMap.h"
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


/*******************************/
/*          CONSTANTS          */
/*******************************/
#ifndef CANDIDATE
#define CANDIDATE 1024
#endif

#ifndef LEN
#define LEN 256
#endif

#ifndef SMALL
#define SMALL -100
#endif


/*******************************************************/
/*                 Forward Declaration                 */
/*******************************************************/
void program_backoff(const string s, bool keyword_error);
/*******************************************************/
void viterbi_recursion(Ngram* lm, 
                       VocabMap map, 
                       Vocab* voc, 
                       Vocab* ZhuYin, 
                       Vocab* Big5, 
                       int count, 
                       VocabString* line, 
                       vector<vector<LogP>>* prob, 
                       vector<vector<VocabIndex>>* vidx_graph, 
                       vector<vector<int>>* backtrack, 
                       vector<int>* candi_num,
                       VocabIndex v_idx,
                       VocabIndex* empty_context,
                       VocabIndex* bi_context);
/*******************************************************/
void viterbi_decode(Vocab* Big5, 
                    int count, 
                    vector<vector<LogP>>* prob, 
                    vector<vector<VocabIndex>>* vidx_graph, 
                    vector<vector<int>>* backtrack, 
                    vector<int>* candi_num);
/*******************************************************/


/*********************************************/
/*           Language Model Decode           */
/*********************************************/
void lm_decode(Ngram* lm, File* text_file, VocabMap map, Vocab* voc, Vocab* ZhuYin, Vocab* Big5) {
    
    /******* Go through the text file *******/
    char* buf;
    while(buf = text_file->getline()) {


        /******* Parse to VocabString a.k.a char* *******/
        VocabString line[maxWordsPerLine];
        unsigned int count = Vocab::parseWords(buf, &(line[1]), maxWordsPerLine);
        line[0] = "<s>";
        line[count+1] = "</s>";
        count += 2;


        /******* Initialize variables *******/
        vector<vector<LogP>>            prob = vector<vector<LogP>>(LEN, vector<LogP>(CANDIDATE, 0.0));
        vector<vector<VocabIndex>>      vidx_graph = vector<vector<VocabIndex>>(LEN, vector<VocabIndex>(CANDIDATE, 0)); // Record the (i, j) VocalIndex of Big5
        vector<vector<int>>             backtrack = vector<vector<int>>(LEN, vector<int>(CANDIDATE, 0));
        vector<int>                     candi_num = vector<int>(LEN, 0);
        VocabIndex                      v_idx; // Be mapped by ZhuYin, which is a VocabIndex of Big5
        VocabIndex                      empty_context[] = {Vocab_None};
        VocabIndex                      bi_context[] = {Vocab_None, Vocab_None};


        /******* Viterbi recursion *******/
        viterbi_recursion(lm, map, voc, ZhuYin, Big5, count, line, 
                          &prob, &vidx_graph, &backtrack, &candi_num, 
                          v_idx, empty_context, bi_context);
        

        /******* Viterbi decoding *******/
        viterbi_decode(Big5, count, &prob, &vidx_graph, &backtrack, &candi_num);
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
    Vocab ZhuYin;
    Vocab Big5;


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
    lm_decode(&lm, &text_file, map, &voc, &ZhuYin, &Big5);
    text_file.close();

    return 0;
}


/***************************************/
/*           Program Backoff           */
/***************************************/
void program_backoff(const string s, bool keyword_error) {
    if (keyword_error) cerr << "Error >>> Invalid Argument: " << s << endl;
    else cerr << "Error >>> Invalid number of argument: " << s << endl;
    cerr << "Usage: ./mydisambig -text [testing file] -map [mapping file] -lm [language model] -order [order of ngram language model] > [output file path]\n" << endl;
    exit(1);
}


/*****************************************/
/*           Viterbi Recursion           */
/*****************************************/
void viterbi_recursion(Ngram* lm, 
                       VocabMap map, 
                       Vocab* voc, 
                       Vocab* ZhuYin, 
                       Vocab* Big5, 
                       int count, 
                       VocabString* line, 
                       vector<vector<LogP>>* prob, 
                       vector<vector<VocabIndex>>* vidx_graph, 
                       vector<vector<int>>* backtrack, 
                       vector<int>* candi_num,
                       VocabIndex v_idx,
                       VocabIndex* empty_context,
                       VocabIndex* bi_context) {
    

    /******* Initialize variable *******/
    Prob p;
    int size = 0;
    VocabMapIter iter(map, ZhuYin->getIndex(line[0]));
    iter.init();


    /******* Initialization for Viterbi *******/
    while (iter.next(v_idx, p)) {
        
        VocabIndex candi = voc->getIndex(Big5->getWord(v_idx));
        if (candi == Vocab_None) candi = voc->getIndex(Vocab_Unknown);  // if voc == none, mark as unknown rather than discard it

        LogP logp = lm->wordProb(candi, empty_context); //unigram for start <s>
        prob->at(0).at(size) = (logp == LogP_Zero)? SMALL: logp;
        vidx_graph->at(0).at(size) = v_idx;
        backtrack->at(0).at(size) = -1; // start: -1
        size++;
    }
    candi_num->at(0) = size;


    /******* Recursion for Viterbi *******/
    for (int i = 1; i < count; i++) {
        
        VocabMapIter iter(map, ZhuYin->getIndex(line[i]));
        iter.init();
        size = 0;

        while (iter.next(v_idx, p)) {
            VocabIndex candi = voc->getIndex(Big5->getWord(v_idx));
            if (candi == Vocab_None) candi = voc->getIndex(Vocab_Unknown);

            // See last column which has the highest probability
            LogP maxp = LogP_Zero;
            for (int j = 0; j < candi_num->at(i-1); j++) {
                VocabIndex last = voc->getIndex(Big5->getWord(vidx_graph->at(i-1).at(j)));
                last = (last == Vocab_None)? voc->getIndex(Vocab_Unknown): last;
                bi_context[0] = last;

                LogP logp = lm->wordProb(candi, bi_context);
                LogP backoff = lm->wordProb(candi, empty_context);  // Check backoff prob
                if (logp == LogP_Zero && backoff == LogP_Zero) logp = SMALL;

                logp += prob->at(i-1).at(j);
                if (logp > maxp) {
                    maxp = logp;
                    backtrack->at(i).at(size) = j;
                }
            }
            prob->at(i).at(size) = maxp;
            vidx_graph->at(i).at(size) = v_idx;
            size++;
        }

        if (size > CANDIDATE) cout << size << endl;
        candi_num->at(i) = size;
    }
}


/**************************************/
/*           Viterbi Decode           */
/**************************************/
void viterbi_decode(Vocab* Big5, 
                    int count, 
                    vector<vector<LogP>>* prob, 
                    vector<vector<VocabIndex>>* vidx_graph, 
                    vector<vector<int>>* backtrack, 
                    vector<int>* candi_num) {
    
    /******* Initialize variables *******/
    LogP maxp = LogP_Zero;
    int max_col = -1;
    VocabString decode_path[maxWordLength];
    decode_path[0] = "<s>";
    decode_path[count-1] = "</s>";


    /******* Find the max probability path *******/
    for (int i = 0; i < candi_num->at(count-1); i++) {
        if (prob->at(count-1).at(i) > maxp) {
            maxp = prob->at(count-1).at(i);
            max_col = i;
        }
    }
    for (int i = count-1; i > 0; i--) {
        decode_path[i] = Big5->getWord(vidx_graph->at(i).at(max_col));
        max_col = backtrack->at(i).at(max_col);
    }


    /******* Output answer path *******/
    for (int i = 0; i < count - 1; i++)
        cout << decode_path[i] << ' ';
    cout << decode_path[count - 1] << endl;
}

