# -*- coding: utf-8 -*- #
#*********************************************************************************************#
#   FileName     [ Makefile ]
#   Synopsis     [ make file that runs everything ]
#   Author       [ Ting-Wei Liu (Andi611) ]
#   Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
#   Reference    [ https://github.com/orbxball/DSP/blob/master/hw3/Makefile ]
#*********************************************************************************************#


SRIPATH ?= /home/andi611/dsp/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w --std=c++11
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
TO2 = ZhuYin-Utf8.map
FROM = Big5-ZhuYin.map
.PHONY: all separate map build_lm run_disambig run run_cout diff clean cleanest

RESULT_1_DIR=result1
TESTDATA=testdata
TESTDATA_ORI=origin_data
RESULT_2_DIR=result2

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

separate:
	perl separator_big5.pl corpus.txt > seg_corpus.txt
	mv $(TESTDATA) $(TESTDATA_ORI);
	mkdir $(TESTDATA)
	for i in $(shell seq 1 10); do \
		perl separator_big5.pl $(TESTDATA_ORI)/$${i}.txt > $(TESTDATA)/$${i}.txt; \
	done

map:
	echo "Mapping!"
	python3 mapping.py $(FROM) $(TO) --output_utf8 $(TO2)

build_lm:
	$(SRIPATH)/bin/$(MACHINE_TYPE)/ngram-count -text seg_corpus.txt -write lm.cnt -order 2
	$(SRIPATH)/bin/$(MACHINE_TYPE)/ngram-count -read lm.cnt -lm bigram.lm -unk -order 2

run_disambig: 
	[ -d $(RESULT_1_DIR) ] || mkdir -p $(RESULT_1_DIR);
	@for i in $(shell seq 1 10); do \
		$(SRIPATH)/bin/$(MACHINE_TYPE)/disambig -text $(TESTDATA)/$${i}.txt -map $(TO) -lm $(LM) -order 2 > $(RESULT_1_DIR)/$${i}.txt; \
	done;

run:
	[ -d $(RESULT_2_DIR) ] || mkdir -p $(RESULT_2_DIR);
	@for i in $(shell seq 1 10) ; do \
			echo "Running $$i.txt"; \
			./mydisambig -text $(TESTDATA)/$${i}.txt -map $(TO) -lm $(LM) -order 2 > result2/$${i}.txt; \
		done;

run_cout:
	[ -d $(RESULT_2_DIR) ] || mkdir -p $(RESULT_2_DIR);
	@for i in $(shell seq 1 10) ; do \
			echo "Running $$i.txt"; \
			./mydisambig -text $(TESTDATA)/$${i}.txt -map $(TO) -lm $(LM) -order 2; \
		done;
		  # ./mydisambig –text testdata/seg_1.txt –map ZhuYin-Big5.map –lm bigram.lm –order 2 > result2/1.txt

diff:
	@for i in $(shell seq 1 10); do \
		diff $(RESULT_1_DIR)/$${i}.txt $(RESULT_2_DIR)/$${i}.txt; \
	done;

clean:
	$(RM) $(OBJ) $(TARGET)

cleanest:
	$(RM) -r $(OBJ) $(TARGET) $(RESULT_2_DIR) $(RESULT_1_DIR) $(TO) $(TO2) seg_corpus.txt lm.cnt bigram.lm
	if [ -d "$(TESTDATA_ORI)" ]; then \
		rm -rf ./$(TESTDATA); \
	fi
	if [ -d "$(TESTDATA_ORI)" ]; then \
		mv $(TESTDATA_ORI) $(TESTDATA); \
	fi

