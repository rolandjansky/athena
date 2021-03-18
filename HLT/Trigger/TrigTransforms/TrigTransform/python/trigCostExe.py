#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @brief: Simple executor to call RunTrigCostAnalysis.py
# @details: Trig_reco_tf.py executor for cost processing
# @author: Mark Stockton

from PyJobTransforms.trfExe import scriptExecutor

# Setup logging here
import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

class trigCostExecutor(scriptExecutor):

    def preExecute(self, input = set(), output = set()):

        # Build up the command line: RunTrigCostAnalysis.py Input.Files=inputDRAW_TRIGCOSTFile --outputHist=outputNTUP_TRIGCOSTFile
        # All arguments have to be provided for step to be called
        # inputDRAW_TRIGCOSTFile can be multiple files?
        self._cmd = [self._exe]
        self._cmd.extend(['Input.Files=' + str(self.conf.dataDictionary['DRAW_TRIGCOST'].value) ])
        self._cmd.extend(['--outputHist='+self.conf.argdict['outputNTUP_TRIGCOSTFile'].value[0]])

        super(trigCostExecutor, self).preExecute()
