#! /usr/bin/python
import os
import threading
import glob
import logging

import StatusFiles
import MakeInputTables

import pyAMI
import pyAMI.client
import pyAMI.atlas.api


log = logging.getLogger("myCentralPageLogger")


class InputTablesMT (threading.Thread):

    def __init__(self, threadID, q, twikiFolder, fileEVNT, fileAOD, fileDAOD):
        log.debug('[threadID %d] - Init', threadID)

        threading.Thread.__init__(self)
        self.threadID = threadID
        self.q = q
        self.twikiFolder = twikiFolder
        self.fileEVNT = fileEVNT
        self.fileAOD = fileAOD
        self.fileDAOD = fileEVNT

    def run(self):
        while True:
            log.debug('[threadID %d] - Run', self.threadID)

            subFolder = self.q.get()
            self.process_table(subFolder)

            log.debug('[threadID %d] - Done', self.threadID)

            self.q.task_done()

    def process_table(self, subFolder):

        log.debug('[threadID %d] - in process_table()', self.threadID)

        fileList = glob.glob(subFolder+"/StatusFile*")

        # now loop over file list and find file with largest stats:
        helpFile = "bla"
        helpEvents = -1000
        dsid = -1000

        for file in fileList:
            allInfo = StatusFiles.StatusFileReader(file)
            events = allInfo.totalEvents
            dsid = allInfo.DSID

            if "NULL" == events:
                events = 0

            if "defined" in events:
                continue

            if int(events) > int(helpEvents):
                helpFile = file
                helpEvents = events

        twikiFile = MakeInputTables.fillTables(helpFile, subFolder, dsid, self.twikiFolder, self.fileEVNT, self.fileAOD, self.fileDAOD)

        return
