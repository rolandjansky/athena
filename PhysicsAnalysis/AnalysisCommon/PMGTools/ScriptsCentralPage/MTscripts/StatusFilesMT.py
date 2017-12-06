#! /usr/bin/python

import os
import threading
import logging

import SimpleFunctions
import StatusFiles
import DefineTags
import GetAmiInfo

import pyAMI
import pyAMI.client
import pyAMI.atlas.api

log = logging.getLogger("myCentralPageLogger")


class myStatusFileMT (threading.Thread):
    def __init__(self, threadID, q, keywordArray, twikiFolder, mainFolder):
        log.debug('[threadID %d] - Init', threadID)

        threading.Thread.__init__(self)
        self.threadID = threadID
        self.q = q
        self.keywordArray = keywordArray
        self.twikiFolder = twikiFolder
        self.mainFolder = mainFolder

        self.client = pyAMI.client.Client('atlas')
        pyAMI.atlas.api.init()

    def run(self):
        while True:
            log.debug('[threadID %d] - Run', self.threadID)

            entry = self.q.get()
            self.process_data(entry)

            log.debug('[threadID %d] - Done', self.threadID)

            self.q.task_done()

    def process_data(self, entry):

        log.debug('[threadID %d] - in process_data()', self.threadID)

        dsid = entry[0]
        xsec = entry[1]
        kfactor = entry[2]
        filterEff = entry[3]
        branchingRatio = entry[4]
        higherOrderXsec = entry[5]
        sortingTag = entry[6]
        comment = entry[7]
        title = entry[8]
        container = entry[9]

        if len(entry) == 11:
            container = entry[10]

        log.debug('[threadID %d] - container %s', self.threadID, container)

        if sortingTag == "":
            Keywords = SimpleFunctions.getKeywords(self.keywordArray, dsid)
            if Keywords == "empty" or Keywords == "NULL":
                # this is slow and should be kept to the minumum by updating the Keyword lookup file
                Keywords = GetAmiInfo.getKeywords(self.client, container)

            sortingTag = DefineTags.SortKeyWordsToSampleCategory(Keywords, container)

        outputTwiki = self.twikiFolder+"/Twiki_"+sortingTag+"_"+str(dsid)+".twiki"

        # if os.path.exists(outputTwiki):
        #    continue

        StatusFile = "StatusFile_"+sortingTag+"_"+str(dsid)+"_"+container.replace("/", "")+".txt"

        NewFolder = self.mainFolder+"/AllSampleInfo/DSID_"+str(dsid[0:3])+"XXX/"+str(dsid)+"/"

        # if not os.path.exists(NewFolder+"/"+StatusFile):
        StatusFiles.StatusFileCreator(StatusFile, NewFolder, container, comment, sortingTag, xsec, kfactor, filterEff, branchingRatio, higherOrderXsec, title, dsid)

        return
