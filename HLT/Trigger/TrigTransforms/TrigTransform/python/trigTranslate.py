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

  if hasattr(runArgs,'joboptionsvc_type'):
    option['joboptionsvc-type'] = runArgs.joboptionsvc_type
  if hasattr(runArgs,'use_database'):
    option['use-database'] = runArgs.use_database
  if hasattr(runArgs,'db_type'):
    option['db-type'] = runArgs.db_type
  if hasattr(runArgs,'db_server'):
    option['db-server'] = runArgs.db_server
  if hasattr(runArgs,'db_smkey'):
    option['db-smkey'] = runArgs.db_smkey
  if hasattr(runArgs,'db_hltpskey'):
    option['db-hltpskey'] = runArgs.db_hltpskey
  if hasattr(runArgs,'db_extra'):
    option['db-extra'] = eval(runArgs.db_extra)
  
  return option
  
if __name__ == '__main__':
    main()
