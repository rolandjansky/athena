#! /usr/bin/python
import os
import threading
import glob
import logging

import StatusFiles
import MakeInputTables

log = logging.getLogger("myCentralPageLogger")


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


class InputTablesMT (threading.Thread):

    def __init__(self, threadID, q, twikiFolder, fileEVNT, fileAOD, fileDAOD, mcProd):
        log.debug('[threadID %d] - Init', threadID)

        threading.Thread.__init__(self)
        self.threadID = threadID
        self.q = q
        self.twikiFolder = twikiFolder
        self.fileEVNT = fileEVNT
        self.fileAOD = fileAOD
        self.fileDAOD = fileDAOD
        self.production = mcProd

    def run(self):
        while True:
            log.debug('[threadID %d] - Run', self.threadID)

            subFolder = self.q.get()
            self.process_table(subFolder)

            log.debug('[threadID %d] - Done', self.threadID)

            self.q.task_done()

    def process_table(self, subFolder):

        log.debug('[threadID %d] - in process_table()', self.threadID)
        log.debug('[threadID %d] reading from %s', self.threadID, subFolder)
        log.debug('[threadID %d] writing files to %s', self.threadID, self.twikiFolder)

        fileList = glob.glob(subFolder+"/StatusFile*")
        log.debug('[threadID %d] file list size %d', self.threadID, len(fileList))

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

            if not is_number(events):
                log.warning('[threadID %d] Number of events is NAN (%s) - skipping', self.threadID, str(events))
                continue

            if int(events) > int(helpEvents):
                helpFile = file
                helpEvents = events

        if self.production == "mc15_13TeV":
            twikiFile = MakeInputTables.fillTables(helpFile, subFolder, dsid, self.twikiFolder, self.fileEVNT, self.fileAOD, self.fileDAOD)
        elif self.production == "mc16_13TeV":
            twikiFile = MakeInputTables.fillTablesMC16(helpFile, subFolder, dsid, self.twikiFolder, self.fileEVNT, self.fileAOD, self.fileDAOD)

        return
