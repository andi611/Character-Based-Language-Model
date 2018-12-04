#!/bin/bash
#*********************************************************************************************#
#   FileName     [ run_disambig.sh ]
#   Synopsis     [ script to run the disambig command over all test data ]
#   Author       [ Ting-Wei Liu (Andi611) ]
#   Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
#*********************************************************************************************#
END=9
for i in $(seq 1 $END);
do
    /home/andi611/dsp/srilm-1.5.10/bin/i686-m64/disambig -text ./testdata/seg_"${i}".txt -map ./ZhuYin-Big5.map -lm ./bigram.lm -order 2 > ./result1/seg_"${i}"_ans.txt;
done