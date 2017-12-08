#! /usr/bin/python                                                                                                                                                      

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys
import datetime
import glob

import pyAMI
import pyAMI.client
import pyAMI.atlas.api

sys.path.append('NewScripts/')

import SimpleFunctions
from SimpleFunctions import MakeFolder, GetDSIDs
import GetAmiInfo
import DefineTags
import StatusFiles
import MakeInputTables

# mode to debug the rest of the script without having to rerun every step                                                                                                            
                    
Debug = False

################################################################################                                                                                                      
fLocalPath    = os.getcwd()
MainFolder    = os.getcwd().replace("HelperScripts", "")
ListLocation  = MainFolder+"/InputInformation/"
InputLists    = glob.glob(ListLocation+"/ExtraInfo_*")
MainFolder    = os.getcwd()+"/AllSampleInfo/"
SummaryFolder = MainFolder+"/SummaryInfo/"
AODFile       = SummaryFolder+"/ListAllAODs.txt"
EVNTFile      = SummaryFolder+"/ListAllEVNTs.txt"
DerivFile     = SummaryFolder+"/ListAllDerivations.txt"
ReducedFile   = SummaryFolder+"/ReducedInputList.txt"
KeywordFile   = SummaryFolder+"/KeywordList.txt"
twikiFolder   = MainFolder+"/twikiPages/"
################################################################################  

# first collect all the general info
client        = pyAMI.client.Client('atlas')
pyAMI.atlas.api.init()

# now make a folder to store all info in                                                                                                                                               
                       
MakeFolder(MainFolder)
MakeFolder(SummaryFolder)
MakeFolder(twikiFolder)

TimeStart  = datetime.datetime.now()

ArrayAOD   = []
ArrayEVNT  = []
ArrayDeriv = []

#if not Debug or not os.path.exists(AODFile):
#if not os.path.exists(AODFile):
print "=========================> Get all AOD File info... "
SuperArrayAOD  = []
AMIInfoArray1  = pyAMI.atlas.api.list_datasets(client, "mc15_13TeV.4%merge.AOD%/", fields="events,modified")                                                           
AMIInfoArray2  = pyAMI.atlas.api.list_datasets(client, "mc15_13TeV.3%merge.AOD%/", fields="events,modified")
AMIInfoArray3  = pyAMI.atlas.api.list_datasets(client, "mc15_13TeV.187%merge.AOD%/", fields="events,modified")
SuperArrayAOD.append(AMIInfoArray1)
SuperArrayAOD.append(AMIInfoArray2)
SuperArrayAOD.append(AMIInfoArray3)
SimpleFunctions.WriteArraysToTxt(SuperArrayAOD, AODFile)
ArrayAOD = SimpleFunctions.GetArraysFromTxt(AODFile)
#else:
#    if os.path.exists(AODFile):
#        ArrayAOD = SimpleFunctions.GetArraysFromTxt(AODFile)

#for now re-make the EVNT file every time, not only if the file does not exist!
#if not os.path.exists(EVNTFile):
print "=========================> Get all EVNT File info... "
SuperArrayEVNT = []
AMIInfoArray4  = pyAMI.atlas.api.list_datasets(client, "mc15_13TeV.4%EVNT%/",      fields="events,modified")                                
AMIInfoArray5  = pyAMI.atlas.api.list_datasets(client, "mc15_13TeV.3%EVNT%/",      fields="events,modified")                                                                          
AMIInfoArray6  = pyAMI.atlas.api.list_datasets(client, "mc12_13TeV.187%EVNT%/",    fields="events,modified")
SuperArrayEVNT.append(AMIInfoArray4)                                                                                                                                                 
SuperArrayEVNT.append(AMIInfoArray5)                                                                                                                                                  
SuperArrayEVNT.append(AMIInfoArray6)
SimpleFunctions.WriteArraysToTxt(SuperArrayEVNT, EVNTFile)
ArrayEVNT = SimpleFunctions.GetArraysFromTxt(EVNTFile)

# now just get a plain list of derivations with dq2                                                                                                                                                          
os.system("dq2-ls mc15_13TeV.*merge.DAOD_*/ | sort > "+DerivFile)  # takes about four seconds 

ArrayDeriv = SimpleFunctions.GetArraysFromTxt(DerivFile)

TimeListCont = datetime.datetime.now()

# 3.47 sec

print "==================================================================="
print "========== Time to list containers:   "+str(TimeListCont-TimeStart)+" =============="
print "==================================================================="

# get list of all DSIDs that are used at the moment                                                                                                                                                       
FullDSIDList = GetDSIDs(ArrayEVNT)

print "Number of DSIDs used: ",len(FullDSIDList)

TimeGetDSIDs = datetime.datetime.now()

print "==================================================================="
print "========== Time to get list of DSIDs: "+str(TimeGetDSIDs-TimeListCont)+" =============="
print "==================================================================="

ReducedList = []

if not Debug or not os.path.exists(ReducedFile):
    for DSID in FullDSIDList:
        #continue
        NewFolder = MainFolder+"/DSID_"+str(DSID[0:3])+"XXX/"
        SubFolder = NewFolder+str(DSID)
        MakeFolder(NewFolder)
        MakeFolder(SubFolder)
        # now make list with all relevant derivations
        DerivFileName = SubFolder+"/List_Derivations.txt"
        ListDeriv = []
        if not Debug or not os.path.exists(DerivFileName):
            ListDeriv = SimpleFunctions.GetAllFilesWithStrings(ArrayDeriv, [DSID])
            SimpleFunctions.WriteListToTxt(ListDeriv, DerivFileName)
        # now get all EVNT files and the latest etag
        ListEVNT  = SimpleFunctions.GetAllFilesWithStrings(ArrayEVNT, [DSID])
        
        for entry in ListEVNT:
            dataset = entry
            ReducedList.append(dataset)
            
    SimpleFunctions.WriteListToTxt(ReducedList, ReducedFile)

else:
    ReducedList =  SimpleFunctions.GetArraysFromTxt(ReducedFile)

# get full list for all DSIDs in the lists with details                                                                                                                 
                                      
FullDSIDInfo     = SimpleFunctions.GetFullList(InputLists, FullDSIDList) 
FullDSIDInfoEVNT = SimpleFunctions.MatchEVNT(FullDSIDInfo, ReducedList)
KeywordArray     = SimpleFunctions.GetArraysFromTxt(KeywordFile)

counter = 0

print "number of files = ",len(FullDSIDInfoEVNT)

if not Debug:
    for entry in FullDSIDInfoEVNT:

        counter += 1

        if len(entry) == 1:
            if entry[0] != '\n':
                #print "ERROR: entry has length "+str(len(entry))
                #print entry
                continue
            else:
                continue

        DSID            = entry[0]
        Xsec            = entry[1]
        Kfactor         = entry[2]
        FilterEff       = entry[3]
        BR              = entry[4]
        HigherOrderXsec = entry[5]
        SortingTag      = entry[6]
        Comment         = entry[7]
        Title           = entry[8]
        container       = entry[9]

        if len(entry) == 11:
            container       = entry[10]

        if SortingTag == "":
            # this step seems to take very long! store tomorrow all keywords in lookup file
            Keywords   = SimpleFunctions.GetKeywords(KeywordArray, DSID)
            if Keywords == "empty" or Keywords == "NULL":
                Keywords   = GetAmiInfo.GetKeywords(client, container)
                #print "============================================================================================="
                #print Keywords,"   ",container,"  ",len(entry)
            SortingTag = DefineTags.SortKeyWordsToSampleCategory(Keywords, container)

    #    if not "187" in DSID:
    #        continue

        #print "============================================================================================="
        #print entry
        #print DSID,"  ",SortingTag,"  ",container

        outputTwiki = twikiFolder+"/Twiki_"+SortingTag+"_"+str(DSID)+".twiki"

        #if os.path.exists(outputTwiki):
        #    continue   

        StatusFile = "StatusFile_"+SortingTag+"_"+str(DSID)+"_"+container.replace("/", "")+".txt"

        #print StatusFile

        NewFolder  = MainFolder+"/DSID_"+str(DSID[0:3])+"XXX/"+str(DSID)+"/"

        #if not os.path.exists(NewFolder+"/"+StatusFile):
        StatusFiles.StatusFileCreator(StatusFile, NewFolder, container, Comment, SortingTag, Xsec, Kfactor, FilterEff, BR, HigherOrderXsec, Title, DSID)

        # now it is taking always the latest tag to fill the twiki file, this is not really what we want right?
        # twikiFile = MakeInputTables.fillTables(StatusFile, NewFolder, DSID, twikiFolder)




# to use only the tag with the largest stats to fill the xsec files, we need another loop now

# first loop over all folders that we have:

FolderList = glob.glob(MainFolder+"/DSID_*/")

for helpFolder in FolderList:

    # get subfolders:

    SubFolderList = glob.glob(helpFolder+"/*")

    for SubFolder in SubFolderList:
        
        # now get list of EVNT files:
        FileList = glob.glob(SubFolder+"/StatusFile*")

        #if not "410000" in SubFolder:
        #    continue
    
        # now loop over file list and find file with largest stats:

        helpFile   = "bla"
        helpEvents = -1000
        DSID       = -1000


        for File in FileList:
            AllInfo = StatusFiles.StatusFileReader(File)
            events  = AllInfo.totalEvents
            #print events,"    ",File
            DSID    = AllInfo.DSID

            if "NULL" == events:
                events = 0
                #continue

            if int(events) > int(helpEvents):
                helpFile   = File
                helpEvents = events
                
        twikiFile = MakeInputTables.fillTables(helpFile, SubFolder, DSID, twikiFolder)


        TwikiFolderWWW = "/afs/cern.ch/user/a/atltopmc/www/TopMC12twiki/TestCentralMC15ProductionPage/"

        if twikiFile != "empty":
            os.system("cp "+twikiFile+" "+TwikiFolderWWW)


TimeFillInfo = datetime.datetime.now()

print "\t"
print "==================================================================="
print "========== Time to fill all info:     "+str(TimeFillInfo-TimeGetDSIDs)+" =============="
print "==================================================================="

# now read this info and all plain txt files and make the tables for the twiki
# loop over all DSIDs
# for entry in FullDSIDInfo:

TwikiList = glob.glob(twikiFolder+"/Twiki_*")

for Category in DefineTags.ReturnSampleCategory():
    SubList = []
    for entry in TwikiList:

        if Category in entry:
            if Category == "TTbar" and "TTbarX" in entry:
                continue
            elif Category == "TTbar" and "TTbarBulk" in entry:
                continue

            SubList.append(entry)
    SubList = sorted(SubList)

    SubTwikiFileName = twikiFolder+"/MainTwiki_"+Category+".twiki"
    SubTwikiFile     = open(SubTwikiFileName, "w")

    for SubListEntry in SubList:
        fInFile     = open(SubListEntry, 'r')
        fInputLines = fInFile.readlines()
        for line in fInputLines:
            SubTwikiFile.write(line)
        SubTwikiFile.write("<br> \n")
        fInFile.close()

os.system("cp "+twikiFolder+"/MainTwiki_* "+"/afs/cern.ch/user/a/atltopmc/www/TopMC12twiki/TestCentralMC15ProductionPage/")


