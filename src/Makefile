# -*- coding: utf-8 -*- #
#*********************************************************************************************#
#   FileName     [ template.py ]
#   Synopsis     [ code template ]
#   Author       [ Ting-Wei Liu (Andi611) ]
#   Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
#   Reference    [ https://github.com/orbxball/DSP/blob/master/hw3/Makefile ]
#*********************************************************************************************#


SRIPATH ?= /home/andi611/dsp/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

TEST1DIR=result1
TESTDATA=testdata
ANSDIR=result2

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

diff:
	@for i in $(shell seq 1 10); do \
		diff $(TEST1DIR)/$${i}.txt $(ANSDIR)/$${i}.txt; \
	done;

run:
	[ -d $(ANSDIR) ] || mkdir -p $(ANSDIR);
	@for i in $(shell seq 1 10) ; do \
            echo "Running $$i.txt"; \
            ./mydisambig -text $(TESTDATA)/seg_$${i}.txt -map $(TO) -lm $(LM) -order 2 > result2/$${i}.txt; \
        done;

map:
	echo "Mapping!"
	python3 mapping.py $(FROM) $(TO)


build_lm:
	$(SRIPATH)/bin/i686-m64/ngram-count -text seg_corpus.txt -write lm.cnt -order 2
	$(SRIPATH)/bin/i686-m64/ngram-count -read lm.cnt -lm bigram.lm -unk -order 2

run_disambig: 
	[ -d $(TEST1DIR) ] || mkdir -p $(TEST1DIR);
	for i in $(shell seq 1 10); do \
		$(SRIPATH)/bin/i686-m64/disambig -text $(TESTDATA)/seg_$${i}.txt -map $(TO) -lm $(LM) -order 2 > $(TEST1DIR)/$${i}.txt; \
	done;

separate:
	perl separator_big5.pl corpus.txt > seg_corpus.txt
	for i in $(shell seq 1 10); do \
		perl separator_big5.pl $(TESTDATA)/$${i}.txt > $(TESTDATA)/seg_$${i}.txt; \
	done

clean:
	$(RM) $(OBJ) $(TARGET)

cleanest:
	$(RM) -r $(OBJ) $(TARGET) $(ANSDIR) $(TO) seg_corpus.txt lm.cnt 