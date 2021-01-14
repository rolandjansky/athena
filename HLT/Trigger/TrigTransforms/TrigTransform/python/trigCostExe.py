#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @brief: Cost executor to call base transforms
# @details: Based on athenaExecutor with some modifications
# @author: Mark Stockton

import os

from PyJobTransforms.trfExe import athenaExecutor
import PyJobTransforms.trfExceptions as trfExceptions
from PyJobTransforms.trfExitCodes import trfExit as trfExit

# Setup logging here
import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

class trigCostExecutor(athenaExecutor):
    # Trig_reco_tf.py executor for cost monitoring
    # used to change the output filenames

    def postExecute(self):

        msg.info("Check for trig_cost.root file")
        # costmon generates the file trig_cost.root
        # to save on panda it needs to be renamed via the outputNTUP_TRIGCOSTFile argument
        expectedFileName = 'trig_cost.root'
        # first check argument is in dict
        if 'outputNTUP_TRIGCOSTFile' in self.conf.argdict:
            # check file is created
            if(os.path.isfile(expectedFileName)):
                msg.info('Renaming %s to %s', expectedFileName, self.conf.argdict['outputNTUP_TRIGCOSTFile'].value[0])
                try:
                    os.rename(expectedFileName, self.conf.argdict['outputNTUP_TRIGCOSTFile'].value[0])
                except OSError as e:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                                    'Exception raised when renaming {0} to {1}: {2}'.format(expectedFileName, self.conf.argdict['outputNTUP_TRIGCOSTFile'].value[0], e))
            else:
                msg.error('NTUP_TRIGCOST argument defined %s but %s not created', self.conf.argdict['outputNTUP_TRIGCOSTFile'].value[0], expectedFileName)
        else:
            msg.info('NTUP_TRIGCOST argument not defined so skip %s check', expectedFileName)

        msg.info("Check for trig_rate.root file")
        # costmon generates the file trig_rate.root
        # to save on panda it needs to be renamed via the outputNTUP_TRIGRATEFile argument
        expectedFileName = 'trig_rate.root'
        # first check argument is in dict
        if 'outputNTUP_TRIGRATEFile' in self.conf.argdict:
            # check file is created
            if(os.path.isfile(expectedFileName)):
                msg.info('Renaming %s to %s', expectedFileName, self.conf.argdict['outputNTUP_TRIGRATEFile'].value[0])
                try:
                    os.rename(expectedFileName, self.conf.argdict['outputNTUP_TRIGRATEFile'].value[0])
                except OSError as e:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                                    'Exception raised when renaming {0} to {1}: {2}'.format(expectedFileName, self.conf.argdict['outputNTUP_TRIGRATEFile'].value[0], e))
            else:
                msg.error('NTUP_TRIGRATE argument defined %s but %s not created', self.conf.argdict['outputNTUP_TRIGRATEFile'].value[0], expectedFileName)
        else:
            msg.info('NTUP_TRIGRATE argument not defined so skip %s check', expectedFileName)

        msg.info("Check for trig_ebweight.root file")
        # costmon generates the file trig_ebweight.root
        # to save on panda it needs to be renamed via the outputNTUP_TRIGEBWGHTFile argument
        expectedFileName = 'trig_ebweight.root'
        # first check argument is in dict
        if 'outputNTUP_TRIGEBWGHTFile' in self.conf.argdict:
            # check file is created
            if(os.path.isfile(expectedFileName)):
                msg.info('Renaming %s to %s', expectedFileName, self.conf.argdict['outputNTUP_TRIGEBWGHTFile'].value[0])
                try:
                    os.rename(expectedFileName, self.conf.argdict['outputNTUP_TRIGEBWGHTFile'].value[0])
                except OSError as e:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                                    'Exception raised when renaming {0} to {1}: {2}'.format(expectedFileName, self.conf.argdict['outputNTUP_TRIGEBWGHTFile'].value[0], e))
            else:
                msg.error('NTUP_TRIGEBWGHT argument defined %s but %s not created', self.conf.argdict['outputNTUP_TRIGEBWGHTFile'].value[0], expectedFileName)
        else:
            msg.info('NTUP_TRIGEBWGHT argument not defined so skip %s check', expectedFileName)

        msg.info('Now run athenaExecutor:postExecute')
        super(trigCostExecutor, self).postExecute()
