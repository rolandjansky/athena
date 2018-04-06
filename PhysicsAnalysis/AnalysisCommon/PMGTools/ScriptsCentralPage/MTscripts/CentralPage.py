#! /usr/bin/python

import glob
import os

import pyAMI
import pyAMI.client
import pyAMI.atlas.api

import Queue
import threading

import SimpleFunctions
import StatusFilesMT
import DefineTags
import MakeInputTablesMT
import MetaDataMT

import logging
log = logging.getLogger("myCentralPageLogger")


class CentralPageMaker(object):
    # This class handles the core work behind the central page: reading from
    # AMI, dumping the information in StatusFiles and writing the twikis

    def __init__(self, mcProd, dsidSet, mainFolder, skipMeta, skipStatus, skipTwiki):
        # Init of CentralPageMaker. The data members are mainly the target
        # folders and dataset lists. Can be streamlined in the future.

        log.debug('Init of CentralPage Class')

        self.mcProd = mcProd
        self.dsidSet = dsidSet

        self.mainFolder = mainFolder
        SimpleFunctions.MakeFolder(self.mainFolder)

        self.skipMeta = skipMeta
        self.skipStatus = skipStatus
        self.skipTwiki = skipTwiki

        # The various folder paths should be streamlined
        self.twikiFolder = mainFolder+"/AllSampleInfo_"+self.mcProd+"/twikiPages/"
        SimpleFunctions.MakeFolder(self.twikiFolder)

        self.inputLists = glob.glob(mainFolder+"/InputInformation/ExtraInfo_*")

        summaryFolder = mainFolder+"/AllSampleInfo_"+self.mcProd+"/SummaryInfo/"
        SimpleFunctions.MakeFolder(summaryFolder)

        self.allSampleInfoFolder = self.mainFolder+"/AllSampleInfo_"+self.mcProd+"/"

        self.metaFolder = self.mainFolder+"/AllSampleInfo_"+self.mcProd+"/Metadata/"
        SimpleFunctions.MakeFolder(self.metaFolder)

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

        self.query_fields = "dataset_number,ldn,crossSection,genFiltEff,kFactor,crossSectionRef,events,processGroup,physics_comment,ecm_energy,generator_tune,production_step,prodsys_status,generator_name,type,events,ami_status,beam,pdf,atlas_release,modified,project,ami_tags,physics_short"

        self.client = pyAMI.client.Client('atlas')
        pyAMI.atlas.api.init()

    def run(self):

        if not os.path.exists(self.reducedListfile):
            log.info('==> Retrieving AMI container info ... ')
            self.getAmiArrays()

            log.info('==> Retrieving DSID list ... ')
            self.fullDSIDList = self.GetDSIDs(self.arrayEVNT)
            log.debug('Total number of DSIDs: %d', len(self.fullDSIDList))

            log.info('==> Computing reduced DSID list ... ')
            self.computeReducedList()
        else:
            self.reducedList = SimpleFunctions.GetArraysFromTxt(self.reducedListfile)

        log.info('Number of datasets to process: %d', len(self.reducedList))

        if not self.skipMeta:
            log.info('==> Storing Metadata ... ')
            self.storeMetadataMT()
        else:
            log.info('==> Skipping Metadata ... ')

        if not self.skipStatus:
            log.info('==> Generating Status files ... ')
            self.generateStatusFilesMT()
        else:
            log.info('==> Skipping Status file creation ... ')

        if not self.skipTwiki:
            log.info('==> Generating Twiki files ... ')
            self.folderList = glob.glob(self.mainFolder+"/AllSampleInfo_"+self.mcProd+"/DSID_"+self.dsidSet[:3]+"*/")
            self.generateTwikiFilesMT()
        else:
            log.info('==> Skipping twiki generation ... ')

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

        for DSID in self.fullDSIDList:

            newFolder = self.mainFolder+"/AllSampleInfo_"+self.mcProd+"/DSID_"+str(DSID[0:3])+"XXX/"
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

        return

    def storeMetadataMT(self):

        workQueue = Queue.Queue(0)  # 0 equals infinity

        # Create new threads
        num_worker_threads = 10
        for i in range(num_worker_threads):
            thread = MetaDataMT.StoreMetaData(i, workQueue)
            thread.daemon = True
            thread.start()

        n = 100  # size of metadata chunks

        # Fill the queue
        for i in range(0, len(self.reducedList), n):
            inputArray = []
            reducedArray = self.reducedList[i:i+n]

            for j in range(0, len(reducedArray)):
                inputArray.append(reducedArray[j])

            inputList = ','.join(inputArray)
            outFile_meta = self.metaFolder+"Metadata_chunk_"+str(i)+"_"+str(i+n)+".txt"

            entry = []
            entry.append(inputList)
            entry.append(outFile_meta)
            entry.append(self.query_fields)

            workQueue.put(entry)

        # Wait for all threads to complete
        workQueue.join()
        log.debug('Exiting MT Metadata retrieval')

        return

    def generateStatusFilesMT(self):

        keywordArray = SimpleFunctions.GetArraysFromTxt(self.keywordFile)

        workQueue = Queue.Queue(0)  # 0 equals infinity

        # Create new threads
        num_worker_threads = 100
        for i in range(num_worker_threads):
            thread = StatusFilesMT.myStatusFileMT(i, workQueue, keywordArray, self.twikiFolder, self.allSampleInfoFolder)
            thread.daemon = True
            thread.start()

        # Fill the queue
        listMetaDataFiles = glob.glob(self.metaFolder+"Metadata_chunk_*.txt")

        allMetaInfo = []
        key_names = self.query_fields.split(',')

        log.debug('Key Names: %s', str(key_names))

        for metaDataFile in listMetaDataFiles:
            fInFile = open(metaDataFile, 'r')
            fInputLines = fInFile.readlines()
            fInFile.close()

            for entry in fInputLines:
                line = entry.replace("\n", "")

                if line.startswith("data"):
                    continue
                if line.startswith("#") and not line.startswith("#UNKNOWN"):
                    continue
                if line == "":
                    continue

                dictionary = dict(zip(key_names, line.split(";;;")))
                workQueue.put(dictionary)

        # Wait for all threads to complete
        workQueue.join()
        log.debug('Exiting MT Status file')

        return

    def generateTwikiFilesMT(self):

        twikiQueue = Queue.Queue(0)  # 0 equals infinity

        # Create new threads
        num_worker_threads = 10
        for i in range(num_worker_threads):
            thread = MakeInputTablesMT.InputTablesMT(i, twikiQueue, self.twikiFolder, self.listEVNTfile, self.listAODfile, self.listDAODfile, self.mcProd)
            thread.daemon = True
            thread.start()

        # Fill the queue
        log.debug('Filling MT Twiki queue with %d entries', len(self.folderList))

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
            DSID = sample.split(".")[1]

            if DSID not in DSIDList:
                DSIDList.append(DSID)

        return DSIDList
