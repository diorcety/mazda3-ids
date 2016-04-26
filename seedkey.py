#!/usr/bin/env python

from __future__ import print_function

import sys

# Fix Python 2.x.
try: input = raw_input
except NameError: input = input

_author__ = "Yann Diorcet"
__license__ = "GPL"
__version__ = "0.0.1"

initialValue=0x00C541A9
v1=0x00109028
v2=0xFFEF6FD7

def calculateKey(vehicle_seed, session_seed):
	assert len(session_seed) == 3
	assert len(vehicle_seed) == 5

	challenge = bytearray(8)
	challenge[0:3] = session_seed[0:3]
	challenge[3:8] = vehicle_seed[0:5]
	buff = initialValue

	for b in challenge:
		for j in range(0, 8):
			tempBuffer = 0
			if (b ^ buff) & 0x1:
				buff = buff | 0x1000000
				tempBuffer = v1
			b = b >> 1
			tempBuffer = tempBuffer ^ (buff >> 1)
			tempBuffer = tempBuffer & (v1)
			tempBuffer = tempBuffer | (v2 & (buff >>1))
			buff = tempBuffer & 0xffffff

	return bytearray([(buff >> 4 & 0xff), ((buff >> 20) & 0x0f) + ((buff >> 8) & 0xf0), ((buff << 4) & 0xff) + ((buff >>16) & 0x0f)])


####################
####################
####################

def main(argv):
	str_vs = input("Enter the vehicule seed: ")
	vs = bytearray.fromhex(str_vs)
	if not isinstance(vs, bytearray) or len(vs) != 5:
		raise ValueError("The vehicule seed must be a array of 5 bytes")
	str_ss = input("Enter the session seed: ")
	ss = bytearray.fromhex(str_ss)
	if not isinstance(ss, bytearray) or len(ss) != 3:
		raise ValueError("The session seed must be a array of 3 bytes")
	key = calculateKey(vs, ss)
	print("The session key: %s" % ("".join(["%02X" % x for x in key])))

if __name__ == "__main__":
	main(sys.argv)

