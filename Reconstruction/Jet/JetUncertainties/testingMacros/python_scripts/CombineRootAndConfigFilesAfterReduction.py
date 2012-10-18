# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#/bin/python

import sys
import re
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]


#def CombineRootFiles(self,fullFileName="JESUncertainty_Preliminary.root",reducedFileName="ReducedNP_file.root",combinedFileName="JESUncertainty_ReducedNP.root"):
def CombineRootFiles(fullFileName,reducedFileName,combinedFileName):
  print "Creating reduced root file..."
  print "1 " + fullFileName
  print "2 " + reducedFileName
  print "3 " + combinedFileName
  AllNuisanceParametersFile = TFile.Open(fullFileName,"READ")
  ReducedNuisanceParametersFile = TFile.Open(reducedFileName,"READ")
  OutputFile = TFile.Open(combinedFileName,"RECREATE")
  
  # Only the last five/six parameters are not included in the reduction
  #   EtaIntercalibration_TotalUnc = EtaIntercalibration_TotalStat, EtaIntercalibration_Modelling
  #   Pileup_OffsetMu
  #   Pileup_OffsetNPV
  #   SingleParticle_HighPt
  #   RelativeNonClosure_MCTYPE
  nonReducedParams = ["EtaIntercalibration_TotalUnc","EtaIntercalibration_TotalStat","EtaIntercalibration_Modelling",
                      "Pileup_OffsetMu","Pileup_OffsetNPV","SingleParticle_HighPt","RelativeNonClosure_"]
  for histName in AllNuisanceParametersFile.GetKeyNames():
    for aString in nonReducedParams:
      if aString in histName:
        hist = AllNuisanceParametersFile.Get(histName)
        OutputFile.cd()
        hist.Write()
  
  for histName in ReducedNuisanceParametersFile.GetKeyNames():
    hist = ReducedNuisanceParametersFile.Get(histName)
    OutputFile.cd()
    hist.Write()
  
  #OutputFile.ls()
  
  AllNuisanceParametersFile.Close()
  ReducedNuisanceParametersFile.Close()
  OutputFile.Close()


# Needed due to end of line differences between OS
def readSanitizedLine(file):
  line = file.readline()
  sanitizedLine = re.sub('\r','',line)
#  line = re.sub('\n','',sanitizedLine)
#  sanitizedLine = line+"\n\r"
  return sanitizedLine 
  

#def CombineConfigFiles(self,fullFileName="InsituJES2011_Preliminary.config",combinedFileName="InsituJES2011_Reduced.config",numNuisanceParam=5):
def CombineConfigFiles(fullFileName,combinedFileName,combinedRootFileName,numNuisanceParam):
  print "Creating reduced config file..."

  # Open the input file for reading, output file for writing
  inputFile = open(fullFileName,"r")
  outputFile = open(combinedFileName,"w")

  # Read the first file line by line, printing the header to the second file
  line = readSanitizedLine(inputFile)
  while ( not "JESUncertaintyRootFile" in line ):
    outputFile.write(line)
    line = readSanitizedLine(inputFile)
  outputFile.write("JESUncertaintyRootFile:      %s\n"%(combinedRootFileName))
  
  # Now print the settings header
  line = readSanitizedLine(inputFile)
  while (not "JESComponent" in line ):
    outputFile.write(line)
    line = readSanitizedLine(inputFile)

  # Now print the effective parameters
  # Adjust for zero-based vs one-based indexing in the range
  for i in range(1,numNuisanceParam+1,1):
    outputFile.write("JESComponent.%d.Desc:\t\tEffective JES uncertainty component %d\n"%(i,i))
    outputFile.write("JESComponent.%d.Name:\t\tEffectiveNP_%d\n\n"%(i,i))
  
  # Print the rest term
  outputFile.write("JESComponent.%d.Desc:\t\tEffective JES uncertainty component %d\n"%(numNuisanceParam+1,numNuisanceParam+1))
  outputFile.write("JESComponent.%d.Name:\t\tEffectiveNP_%drestTerm\n\n"%(numNuisanceParam+1,numNuisanceParam+1))
  
  # Now move through the file to the special components (pileup, eta calibration, etc)
  while (not "#############" in line ):
    line = readSanitizedLine(inputFile)

  # Copy the rest of the file (all the special components)
  # EOF returns an empty string
  while ( line != "" ):
    outputFile.write(line)
    line = readSanitizedLine(inputFile)

  # Done, close files
  inputFile.close()
  outputFile.close()


# Main method here
if (len(sys.argv) < 7):
  print "Too few arguments.  Expected the following:"
  print "\t1. Input (full) config file"
  print "\t2. Input (full) root file"
  print "\t3. Reduced root file (only effective nuisance parameters)"
  print "\t4. Output config file"
  print "\t5. Output root file"
  print "\t6. Number of effective nuisance parameters"
  exit(1)

fullConfigFile  = sys.argv[1]
fullRootFile    = sys.argv[2]
reducedRootFile = sys.argv[3]
outConfigFile   = sys.argv[4]
outRootFile     = sys.argv[5]
numEffNP        = int(sys.argv[6])


TFile.GetKeyNames = GetKeyNames


CombineRootFiles(fullRootFile,reducedRootFile,outRootFile)
CombineConfigFiles(fullConfigFile,outConfigFile,outRootFile,numEffNP)


