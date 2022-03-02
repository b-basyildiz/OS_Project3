#!/usr/bin/env python3
import random
import string
import itertools
from argparse import ArgumentParser

parser = ArgumentParser(description="Profile pzip execution time")
parser.add_argument('nOccurence', type=int, help="Number of threads")
res = parser.parse_args()

first_random = random.randint(0,255)
random_letter=''

while res.nOccurence > 0:
	if res.nOccurence > 255:
		random_letter+=random.choice(string.ascii_lowercase) * first_random
		res.nOccurence-=first_random
	else:
		random_letter+=random.choice(string.ascii_lowercase) * res.nOccurence
		res.nOccurence-=res.nOccurence
print(random_letter,  end = '')
