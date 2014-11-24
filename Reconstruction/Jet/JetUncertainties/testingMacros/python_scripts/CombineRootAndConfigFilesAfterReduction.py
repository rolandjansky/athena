# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#/bin/python

import sys
import re
import datetime
import calendar
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]

#Copy everything from the reduced file to the output file
#Copy everything excluding any overlap with the reduced file from the full file to the output file
def CombineRootFiles(fullFileName,reducedFileName,combinedFileName):
  print "Creating reduced root file..."
  print "Full file:     " + fullFileName
  print "Reduced file:  " + reducedFileName
  print "Output file:   " + combinedFileName
  AllNuisanceParametersFile = TFile.Open(fullFileName,"READ")
  ReducedNuisanceParametersFile = TFile.Open(reducedFileName,"READ")
  OutputFile = TFile.Open(combinedFileName,"RECREATE")
  
  for histName in AllNuisanceParametersFile.GetKeyNames():
    hist = AllNuisanceParametersFile.Get(histName)
    OutputFile.cd()
    hist.Write(histName)

  reducedComponentNames = []
  for histName in ReducedNuisanceParametersFile.GetKeyNames():
    if "EffectiveNP_" in histName:
      hist = ReducedNuisanceParametersFile.Get(histName)
      OutputFile.cd()
      hist.Write(histName)
      reducedComponentNames.append(histName)
  
  ''' This method is no longer relevant due to Reduction changes, retained in case this changes

  for histName in ReducedNuisanceParametersFile.GetKeyNames():
    hist = ReducedNuisanceParametersFile.Get(histName)
    OutputFile.cd()
    hist.Write(histName)
    reducedComponentNames.append(histName)

  for histName in AllNuisanceParametersFile.GetKeyNames():
    alreadyCopied = False
    for aComponent in reducedComponentNames:
      if histName in aComponent:
        alreadyCopied = True
        break
    if not alreadyCopied:
      hist = AllNuisanceParametersFile.Get(histName)
      OutputFile.cd()
      hist.Write(histName)
  '''
  
  AllNuisanceParametersFile.Close()
  ReducedNuisanceParametersFile.Close()
  OutputFile.Close()

  return reducedComponentNames

# Needed due to end of line differences between OS
def readSanitizedLine(file):
  line = file.readline()
  sanitizedLine = re.sub('\r','',line)
#  line = re.sub('\n','',sanitizedLine)
#  sanitizedLine = line+"\n\r"
  return sanitizedLine 

def sanitizeComponentName(aName):
  sanitizedName = re.sub(';1','',aName)
  sanitizedName = re.sub('_AntiKt4Topo_EMJES','',sanitizedName)
  sanitizedName = re.sub('_AntiKt6Topo_EMJES','',sanitizedName)
  sanitizedName = re.sub('_AntiKt4Topo_LCJES','',sanitizedName)
  sanitizedName = re.sub('_AntiKt6Topo_LCJES','',sanitizedName)
  return sanitizedName

def getDescFromName(aName):
  if "Stat" in aName:
    return "Effective JES Statistical and Method Component %c"%(aName[-1])
  elif "Detector" in aName:
    return "Effective JES Detector Component %c"%(aName[-1])
  elif "Model" in aName:
    return "Effective JES Modelling Component %c"%(aName[-1])
  elif "Mixed" in aName:
    return "Effective JES Mixed Modelling and Detector Component %c"%(aName[-1])
  elif "Special" in aName:
    print "WARNING: Including a special component directly - %s"%(aName)
    return "Effective JES Special Component %c"%(aName[-1])
  elif "UNKNOWN" in aName:
    print "WARNING: Including an unknown component - %s"%(aName)
    return "Effecitve JES Unknown Category Component %c"%(aName[-1])
  elif "EffectiveNP_" in aName:
    if "restTerm" in aName:
      return "Effective JES Uncertainty Component %c"%(aName[-9])
    else:
      return "Effective JES Uncertainty Component %c"%(aName[-1])
  else:
    print "ERROR: Including an unrecognized component"
    exit(1)

def getCategoryFromName(aName):
  # Special for eta intercalibration components
  if "etaintercalibration_totalstat" in aName.lower():
    return "Statistical"
  elif "etaintercalibration_modelling" in aName.lower():
    return "Modelling"
  elif "stat" in aName.lower():
    return "Statistical"
  elif "detector" in aName.lower():
    return "Detector"
  elif "model" in aName.lower():
    return "Modelling"
  elif "mixed" in aName.lower():
    return "Mixed"
  elif "special" in aName.lower():
    return "Special"
  elif "EffectiveNP_" in aName:
    return "Special" # Global reduction parameters are of type "Special"
  else:
    return "UNKNOWN"

def checkIfWasCategoryReduction(listOfNames):
  for aName in listOfNames:
    if "Stat" in aName:
      return True
    elif "Detector" in aName:
      return True
    elif "Model" in aName:
      return True
    elif "Mixed" in aName:
      return True
  return False

def replaceComponentNumber(line,newNumber):
  # Line is in the following form:
  #    JESComponent.#.etc
  # Replace the first part
  return re.sub("JESComponent.[0-9]*.","JESComponent.%d."%(newNumber),line)

def CombineConfigFiles(fullFileName,combinedFileName,combinedRootFileName,componentNames):
  print "Creating reduced config file..."

  # Open the input file for reading, output file for writing
  inputFile = open(fullFileName,"r")
  outputFile = open(combinedFileName,"w")

  # Determine the type of reduction (for the header)
  reductionString = ""
  if "GlobalReduction" in combinedFileName:
    reductionString = "Reduction was applied globally"
  elif "CategoryReduction" in combinedFileName:
    reductionString = "Reduction was applied within each category"
  else:
    reductionString = "Unknown type of reduction"

  # Create the file header
  now = datetime.datetime.now()
  outputFile.write("\n#######################################\n#\n")
  outputFile.write("#  JESProvider Input Settings\n")
  outputFile.write("#    Reduced set of nuisance parameters from final 2011 iteration of the in-situ JES\n")
  outputFile.write("#    %s\n"%(reductionString))
  #outputFile.write("#      %s\n"%(fullFileName))
  outputFile.write("#    %s %d, %d\n#\n"%(calendar.month_name[now.month],now.day,now.year))
  #outputFile.write("#    \n") # AUTHORS HERE
  outputFile.write("#\n#######################################\n\n")
  
  # Specify the root file that goes with the header
  outputFile.write("JESUncertaintyRootFile:       %s\n"%(combinedRootFileName))

  # Settings header
  outputFile.write("\n#######################################\n#\n")
  outputFile.write("#   Settings for JES Components\n")
  outputFile.write("#\n#######################################\n\n")

  # JES Components
  # Parse the base names
  compNamesSanitized = []
  for aName in componentNames:
    sanitizedName = sanitizeComponentName(aName)
    if sanitizedName not in compNamesSanitized:
      compNamesSanitized.append(sanitizedName)

  i=1
  for aName in compNamesSanitized:
    outputFile.write("JESComponent.%d.Desc:            %s\n"%(i,getDescFromName(aName)))
    outputFile.write("JESComponent.%d.Name:            %s\n"%(i,aName))
    outputFile.write("JESComponent.%d.Type:            %s\n"%(i,getCategoryFromName(aName)))
    outputFile.write("\n")
    i = i + 1

  # Special components
  # Currently, read from other config file
  outputFile.write("\n#######################################\n\n")

  # Look for "Special" type components in the original file
  # EOF returns an empty string
  inputFile = open(fullFileName,"r")
  nextLine = readSanitizedLine(inputFile)
  currLine = ""
  previousLine = ""
  while (nextLine != ""):
    if "Special" not in nextLine and "EtaIntercal" not in currLine:
      # Not interesting (except for eta intercal), just iterate
      previousLine = currLine
      currLine = nextLine
      nextLine = readSanitizedLine(inputFile)
    else:
      # Found a special component, so save it
      outputFile.write(replaceComponentNumber(previousLine,i))
      outputFile.write(replaceComponentNumber(currLine,i))
      outputFile.write(replaceComponentNumber(nextLine,i))
      
      # Iterate
      previousLine = currLine
      currLine = nextLine
      nextLine = readSanitizedLine(inputFile)
      
      # Check if sub-components or correlated sub-components
      if ("SubComponent" in nextLine):
        outputFile.write(replaceComponentNumber(nextLine,i))
        previousLine = currLine
        currLine = nextLine
        nextLine = readSanitizedLine(inputFile)
             
      outputFile.write("\n")
      i = i + 1


  outputFile.write("\n#\n#######################################\n")

  # Done, close files
  inputFile.close()
  outputFile.close()
  print "Created config file: %s\n"%(combinedFileName)

# Main method here
if (len(sys.argv) < 7):
  print "Too few arguments.  Expected the following:"
  print "\t1. Path to the JetUncertainties share directory"
  print "\t2. Input (full) config file"
  print "\t3. Input (full) root file"
  print "\t4. Reduced root file (only effective nuisance parameters)"
  print "\t5. Output config file"
  print "\t6. Output root file"
  exit(1)

sharePath       = sys.argv[1]
fullConfigFile  = sys.argv[2]
fullRootFile    = sys.argv[3]
reducedRootFile = sys.argv[4]
outConfigFile   = sys.argv[5]
outRootFile     = sys.argv[6]


TFile.GetKeyNames = GetKeyNames


compNames = CombineRootFiles(sharePath+"/"+fullRootFile,reducedRootFile,sharePath+"/"+outRootFile)
CombineConfigFiles(sharePath+"/"+fullConfigFile,sharePath+"/"+outConfigFile,outRootFile,compNames)


