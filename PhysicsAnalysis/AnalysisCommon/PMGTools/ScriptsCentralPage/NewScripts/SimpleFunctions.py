#! /usr/bin/python                                                                                                                                                  

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   
import subprocess,sys
import os,string
import time
import datetime


def GetKeywords(KeywordArray, DSID):
    for entry in KeywordArray:
        #print entry,"  ",DSID
        num = len(entry)

        if DSID in entry[0]:
            return entry[1]
            #Keywords = ""
            #for i in range(1, num-1):
            #    Keywords += entry[i]+"\t"
            #return Keywords

    return "empty"
        

def MatchEVNT(FullDSIDInfo, ReducedList):
    NewArray = []

    #print FullDSIDInfo

    #sys.exit()

    total = len(FullDSIDInfo)

    counter = 0

    for DSIDInfo in FullDSIDInfo:
        HelpArray = DSIDInfo

        if not len(HelpArray) == 9 and not len(HelpArray) == 1:
            print "Wrong length!!!! ======================> ",HelpArray
            #sys.exit()
            #return bla
        #else:
        #    continue
    
        # loop over all EVNT files    
            

        for entry in ReducedList:
            #print HelpArray[0],"   ",entry[0],"   ",entry
            NewHelpArray = []
            array_len = len(NewHelpArray)
            if HelpArray[0] in entry:
                
                if len(HelpArray) < 9:
                    continue

                for i in range(0, 9):
                    NewHelpArray.append(HelpArray[i])

                NewHelpArray.append(entry)

                new_len = len(NewHelpArray)

                #if (array_len) != (new_len):
                #if new_len > 10:
                #    print "============================================================================================================================================"
                    #print "Here!!!! ---> ",array_len,"  ",new_len
                    #rint "--------------------------------------------------------------------------------------------->  ", NewHelpArray
                #    print HelpArray[0],"   ",NewHelpArray,"   ",entry 
                #    print "============================================================================================================================================"
                #sys.exit()

                #print NewHelpArray
                NewArray.append(NewHelpArray)
                 
                #['361100', '', '', '1.0', '', 'XsecWplusPowPy', '', 'No lepton filter m(lnu) above 2.5 GeV and filter efficiency fixed by hand (A.Knue)', 'Powheg+Pythia8 EvtGen W(+) in enu production without lepton filter and AZNLO CT10 tune\n', 'mc15_13TeV.361100.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusenu.evgen.EVNT.e3601']

            
                #print "--------------------------------------------------------------------------------------------->  ", NewHelpArray
                
                #if counter%total == 100:
                #print counter,"  ",total

                # needs to be switched on again in case we only want one evnt file as source for xsec info
                #break

    #print len(NewArray)

    return NewArray

def HasBeenUpdated(timeStamp, days):
    date_format        = "%Y-%m-%d %H:%M:%S"
    today              = datetime.datetime.now()
    oldTimeStampFormat = datetime.datetime.strptime(timeStamp, date_format)
    delta              = today - oldTimeStampFormat
    difference         = delta.days
    if difference < days:
        return True
    
    return False

def TagsInSample(TagList, SampleName):
    for Tag in TagList:
        if Tag in SampleName:
            return True
    return False

def GetAllFilesWithStrings(ArrayDeriv, ListStrings):
    Array = []
    for entry in ArrayDeriv:
        allAvailable = True
        for string in ListStrings:
            if not string in entry[0]:
                allAvailable = False
        if allAvailable:
            Array.append(entry[0])
    return Array

def WriteListToTxt(List, FileName):
    fOutFile = open(FileName, 'w')
    for line in List:
        line = line.replace("mc15_13TeV:", "")
        fOutFile.write(line+"\n")
    fOutFile.close()

# write array of arrays into txt file
def WriteArraysToTxt(BigArray, FileName):
    fOutFile = open(FileName, 'w')
    for array in BigArray:
        for entry in array:
            name   = entry["ldn"]
            events = entry["events"]
            modif  = entry["modified"]
            #key    = entry["keyword"]
            #print name,"  ",events,"  ",modif
            fOutFile.write(name+"\t"+str(events)+"\t"+str(modif)+"\n")
    fOutFile.close()

def GetArraysFromTxt(FileName):
    fInFile = open(FileName, 'r')
    fInputLines = fInFile.readlines()
    fInFile.close()

    Array = []

    for line in fInputLines:
        newLine = line.replace("\n", "")
        array = newLine.split("\t")
        #print FileName,"  ",array
        #if len(array) != 3 and len(array) != 1 and len(array) !=4:
        #    continue
        Array.append(array)

    return Array

def GetArraysFromTxtWithString(FileName, searchString):
    fInFile = open(FileName, 'r')
    fInputLines = fInFile.readlines()
    fInFile.close()

    Array = []

    for line in fInputLines:
        newLine = line.replace("\n", "")
        array = newLine.split("\t")
        if len(array) != 3 and len(array) != 1:
            continue
        if not searchString in array[0]:
            continue
        Array.append(array)

    return Array

# loop over EVNT files and find all possible DSIDs. after: collect then the info from the info container and return info as array

def GetDSIDs(Array):  
    DSIDList = []
    prefix = "mc15_13TeV."
    for array in Array:
        sample = array[0]
        DSID   = sample[len(prefix)+0:len(prefix)+6]
        #print array,"  ", DSID
        inList = False
        for entry in DSIDList:
            if str(entry) == str(DSID):
                inList = True
        if not inList:
            DSIDList.append(DSID)

    return DSIDList

def GetFullList(InputLists, fDSIDList):
    FullDSIDList = []
    for List in InputLists:
        fInFile     = open(List, 'r')
        fInputLines = fInFile.readlines()
        fInFile.close()
        for line in fInputLines:
            array = line.split(",")
            FullDSIDList.append(array)

    timestamp = datetime.date.today()

    counter   = 0

    for entry in fDSIDList:
        counter += 1
        entry = entry.replace("\n", "")
        inList = False
        for entry2 in FullDSIDList:
            if str(entry) == str(entry2[0]):
                inList = True
        if not inList:
            HelpList = [entry, "", "", "", "", "", "", "", ""]
            FullDSIDList.append(HelpList)

    print "######## Done filling List ############"

    return FullDSIDList



def MakeFolder(FolderName):
    if not os.path.exists(FolderName):
        os.system("mkdir "+FolderName)


def GiveFilesWithSpecialStatus(Folder, Status):
    FileList = []
    for localFile in  os.listdir(Folder):
        file = Folder+"/"+localFile
        if not ".txt" in file:
            continue
        with open(file) as f:
            if any(x in f.read() for x in Status):
            #if Status in f.read():
                FileList.append(file)
    return FileList

def GetHigherOrderXsecInfo(XsecTag):
    fInFile     = open("InputInformation/CrossSectionReference", 'r')
    fInputLines = fInFile.readlines()
    fInFile.close()
    Xsec = ""
    Link = ""
    OutputArray = ["", ""]
    for line in fInputLines:
        line = line.replace("\n", "")
        array = line.split(",")
        #print array,"   ",XsecTag
        if XsecTag == array[0]:
            Xsec = array[1]
            Link = array[2]
            OutputArray = [Xsec, Link]

    return OutputArray
