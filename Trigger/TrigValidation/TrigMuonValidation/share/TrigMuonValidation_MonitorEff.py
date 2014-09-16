#!/usr/bin/env python

from RTTXMLForTimePlotsMaker import RTTXMLForTimePlotsMaker
from RTTXMLForTimePlotsMaker import to_xml
from TestResult import TestResult
from Logger import TimedLogger

import os
import sys
import re

class TrackEfficiency(RTTXMLForTimePlotsMaker):
    def __init__(self, argDict):
        argDict['outputFile']='TrigMuonValidation/output1.xml'
        RTTXMLForTimePlotsMaker.__init__(self, argDict)

   
    def makeDataDict(self):
        # grep a log file, interrogate a database, whatever...
        eff1=0.0
        File1 = open('RttLibraryTools_ROOTMacroRunner.postprocessing.log')
        for line in File1.readlines():
            if line.startswith("Efficiency"):
                effstr = line.split()
                eff2 = effstr[1]
                eff1 = float(eff2)
        File1.close()

        # Need to ensure the method returns a dictionary
        myDictOfValues = {'EFFvsTime': (eff1, 0.0, '')}
        return myDictOfValues



