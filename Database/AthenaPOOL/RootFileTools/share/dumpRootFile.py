#!/usr/bin/env python

# @file:    dumpRootFile.py
# @purpose: dump all contents of the ROOT file given
# @author:  Gerhard Brandt <gbrandt@cern.ch>
# @date:    October 2009

__version__ = "$Revision: 227455 $"
__author__  = "Gerhard Brandt <gbrandt@cern.ch>"

import sys
import os

from optparse import OptionParser

import AthenaPython.PyAthena as PyAthena

if __name__ == "__main__":

	parser = OptionParser(usage="usage: %prog [-f] file.root")
	p = parser.add_option 
	p( "-f", 
	   "--file", 
	   dest = "fileName", 
	   help = "The path to the ROOT file to analyze" )
	p( "-c", 
	   "--show-containers", 
	   action  = "store_true",
	   dest = "show_branches",  
	   default = False,
	   help = "Show containing objects" )

	(options, args) = parser.parse_args()

	if len(args) > 0:    
		for arg in args:
			if arg[0] != "-":
				fileName=arg

	if options.fileName == None and len(fileName) == 0:
		str(parser.print_help() or "")
		sys.exit(1)

	dumpall = PyAthena.DumpAll(fileName)
	
#	if options.show_containers != None:
#		dumpall.SetShowContainers(True)
	
	dumpall.SetVerbose(2)
	dumpall.SetMaxEntries(1)
	#dumpall.SetBranchFilter("Mc"); //"MuonClusterCollection");
	dumpall.SetShowContainers(1)
	dumpall.SetMaxArrayObj(10)
	dumpall.SetMaxArraySdt(10)
	dumpall.SetInspect(1)
	dumpall.SetFollowPointers(1)
	dumpall.DumpTrees()

