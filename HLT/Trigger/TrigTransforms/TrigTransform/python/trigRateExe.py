#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @brief: Simple executor to call RatesAnalysisFullMenu.py
# @details: Trig_reco_tf.py executor for rate processing
# @author: Mark Stockton

from PyJobTransforms.trfExe import scriptExecutor

# Setup logging here
import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

class trigRateExecutor(scriptExecutor):

    def preExecute(self, input = set(), output = set()):
    
        # Build up the command line: RatesAnalysisFullMenu.py Input.Files=inputAODFile --outputHist=outputNTUP_TRIGRATEFile
        # All arguments have to be provided for step to be called
        # inputAODFile can be multiple files
        self._cmd = [self._exe]        
        self._cmd.extend(['Input.Files=' + str(self.conf.argdict['inputAODFile'].value) ])
        self._cmd.extend(['--outputHist='+self.conf.argdict['outputNTUP_TRIGRATEFile'].value[0]])
    
        super(trigRateExecutor, self).preExecute()
