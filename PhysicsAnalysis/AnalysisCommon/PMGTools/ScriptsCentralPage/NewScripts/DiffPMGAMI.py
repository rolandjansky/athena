#! /usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys
import datetime
import glob
import math

substr = sys.argv[1]
output = sys.argv[2]

def ReturnFloat(Value):
    return math.fabs(float(Value))

def ReturnArray(File):
    fInFile     = open(File, 'r')
    fInputLines = fInFile.readlines()
    array = []
    for line in fInputLines:
        line = line.replace("\n", "")
        array.append(line.split(","))
    return array
         
    
def CheckDifference(Flag, DSID, Value1, Value2, Value3, Value4, Value5, Value6, Value7, Value8, Value9, Value10, OutputArray):
    Diff = math.fabs((Value1 - Value2)/Value1*100.0)
    XSecDiff = math.fabs(Value3 - Value4)
    KfactDiff = math.fabs(Value5 - Value6)
    FiltDiff = math.fabs(Value7 - Value8)
    if Diff > 0.01:
        if XSecDiff > 0:
            Flag = "Xsection"
            OutputArray.append(str(Value9)+" "+str(Flag)+" PMG "+str(Value4)+" AMI "+str(Value3)+" HighXsec "+str(Value10)) 
        if KfactDiff > 0:
            Flag = "Kfactor"
            OutputArray.append(str(Value9)+" "+str(Flag)+" PMG "+str(Value6)+" AMI "+str(Value5)+" HighXsec "+str(Value10))
        if FiltDiff > 0:
            Flag= "Filtereff"
            OutputArray.append(str(Value9)+" "+str(Flag)+" PMG "+str(Value8)+" AMI "+str(Value7)+" HighXsec "+str(Value10))

for InputFiles in glob.glob('/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/csvFiles/*'):
    OutputArray = [] 
    if InputFiles == str(substr):
        Array = ReturnArray(InputFiles)
        
        for entry in Array:
            DSID      = entry[0]
            PMGXsec   = ReturnFloat(entry[1])
            PMGKfac   = ReturnFloat(entry[2])
            PMGFilter = ReturnFloat(entry[3])
            PMGTo     = ReturnFloat(entry[4])
            AMIXsec   = ReturnFloat(entry[5])
            AMIKfac   = ReturnFloat(entry[6])
            AMIFilter = ReturnFloat(entry[7])
            AMITo     = ReturnFloat(entry[8])
            Sampl     = entry[9]
            PMGHT     = entry[10]
            CheckDifference("Difference",   DSID, PMGTo,   AMITo,   AMIXsec,   PMGXsec,   AMIKfac,   PMGKfac,   AMIFilter,   PMGFilter,   Sampl,  PMGHT,  OutputArray)

fOutFile = open(str(output), 'w')

for entry in OutputArray:
    fOutFile.write(entry+"\n")
fOutFile.close()
                  
