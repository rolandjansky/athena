#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file CostAnalysisPostProcessing.py
@brief Script to consume merged cost histograms from the CostAnalysis and
    produce structured CSV output.
'''

import ROOT
from TrigCostAnalysis.Util import exploreTree
from TrigCostAnalysis.CostMetadataUtil import saveMetadata
from AthenaCommon.Logging import logging
log = logging.getLogger('CostAnalysisPostProcessing')

def main():
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--file', default='TrigCostRoot_Results.root', help='Input ROOT file to generate output from')
    parser.add_argument('--userDetails', help='User supplied metadata string giving any extra details about this run.')
    parser.add_argument('--underflowThreshold', default=0.5, help='Threshold of underflow percent value to save warning in metadata tree.')
    parser.add_argument('--overflowThreshold', default=0.1, help='Threshold of underflow percent value to save warning in metadata tree.')
    parser.add_argument('--dumpAlgorithmSummary', action='store_true', help='Print algorithm\'s mean time of execution to the log file')     
    args = parser.parse_args()
    
    inputFile = ROOT.TFile(args.file, 'READ')

    if inputFile.IsOpen():
        warningMsg = exploreTree(inputFile, args.dumpAlgorithmSummary, args.underflowThreshold, args.overflowThreshold)
        saveMetadata(inputFile, args.userDetails, warningMsg)
    else:
        log.error("File %s not found", args.file)

    # to speed up closing the ROOT file
    ROOT.gROOT.GetListOfFiles().Remove(inputFile)
    inputFile.Close()


if __name__== "__main__":
    main()
