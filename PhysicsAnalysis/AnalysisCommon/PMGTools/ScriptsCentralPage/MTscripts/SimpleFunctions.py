#! /usr/bin/python
import os
import datetime
import _strptime  # workaround for datetime which is not thread safe!

import logging
log = logging.getLogger("myCentralPageLogger")


def getKeywords(keywordArray, dsid):

    for entry in keywordArray:

        log.debug('DSID %s entry %s', str(dsid), str(entry))

        if dsid in entry[0]:
            return entry[1]

    return "empty"


def hasBeenUpdated(timeStamp, days):

    date_format = "%Y-%m-%d %H:%M:%S"
    today = datetime.datetime.now()
    oldTimeStampFormat = datetime.datetime.strptime(timeStamp, date_format)
    delta = today - oldTimeStampFormat
    difference = delta.days

    if difference < days:
        return True
    else:
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
            if string not in entry[0]:
                allAvailable = False
        if allAvailable:
            Array.append(entry[0])
    return Array


def WriteListToTxt(List, FileName):
    fOutFile = open(FileName, 'w')
    for line in List:
        line = line.replace("mc15_13TeV:", "")
        line = line.replace("mc16_13TeV:", "")
        fOutFile.write(line+"\n")
    fOutFile.close()


def WriteArraysToTxt(BigArray, FileName):
    # write array of arrays into txt file
    fOutFile = open(FileName, 'w')
    for array in BigArray:
        for entry in array:
            name = entry["ldn"]
            events = entry["events"]
            modif = entry["modified"]

            log.debug('name %s events %s modified %s', str(name), str(events), str(modif))

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
        # print FileName,"  ",array
        # if len(array) != 3 and len(array) != 1 and len(array) !=4:
        #    continue
        Array.append(array)

    return Array


def GetArraysFromTxtWithString(FileName, searchString):
    fInFile = open(FileName, 'r')
    # print FileName
    fInputLines = fInFile.readlines()
    fInFile.close()

    Array = []

    # print "searchstring = ",searchString

    for line in fInputLines:
        newLine = line.replace("\n", "")
        array = newLine.split("\t")
        # print "Simple functions = ",array
        if len(array) != 3 and len(array) != 1:
            continue
        if searchString not in array[0]:
            continue
        Array.append(array)

        # print array

    return Array


def MakeFolder(FolderName):
    if not os.path.isdir(FolderName):
        os.system("mkdir "+FolderName)


def GiveFilesWithSpecialStatus(Folder, Status):
    FileList = []
    for localFile in os.listdir(Folder):
        file = Folder+"/"+localFile
        if ".txt" not in file:
            continue
        with open(file) as f:
            if any(x in f.read() for x in Status):
                # if Status in f.read():
                FileList.append(file)
    return FileList


def GetHigherOrderXsecInfo(XsecTag):
    fInFile = open("InputInformation/CrossSectionReference", 'r')
    fInputLines = fInFile.readlines()
    fInFile.close()
    Xsec = ""
    Link = ""
    OutputArray = ["", ""]
    for line in fInputLines:
        line = line.replace("\n", "")
        array = line.split(",")
        # print array,"   ",XsecTag
        if XsecTag == array[0]:
            Xsec = array[1]
            Link = array[2]
            OutputArray = [Xsec, Link]

    return OutputArray
