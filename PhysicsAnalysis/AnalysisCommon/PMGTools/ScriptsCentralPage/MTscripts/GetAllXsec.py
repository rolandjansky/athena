#! /usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import StatusFiles
import glob

import DefineTags
import SimpleFunctions

import logging

# Init the logger for the script and various modules
logging.basicConfig(format='%(asctime)s %(levelname)s %(message)s', datefmt='%H:%M:%S')
log = logging.getLogger("myXsecLogger")
log.setLevel(logging.INFO)


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


mainFolder = os.getcwd()
xsecFolder = mainFolder+"/XsecFiles/"
SimpleFunctions.MakeFolder(xsecFolder)

# Retrieve list of classification tags
allProcesses = DefineTags.ReturnSampleCategory()

# Get a pointer to all Status files
allFolders_mc15 = glob.glob(mainFolder+"/AllSampleInfo_mc15_13TeV/DSID*/*")
allFolders_mc15 = sorted(allFolders_mc15)

allFolders_mc16 = glob.glob(mainFolder+"/AllSampleInfo_mc16_13TeV/DSID*/*")
allFolders_mc16 = sorted(allFolders_mc16)

allFolders = allFolders_mc15 + allFolders_mc16

allXsec = []

# Using this list to avoid duplicates
# could be made more clever in the future
processed_DSIDs = []

for entry in allFolders:

    DSID = (entry.split("XXX/"))[1]
    if DSID in processed_DSIDs:
        continue
    processed_DSIDs.append(DSID)

    statusFiles_list = glob.glob(entry+"/Status*.txt")

    if len(statusFiles_list) == 0:
        continue

    nEvents = 0

    for iStatus in statusFiles_list:

        AllInfo = StatusFiles.StatusFileReader(iStatus)
        totalEvents = AllInfo.totalEvents

        log.debug("Reading nevents for %s", AllInfo.dataset)

        if not is_number(totalEvents):
            log.error("Total Number of Events: %s - Sample %s", str(totalEvents), str(iStatus))
            totalEvents = 0
        else:
            totalEvents = float(AllInfo.totalEvents)
        if totalEvents > nEvents:
            inputFile = iStatus
            nEvents = totalEvents

    AllInfo = StatusFiles.StatusFileReader(inputFile)

    SortingTag = AllInfo.SortingTag

    xsec = AllInfo.Xsec
    if not is_number(xsec):
        log.error("Xsec is NAN (%s) for DSID %s", str(xsec), DSID)
        continue
    else:
        xsec = float(xsec)*1000.
    filterEff = AllInfo.filterEff
    if not is_number(filterEff):
        log.error("filterEff is NAN (%s) for DSID %s", str(filterEff), DSID)
        continue
    kFactor = AllInfo.kFactor
    if not is_number(kFactor):
        log.error("kFactor is NAN (%s) for DSID %s", str(kFactor), DSID)
        continue
    dataset = AllInfo.dataset
    higherOrderXsec = AllInfo.higherOrderXsec
    higherOrderXsecSample = str(float(xsec)*float(kFactor)*float(filterEff))

    # Check AODs for various subcampaigns here
    nEv_mc15 = 0.
    nEv_mc16a = 0.
    nEv_mc16c = 0.
    nEv_mc16d = 0.
    nEv_mc16e = 0.

    if os.path.isfile(mainFolder+"/AllSampleInfo_mc15_13TeV/SummaryInfo/ListAOD_mc15_13TeV..txt"):
        arrayAOD_mc15 = SimpleFunctions.GetArraysFromTxtWithString(mainFolder+"/AllSampleInfo_mc15_13TeV/SummaryInfo/ListAOD_mc15_13TeV..txt", DSID)
        for iAOD in arrayAOD_mc15:
            if "r7725" in iAOD[0] or "r7772" in iAOD[0] or "a818" in iAOD[0] or "a821" in iAOD[0]:
                nEv_mc15 = nEv_mc15 + float(iAOD[1])

    if os.path.isfile(mainFolder+"/AllSampleInfo_mc16_13TeV/SummaryInfo/ListAOD_mc16_13TeV..txt"):
        arrayAOD_mc16 = SimpleFunctions.GetArraysFromTxtWithString(mainFolder+"/AllSampleInfo_mc16_13TeV/SummaryInfo/ListAOD_mc16_13TeV..txt", DSID)
        for iAOD in arrayAOD_mc16:
            if "r9294" in iAOD[0] or "r9315" in iAOD[0]:
                nEv_mc16a = nEv_mc16a + float(iAOD[1])
            if "r9778" in iAOD[0] or "r10009" in iAOD[0]:
                nEv_mc16c = nEv_mc16c + float(iAOD[1])
            if "r10210" in iAOD[0]:
                nEv_mc16d = nEv_mc16d + float(iAOD[1])
            if "r999999999999" in iAOD[0]:  # Ask Josh which tag to put here
                nEv_mc16e = nEv_mc16e + float(iAOD[1])

    SampleLumi = "Undef"
    datasetSplit = dataset.split(".evgen")
    datasetLabel = dataset

    entry = ["", "", "", "", "", "", "", "", "", ""]

    if float(higherOrderXsecSample) > 0.:
        if nEv_mc16a > 0.:
            SampleLumi = round(float(nEv_mc16a)/(float(higherOrderXsecSample))/1000.0, 2)
        elif nEv_mc15 > 0.:
            SampleLumi = round(float(nEv_mc15)/(float(higherOrderXsecSample))/1000.0, 2)
        else:
            SampleLumi = 0.

    tableEntry = [DSID, datasetLabel, str(float(xsec)), str(float(filterEff)), higherOrderXsec, str(float(kFactor)), SortingTag, str(SampleLumi), str(nEv_mc15), str(nEv_mc16a), str(nEv_mc16c), str(nEv_mc16d), str(nEv_mc16e), higherOrderXsecSample]

    allXsec.append(tableEntry)

for Process in allProcesses:
    OutputFile = xsecFolder+"/list_Xsec_"+Process+".txt"
    f = open(OutputFile, "w")
    f.write("| DSID |  Sample  | Events (mc15) | Events (mc16a) | Events (mc16c) | Events (mc16d) | Events (mc16e) | Sample Lumi (mc15/16a only) [fb-1] | Cross section [pb] | FiltEff | Total higher order xsec [pb] | K-factor | Higher order xsec sample [pb] | \n")

    for entry in allXsec:

        if Process == entry[6]:

            finalXsec = str(float(entry[2])*float(entry[3])*float(entry[5]))

            f.write("| "+entry[0]+" | "+entry[1]+" | "+entry[8]+" | "+entry[9]+" | "+entry[10]+" | "+entry[11]+" | "+entry[12]+" | "+ entry[7] +" | "+entry[2]+" | " +str(round(float(entry[3]), 6))+" | " +entry[4]+" | "+str(round(float(entry[5]), 4))+" | " +str(round(float(entry[13]), 9))+ " | \n")

    f.close()

    OutputFile = xsecFolder+"/list_Xsec_"+Process+"_Download.txt"

    f = open(OutputFile, "w")
    f.write("# DSID \t  Sample  \t Xsec \t FiltEff \t Total higher order xsec \t K-factor \t Higher order  xsec sample  \n")

    for entry in allXsec:
        if Process == entry[6]:

            finalXsec = str(float(entry[2])*float(entry[3])*float(entry[5]))

            if len(entry[4]) != 1:
                f.write(entry[0]+" \t "+entry[1]+" \t "+entry[2] +" \t " +entry[3]+" \t " +entry[4]+" \t "+entry[5]+" \t " +entry[9]+ "  \n")
            else:
                if "Undef" in entry[9]: 
                    f.write(entry[0]+" \t "+entry[1]+" \t "+entry[2] +" \t " +entry[3]+" \t " +entry[4]+" \t Undef. \t " +finalXsec+ "  \n")
                else:
                    f.write(entry[0]+" \t "+entry[1]+" \t "+entry[2] +" \t " +entry[3]+" \t " +entry[4]+" \t Undef. \t " +entry[9]+ "  \n")
    f.close()

OutputFile = xsecFolder+"/list_Xsec_TopDataPrep.txt"
f = open(OutputFile, "w")
f.write("# DSID \t  \t AMIXsec * FiltEff \t K-factor \n")
