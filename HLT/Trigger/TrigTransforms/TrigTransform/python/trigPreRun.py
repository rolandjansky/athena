#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Pre-transform operations for trigger transform
# @details: Code to carry out operations that are needed before running the transform
# @author: Mark Stockton

import logging
msg = logging.getLogger(__name__)

from TrigTransform.trigUpdateArgs import trigUpdateArgs

def trigPreRun(trfMT):
    
     msg.info('Running pre-transform operations')
    
     #TODO might be useful to switch off validation if just dumping options, 
     #     better to only stop post trf validation
     ##Turn off validation for dump-options
     #if 'dump-options' in trfMT.argdict:
     #   trfMT.standardFileValidation=False

     #Do conversion of input arguments into the athenaMT/PT variable names
     trigUpdateArgs(trfMT.argdict,trfMT.parser)
     
     #Perform tasks needed to be done before running

     msg.info('Finished running pre-transform operations')
     
        
  