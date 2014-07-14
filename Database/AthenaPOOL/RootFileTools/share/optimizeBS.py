#!/usr/bin/env python
 
# @file:    optimizeBS.py
# @purpose: optimizes basket sizes in trees of root file and creates a new file with a new compression level 
# @author:  Ilija Vukotic <ilija@vukotic.me>
# @date:    October 2009

###########################################################################################################
# example:
# for standard AOD file this would be a reasonable command line:
# optimizeBS -s '##Links,##Shapes,##Params' -t 'POOLContainer_DataHeader=10240' -o ofilename ifilename
#
###########################################################################################################

__version__ = "$Revision: 226925 $"
__author__  = "Ilija Vukotic <ilija@vukotic.me>"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

	parser = OptionParser(usage="usage: %prog [options] my.file.pool.root")
	p = parser.add_option 
	p( "-f", "--file", dest = "fileName", help = "The path to the POOL file to analyze" )
	p( "-o", "--output", dest = "outFileName", default = None, help = "Name of the output file. If ommited output file name will be the same as input one with _optBS appended." )
	p( "-m", "--memory", dest = "Memory", default = None, help = "Memory to be used for all the baskets for all the trees for which it is not explicitly set using option -t. Given in kb." )
	p( "-t", "--treemem", dest = "treeMemList", default = None, help = "Comma separated list of form 'treename=memory,treename=memory'. Memory is given in kb." )
	p( "-c", "--compression", dest = "compressionF", default = 7, help = "Compression factor of the output file. 7 by default.")
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
		ofileName=fileName + "_optBS"
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
			
	if options.compressionF != None:
		trans.setCompressionLevel(options.compressionF)
		print 'Compression factor:', options.compressionF
	else:
		print 'Compression factor: 7'
		
	if options.Memory != None:
		trans.setDedicatedMemory(options.Memory)
		print 'Memory per tree:', options.Memory,'kb'
	else:
		print 'Memory per tree: 30720 kb'
		
	if options.treeMemList != None:
		pairs = options.treeMemList.split(",")
		for p in pairs:
			elem = p.split('=')
			trans.setTreeMemory(int (elem[1]),elem[0])
			print  'Memory for tree: ', elem[0], ' set to', int(elem[1])

	trans.resizeBaskets()
