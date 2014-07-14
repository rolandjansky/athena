#!/usr/bin/env python

# @file:    showTrees.py
# @purpose: print information about the TTrees in the ROOT file given
# @author:  Gerhard Brandt <gbrandt@cern.ch>
# @date:    October 2009

__version__ = "$Revision: 227455 $"
__author__  = "Gerhard Brandt <gbrandt@cern.ch>"

import sys
import os

from optparse import OptionParser

import AthenaPython.PyAthena as PyAthena
mcobs = PyAthena.cobs()

if __name__ == "__main__":

	parser = OptionParser(usage="usage: %prog [-f] file.root")
	p = parser.add_option 
	p( "-f", 
	   "--file", 
	   dest = "fileName", 
	   help = "The path to the ROOT file to analyze" )
	p( "-b", 
	   "--branches", 
	   action  = "store_true",
	   dest = "show_branches",  
	   default = False,
	   help = "Print branches" )

	(options, args) = parser.parse_args()

	if len(args) > 0:    
		for arg in args:
			if arg[0] != "-":
				fileName=arg

	if options.fileName == None and len(fileName) == 0:
		str(parser.print_help() or "")
		sys.exit(1)
	
	if options.show_branches != None:
		mcobs.setShowBranches(True)
	
	mcobs.printTrees(fileName)      

#       import AthenaPython.PyAthena as PyAthena
#       print "Optimizing file: ", fileName
#       print "Output file:     ", ofileName
#       mcobs = PyAthena.cobs(fileName, ofileName)
#
#       if options.skip != None:
#               trees=options.skip.split(",")
#               for t in trees:
#                       print "Skipping tree:   ", t
#                       mcobs.setTreeToSkip(t)
#       
#       mcobs.order(options.orderingMode)
