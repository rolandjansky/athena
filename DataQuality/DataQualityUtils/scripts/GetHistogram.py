#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# +---------------------------------------------+
# | Vassilis D. Kazazakis, GetHistogram.py v1.0	|
# | Extracts histograms from ROOT files		|
# |  Interface used by QuerryDB.py application	|
# +---------------------------------------------+

import DataQualityUtils.DQGetHistogramMod as mod


def usage():
	print "Usage:", sys.argv[0] , "[options]"
	print """
	-i/--input filename		 Input ROOT file
	-o/--output filename		 Output ROOT file
	-n/--name histogram		 Name of the histogram to extract
	-d/--directory directory[:flag]	 Name of the directory from which to extract all[or flagged] histograms
	-f/--flag flag			 Flag of the histograms to extract (shift/expert/etc)
	"""


if __name__ == "__main__":
	input = output = None
	nameList = []
	try:
		optlist, unparsed = getopt.getopt(sys.argv[1:],
					"i:o:n:d:f:",
					["input=", "output=", "name=", "directory=", "flag="])
	except getopt.GetoptError:
		usage()
		sys.exit(-1)

	if len(optlist) == 0:
		usage()
		sys.exit(0)

	if len(unparsed) != 0:
		print "Extra arguments", unparsed, "not used."

	for opt, arg in optlist:
		if opt in ("-i", "--input"):
			input = arg
		elif opt in ("-o", "--output"):
			output = arg
		elif opt in ("-n", "--name"):
			nameList.append(arg)
		elif opt in ("-d", "--directory"):
			tmpList = arg.split(":")
			if len(tmpList) < 2:
				tmpList.append("")
			if not tmpList[0].endswith('/'):
				if tmpList[1] != "":
					tmpList[1] = ":" + tmpList[1]
				arg = tmpList[0] + "/" + tmpList[1]
			nameList.append(arg)
		elif opt in ("-f", "--flag"):
			nameList.append(":" + arg)

	if None in (input, output):
		print >>sys.stderr, "Error: Must define input/output files."
		usage()
		sys.exit(-1)
	if len(nameList) == 0:
		print >>sys.stderr, "Error: Must give at least one histogram or directory name."
		usage()
		sys.exit(-1)
	mod.GetHistogram(input, output, nameList)
