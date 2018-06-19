#! /usr/bin/python

import os
import threading
import logging

log = logging.getLogger("myCentralPageLogger")


class StoreMetaData(threading.Thread):

    def __init__(self, threadID, q):

        threading.Thread.__init__(self)
        self.threadID = threadID
        self.q = q

    def run(self):
        while True:

            entry = self.q.get()
            self.process_data(entry)
            self.q.task_done()

    def process_data(self, entry):

        inputList = entry[0]
        outFile_meta = entry[1]
        query_fields = entry[2]

        cmd = "getMetadata.py --allowBadStatus --inDS="+inputList+" --outFile="+outFile_meta+" --delim=\";;;\" --fields="+query_fields

        os.system(cmd)
