# Digital Signal Processing: Character-Based Language Model


## Introduction
- A character-based language model build with the **SRILM toolkit**,
- Plus, a viterbi-based decoding process of the language model **implemented in C++**.
- Given ZhuYin-mixed sequences obtained from an imperfect acoustic models with phoneme loss, reconstruct and decode the correct sentence using a character-based language model, this language model can be construct with the SRILM toolkit or this C++ implementation.
- Goal:
    - Given ZhuYin-mixed sequences: 
    	- 讓 他 十分 ㄏ怕 / 只 ㄒ望 ㄗ己 明ㄋ 度 別 再 這ㄇ ㄎ命 了
    - Reconstruct correct sentence: 
    	- 讓 他 十分 害怕 / 只 希望 自己 明年 度 別 再 這麼 苦命 了


## Environment
* **< Ubuntu 6.4.0-17 >**    
    * Computer Architecture: x86_64
    * CPU op-mode(s): 32-bit, 64-bit           
* **< [SRILM 1.5.10](http://www.speech.sri.com/projects/srilm/) >** 
	- [How to Install SRILM](#Setup)
* **< g++ [gcc version 8.2.0 (GCC)] >** (Tested)
* **< g++ [gcc version 6.4.0 (GCC)] >** (Tested)
* **< g++ [gcc version 4.2.1 (GCC)] >** (Tested)


## File Description
```
.
├── src/
|   ├── Makefile -------------> g++ compiler make file
|   ├── corpus.txt -----------> Training corpus in big5 encoding
|   ├── Big5-ZhuYin.map ------> character to Zhu-Yin mapping in big5 encoding
|   ├── mapping.py -----------> Creates Zhu-Yin to char mapping from its inverse mapping
|   ├── mydisambig.cpp -------> My implementation of a viterbi-based decoding process of the language model
|   ├── separator_big5.pl ----> Separate words into characters with white space inserted in between each character
|   └── testdata/ ------------> testing data 1.txt ~ 5.txt are the easy ones, 6.txt ~ 10.txt are the hard ones
├── image/
├── srilm-1.5.10.tar.gz ------> SRILM binary source code
├── problem_description.pdf --> Work spec
└── Readme.md ----------------> This file
```


## Usage
- **Compile code**:
```
make all
```
- **Separate training and testing data into separate characters**:
```
make separate
```
- **Build Zhu-Yin to char mapping**:
```
make map
```
- This generates 2 files: I) ZhuYin-Big5.map, and II) ZhuYin-Utf8.map where:
```
I) ZhuYin-Big5.map: the Zhu-Yin to Chinese character mapping in big5 encoding
II) ZhuYin-Utf8.map: the Zhu-Yin to Chinese character mapping in utf-8 encoding for user verification in ordinary linux environment
```
- **Build language model**:
```
make build_lm
```
- **Decode with SRILM disambig**:
```
make run_disambig
```
- **Decode with MY disambig**:
```
make run
```
- **Decode with MY disambig** but show output on screen instead of write to file:
```
make run_cout
```
- **Clean executables**:
```
make clean
```
- **Clean everything generated in the above steps**:
```
make cleanest
```
- The variables `SRIPATH` and `MACHINE_TYPE` can be specified by the user through the make command:
```
make MACHINE_TYPE=i686-m64 SRIPATH=/home/user/srilm-1.5.10 all
make MACHINE_TYPE=i686-m64 SRIPATH=/home/user/srilm-1.5.10 separate
make MACHINE_TYPE=i686-m64 SRIPATH=/home/user/srilm-1.5.10 map
make MACHINE_TYPE=i686-m64 SRIPATH=/home/user/srilm-1.5.10 build_lm
make MACHINE_TYPE=i686-m64 SRIPATH=/home/user/srilm-1.5.10 run_disambig
make MACHINE_TYPE=i686-m64 SRIPATH=/home/user/srilm-1.5.10 run
```
- Default settings of `SRIPATH` and `MACHINE_TYPE` are:
- <img src=https://github.com/andi611/Character-Based-Language-Model/blob/master/image/variables.png width="300" height="31">
- `SRIPATH` can be obtained by running the command `$ pwd` under the `srilm-1.5.10/` directory.
- `MACHINE_TYPE` can be verified through the command: `$ lscpu`


## <a name="Setup"></a>Environment Setup

### Install dependencies
- Install **csh** if not already installed: `$ sudo apt-get install csh`
- Install **gawk** if not already installed: `$ sudo apt-get install gawk`

### Compile SRILM from binary
- The following instructions are for a **Ubuntu 64 bit** machine.
- <img src=https://github.com/andi611/Character-Based-Language-Model/blob/master/image/lscpu.png width="506" height="426">
- Use the SRILM source code provided in this repo, or download it [here](http://speech.ee.ntu.edu.tw/homework/DSP_HW3/srilm-1.5.10.tar.gz).
- Untar the source code package: `$ tar zxvf srilm-1.5.10.tar.gz`
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
- **Comment out lines 14 ~ 29 in `srilm-1.5.10/lm/src/matherr.c` since glibc 2.27 has removed struct exception.**
- Make sure that all the programs under `srilm-1.5.10/sbin` are executable, if not: `$ sudo chmod 755 *`
- Compile: `$ sudo make World`
- Clean up: `$ make cleanest`
- The compiled executable files should be in `srilm-1.5.10/bin/i686-m64`

### Other Environment Setup Issues
- Refer to the following links for further environment issues:
    - SRILM compilation problem:
        - [FQA 1](http://speech.ee.ntu.edu.tw/DSP2018Autumn/hw3/faq.html)
        - [FQA 2](http://speech.ee.ntu.edu.tw/DSP2018Autumn/hw3/faq.html)
        - [Guide](http://www.52nlp.cn/ubuntu-64-bit-system-srilm-configuration)
    - Encoding problem:
        - [Sublime Text with BIG5 encoding](https://ephrain.net/sublime-text-%E8%AE%93-sublime-text-%E6%94%AF%E6%8F%B4-big5-%E7%B7%A8%E7%A2%BC%E7%9A%84%E6%96%87%E5%AD%97%E6%AA%94/)

