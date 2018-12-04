#!/bin/bash
#*********************************************************************************************#
#   FileName     [ run_separator.sh ]
#   Synopsis     [ script to run the separator command over all test data ]
#   Author       [ Ting-Wei Liu (Andi611) ]
#   Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
#*********************************************************************************************#
END=9
for i in $(seq 1 $END);
do
    perl separator_big5.pl testdata/"${i}".txt > testdata/seg_"${i}".txt;
done
