from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

SystematicNames = {'EtaIntercalibration_TotalStat' : 'EtaIntercalibration_TotalStat',\
                   'EtaIntercalibration_TotalSyst' : 'EtaIntercalibration_Modelling',\
                   'E_4TeV'                        : 'EtaIntercalibration_NonClosure_highE',\
                   'Eta_2p4_to_2p6'                : 'EtaIntercalibration_NonClosure_posEta',\
                   'Eta_m2p6_to_m2p4'              : 'EtaIntercalibration_NonClosure_negEta'
                 }

jetDefDict =    {
                    'AntiKt4EMTopo' : 'AntiKt4Topo_EMJES',
                    'AntiKt4EMPFlow' : 'AntiKt4PFlow_EMJES',
                }

def ReadEtaIntercalibrationHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # Run over each file (one per jet definition)
    histos = {}
    files = sorted(glob.glob(dirName+"*.root"))
    for aFileName in files:
        # Determine the jet definition
        jetDef = ""
        for aFileDef,aJetDef in jetDefDict.iteritems():
            if aFileDef in aFileName:
                jetDef = aJetDef
                break
        if jetDef == "":
            print "Failed to determine jet definition for file:",aFileName
            return None
        if jetDef not in histos.keys() :
          histos[jetDef] = {}

        # Read in the histograms of interest from the file
        inFile = TFile(aFileName,"READ")
        contents = inFile.GetKeyNames()
        for aName in SystematicNames.keys():
            if not "NonClosure" in SystematicNames[aName] :
              if "Topo" in jetDef :
                getsystematicName = aName + "_" + jetDef.replace("Topo","")
              else :
                getsystematicName = aName + "_" + jetDef.replace("4PFlow","4EMPFlow").replace("_EMJES","_PFlowJES")
            else :
              getsystematicName = aName

            if not getsystematicName in contents :
              continue

            histo = inFile.Get(getsystematicName)

            systematicName = SystematicNames[aName] + "_" + jetDef
            histo.SetName(systematicName+"_old")
 
            # Histo has different range, extend it
            histoNew = ProviderHistoHelpers.ExtendPtRangeOfHisto(histo,systematicName)
            histoNew.SetDirectory(0)
            histos[jetDef][SystematicNames[aName]] = histoNew

        # Done reading, close the file
        inFile.Close()
    
    # Since histos came from a whole mix of files, make sure we got them all in the end.
    for aName,storeName in SystematicNames.iteritems():
      for aJetDef in histos.keys():
        if not storeName in histos[aJetDef].keys() :
          print "Failed to find histogram:", storeName,"for jet definition",aJetDef

    print "About to return eta intercalib histos"
    print histos
    print "\n"
    return histos
    
