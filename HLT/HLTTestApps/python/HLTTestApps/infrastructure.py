# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Sep 30, 2013

@author: ricab
'''

import os, re, shutil, logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

class infrastructure(object):
  """ 
  Base infrastructure class. This class is intended as an abstract class and
  exists mostly for documentation purposes, so that it is apparent which methods
  are expected from an infrastructure like object. 
  """
  NAME = 'infrastructure'
  def __init__(self, config):
    logging.info("Initializing infrastructure")
    self.config = config
  def configure(self):
    logging.info("Configuring infrastructure")
    return True
  def connect(self):
    logging.info("Connecting infrastructure")
    return True
  def prepareForRun(self):
    logging.info("Preparing infrastructure")
    return True
  def prepareWorker(self):
    logging.info("Preparing worker infrastructure")
    return True
  def run(self):
    logging.info("Running infrastructure")
    return True
  def stopRun(self):
    logging.info("Stopping infrastructure")
    return True
  def finalizeWorker(self):
    logging.info("Finalizing worker infrastructure")
    return True
  def disconnect(self):
    logging.info("Disconnecting infrastructure")
    return True
  def unconfigure(self):
    logging.info("Unconfiguring infrastructure")
    return True
  def __del__(self):
    pass

class offline_infrastructure(infrastructure):
  NAME = 'offline infrastructure'
  def __init__(self, config):
    infrastructure.__init__(self, config)
    
def build_infrastructure(config):
  if config['oh-monitoring']:
    from online_infrastructure import online_infrastructure
    return online_infrastructure(config)
  else:
    logging.debug("Creating offline infrastructure")
    return offline_infrastructure(config)
  

################################################################################
#################################### Tests #####################################
################################################################################

import unittest, signal
from HLTTestApps import ptree
from configuration import configuration, dummy_configuration
from option import file_opt_spec

class infrastructure_transitions_test(unittest.TestCase):
  class _dummy_infrastructure(infrastructure):
    def __init__(self):
      infrastructure.__init__(self, dummy_configuration())
  def setUp(self):
    self.cli_args = ["-n", '10', "-f", filelist[0],
                     'TrigExMTHelloWorld/MTHelloWorldOptions.py']
  def _testInfrastructureTransitions(self, infrastruct):
    self.assertTrue(infrastruct.configure())
    self.assertTrue(infrastruct.connect())
    self.assertTrue(infrastruct.prepareForRun())
    self.assertTrue(infrastruct.prepareWorker())
    self.assertTrue(infrastruct.run())
    self.assertTrue(infrastruct.stopRun())
    self.assertTrue(infrastruct.disconnect())
    self.assertTrue(infrastruct.unconfigure())
  def test_infrastructure(self):
    config = configuration(file_opt_spec, self.cli_args)
    infras = build_infrastructure(config)
    self._testInfrastructureTransitions(infras)
    infras.__del__()
  def test_simple(self):
    self._testInfrastructureTransitions(self._dummy_infrastructure())

if __name__ == '__main__':
  from HLTTestApps import test_main
  test_main()
  
