from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers
import os

# Dictionary to translate from Bogan's names to provider names
SystematicNameDictionary = {
                            'gammajetdphi'          : 'Gjet_dPhi',
                            'gammajetgenerator'     : 'Gjet_Generator',
                            'gammajetjvt'	        : 'Gjet_Jvt',
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
                            'zjetdphi'              : 'Zjet_dPhi',
                            'zjetescale'            : 'Zjet_ElecESZee',
                            'zjetesmear'            : 'Zjet_ElecEsmear',
                            'zjetgenerator'         : 'Zjet_MC',
                            'zjetjvt'		        : 'Zjet_Jvt',
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
                           }

jetDefDict =    {
                    'EMJES_R4'  : 'AntiKt4Topo_EMJES',
                    'LCJES_R4'  : 'AntiKt4Topo_LCJES',
                    'PFJES_R4'  : 'AntiKt4PFlow_EMJES'
                }

def ReadInSituHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Run over each subdirectory (one per jet definition)
    histos = {}
    subDirs = sorted(glob.glob(dirName+"*"))
    subDirs = [i for i in subDirs if os.path.isdir(os.path.join(os.getcwd(), i))]
    for aSubDirName in subDirs:
        # Determine the jet definition
        jetDef = ""
        for aDirDef,aJetDef in jetDefDict.iteritems():
            if aDirDef in aSubDirName:
                jetDef = aJetDef
                break
        if jetDef == "":
            print "Failed to determine jet definition for directory:",aSubDirName
            continue
        else :
            print "Jet definition is",jetDef
        histos[jetDef] = {}
        
        # Loop over the systematic files in the subdirectory
        systFiles = sorted(glob.glob(aSubDirName+"/SystError_*.txt"))
        for aSystFile in systFiles:
            # If anything else is present, skip it
            if not "SystError_" in aSystFile :
              continue
        
            # Figure out which component this is
            systematicNameHandle = re.sub(aSubDirName+"/SystError_","",aSystFile)
            systematicNameHandle = re.sub(".txt","",systematicNameHandle)
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

            # Convert to a 2D provider-stlye histo -- WHY WOULD WE DO THAT
            # STOP DOING THAT IT BREAKS THINGS
#            histo = ProviderHistoHelpers.ConvertPtHistoToProviderHisto(histo1D,histoName)
#            histo.SetDirectory(0)
#            print "Histo is type",type(histo),"whereas input was",histo1D
#            histos[jetDef][systematicName] = histo
            histos[jetDef][systematicName] = histo1D
    
        # EM has 10 stat parameters, LC has 11
        # So for EM algorithms, make an empty histo for stat 11
#        if "EM" in jetDef:
#            systematicName = SystematicNameDictionary['mjbstat11']
#            histos[jetDef][systematicName] = ProviderHistoHelpers.MakeProviderHisto(systematicName+"_"+jetDef)
#            histos[jetDef][systematicName].SetDirectory(0)


    # Done, return dictionary of histos
    return histos
            



