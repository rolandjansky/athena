#! /usr/bin/python

import os
import threading
import logging

import StatusFiles

log = logging.getLogger("myCentralPageLogger")


class myStatusFileMT (threading.Thread):
    def __init__(self, threadID, q, keywordArray, twikiFolder, mainFolder):
        log.debug('[threadID %d] - Init', threadID)

        threading.Thread.__init__(self)
        self.threadID = threadID
        self.q = q
        self.twikiFolder = twikiFolder
        self.mainFolder = mainFolder

    def run(self):
        while True:
            log.debug('[threadID %d] - Run', self.threadID)

            entry = self.q.get()
            self.process_data(entry)

            log.debug('[threadID %d] - Task done', self.threadID)

            self.q.task_done()

    def process_data(self, entry):

        log.debug('[threadID %d] - processing entry %s', self.threadID, entry)

        StatusFiles.StatusFileCreator(entry, self.mainFolder, self.twikiFolder)

        return
