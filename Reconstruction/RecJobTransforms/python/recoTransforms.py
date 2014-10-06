# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Specialist reconstruction and bytestream transforms
#  @author atlas-comp-jt-dev@cern.ch
#  @version $Id: recoTransforms.py 611371 2014-08-12 09:21:15Z seuster $

import os
import re
import subprocess

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfExceptions as trfExceptions

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfExe import scriptExecutor

## @brief Executor for skimming RAW events according to a list of run and event
#  numbers
class skimRawExecutor(scriptExecutor):

    def preExecute(self, input = set(), output = set()):
        # First we need to strip the filter file down to events that are present 
        # in the RAW file we are going to skim. This is because the HI workflow
        # will provide millions of events in their filter file, more than acmd.py
        # can cope with.
        listEvtCommand = ['AtlListBSEvents.exe', '-l']
        listEvtCommand.extend(self.conf.argdict['inputBSFile'].value)
        # For best lookup speed, we store the runnumber/eventnumber in a dictionary (set would also
        # be fast)
        rawEventList = {} 
        try:
            for line in subprocess.check_output(listEvtCommand).split("\n"):
                if line.startswith("Index="):
                    try:
                        splitStrings = line.split(" ")
                        runprefix, runstr = splitStrings[1].split("=")
                        evtprefix, evtstr = splitStrings[2].split("=")
                        # Check sanity
                        if runprefix != "Run" or evtprefix != "Event":
                            msg.warning("Failed to understand this line from AtlListBSEvents: {0}".format(line))
                        else:
                            runnumber = int(runstr)
                            evtnumber = int(evtstr)
                            # We build up a string key as "RUN-EVENT", so that we can take advantage of
                            # the fast hash search against a dictionary 
                            rawEventList[runstr + "-" + evtstr] = True
                            msg.debug("Identified run {0}, event {1} in input RAW files".format(runstr, evtstr))
                    except ValueError, e:
                        msg.warning("Failed to understand this line from AtlListBSEvents: {0}".format(line))
        except subprocess.CalledProcessError, e:
            errMsg = "Call to AtlListBSEvents.exe failed: {0}".format(e)
            msg.error(erMsg)
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode("TRF_EXEC_SETUP_FAIL"), errMsg)
        msg.info("Found {0} events as skim candidates in RAW inputs".format(len(rawEventList)))
        
        # Now open the list of filter events, and check through them
        slimmedFilterFile = "slimmedFilterFile.{0}".format(os.getpid())
        with open(slimmedFilterFile, "w") as slimFF, open(self.conf.argdict['filterFile'].value) as masterFF:
            count = 0
            for line in masterFF:
                try:
                    runstr, evtstr = line.split()
                    if runstr + "-" + evtstr in rawEventList:
                        msg.debug("Found run {0}, event {1} in master filter list".format(runstr, evtstr))
                        os.write(slimFF.fileno(), line)
                        count += 1
                except ValueError, e:
                    msg.warning("Failed to understand this line from master filter file: {0} {1}".format(line, e))
            if count == 0:
                # If there are no matched events, create a bogus request for run and event 0 to keep
                # AtlCopyBSEvent.exe CLI
                msg.info("No events matched in this input file - empty RAW file output will be made")
                os.write(slimFF.fileno(), "0 0\n")
        msg.info("Matched {0} lines from the master filter file against input events; wrote these to {1}".format(count, slimmedFilterFile))
        
        # Build up the right command line for acmd.py
        self._cmd = ['acmd.py', 'filter-files']
        
        self._cmd.extend(('-o', self.conf.argdict['outputBS_SKIMFile'].value[0]))
        self._cmd.extend(('-s', slimmedFilterFile))
        self._cmd.extend(self.conf.argdict['inputBSFile'].value)

        super(skimRawExecutor, self).preExecute()

