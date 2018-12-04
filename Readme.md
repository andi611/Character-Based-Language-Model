# Digital Signal Processing: Character-Based Language Model


## Introduction
- A character-based language model build with the **SRILM toolkit**,
- Plus, a viterbi-based decoding process of the language model **implemented in C++**.
- Given ZhuYin-mixed sequences obtained from an imperfect acoustic models with phoneme loss, reconstruct and decode the correct sentence using a character-based language model, this language model can be construct with the SRILM toolkit or this C++ implementation.
- Goal:
    - Given ZhuYin-mixed sequences: 讓 他 十分 ㄏ怕 / 只 ㄒ望 ㄗ己 明ㄋ 度 別 再 這ㄇ ㄎ命 了
    - Reconstruct correct sentence: 讓 他 十分 害怕 / 只 希望 自己 明年 度 別 再 這麼 苦命 了


## Environment
* **< Ubuntu 6.4.0-17 >**    
    * Computer Architecture: x86_64
    * CPU op-mode(s): 32-bit, 64-bit           
* **< [SRILM 1.5.10](http://www.speech.sri.com/projects/srilm/) >**
* **< g++ [gcc version 8.2.0 (GCC)] >** (Tested)
* **< g++ [gcc version 6.4.0 (GCC)] >** (Tested)
* **< g++ [gcc version 4.2.1 (GCC)] >** (Tested)
 

## Environment Setup

### Install dependencies
- Install **csh** if not already installed: `$ sudo apt-get install csh`
- Install **gawk** if not already installed: `$ sudo apt-get install gawk`

### Compile SRILM from binary
- The following instructions are for a **Ubuntu 64 bit** machine.
- <img src=https://github.com/andi611/Character-Based-Language-Model/blob/master/image/lscpu.png width="506" height="426">
- Use the SRILM source code provided, or download it [here](http://speech.ee.ntu.edu.tw/homework/DSP_HW3/srilm-1.5.10.tar.gz).
- Untar the source code package: `$ tar zxvf srilm-1.5.10.tar.gz`
- **The resulting `srilm-1.5.10/` directory should be under the same directory as `src`**.
- Enter the resulting SRILM directory: `$ cd srilm-1.5.10/`
- Get the absolute path to the `srilm-1.5.10/` directory: `$ pwd`
- Modify `srilm-1.5.10/Makefile` and change the SRILM variable to the absolute path of `srilm-1.5.10/`, and change the MACHINE_TYPE variable to match the 64-bit Ubuntu architecture:
```
# SRILM = /home/speech/stolcke/project/srilm/devel
SRILM = /home/andi611/dsp/srilm-1.5.10
# MACHINE_TYPE := $(shell $(SRILM)/sbin/machine-type)
MACHINE_TYPE := i686-m64
```
- Modify the following lines in `srilm-1.5.10/common/Makefile.machine.i686-m64` to:
```
- line 17: 
    CC = /usr/bin/gcc $(GCC_FLAGS)
- line 18: 
    CXX = /usr/bin/g++ $(GCC_FLAGS) -DINSTANTIATE_TEMPLATES
- Line 52, 53, 54 (Lines under # Tcl support):    
    TCL_INCLUDE =
    TCL_LIBRARY =
    NO_TCL = X
- Line 69: 
    GAWK = /usr/bin/gawk
```
- Comment out line 14 ~ 29 in `srilm-1.5.10/lm/src/matherr.c` since glibc 2.27 has removed struct exception.
- Make sure that all the programs under `srilm-1.5.10/sbin` are executable, if not: `$ sudo chmod 755 *`
- Compile: `$ sudo make World`
- Clean up: `$ make cleanest`
- Check that there are executable files in `srilm-1.5.10/bin/i686-m64`

### My C++ version of disambig

### Other solutions
- Refer to the following links for further environment issues:
    - SRILM compilation problem:
        - [FQA 1](http://speech.ee.ntu.edu.tw/DSP2018Autumn/hw3/faq.html)
        - [FQA 2](http://speech.ee.ntu.edu.tw/DSP2018Autumn/hw3/faq.html)
        - [Guide](http://www.52nlp.cn/ubuntu-64-bit-system-srilm-configuration)
    - Encoding problem:
        - [Sublime Text with BIG5 encoding](https://ephrain.net/sublime-text-%E8%AE%93-sublime-text-%E6%94%AF%E6%8F%B4-big5-%E7%B7%A8%E7%A2%BC%E7%9A%84%E6%96%87%E5%AD%97%E6%AA%94/)


## Usage
- Separate training and testing data into separate characters:
```
make separate
```
- **Build Zhu-Yin to char mapping**:
```
make map
```
- This generates 2 files: I) ZhuYin-Big5.map, and II) ZhuYin-Utf8.map where:
```
I) ZhuYin-Big5.map: the Zhu-Yin to Chinease character mapping in big5 encoding
II) ZhuYin-Utf8.map: the Zhu-Yin to Chinease character mapping in big5 encoding for user verification in ordinary linux environment
```
- **Get counts** with:
```
/home/andi611/dsp/srilm-1.5.10/bin/i686-m64/ngram-count -text ./corpus_seg.txt -write ./lm.cnt -order 2
```
- **Compute probability**:
```
/home/andi611/dsp/srilm-1.5.10/bin/i686-m64/ngram-count -read ./lm.cnt -lm ./bigram.lm -unk -order 2
```
- **Decode with SRILM disambig**: decode all 9 testing data one by one, OR
```
/home/andi611/dsp/srilm-1.5.10/bin/i686-m64/disambig -text ./testdata/seg_x.txt -map ./ZhuYin-Big5.map -lm ./bigram.lm -order 2 > ./result1/seg_x_ans.txt
```
- **Decode with SRILM disambig**: decode all 9 testing data with script
```
./run_disambig.sh
```




## TBC...working
 ===== FILES =====
corpus.txt是此次作業的語料檔案，編碼為big5
Big5-ZhuYin.map是文字跟注音的對照表，編碼為big5
separator_big5.pl將文字之間安插空白
Submit_Files 是繳交作業的資料結構
separator_big5.pl執行方法為

  ./separator_big5.pl < textfile > output

(使用前先執行chmod 755 separator_big5.pl)

===== TEST =====
測資放在testdata資料夾中
共有十個測資
1~5是比較簡單一點的測資
6~10是稍微難一點的測資


## File Description
```
.
├── src/
|   ├── Makefile                g++ compiler make file
|   ├── hmm.h                   HMM implementation
|   ├── hmm.cpp                 HMM implementation
|   ├── test_hmm.cpp            Testing algorithm implementation
|   ├── train_hmm.cpp           Training algorithm implementation
|   ├── test                    Unix executable binary code for test_hmm.cpp  (See the next "Usage" section for more details)
|   ├── train                   Unix executable binary code for train_hmm.cpp (See the next "Usage" section for more details)
|   ├── plot.py                 Draws the training plot
|   ├── model_01~05.txt         Trained models
|   └── modellist.txt           Model name list
├── data/
|   ├── model_init.txt          Initial model for training
|   ├── seq_model_01~05.txt     Training data (observation sequences)
|   ├── testing_data1.txt       Testing data (observation sequences)
|   ├── testing_answer.txt      Real answer for "testing_data1.txt"
|   ├── testing_result.txt      Model generated answer for "testing_data1.txt"
|   └── testing_data2.txt       Testing data without answer
├──problem_description.pdf      Work Spec
└── Readme.md                   This File
```