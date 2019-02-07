from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# Dictinary below last updated to match inputs from Bogdan and Meghan
# sent December 14 2016

# Dictionary to translate from Bogan's names to provider names
SystematicNameDictionary = {
                            'gammajetdphi'          : 'Gjet_dPhi',
                            'gammajetgenerator'     : 'Gjet_Generator',
                            'gammajetjvt'           : 'Gjet_Jvt',
                            'gammajetshower'        : 'Gjet_ShowerTopology',
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
                            'gammajetstat16'        : 'Gjet_Stat16',
                            'gammajetsubleadingjet' : 'Gjet_Veto',
                            'mjbalpha'              : 'MJB_Alpha',
                            'mjbbeta'               : 'MJB_Beta',
                            'mjbfrag'               : 'MJB_Fragmentation', # WAS: mjbmodelling
                            'mjbjvt'                : 'MJB_JVT',
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
                            'mjbstat17'             : 'MJB_Stat17',
                            'mjbstat18'             : 'MJB_Stat18',
                            'mjbstat19'             : 'MJB_Stat19',
                            'mjbstat20'             : 'MJB_Stat20',
                            'mjbstat21'             : 'MJB_Stat21',
                            'mjbstat22'             : 'MJB_Stat22',
                            'mjbstat23'             : 'MJB_Stat23',
                            'mjbstat24'             : 'MJB_Stat24',
                            'mjbstat25'             : 'MJB_Stat25',
                            'mjbstat26'             : 'MJB_Stat26',
                            'mjbstat27'             : 'MJB_Stat27',
                            'mjbstat28'             : 'MJB_Stat28',
                            'zjetdphi'              : 'Zjet_dPhi',
                            'zjetescale'            : 'Zjet_ElecESZee',
                            'zjetesmear'            : 'Zjet_ElecEsmear',
                            'zjetgenerator'         : 'Zjet_MC',
                            'zjetjvt'               : 'Zjet_Jvt',
                            'zjetmuscale'           : 'Zjet_MuScale',
                            'zjetmusmearid'         : 'Zjet_MuSmearID',
                            'zjetmusmearms'         : 'Zjet_MuSmearMS',
                            'zjetmusagittares'      : 'Zjet_MuSagittaRes',
                            'zjetmusagittarho'      : 'Zjet_MuSagittaRho',
                            'zjetelstat1'           : 'Zjet_ElecStat1',
                            'zjetelstat2'           : 'Zjet_ElecStat2',
                            'zjetelstat3'           : 'Zjet_ElecStat3',
                            'zjetelstat4'           : 'Zjet_ElecStat4',
                            'zjetelstat5'           : 'Zjet_ElecStat5',
                            'zjetelstat6'           : 'Zjet_ElecStat6',
                            'zjetelstat7'           : 'Zjet_ElecStat7',
                            'zjetelstat8'           : 'Zjet_ElecStat8',
                            'zjetelstat9'           : 'Zjet_ElecStat9',
                            'zjetelstat10'          : 'Zjet_ElecStat10',
                            'zjetelstat11'          : 'Zjet_ElecStat11',
                            'zjetelstat12'          : 'Zjet_ElecStat12',
                            'zjetelstat13'          : 'Zjet_ElecStat13',
                            'zjetelstat14'          : 'Zjet_ElecStat14',
                            'zjetmustat1'           : 'Zjet_MuStat1',
                            'zjetmustat2'           : 'Zjet_MuStat2',
                            'zjetmustat3'           : 'Zjet_MuStat3',
                            'zjetmustat4'           : 'Zjet_MuStat4',
                            'zjetmustat5'           : 'Zjet_MuStat5',
                            'zjetmustat6'           : 'Zjet_MuStat6',
                            'zjetmustat7'           : 'Zjet_MuStat7',
                            'zjetmustat8'           : 'Zjet_MuStat8',
                            'zjetmustat9'           : 'Zjet_MuStat9',
                            'zjetmustat10'          : 'Zjet_MuStat10',
                            'zjetmustat11'          : 'Zjet_MuStat11',
                            'zjetmustat12'          : 'Zjet_MuStat12',
                            'zjetmustat13'          : 'Zjet_MuStat13',
                            'zjetmustat14'          : 'Zjet_MuStat14',
                            'zjetsubleadingjet'     : 'Zjet_Veto',
                            'zjetshower'            : 'Zjet_ShowerTopology',
                            # The following are from the terms which
                            # were propagated through the MJB calculation
                            'etaintercalibrationmodelling' : 'InSituProp_EtaIntercalModelling',
                            'etaintercalibrationnonclosurehighe' : 'InSituProp_EtaIntercalNonClosureHighE',
                            'etaintercalibrationnonclosureposeta' : 'InSituProp_EtaIntercalNonClosurePosEta',
                            'etaintercalibrationnonclosurenegeta' : 'InSituProp_EtaIntercalNonClosureNegEta',
                            'etaintercalibrationofcs' : 'InSituProp_EtaIntercalOFCs',
                            'etaintercalibrationtotalstat' : 'InSituProp_EtaIntercalStat',
                            'flavorcomposition'     : 'InSituProp_FlavorComposition',
                            'flavorresponse'        : 'InSituProp_FlavorResponse',
                            'pileupoffsetmu'        : 'InSituProp_PileupOffsetMu',
                            'pileupoffsetnpv'       : 'InSituProp_PileupOffsetNPV',
                            'pileupptterm'          : 'InSituProp_PileupPtTerm',
                            'pileuprhotopology'     : 'InSituProp_PileupRhoTopology',
                            'punchthroughmc16'      : 'InSituProp_PunchThroughMC',
                           }

jetDefDict =    {
                            'EMJES_R4'  : 'AntiKt4Topo_EMJES',
                            'PFJES_R4'  : 'AntiKt4PFlow_EMJES'
                          }

def ReadInSituHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    print dirName
    import os
    print os.listdir(dirName)

    # Run over each subdirectory (one per jet definition)
    histos = {}
    subDirs = sorted(glob.glob(dirName+"*/"))
    print "Checking subdirs",subDirs
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
            



