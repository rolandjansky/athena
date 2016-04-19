# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys
import logging
from HLTTestApps.processor import Processor
from HLTTestApps.configuration import configuration, run_number_error
from HLTTestApps.option import file_opt_spec, emon_opt_spec

def file_based(cli_args):
  processor = None
  
  config = configuration(file_opt_spec, cli_args)
  return_code=0
  
  try:
    logging.info('Instantiating and loading framework...')
    
    processor = Processor(config)
    processor.go()
  except run_number_error, error:
    logging.fatal(error)
    logging.info('I will try to shutdown cleanly')
    return_code=1
  except BaseException, e:
    logging.fatal('Caught an untreated exception - %s: %s' % 
                  (e.__class__.__name__, e))
    import traceback
    traceback.print_exc()
    logging.info('I will try to shutdown cleanly')
    return_code=1
  finally:
    if processor:
      del processor
    logging.info('Exiting... Bye.')
    sys.exit(return_code)

  
