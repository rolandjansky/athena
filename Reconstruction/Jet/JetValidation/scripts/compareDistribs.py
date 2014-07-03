# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# External dependencies
import ROOT
import sys
import os.path

inFileList = sys.argv[1:]
if len(inFileList)<2:
    print "Need more than one input file"
    exit(2)


from JetValidation.D3PDHistoBuildLib import HistoComparatorFromFiles

comp = HistoComparatorFromFiles(*inFileList)


#comp.writeAllHistoToFile("histoCompare.pdf", logFile="histoCompare.log")
comp.compareToStructuredPDF("histoCompare.pdf")
