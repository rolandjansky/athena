# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# Dictinary below last updated to match inputs from Bogdan and Meghan
# sent Jan 30 2016

# Dictionary to translate from Bogan's names to provider names
SystematicNameDictionary = {
                            'gammajetdphi'          : 'Gjet_dPhi',
                            'gammajetgenerator'     : 'Gjet_Generator',
                            'gammajetjvt'	    : 'Gjet_JVT',
                            'gammajetoutofcone'     : 'Gjet_OOC',
#                            'gammajetphscalemat'    : 'Gjet_GamESmaterial', # Not available in 2015 data
#                            'gammajetphscaleps'     : 'Gjet_GamESpresampler', # Not available in 2015 data
                            'gammajetphscalez'      : 'Gjet_GamESZee',
                            'gammajetphsmear'       : 'Gjet_GamEsmear',
                            'gammajetpurity'        : 'Gjet_Purity',
                            'gammajetstat1'         : 'Gjet_Stat1',
                            'gammajetstat2'         : 'Gjet_Stat2',
                            'gammajetstat3'         : 'Gjet_Stat3',
                            'gammajetstat4'         : 'Gjet_Stat4',
                            'gammajetstat5'         : 'Gjet_Stat5',
                            'gammajetstat6'         : 'Gjet_Stat6',
                            'gammajetstat7'         : 'Gjet_Stat7',
                            'gammajetstat8'         : 'Gjet_Stat8',
                            'gammajetstat9'         : 'Gjet_Stat9',
                            'gammajetstat10'        : 'Gjet_Stat10',
                            'gammajetstat11'        : 'Gjet_Stat11',
                            'gammajetstat12'        : 'Gjet_Stat12',
                            'gammajetstat13'        : 'Gjet_Stat13',
                            'gammajetstat14'        : 'Gjet_Stat14',
                            'gammajetstat15'        : 'Gjet_Stat15',
                            'gammajetsubleadingjet' : 'Gjet_Veto',
                            'mjbalpha'              : 'MJB_Alpha',
                            'mjbbeta'               : 'MJB_Beta',
                            'mjbfrag'               : 'MJB_Fragmentation', # WAS: mjbmodelling
                            'mjbptasymm'            : 'MJB_Asym',
                            'mjbptthreshold'        : 'MJB_Threshold',
                            'mjbstat1'              : 'MJB_Stat1',
                            'mjbstat2'              : 'MJB_Stat2',
                            'mjbstat3'              : 'MJB_Stat3',
                            'mjbstat4'              : 'MJB_Stat4',
                            'mjbstat5'              : 'MJB_Stat5',
                            'mjbstat6'              : 'MJB_Stat6',
                            'mjbstat7'              : 'MJB_Stat7',
                            'mjbstat8'              : 'MJB_Stat8',
                            'mjbstat9'              : 'MJB_Stat9',
                            'mjbstat10'             : 'MJB_Stat10',
                            'mjbstat11'             : 'MJB_Stat11',
                            'mjbstat12'             : 'MJB_Stat12',
                            'mjbstat13'             : 'MJB_Stat13',
                            'mjbstat14'             : 'MJB_Stat14',
                            'mjbstat15'             : 'MJB_Stat15',
                            'mjbstat16'             : 'MJB_Stat16',
                            'zjetdphi'              : 'Zjet_dPhi',
#                            'zjetescalemat'         : 'Zjet_ElecESmaterial', # Not available in 2015 data
#                            'zjetescaleps'          : 'Zjet_ElecESpresampler', # Not available in 2015 data
                            'zjetescalez'           : 'Zjet_ElecESZee',
                            'zjetesmear'            : 'Zjet_ElecEsmear',
                            'zjetgenerator'         : 'Zjet_MC',
#                            'zjetjvf'               : 'Zjet_JVF', # Not being used in 2015 data
                            'zjetjvt'		    : 'Zjet_JVT',
                            'zjetmuscale'           : 'Zjet_MuScale',
                            'zjetmusmearid'         : 'Zjet_MuSmearID',
                            'zjetmusmearms'         : 'Zjet_MuSmearMS',
                            'zjetoutofcone'         : 'Zjet_KTerm',
                            'zjetstat1'             : 'Zjet_Stat1',
                            'zjetstat2'             : 'Zjet_Stat2',
                            'zjetstat3'             : 'Zjet_Stat3',
                            'zjetstat4'             : 'Zjet_Stat4',
                            'zjetstat5'             : 'Zjet_Stat5',
                            'zjetstat6'             : 'Zjet_Stat6',
                            'zjetstat7'             : 'Zjet_Stat7',
                            'zjetstat8'             : 'Zjet_Stat8',
                            'zjetstat9'             : 'Zjet_Stat9',
                            'zjetstat10'            : 'Zjet_Stat10',
                            'zjetstat11'            : 'Zjet_Stat11',
                            'zjetstat12'            : 'Zjet_Stat12',
                            'zjetstat13'            : 'Zjet_Stat13',
                            'zjetsubleadingjet'     : 'Zjet_Veto',
                            # The following are new this year, as more uncertainties
                            # were propagated through the MJB calculation
                            'etaintercalibrationmodelling' : 'InSituProp_EtaIntercalModelling',
                            'etaintercalibrationtotalstat' : 'InSituProp_EtaIntercalStat',
                            'flavorcomposition'     : 'InSituProp_FlavorComposition',
                            'flavorresponse'        : 'InSituProp_FlavorResponse',
                            'pileupoffsetmu'        : 'InSituProp_PileupOffsetMu',
                            'pileupoffsetnpv'       : 'InSituProp_PileupOffsetNPV',
                            'pileupptterm'          : 'InSituProp_PileupPtTerm',
                            'pileuprhotopology'     : 'InSituProp_PileupRhoTopology',
                            'punchthroughmc12'      : 'InSituProp_PunchThroughMC',
                           }

jetDefDict =    {
                    'EMJES_R4'  : 'AntiKt4Topo_EMJES',
                    'EMJES_R6'  : 'AntiKt6Topo_EMJES',
                    'LCJES_R4'  : 'AntiKt4Topo_LCJES',
                    'LCJES_R6'  : 'AntiKt6Topo_LCJES'
                }

def ReadInSituHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Run over each subdirectory (one per jet definition)
    histos = {}
    subDirs = sorted(glob.glob(dirName+"*/"))
    for aSubDirName in subDirs:
        # Determine the jet definition
        jetDef = ""
        for aDirDef,aJetDef in jetDefDict.iteritems():
            if aDirDef in aSubDirName:
                jetDef = aJetDef
                break
        if jetDef == "":
            print "Failed to determine jet definition for directory:",aSubDirName
            return None
        histos[jetDef] = {}
        
        # Loop over the systematic files in the subdirectory
        systFiles = sorted(glob.glob(aSubDirName+"/SystError_*.txt"))
        for aSystFile in systFiles:
            # Figure out which component this is
            aSubDirName = aSubDirName.rstrip("/")
            systematicNameHandle = aSystFile.replace(aSubDirName+"/SystError_","")
            systematicNameHandle = systematicNameHandle.replace(".txt","")
            systematicName = SystematicNameDictionary[systematicNameHandle]
            print "Making histogram %s --> %s"%(systematicNameHandle,systematicName)

            # Open the file
            systematicFile = open(aSystFile,"r")

            # Read the lines of the file into arrays
            lowBinEdges  = []
            systValues   = []
            for line in systematicFile.readlines():
                line     = line.strip("\r\n")
                lowEdge  = float(line.split()[0])
                systVal  = float(line.split()[2].strip())

                lowBinEdges.append(lowEdge)
                systValues.append(systVal)
            
            # Done reading, close the file
            systematicFile.close()

            # Make the last bin go up to 2500
            lowBinEdges.append(2500.)
            systValues.append(systValues[-1])

            # Turn the lists into arrays and build the 1D histogram
            lowBinEdgesArray = array('d',lowBinEdges)
            systValuesArray  = array('d',systValues)
            histoName = systematicName+"_"+jetDef
            histo1D = TH1D(histoName+"_1D",histoName+"_1D",len(lowBinEdges)-1,lowBinEdgesArray)

            # Fill it from the file values
            for iBin in xrange(1,histo1D.GetNbinsX()+1):
                histo1D.SetBinContent(iBin,systValues[iBin-1])

            histos[jetDef][systematicName] = histo1D

        # EM has 10 stat parameters, LC has 11
        # So for EM algorithms, make an empty histo for stat 11
#        if "EM" in jetDef:
#            systematicName = SystematicNameDictionary['mjbstat11']
#            histos[jetDef][systematicName] = ProviderHistoHelpers.MakeProviderHisto(systematicName+"_"+jetDef)
#            histos[jetDef][systematicName].SetDirectory(0)


    # Done, return dictionary of histos
    return histos
            



