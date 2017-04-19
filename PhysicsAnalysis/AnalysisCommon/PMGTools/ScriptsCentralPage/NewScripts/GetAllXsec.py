#! /usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
import StatusFiles
import glob

import DefineTags
import SimpleFunctions


InputFolder = "AllSampleInfo/"
XsecFolder  = "XsecFiles/"

os.system("mkdir "+XsecFolder)

AllProcesses = DefineTags.ReturnSampleCategory()

AllFolders   = glob.glob(InputFolder+"/DSID*/*") 

AllXsec = []

AllFolders   = sorted(AllFolders)

for entry in AllFolders:
    DSID = (entry.split("XXX/"))[1]
    folder    = entry
    allAODs   = glob.glob(folder+"/Status*.txt")

    if len(allAODs) == 0:
        continue
    
    inputFile = allAODs[0]
    nEvents   = 0
    helpTag   = 0

    for iAOD in allAODs:
        AllInfo     = StatusFiles.StatusFileReader(iAOD)
        totalEvents = AllInfo.totalEvents
        if "NULL" in totalEvents:
            totalEvents = 0
        else:
            totalEvents = float(AllInfo.totalEvents)
        if totalEvents > nEvents:
            tag         = float((AllInfo.tag).replace("e", ""))
            helpTag     = tag
            inputFile   = iAOD
            nEvents     = totalEvents

            if "361316" in str(AllInfo.DSID):
                print nEvents,"   ",inputFile

    AllInfo = StatusFiles.StatusFileReader(inputFile)

    SortingTag      = AllInfo.SortingTag
    
    xsec            = AllInfo.Xsec
    filterEff       = AllInfo.filterEff
    BR              = AllInfo.BR
    kFactor         = AllInfo.kFactor
    dataset         = AllInfo.dataset
    higherOrderXsec = AllInfo.higherOrderXsec
    higherOrderXsecSample = AllInfo.higherOrderXsecSample
    totalEvents     = AllInfo.totalEvents

    SampleLumi      = "Undef"
    datasetSplit    = dataset.split(".evgen")
    datasetLabel    = dataset

    entry = ["", "", "", "", "", "", "", "", "", ""]

    if "Undefined" in xsec:
        continue
    if "Undefined" in filterEff:
        continue
    if kFactor == "Undefined":
        kFactor = "1.0"
        
    if not "Undefined" in xsec and not "Undefined" in filterEff and not "division" in filterEff and not "Error" in filterEff:
        if float(xsec) > 0.0 and float(filterEff) > 0.0:
            SampleLumi = round(float(totalEvents)/(float(xsec)*float(filterEff))/1000.0, 2)


    entry = [DSID, datasetLabel, xsec, filterEff, higherOrderXsec, kFactor, SortingTag, str(SampleLumi), str(totalEvents), higherOrderXsecSample, BR]

    AllXsec.append(entry)


for Process in AllProcesses:
    OutputFile = XsecFolder+"/list_Xsec_"+Process+".txt"
    f = open(OutputFile, "w")
    f.write("| DSID |  Sample  | Total events | Sample Lumi [fb-1] | AMIXsec [pb] | BR  | FiltEff | Total higher order xsec [pb] | K-factor | Higher order xsec sample [pb] | \n")

    for entry in AllXsec:

        if Process == entry[6]:
            #if "Calculation impossible" in entry[3]:
            #    entry[3] = "1.0"
            if "division" in entry[3]:
                continue
            if "Error" in entry[3]:
                continue
            if "defin" in entry[5]:
                continue

            finalXsec = str(float(entry[2])*float(entry[3])*float(entry[5]))

            if "362409" in entry[0]:
                print entry
                print finalXsec
                print entry[9]

            if "Undef" in entry[9]:
                f.write("| "+entry[0]+" | "+entry[1]+" | "+entry[8]+" | "+ entry[7] +" | "+entry[2]+" | "+str(round(float(entry[10]), 6))+" | " +str(round(float(entry[3]), 6))+" | " +entry[4]+" | "+str(round(float(entry[5]), 4))+" | " +str(round(float(finalXsec), 9))+ " | \n")
            else:
                f.write("| "+entry[0]+" | "+entry[1]+" | "+entry[8]+" | "+ entry[7] +" | "+entry[2]+" | "+str(round(float(entry[10]), 6))+" | " +str(round(float(entry[3]), 6))+" | " +entry[4]+" | "+str(round(float(entry[5]), 4))+" | " +str(round(float(entry[9]), 9))+ " | \n")


    f.close()

    OutputFile = XsecFolder+"/list_Xsec_"+Process+"_Download.txt"

    f = open(OutputFile, "w")
    f.write("# DSID \t  Sample  \t AMIXsec \t BR \t FiltEff \t Total higher order xsec \t K-factor \t Higher order  xsec sample  \n")

    for entry in AllXsec:
        if Process == entry[6]:

            if "division" in entry[3]:
                continue

            if "Error" in entry[3]:
                continue

            if "def" in entry[5]:
                continue

            finalXsec = str(float(entry[2])*float(entry[3])*float(entry[5]))


            if "362409" in entry[0]:
                print entry
                print finalXsec,"  ",entry[2],"  ",entry[3],"  ",entry[5]
                print entry[9]


            if len(entry[4]) != 1:
                f.write(entry[0]+" \t "+entry[1]+" \t "+entry[2]+" \t "+entry[10]+" \t " +entry[3]+" \t " +entry[4]+" \t "+entry[5]+" \t " +entry[9]+ "  \n")
            else:
                if "Undef" in entry[9]: 
                    f.write(entry[0]+" \t "+entry[1]+" \t "+entry[2]+" \t "+entry[10]+" \t " +entry[3]+" \t " +entry[4]+" \t Undef. \t " +finalXsec+ "  \n")
                else:
                    f.write(entry[0]+" \t "+entry[1]+" \t "+entry[2]+" \t "+entry[10]+" \t " +entry[3]+" \t " +entry[4]+" \t Undef. \t " +entry[9]+ "  \n")
    f.close()

OutputFile = XsecFolder+"/list_Xsec_TopDataPrep.txt"
f = open(OutputFile, "w")
f.write("# DSID \t  \t AMIXsec * FiltEff \t K-factor \n")
