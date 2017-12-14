#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-
# A Job Transformation builder class. The class reads a configuration file and builds
# corresponding job transformation commands .
# Supported commands are Generate_tf.py','AtlasG4_trf.py','Digi_trf.py','Reco_trf.py'

# Configuration files have sections and each section has several settings.
# Section names should be 'AtlasG4_trf.py','Digi_trf.py','Reco_trf.py' Not all sections have to be implemented in a cfg file

# To-Do:
# if a section is absent and the command corresponding to this section is tried to be built an error should be thrown
# implement a reverse enginnering method to succesfully build commands from AMI's GetCommand.py (remember GetCommand.py returns some wrong chars such as whitespace etc...)

# author : Serhat Istin
# ....:istin@cern.ch

import ConfigParser
import string
import copy
import os


class TrfBuilder:

  # implement a constructor

    def __init__(self, cfgFile):  # construct with a config file
        self.filename = cfgFile
        self.trfCommands = ['Generate_tf.py', 'AtlasG4_trf.py',
                            'Digi_tf.py', 'Reco_tf.py']
        self.DigiRecoConfig = ConfigParser.ConfigParser()
        self.commandList = []
        if not os.path.isfile(cfgFile):  # check if file exists first
            print 'TrfBuilder: ERROR:: ConfigFile ' + cfgFile \
                + ' Does Not Exist'
            os.sys.exit(-1)

        self.DigiRecoConfig.optionxform = str  # set this flag in order to use upper-lower case distinction
        self.DigiRecoConfig.read(cfgFile)  # parse the configuration file

  # implement a function which reads the corresponding settings block from tyhe cfg file and returns the command

    def buildCommand(self, sectionName):
        pfx = copy.copy(sectionName)  # copy it not to alter sectionName as python uses ref variables
        pfx += ' '
        sectionCounter = 0
        if sectionName in self.trfCommands:
            for section in self.DigiRecoConfig.sections():  # loop over all sections in the file
                if section == sectionName:  # take only our section for sectionName into account
                    sectionCounter = sectionCounter + 1
                    for option in self.DigiRecoConfig.options(section):  # loop over all settings in specified our section
                        # to support multiple values --preExec "AAA:aaa" "BBB:bbb"
                        pfx += ' --' + option + ' ' \
                            + self.DigiRecoConfig.get(section, option) \
                            + ' '
            if sectionCounter == 0:
                print 'TrfBuilder: ERROR:: File Error .No such section: ' \
                    + sectionName + ' in the file: ' + self.filename
                os.sys.exit(-1)
        else:

         # throw an error if something non-sense is supplied by the user

            print 'TrfBuilder: ERROR:: Invalid Command Type: ' \
                + sectionName
            os.sys.exit(-1)
        self.commandList.append(pfx)
        return pfx

  # implement a method to dump all sections to the screen

    def printSections(self):
        for section in self.DigiRecoConfig.sections():
            print section

  # implement a method to write the command into a file

    def write(self, sectionName, filename):
        command = self.buildCommand(sectionName)
        file = open(filename, 'w')
        file.write(command)

    def buildAll(self):
        for sectionName in self.DigiRecoConfig.sections():
            self.buildCommand(sectionName)

    def writeAll(self, filename):
        file = open(filename, 'w')
        self.filename.write(comandList)


