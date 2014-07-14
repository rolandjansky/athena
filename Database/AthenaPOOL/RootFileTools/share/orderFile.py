#!/usr/bin/env python

# @file:    orderFile.py
# @purpose: optimizes ordering in trees of root file and creates a new file with a new compression level 
# @author:  Ilija Vukotic <ilija@vukotic.me>
# @date:    October 2009

###########################################################################################################
# example:
# for standard AOD file this would be a reasonable command line:
# orderFile -o ofilename ifilename
#
###########################################################################################################

__version__ = "$Revision: 226925 $"
__author__  = "Ilija Vukotic <ilija@vukotic.me>"

import sys
import os

from optparse import OptionParser


import AthenaPython.PyAthena as PyAthena
trans = PyAthena.cobs('/data/ilija/AOD.067184.big.pool.root','/data/ilija/ordered.pool.root')

if __name__ == "__main__":

	parser = OptionParser(usage="usage: %prog [options] my.file.pool.root")
	p = parser.add_option 
	p( "-f", "--file", dest = "fileName", help = "The path to the POOL file to analyze" )
	p( "-m", "--mode", dest = "orderingMode", default = 2, help = "ordering mode 1 - by Offset, 2 - byEntry, 3 - byBranch " )
	p( "-o", "--output", dest = "outFileName", default = None, help = "Name of the output file. If ommited output file name will be the same as input one with _ordered appended." )
	p( "-s", "--skip", dest="skip", default = None, help ="List of tree names to leave as is. Compression level is still changed.")
	(options, args) = parser.parse_args()

	fileName=""
	if len(args) > 0:    
		for arg in args:
			if arg[0] != "-":
				fileName=arg

	if options.fileName == None and len(fileName) == 0:
		str(parser.print_help() or "")
		sys.exit(1)
	
	ofileName=""	
	if options.outFileName == None:
		ofileName=fileName + "_ordered"
	else:
		ofileName=options.outFileName

	import AthenaPython.PyAthena as PyAthena
	print "Optimizing file: ", fileName
	print "Output file:     ", ofileName
	trans = PyAthena.cobs(fileName, ofileName)

	if options.skip != None:
		trees=options.skip.split(",")
		for t in trees:
			print "Skipping tree:   ", t
			trans.setTreeToSkip(t)
	if options.orderingMode==1:
		print "Ordering mode: by OFFSET"
	if options.orderingMode==2:
		print "Ordering mode: by EVENT"
	if options.orderingMode==1:
		print "Ordering mode: by BRANCH"
		
	trans.order(options.orderingMode)