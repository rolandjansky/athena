#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger translator to setup arguments for athenaHLT
# @details: to be used with Trig_tf_reco.py and trigRecoExe.py 
# @author: Mark Stockton

import imp

def main(runArgs):
  option = {}
  if hasattr(runArgs,'inputBS_RDOFile'):
     option['file'] = runArgs.inputBS_RDOFile
  if hasattr(runArgs,'outputBSFile'):
     option['save-output'] = runArgs.outputBSFile
  if hasattr(runArgs,'maxEvents'):
    option['number-of-events'] = runArgs.maxEvents
  if hasattr(runArgs,'skipEvents'):
    option['skip-events'] = runArgs.skipEvents
  if hasattr(runArgs,'precommand'):
    option['precommand'] = runArgs.precommand

  if hasattr(runArgs,'jobOptionSvcType'):
    option['joboptionsvc-type'] = runArgs.jobOptionSvcType
  if hasattr(runArgs,'useDB'):
    option['use-database'] = runArgs.useDB
  if hasattr(runArgs,'DBtype'):
    option['db-type'] = runArgs.DBtype
  if hasattr(runArgs,'DBserver'):
    option['db-server'] = runArgs.DBserver
  if hasattr(runArgs,'DBsmkey'):
    option['db-smkey'] = runArgs.DBsmkey
  if hasattr(runArgs,'DBhltpskey'):
    option['db-hltpskey'] = runArgs.DBhltpskey
  if hasattr(runArgs,'DBextra'):
    option['db-extra'] = eval(runArgs.DBextra)
  
  return option
  
if __name__ == '__main__':
    main()
