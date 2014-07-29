#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Wed 05 Mar 2008 02:13:05 PM CET 

from libpyhlttestapps import HLTInterface, TimeoutGuard, ptree
import time, os
import logging
import HLTTestApps
from infrastructure import build_infrastructure
from pausable_istream import PauseIterationException

def keep_processing(config, processed, total):
  """Determines if we should continue processing events or not.
  
  Two situations are forseen:
  i.  The config'ed value is < 0 and we should process all events once.
  ii. The config'ed value is != 0
  """
  return processed < (total if config < 0 else config)

class FSMTransitionError(RuntimeError): pass

class Processor:
  """Defines a "pythonic" version of the hltinterface"""

  def __init__(self, config):
    """Wrapper for HLTInterface. """
    
    self.config = config

    # Need to load libPyROOT before TrigServices (see Savannah #82614)
    import PyCintex  
    
    logging.debug1('Loading libraries %s' % ', '.join(self.libraries))
    
    self.state = 'UNKNOWN'
    self.impl = HLTInterface(self.libraries)
    self.watchdog = TimeoutGuard(config['timeout']['timeout'], 
                                 config['timeout']['warn_fraction'], 
                                 self.impl)
    self.state = 'LOADED'
    self.fatal = None
    self.infrastructure = build_infrastructure(config)
    
    # for easiness sake
    self.raw_file_name = self.use_raw_file_convention
    self.stream = config.stream
    self.debug_stage = config['debug'].lower()
    
    logging.info('Current HLT processor state is "%s"' % (self.state,))
  
  def __del__(self):
    if 'infrastructure' in self.__dict__:
      del self.infrastructure
  
  def __getattr__(self, attr):
    try:
      return self.config[attr]
    except KeyError:
      if attr[0] != '_':
        uattr = attr.replace('_', '-')
        try:
          return self.config[uattr]
        except KeyError:
          pass
    raise AttributeError, ("%s instance has no attribute '%s'" 
                           % (self.__class__.__name__, attr))
    
  def go(self):
    if self.interactive:
      self.interact()
    else:
      self.configure()
      self.connect()
      self.prepareForRun()
      self.run() 
      self.stopRun()
      self.disconnect()
      self.unconfigure()
   
  def configure(self):
    return self._transit('LOADED', 'configure', 'CONFIGURED', 
                        self.config.get_config_ptree())
        
  def connect(self):
    return self._transit('CONFIGURED', 'connect', 'CONNECTED',
                        self.config.get_connect_ptree())

  def prepareForRun(self, run_number=-1):
    if self.rewind: 
      self.stream.rewind()
    
    # we overwrite the run_number we already had only if it was specified as
    # in this latest prepareForRun
    run_number = int(run_number)
    if run_number >= 0:
      self.config['run-number'] = run_number
      
    return self._transit('CONNECTED', 'prepareForRun', 'PREPARED', 
                        self.config.get_prepare_ptree())

  def stopRun(self):
    ret = self._transit('PREPARED', 'stopRun', 'CONNECTED',
                       self.config.get_stopRun_ptree())
    return ret
  
  def disconnect(self):
    return self._transit('CONNECTED', 'disconnect', 'CONFIGURED',
                        self.config.get_disconnect_ptree())

  def unconfigure(self):
    return self._transit('CONFIGURED', 'unconfigure', 'LOADED',
                        self.config.get_unconfigure_ptree())

  def process(self, event):
    self._check_fatal() # check whether the processor is in a fatal state
    if self.max_result_size > 0:
      return self.impl.process(event, self.watchdog, self.max_result_size)
    else:
      # use the default size set in src/util.h
      return self.impl.process(event, self.watchdog)

  def kickstart(self, cfgstring):
    """Starts the processor in one shot."""
    logging.info('Kickstarting HLT processor...')
    self.configure(cfgstring)
    self.connect()
    return self.prepareForRun()

  def kickstop(self):
    """Stops the processor in one shot."""
    logging.info('Kickstopping HLT processor...')
    self.stopRun()
    self.disconnect()
    self.unconfigure()
  
  def tearDown(self):
    """
    Brings the processor to the initial state (LOADED), whatever the current
    state
    """
    for move in self.stopRun, self.disconnect, self.unconfigure:
      try: move()
      except: pass        
      
  def run(self):
      self._try_debug('run')
      self._transit_infrastructure('run')
      processed = 0
      total = len(self.stream)
      # setup the output, if required
      self.datawriter = None

      if len(self.save_output) != 0:
        self.core_name = self.save_output 
        self.directory = '.'
        self.file_in_sequence = 1
        if self.save_output.find(os.sep) != -1:
          self.directory = os.path.dirname(self.core_name)
          self.core_name = os.path.basename(self.core_name)
        self.datawriter = self.stream.datawriter(
                            self.directory, 
                            self.core_name + '-%d' % self.file_in_sequence, 
                            self.use_compression, 
                            self.raw_file_name)
        
      try:
          while keep_processing(self.number_of_events, processed, total):
              try:
                  processed = self._run_aux(processed, total)
              except PauseIterationException, upd_proc: # we can still update 
                  processed = upd_proc.args[0]          #processed
                  self.stopRun()
                  self.prepareForRun()
                  if len(self.save_output):
                    # we need a new output file now and unfortunately we cannot
                    # just close this one with nextFile() because that would 
                    # copy the old metadata. so, has to be brand new...
                    self.file_in_sequence += 1
                    self.datawriter = self.stream.datawriter(
                                self.directory,
                                self.core_name + '-%d' % self.file_in_sequence, 
                                self.use_compression, 
                                self.raw_file_name)

      except KeyboardInterrupt, upd_proc: # we can still update processed
          processed = upd_proc.args[0]
          logging.info("Keyboard interruption caught. Stopping "
                       "event processing cleanly...")
    
      logging.info('Processed %d events' % processed)
      
  def _run_aux(self, processed, total):
    try:
      for event in self.stream:
        event.check()

        # update number of read events
        processed += 1
        
        # modify the event if the user has requested.
        for k in self.config.event_modifiers:
          logging.debug1('---> Applying plugin %s <' % k.__module__)
          event = k(event)
          logging.debug1('---> Finished applying plugin %s <' % k.__module__)
          if not event:
            break
        if not event:
          if keep_processing(self.number_of_events, processed, total): 
            continue
          else:
            break     
        
        logging.debug1('Processing event %d' % (event.lvl1_id()))  
        # this will trigger the HLT processing
        processed_event = self.process(event)
        
        if self.datawriter:
          if processed_event:
            self.datawriter.write(processed_event)
          else:
            logging.debug1('Event %d was REJECTED, not saved' % event.lvl1_id())
        if not keep_processing(self.number_of_events, processed, total): 
          break     
    except PauseIterationException:
      raise PauseIterationException, processed # we can still update processed
    except KeyboardInterrupt:
      raise KeyboardInterrupt, processed # we can still update processed
    return processed

  def interact(self):
    """This will make the processor work in interactive mode"""
    order = ['configure', 'connect', 'prepareForRun', 'start',
             'stopRun', 'disconnect', 'unconfigure']
    motions = {}
    motions['LOADED']     = {'f': Processor.configure, 
                             'b': None}
    motions['CONFIGURED'] = {'f': Processor.connect, 
                             'b': Processor.unconfigure}
    motions['CONNECTED']  = {'f': Processor.prepareForRun, 
                             'b': Processor.disconnect}
    motions['PREPARED']   = {'f': Processor.run, 
                             'b': Processor.stopRun}
    
    while True:
      prompt = ("State is '%s' (<f>orward, <b>ackward, " 
                "<p>rompt, e<x>ec, <h>elp, <#>comment, <e>xit)?\n")
      action = raw_input(prompt % self.state).strip()
      action = action.split(' ', 1)

      if not action[0]: continue
      if action[0][0] not in ('f', 'b', 'e', 'p', 'x', '#', 'h'):
        logging.warning('Invalid command => `%s\'' % action[0][0])
      
      elif action[0][0] == 'h':
        logging.info('Valid commands are:')
        logging.info('\t<f>: move forward to the next state in chain')
        logging.info('\t<b>: move backward to the previous state in chain')
        logging.info('\t<e>: exit the program')
        logging.info('\t<x>: execute file with python commands')
        logging.info('\t<#>: ignore this line')
        logging.info('\t<h>: print this help message')
        logging.info('Assumed State Transition chain order is: %s' % \
            ', '.join(order))

      elif action[0] == 'e': return
      elif action[0] == 'p':
        
        # Prepare for interative prompt (from athena.py)
        logging.info('Interactive mode: Use Ctrl-D to resume')

        # readline support
        import os, sys, atexit, readline, rlcompleter

        readline.parse_and_bind( 'tab: complete' )
        readline.parse_and_bind( 'set show-all-if-ambiguous On' )

        # history support
        fhistory = os.path.expanduser( '~/.athena.history' )
        if os.path.exists( fhistory ):
          readline.read_history_file( fhistory )
        readline.set_history_length( 1024 )
        atexit.register( readline.write_history_file, fhistory )
        
        del atexit, readline, rlcompleter

        # use of shell escapes in interactive mode
        import AthenaCommon.ShellEscapes as ShellEscapes
        sys.excepthook = ShellEscapes.ShellEscapes()
        del ShellEscapes

        # Ready to drop into the interactive prompt
        HLTTestApps.python_prompt();

      elif action[0] == 'x':
        try:
          HLTTestApps.python_exec(' '.join(action[1:]))
        except Exception, e:
          logging.warning('Error executing command: `%s\'' % ' '.join(action[1:]))

      elif action[0][0] == '#':
        continue
      
      else:
        # try move
        if motions[self.state][action[0][0]]: 
          param = (self, action[1]) if len(action) == 2 else (self,)
          motions[self.state][action[0][0]](*param)
        else:
          act = 'backward' if action[0][0] == 'b' else 'forward'
          logging.warning('Moving %s is not allowed from state %s.' % 
                          (act, self.state))

  def _try_debug(self, stage):
    if not self.interactive and self.debug_stage == stage:
      HLTTestApps.hook_debugger()

  def _check_fatal(self):
    """
    Check whether the process is in a fatal state and, if so, raise the 
    corresponding exception
    """
    if self.fatal: 
      raise self.fatal
  
  def _transit(self, pre, trans, post, args):
    """
    Execute the transition with name trans to get from the state pre to the 
    state post.
    """
    self._try_debug(trans)
    
    self._check_fatal() ## check whether the processor is in a fatal state 
    
    logging.debug1('Asked to change state to "%s" from "%s" using trans "%s()"'
                   % (post, self.state, trans))
    
    retval = False
    if self.state == pre:
      self._transit_infrastructure(trans)
      
      start = time.time()
      retval = getattr(self.impl, trans)(args)
      total = time.time() - start
      if retval: 
        self.state = post
      else:
        raise FSMTransitionError, 'Could not execute %s() ' % (trans)
      logging.info('Current state is "%s" (%s() took %.2f s)' % (self.state, 
                                                                 trans, total))
    else:
      raise FSMTransitionError, 'Cannot %s() when state is %s' % (trans, 
                                                                  self.state)
  
    self._check_fatal()  # we check it again because otherwise the main flow of 
                        # execution would only get a chance to exit on the next 
                        # state transition attempt
    return retval
  
  def _transit_infrastructure(self, trans):
    if getattr(self.infrastructure, trans)():
      logging.debug1('Successfully executed transition "%s" on %s' 
                     % (trans, self.infrastructure.NAME))
    else:
      logging.warning('Transition "%s" unsuccessful on %s. Trying to continue.' 
                      % (trans, self.infrastructure.NAME))
      

################################################################################
#                                   Tests                                      #
################################################################################

import unittest
from pausable_istream import pausable_istream
from infrastructure import infrastructure as dummy_infrastructure
from configuration import configuration, run_number_error, dummy_configuration
from option import file_opt_spec

class dummy_processor(Processor):
  def __init__(self, input_stream, run_number = 0):
    self.infrastructure = dummy_infrastructure(None)
    self.stream = input_stream
    self.run_number = run_number
    self.rewind = False
    self.ostream = None
    self.save_output = ''
    self.state = 'LOADED'
    self.event_modifier = []
    self.interactive = False
    self.debug_stage = None
    self._add_config_specific_stuff()
  def _add_config_specific_stuff(self):
    self.config = dummy_configuration()
    self.config.event_modifiers = []
  def _check_fatal(self):
    pass
  def prepareForRun(self):
    runn = (self.run_number if self.run_number != 0
                            else self.stream.current_run_number())
    logging.info('Setting run number to %d' % runn)
    self._transit('CONNECTED', 'prepareForRun', 'PREPARED', [])
  def _transit(self, condition, method, post, *args):
    logging.info('dummy transition: %s' % method)
    self.state = post
  def process(self, event):
    pass  

class pausable_istream_tests(unittest.TestCase):
  def setUp(self):
    self.stream = pausable_istream(filelist)
    self.processor = dummy_processor(self.stream)
    logging.getLogger().setLevel(logging.INFO)
  def test_run_one(self):
    self.aux_test_run_n(1)
  def test_run_all(self):
    self.aux_test_run_n(0)
  def test_run_50(self):
    self.aux_test_run_n(50)
  def test_run_100(self):
    self.aux_test_run_n(100)
  def test_run_101(self):
    self.aux_test_run_n(101)
  def test_run_200(self):
    self.aux_test_run_n(200)
  def test_run_399(self):
    self.aux_test_run_n(399)
  def test_run_400(self):
    self.aux_test_run_n(400)
  def test_run_798(self):
    self.aux_test_run_n(798)
  def aux_test_run_n(self, n):
    logging.info('running through %s event(s)', 'all' if n < 0 else n)
    self.processor.number_of_events = n
    self.processor.run()
    
class dummy_stream:
  def current_run_number(self):
    return 0

class processor_tests(unittest.TestCase):
  def setUp(self):
    cli_args = ["-n", '10', "-f", repr(filelist), 
                'TrigExMTHelloWorld/MTHelloWorldOptions.py']
    self.processor = Processor(configuration(file_opt_spec, cli_args))
  def tearDown(self):
    self.processor.tearDown()
    self.assertState('LOADED')
  def test_run_number_required(self):
    self.processor.config['run-number'] = 0
    self.processor.config.stream = dummy_stream()
    self.assertRaises(run_number_error, self.processor.prepareForRun, "0")
#
# Cant have more than one test with the real processor, because the GAUDI 
# framework does not support configuring twice
#
#  def testBasic(self):
#    self.__test_init()
#    self.__test_configure()
#    self.__test_connect()
#    self.__test_prepare("177531")
#    self.__test_run()
#  def testItAgain(self):
#    self.testBasic()
#
  def testStopStart(self):
    self.__test_init()
    self.__test_configure()
    self.__test_connect()
    self.__test_prepare("177531")
    self.__test_run()
    self.processor.stopRun()
    self.__test_prepare("105200")
    self.__test_run()
  def assertState(self, state):
    self.assertEquals(self.processor.state, state)
  def __test_init(self):
    self.assertState('LOADED')
  def __test_configure(self):
    self.processor.configure()
    self.assertState('CONFIGURED')
  def __test_connect(self):
    self.processor.connect()
    self.assertState('CONNECTED')
  def __test_prepare(self, run_number):
    self.processor.prepareForRun(run_number)
    self.assertState('PREPARED')
  def __test_run(self):
    self.processor.run()
    self.assertState('PREPARED')

if __name__ == '__main__':
  from HLTTestApps import test_main
  test_main()
