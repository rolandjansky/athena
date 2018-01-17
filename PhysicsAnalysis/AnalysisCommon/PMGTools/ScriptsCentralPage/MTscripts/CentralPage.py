#! /usr/bin/python

import glob
import os

import pyAMI
import pyAMI.client
import pyAMI.atlas.api

import Queue
import threading

import SimpleFunctions
import StatusFiles
import StatusFilesMT
import DefineTags
import GetAmiInfo
import MakeInputTables
import MakeInputTablesMT

import logging
log = logging.getLogger("myCentralPageLogger")


class CentralPageMaker(object):
    # This class handles the core work behind the central page: reading from
    # AMI, dumping the information in StatusFiles and writing the twikis

    def __init__(self, mcProd, dsidSet, mainFolder):
        # Init of CentralPageMaker. The data members are mainly the target
        # folders and dataset lists. Can be streamlined in the future.

        log.debug('Init of CentralPage Class')

        self.mcProd = mcProd
        self.dsidSet = dsidSet

        self.mainFolder = mainFolder
        SimpleFunctions.MakeFolder(self.mainFolder)
        self.twikiFolder = mainFolder+"/AllSampleInfo/twikiPages/"
        SimpleFunctions.MakeFolder(self.twikiFolder)

        self.inputLists = glob.glob(mainFolder+"/InputInformation/ExtraInfo_*")

        summaryFolder = mainFolder+"/AllSampleInfo/SummaryInfo/"
        SimpleFunctions.MakeFolder(summaryFolder)

        self.listEVNTfile = summaryFolder+"ListEVNT_"+mcProd+"."+dsidSet+".txt"
        self.listAODfile = summaryFolder+"ListAOD_"+mcProd+"."+dsidSet+".txt"
        self.listDAODfile = summaryFolder+"ListDAOD_"+mcProd+"."+dsidSet+".txt"
        self.reducedListfile = summaryFolder+"ListReduced_"+mcProd+"."+dsidSet+".txt"
        self.keywordFile = summaryFolder+"KeywordList.txt"

        self.arrayEVNT = []
        self.arrayAOD = []
        self.arrayDAOD = []

        self.fullDSIDList = []
        self.fullDSIDInfoEVNT = []
        self.reducedList = []

        self.folderList = []

        self.client = pyAMI.client.Client('atlas')
        pyAMI.atlas.api.init()

    def run(self):

        log.info('==> Retrieving AMI container info ... ')

        self.getAmiArrays()

        log.info('==> Retrieving DSID list ... ')

        self.fullDSIDList = self.GetDSIDs(self.arrayAOD)
        log.debug('Total number of DSIDs: %d', len(self.fullDSIDList))

        log.info('==> Computing reduced DSID list ... ')

        self.computeReducedList()
        fullDSIDInfo = self.GetFullList(self.inputLists, self.fullDSIDList)
        self.fullDSIDInfoEVNT = self.MatchEVNT(fullDSIDInfo, self.reducedList)
        log.info('Number of DSIDs to process: %d', len(self.fullDSIDInfoEVNT))

        log.info('==> Generating Status files ... ')

        self.generateStatusFilesMT()

        log.info('==> Generating Twiki files ... ')

        self.folderList = glob.glob(self.mainFolder+"/AllSampleInfo/DSID_"+self.dsidSet+"*/")
        self.generateTwikiFilesMT()

        log.info('==> Merging Twiki files ... ')

        self.mergeTwikiFiles()

        return

    def getAmiArrays(self):

        log.debug('=> Get all EVNT File info... ')
        superArrayEVNT = []
        amiInfo_EVNT1 = pyAMI.atlas.api.list_datasets(self.client, self.mcProd+"."+self.dsidSet+"%evgen.EVNT%/", fields="events,modified")
        superArrayEVNT.append(amiInfo_EVNT1)
        if self.mcProd == "mc16_13TeV":
            amiInfo_EVNT2 = pyAMI.atlas.api.list_datasets(self.client, "mc15_13TeV."+self.dsidSet+"%evgen.EVNT%/", fields="events,modified")
            superArrayEVNT.append(amiInfo_EVNT2)
        SimpleFunctions.WriteArraysToTxt(superArrayEVNT, self.listEVNTfile)

        log.debug('=> Get all AOD File info... ')
        superArrayAOD = []
        amiInfo_AOD = pyAMI.atlas.api.list_datasets(self.client, self.mcProd+"."+self.dsidSet+"%merge.AOD%/", fields="events,modified")
        superArrayAOD.append(amiInfo_AOD)
        SimpleFunctions.WriteArraysToTxt(superArrayAOD, self.listAODfile)

        log.debug('=> Get all DAOD File info... ')
        superArrayDAOD = []
        amiInfo_DAOD = pyAMI.atlas.api.list_datasets(self.client, self.mcProd+"."+self.dsidSet+"%deriv.DAOD_%/", fields="events,modified")
        superArrayDAOD.append(amiInfo_DAOD)
        SimpleFunctions.WriteArraysToTxt(superArrayDAOD, self.listDAODfile)

        # TODO: include option for caching
        self.arrayEVNT = SimpleFunctions.GetArraysFromTxt(self.listEVNTfile)
        self.arrayAOD = SimpleFunctions.GetArraysFromTxt(self.listAODfile)
        self.arrayDAOD = SimpleFunctions.GetArraysFromTxt(self.listDAODfile)

        return

    def computeReducedList(self):

        if not os.path.exists(self.reducedListfile):

            for DSID in self.fullDSIDList:

                newFolder = self.mainFolder+"/AllSampleInfo/DSID_"+str(DSID[0:3])+"XXX/"
                subFolder = newFolder+str(DSID)

                SimpleFunctions.MakeFolder(newFolder)
                SimpleFunctions.MakeFolder(subFolder)

                # now make list with all relevant derivations
                derivFileName = subFolder+"/List_Derivations.txt"
                listDeriv = SimpleFunctions.GetAllFilesWithStrings(self.arrayDAOD, [DSID])
                # print ListDeriv
                SimpleFunctions.WriteListToTxt(listDeriv, derivFileName)

                # now get all EVNT files and the latest etag
                listEVNT = SimpleFunctions.GetAllFilesWithStrings(self.arrayEVNT, [DSID])

                for entry in listEVNT:
                    dataset = entry
                    self.reducedList.append(dataset)

            SimpleFunctions.WriteListToTxt(self.reducedList, self.reducedListfile)

        else:
            self.reducedList = SimpleFunctions.GetArraysFromTxt(self.reducedListfile)

        return

    def generateStatusFilesMT(self):

        keywordArray = SimpleFunctions.GetArraysFromTxt(self.keywordFile)

        workQueue = Queue.Queue(0)  # 0 equals infinity

        # Create new threads
        num_worker_threads = 100
        for i in range(num_worker_threads):
            thread = StatusFilesMT.myStatusFileMT(i, workQueue, keywordArray, self.twikiFolder, self.mainFolder)
            thread.daemon = True
            thread.start()

        # Fill the queue
        for entry in self.fullDSIDInfoEVNT:

            if len(entry) == 1:
                if entry[0] != '\n':
                    log.error('ERROR: entry has length '+str(len(entry)))
                    log.error('Entry: ', entry)
                    continue
                else:
                    continue

            workQueue.put(entry)

        # Wait for all threads to complete
        workQueue.join()
        log.debug('Exiting MT Status file')

        return

    def generateTwikiFilesMT(self):

        twikiQueue = Queue.Queue(0)  # 0 equals infinity

        # Create new threads
        num_worker_threads = 10
        for i in range(num_worker_threads):
            thread = MakeInputTablesMT.InputTablesMT(i, twikiQueue, self.twikiFolder, self.listEVNTfile, self.listAODfile, self.listDAODfile)
            thread.daemon = True
            thread.start()

        # Fill the queue
        for helpFolder in self.folderList:
            subFolderList = glob.glob(helpFolder+"/*")
            for subFolder in subFolderList:
                twikiQueue.put(subFolder)

        # Wait for all threads to complete
        twikiQueue.join()
        log.debug('Exiting MT Twiki file')

        return

    def mergeTwikiFiles(self):

        # now read all plain txt files and make the tables for the twiki
        twikiList = glob.glob(self.twikiFolder+"/Twiki_*")

        for category in DefineTags.ReturnSampleCategory():
            subList = []
            for entry in twikiList:

                if category in entry:
                    if category == "TTbar" and "TTbarX" in entry:
                        continue
                    elif category == "TTbar" and "TTbarBulk" in entry:
                        continue

                    subList.append(entry)
            subList = sorted(subList)

            subTwikiFileName = self.twikiFolder+"/MainTwiki_"+category+".twiki"
            subTwikiFile = open(subTwikiFileName, "w")

            for subListEntry in subList:
                fInFile = open(subListEntry, 'r')
                fInputLines = fInFile.readlines()
                for line in fInputLines:
                    subTwikiFile.write(line)
                subTwikiFile.write("<br> \n")
                fInFile.close()

        return

    def GetDSIDs(self, Array):
        # loop over EVNT files and find all possible DSIDs. after: collect
        # the info from the info container and return info as array
        DSIDList = []
        prefix = self.mcProd+"."
        for array in Array:
            sample = array[0]
            DSID = sample[len(prefix)+0:len(prefix)+6]

            inList = False
            for entry in DSIDList:
                if str(entry) == str(DSID):
                    inList = True
            if not inList:
                DSIDList.append(DSID)

        return DSIDList

    def GetFullList(self, inputLists, fDSIDList):

        fullDSIDList = []
        for List in inputLists:
            fInFile = open(List, 'r')
            fInputLines = fInFile.readlines()
            fInFile.close()
            for line in fInputLines:
                array = line.split(",")
                fullDSIDList.append(array)

        for entry in fDSIDList:
            entry = entry.replace("\n", "")
            inList = False
            for entry2 in fullDSIDList:
                if str(entry) == str(entry2[0]):
                    inList = True
            if not inList:
                HelpList = [entry, "", "", "", "", "", "", "", ""]
                fullDSIDList.append(HelpList)

        return fullDSIDList

    def MatchEVNT(self, fullDSIDInfo, reducedList):
        newArray = []

        for dsidInfo in fullDSIDInfo:
            helpArray = dsidInfo

            if not len(helpArray) == 9 and not len(helpArray) == 1:
                log.error('Wrong helpArray length (%d)!!!!', len(helpArray))

            # loop over all EVNT files
            for entry in reducedList:

                newHelpArray = []

                if helpArray[0] in entry:

                    if len(helpArray) < 9:
                        continue

                    for i in range(0, 9):
                        newHelpArray.append(helpArray[i])

                    newHelpArray.append(entry)
                    newArray.append(newHelpArray)

        return newArray
