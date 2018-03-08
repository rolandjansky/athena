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

def going_up(transition):
  return transition in ['configure','connect','prepareForRun','prepareWorker']

def keep_processing(config, processed, total, skipped):
  """Determines if we should continue processing events or not."""
  if config < 0:
    return processed + skipped < total
  else:
    return processed < config
    
def log_processed(skipped, processed):
  logging.info('Skipped %d events at the beginning' % skipped)
  logging.info('Processed %d events' % processed)

class FSMTransitionError(RuntimeError): pass

class Processor:
  """Defines a "pythonic" version of the hltinterface"""

  def __init__(self, config):
    """Wrapper for HLTInterface. """
    
    self.config = config

    # Need to load libPyROOT before TrigServices (see Savannah #82614)
    import cppyy 
    
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
    self.stream = config.stream
    self.debug_stage = config['debug']
    self.datawriter = None
    
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
      
    return (self._transit('CONNECTED', 'prepareForRun', '_INTERNAL_', 
                          self.config.get_prepare_ptree()) and 
            self._transit('_INTERNAL_', 'prepareWorker', 'PREPARED',
                          self.config.get_prepareWorker_ptree()))

  def stopRun(self):
    return self._transit('PREPARED', 'stopRun', 'CONNECTED',
                         self.config.get_stopRun_ptree())
  
  def disconnect(self):
    return self._transit('CONNECTED', 'disconnect', 'CONFIGURED',
                         self.config.get_disconnect_ptree())

  def unconfigure(self):
    return (self._transit('CONFIGURED', 'finalizeWorker', '_INTERNAL_',
                          self.config.get_finalizeWorker_ptree()) and
            self._transit('_INTERNAL_', 'unconfigure', 'LOADED',
                          self.config.get_unconfigure_ptree()))

  def process(self, event):
    self._check_fatal() # check whether the processor is in a fatal state
    if self.max_result_size > 0:
      return self.impl.process(event, self.watchdog, self.max_result_size)
    else:
      # use the default size set in src/util.h
      return self.impl.process(event, self.watchdog)
  
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
      logging.info('Running HLT')
      skipped, processed = 0, 0
      total = len(self.stream)

      self._try_save_output()
        
      try:
          while keep_processing(self.number_of_events,processed,total,skipped):
              try:
                  skipped, processed = self._run_aux(skipped, processed, total)
              except PauseIterationException, upd_proc: # we can still update
                  skipped, processed = upd_proc.args    # skipped and processed
                  self.stopRun()
                  self.prepareForRun(self.stream.current_run_number())
                  self._try_save_output()
                  
      except KeyboardInterrupt, upd_proc: # we can still update processed
          skipped, processed = upd_proc.args
          logging.error("Keyboard interruption caught! Aborting event "
                       "processing.")
          log_processed(skipped, processed)
          raise
      
      logging.info("Event processing finished successfully.")
      log_processed(skipped, processed)
      from HLTTestApps import print_ros_hit_map
      print_ros_hit_map(processed)
      logging.info('Current state is "%s"' % self.state)
      
  def _run_aux(self, skipped, processed, total):
    try:
      for event in self.stream:
        if skipped < self.skip_events:
          logging.info("Skipping event %d" % skipped)
          skipped += 1
          if keep_processing(self.number_of_events, processed, total, skipped):
            continue
          else:
            break
        
        event.check()

        # update number of events read
        processed += 1
        
        # modify the event if the user has requested.
        for k in self.config.event_modifiers:
          if event:
            logging.debug1('---> Applying plugin %s <' % k.__module__)
            event = k(event)
            logging.debug1('---> Finished applying plugin %s <' % k.__module__)
          else:
            break
          
        if not event:
          if keep_processing(self.number_of_events, processed, total, skipped): 
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
        if not keep_processing(self.number_of_events, processed, total, skipped): 
          break     
    except PauseIterationException:
      raise PauseIterationException, (skipped, processed) # update these 
    except KeyboardInterrupt:
      raise KeyboardInterrupt, (skipped, processed) # we can still update these
    return skipped, processed

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
  
  def _try_save_output(self):
    if self.config.do_save_output():
      dir, fncore = self.config.parsed_out_data_filename()
      self.datawriter = self.stream.datawriter(dir,fncore,self.use_compression)
    else:
      self.datawriter = None

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
      
      if going_up(trans): # transit infrastructure first
        self._transit_infrastructure(trans)
      
      logging.info("Processor is going to %s" % trans)
      start = time.time()
      retval = getattr(self.impl, trans)(args)
      total = time.time() - start
      
      if not going_up(trans): # transit infrastructure last
        self._transit_infrastructure(trans)
      
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

import unittest, string, random, glob, os, re
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
    self.skip_events = 0
    self._add_config_specific_stuff()
  def _add_config_specific_stuff(self):
    self.config = dummy_configuration()
    self.config.event_modifiers = []
    self.config.do_save_output = lambda: False
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
  jops = 'TrigExMTHelloWorld/MTHelloWorldOptions.py'
  def setUp(self):
    self._setup_cli_args()
    self._init_proc()
  def tearDown(self):
    self.processor.tearDown()
    self._assertState('LOADED')
  def test_run_number_required(self):
    self.processor.config['run-number'] = 0
    self.processor.config.stream = dummy_stream()
    self.assertRaises(run_number_error, self.processor.prepareForRun, "0")
  def testStopStart(self):
    self._test_init()
    self._test_configure()
    self._test_connect()
    self._test_prepare("177531")
    self._test_run()
    self.processor.stopRun()
    self._test_prepare("105200")
    self._test_run()
  def _setup_cli_args(self):
    self.cli_args = ["-n", '10'] + self._typical_cli_args()
  def _typical_cli_args(self):
    return ["-f", repr(filelist), '-M', self.jops]
  def _assertState(self, state):
    self.assertEquals(self.processor.state, state)
  def _init_proc(self):
    self.processor = Processor(configuration(file_opt_spec, self.cli_args))
  def _test_init(self):
    self._assertState('LOADED')
  def _test_configure(self):
    self.processor.configure()
    self._assertState('CONFIGURED')
  def _test_connect(self):
    self.processor.connect()
    self._assertState('CONNECTED')
  def _test_prepare(self, run_number=-1):
    self.processor.prepareForRun(run_number)
    self._assertState('PREPARED')
  def _test_run(self):
    self.processor.run()
    self._assertState('PREPARED')
    
class datawriter_plain_processor_tests(processor_tests):
  def setUp(self):
    self.tmpdir = "/tmp"
    self.tmpbasefilename = "tmpoutfile_athenaHLT_processor_test_"
    self.fname = self._unique_filename()
    super(datawriter_plain_processor_tests, self).setUp()
  def tearDown(self):
    super(datawriter_plain_processor_tests, self).tearDown()
    for f in glob.glob("%s/%s*" % (self.tmpdir, self.tmpbasefilename)):
      os.remove(f)
  def test_save_output_attr(self):
    regexp = "^%s/%s[a-zA-Z]*$" % (self.tmpdir, self.tmpbasefilename)
    self.assertRegexpMatches(self.processor.save_output, regexp)
  def test_save_output(self):
    # test we get the expected output files after running
    self._test_init()
    self._test_configure()
    self._test_connect()
    self._test_prepare()
    self._test_run()
    self.processor.tearDown()
    fre = "%s*" % self.fname
    noutfiles = len(glob.glob(fre))
    self.assertEqual(noutfiles, 2,
                     "Expected to find 2 files matching '%s' but found %d"
                     % (fre, noutfiles))
  def _setup_cli_args(self):
    self.cli_args = (["-o", self.fname,
                      "-C", "HltEventLoopMgr.ForceHltAccept=True", # accept&save
                      "-k", "295", "-n", "10"] # to move through all files
                     + self._typical_cli_args())
  def _unique_filename(self):
    return "%s/%s%s" % (self.tmpdir, self.tmpbasefilename, 
                        ''.join([random.choice(string.ascii_letters) 
                                 for _ in range(8)]))

class datawriter_conventional_processor_tests(processor_tests):
  def setUp(self):
    self.tmpdir = "/tmp"
    self.tmpbasefilename = "tmpprojtag_processor_test"
    super(datawriter_conventional_processor_tests, self).setUp()
  def tearDown(self):
    super(datawriter_conventional_processor_tests, self).tearDown()
    for f in glob.glob("%s/%s*" % (self.tmpdir, self.tmpbasefilename)):
      os.remove(f)
  def _setup_cli_args(self):
    self.cli_args = (["-O", ("{'dir': '%s', 'ProjectTag': '%s'}" 
                             % (self.tmpdir, self.tmpbasefilename)),
                      "-C", "HltEventLoopMgr.ForceHltAccept=True", # accept&save
                      "-k", "140", "-n", "20", # to move through both files
                      "-f", repr(extra_datafiles), '-M', self.jops])
  def test_save_output_conventional(self):
    # test we get the expected output files after running
    self._test_init()
    self._test_configure()
    self._test_connect()
    self._test_prepare()
    self._test_run()
    self.processor.tearDown()
    # build expected substrings 
    # we discard whatever comes before the first 8 digit sequence (run number)
    # and everything after the lumiblock (_lb followed by 4 digits)
    rexp = r"\d{8}.+_lb\d{4}"
    expect_sub1 = re.search(rexp, os.path.basename(extra_datafiles[0])).group()
    expect_sub2 = re.search(rexp, os.path.basename(extra_datafiles[1])).group()
    # find actual filenames
    actual_files = glob.glob("%s/%s*" % (self.tmpdir, self.tmpbasefilename))
    self.assertEquals(len(actual_files), 2, "Expected to find 2 output files, "
                                            "but found %d" % len(actual_files))
    for f in actual_files:
      self.assert_(expect_sub1 in f or expect_sub2 in f,
                   "Found file '%s' which does not contain any of the expected "
                   "substrings ('%s' and '%s')" % (f, expect_sub1, expect_sub2))


def _test_in_subprocesss(test, headmsg, spanwmsg):
  # announce spawning clearly
  print ("\n%s\n%s %s\n%s %s\n%s\n" % (headmsg, headmsg, spawnmsg, headmsg, 
                                     test, headmsg))
  p = Process(target=test_main, args=([test],))
  try: # spawn, run, join, etc.
    p.start(); p.join()
  except KeyboardInterrupt:
    # unittest messes with Ctrl-C and we don't want child to go on alone
    while p.is_alive():
      try:
        print '\nExplicitly terminating child\n'
        p.terminate(); p.join()          
      except KeyboardInterrupt: 
        pass
  # If this test  failed, exit already (otherwise go on)
  if p.exitcode:
    print "%s Test(s) failed in child process" % headmsg
    sys.exit(p.exitcode)


if __name__ == '__main__':
  import sys
  from multiprocessing import Process
  from HLTTestApps import test_main
  # we want to execute these test sets in separate processes, to ensure a clean
  # athena/gaudi slate (otherwise things fail the second time around)
  separate_tests = ('pausable_istream_tests',
                    'processor_tests',
                    'datawriter_plain_processor_tests.test_save_output_attr',
                    'datawriter_plain_processor_tests.test_save_output',
                    'datawriter_conventional_processor_tests.' # no comma 
                    + 'test_save_output_conventional') # explicit same string
  
  spawnmsg = ("Spawning process for the test(s) below (to ensure a clean "
              "athena/gaudi slate):")
  headmsg = "!!!!!!"
  
  print "%s Running multiple tests in separate processes\n" % headmsg
  for test in separate_tests:
    _test_in_subprocesss(test, headmsg, spawnmsg)

  print "\n%s Successfully ran multiple tests in separate processes\n" % headmsg

