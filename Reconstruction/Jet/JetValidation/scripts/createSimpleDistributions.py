# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# External dependencies
import ROOT
import sys
import argparse
import os.path

# Control switch
IsDebug = False

##################################################
# Read and store argument values                 #
##################################################

parser = argparse.ArgumentParser(description='Create distributions for many variables and save them in a ROT file')

parser.add_argument('inputFile', type=str)
parser.add_argument('outputFile', type=str, default="")
parser.add_argument('--treeName', type=str, default="physics")
parser.add_argument('--binDic', type=str, default="")
parser.add_argument('-b' , '--saveBinning',  action='store_true') 
parser.add_argument('-pdf' , '--savePDF',  action='store_true')
parser.add_argument('--branchPattern', type=str, default=["jet_","MET_"], nargs='*')
parser.add_argument('--histoType', type=str, default="leadingJetSel")

args = parser.parse_args()




inFileList = args.inputFile.split(',')

print "inFileList ", inFileList
print 'outputFile', args.outputFile

inChain = ROOT.TChain(args.treeName)

for f in inFileList:
    inChain.AddFile(f)
tree = inChain


##################################################
# Dump everything                                #
##################################################

from JetValidation.D3PDHistoBuildLib import HistoBuilderFromD3PD,isSupportedType, JetType

builder = HistoBuilderFromD3PD(tree)

patterns = args.branchPattern

def addAllHistos( aBranch ):
    if isSupportedType(tree.GetBranchType(aBranch)):
        builder.addHistos(aBranch)
def addleadinJetSelectorHistos( aBranch ):
    if ("LC" in aBranch or "EM"  in aBranch) and isSupportedType(tree.GetBranchType(aBranch)):
        builder.addHistosWithLeadingJetSelectors( JetType(aBranch), aBranch )

if args.histoType == "leadingJetSel":    
    histoAdder = addleadinJetSelectorHistos
else:
    histoAdder = addAllHistos
    
print "Adding branches..."
for aBranch in sorted(tree.GetSetOfTreeBranchNames()):
    
    if any( aBranch.startswith( p ) for p in patterns):
        histoAdder( aBranch)
        
if args.binDic :
    if os.path.exists(args.binDic):
        builder.readHistoBins( args.binDic )
    else:
        print "ERROR : requested to read binning from non existent file : ", args.binDic
        exit(2)

        
print "\nRunning event loop..."
builder.eventLoop()
print "Done event loop!"

## #builder.dumpAllHistosToFile(pdfName)
builder.saveHistos(args.outputFile)
if args.savePDF:
    print "Building a structured pdf..."
    builder.dumpStructuredPDF(args.outputFile.replace('.root','.pdf'))
if args.saveBinning:
    print "Saving histo binning..."
    builder.saveHistoBins(args.outputFile.replace('.root','.pck'))

#inChain.Close()


