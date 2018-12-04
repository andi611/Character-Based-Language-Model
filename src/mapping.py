# -*- coding: utf-8 -*- #
"""*********************************************************************************************"""
#   FileName     [ mapping.py ]
#   Synopsis     [ build zhu-yin to word mapping from its inverse ]
#   Author       [ Ting-Wei Liu (Andi611) ]
#   Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
#   Reference    [ https://github.com/orbxball/DSP/blob/master/hw3/mapping.py ]
"""*********************************************************************************************"""


###############
# IMPORTATION #
###############
import io
import argparse
import subprocess


##################
# CONFIGURATIONS #
##################
def get_config():
	parser = argparse.ArgumentParser(description='mapping argument parser')
	parser.add_argument('input', type=str, help='input file')
	parser.add_argument('output', type=str, help='output file')
	parser.add_argument('--output_utf8', type=str, default='./ZhuYin-Utf8.map', help='output file in utf-8 encoding')
	args = parser.parse_args()
	return args


########
# MAIN #
########
"""
	main function
"""
def main():
	args = get_config()
	zy2char = {}

	with open(args.input, 'r', encoding='BIG5-HKSCS') as f:
		for line in f:
			line = line.split(' ')
			char = line[0]
			ZhuYins = line[1].split('/')
			for ZhuYin in ZhuYins:
				if ZhuYin[0] in zy2char:
					zy2char[ZhuYin[0]].append(char)
				else:
					zy2char[ZhuYin[0]] = [char]
			zy2char[char] = [char]

	#---mapping in big-5 for SRILM---#
	with open(args.output, 'w', encoding='BIG5-HKSCS') as o:
		for key, value in sorted(zy2char.items()):
			o.write(key + '\t' + ' '.join(value) + '\n')

	#---mapping in utf-8 for viewing---#
	with open(args.output_utf8, 'w', encoding='utf-8') as o:
		for key, value in sorted(zy2char.items()):
			o.write(key + '\t' + ' '.join(value) + '\n')


if __name__ == '__main__':
	main()

