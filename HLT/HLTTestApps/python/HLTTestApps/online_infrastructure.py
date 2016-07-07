# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jun 10, 2014

@author: ricab
'''

import os, shutil, time, logging, subprocess, signal
from HLTTestApps import IInfoRegister, ipc_init
from infrastructure import *

EXTERNAL_INIT_TIME = 5 # seconds
EXTERNAL_TERM_TIME = 5 # seconds
SLEEP_RESOLUTION = 0.2 # seconds

def initial_ipc_server(pgid):
  """Starts and manages the IPC server for the current run."""
  if os.system('ipc_ls >& /dev/null') != 0:
    # we start a fresh IPC server
    proc_name = "initial_ipc_server"
    logging.info("Starting a new IPC server for the initial partition...")
    ret = subprocess.Popen(["ipc_server", "-i-"],
                           stderr=new_file("%s.err" % proc_name), 
                           stdout=new_file("%s.out" % proc_name),
                           preexec_fn=lambda:os.setpgid(0,pgid))
     # give it time to initialize
    wait_for_output(proc_name, EXTERNAL_INIT_TIME, SLEEP_RESOLUTION)
    return ret
  else:
    logging.info("IPC server for 'initial' partition is already running!")
    return 0

def ipc_server(part_name, pgid):
  """Starts and manages the IPC server for the current run."""  
  if os.system('ipc_ls -p %s >& /dev/null' % part_name) != 0:
    # we start a fresh IPC server
    proc_name="ipc_server"
    logging.info("Starting a new IPC server for partition '%s'..." % part_name)
    ret = subprocess.Popen(["ipc_server", "-i-", "-p%s" % part_name],
                           stderr=new_file("%s.err" % proc_name), 
                           stdout=new_file("%s.out" % proc_name),
                           preexec_fn=lambda:os.setpgid(0,pgid))
    # give it time to initialize
    wait_for_output(proc_name, EXTERNAL_INIT_TIME, SLEEP_RESOLUTION)
    return ret
  else:
    logging.info("IPC server for '%s' is already running!" % part_name)
    return 0

def is_server(part_name, server_name, pgid):
  """Starts and manages the IS server for the current run."""
  if os.system('is_ls -p %s -n %s >& /dev/null'% (part_name, server_name)) != 0:
    # we start a fresh IS server
    proc_name="is_server"
    logging.info("Starting a new Histogramming IS server '%s' for partition "
                 "'%s'" % (server_name, part_name))
    ret = subprocess.Popen(['is_server', '-p%s' % part_name, 
                            '-n%s' % server_name],
                           stderr=new_file('%s.err' % proc_name), 
                           stdout=new_file('%s.out' % proc_name),
                           preexec_fn=lambda:os.setpgid(0,pgid))
    # give it time to initialize
    wait_for_output(proc_name, EXTERNAL_INIT_TIME, SLEEP_RESOLUTION)
    return ret
  else:
    logging.info("IS server '%s' for '%s' is already running!" % \
        (server_name, part_name))
    return 0

def oh_display(pgid):
  """Starts and manages the IPC server for the current run."""
  return subprocess.Popen(["oh_display"], stderr=new_file("oh_display.err"), 
                          stdout=new_file("oh_display.out"),
                          preexec_fn=lambda:os.setpgid(0,pgid))

def terminate_process(proc):
  proc.send_signal(signal.SIGHUP)
  proc.wait() 

def oh_cp(partition, server, run_index):
  """Uses 'oh_cp' to save histograms in a file."""
  logging.info('Saving histograms from server %s' % (server))
  s = os.system('oh_cp -p%s -s%s -H -1 -O -r %d' % (partition, server, run_index))
  if s != 0: logging.warn('Output status of oh_cp was %d' % s)

def new_file(name):
  """
  Create a new empty file with the provided name
  """
  if os.path.exists(name): os.unlink(name)
  return file(name, 'wt')

def rm_file(name):
  """
  Remove the file with the provided name
  """
  os.remove(name)

def wait_for_output(proc_name, timeout=None, sleep_resolution=1):
  """ 
  Wait for a process to produce output, within a certain timeout.

  Checks, every <sleep_resolution> seconds, for output in the files 
  ./<proc_name>.out and ./<proc_name>.err, for a maximum of <timeout> seconds. 
  If any of these files is detected not to be empty, the wait is interrupted.
  """
  count = 0
  logging.debug('Waiting for %s to initialize' % proc_name)
  while not timeout or count < timeout:
    if os.path.getsize("./%s.err" % proc_name):
      logging.warning('There was a problem with the initialization of %s. '
                      'Trying to continue...' % proc_name)
      return -1
    if os.path.getsize("./%s.out" % proc_name):
      logging.debug('%s initialized correctly' % proc_name)
      return 0
    time.sleep(sleep_resolution)
    count += sleep_resolution
  logging.warning('Could not confirm the initialization of %s after %.2fs. '
                  'Trying to continue...' % (proc_name, count))
  return 1

def finalize_THistSvc():
  # Hack to finalize and get rid of the THistSvc (actually TrigMonTHistSvc)
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
  from AthenaCommon.AppMgr import theApp
  if theApp.state(): # if we still didn't finalize
    logging.debug("Finalizing THistSvc")
    svcMgr.THistSvc.getHandle().finalize()
    theApp.getHandle()._svcmgr.removeService('THistSvc')

class FinalizeNeedEnum:
  """
  Enumeration to describe the finalization needs of an AutoFinIInfoRegister
  """
  NONEED, NEED_FIN, NEED_FINWORKER = range(1, 4)

class SubpEnum:
  """
  Subprocess enumeration
  """
  INITIAL_IPC, IPC, IS, OH_DISPLAY = range(1,5)

class AutoFinIInfoRegister(IInfoRegister):
  """
  Auto finalize if, when deleting, somehow the last prepareForRun call was still 
  not followed by a finalize call or if prepareWorker was not followed by 
  finalizeWorker  
  """
  def __init__(self, finalizeWorker_ptree, finalize_ptree, *args):
    IInfoRegister.__init__(self, *args)
    self.need = FinalizeNeedEnum.NONEED
    self.finalizeWorker_ptree = finalizeWorker_ptree
    self.finalize_ptree = finalize_ptree
  def __del__(self):
    # TrigMonTHistSvc needs the ITHistRegister to finalize, so we need to 
    # finalize it now, because there will be no ITHistRegister later on
    finalize_THistSvc()
    logging.debug("Automatically deleting AutoFinIInfoRegister")
    if self.need is FinalizeNeedEnum.NEED_FINWORKER:
      logging.debug("Automatically calling IInfoRegister::finalizeWorker")
      self.finalizeWorker(self.finalizeWorker_ptree)
    if self.need is FinalizeNeedEnum.NEED_FIN :
      logging.debug("Automatically calling IInfoRegister::finalize")
      self.finalize(self.finalize_ptree)
  def _logged_transition(self, t, tname, args):
    logging.info("Calling IInfoRegister.%s" % tname)
    ret = t(self, *args)
    logging.info("IInfoRegister.%s: %s" % (tname, "success" if ret else "failure"))
    return ret
  def configure(self, *args):
    return self._logged_transition(IInfoRegister.configure, "configure", args)
  def prepareForRun(self, *args):
    ret = self._logged_transition(IInfoRegister.prepareForRun, "prepareForRun", args)
    if ret:
      self.need = FinalizeNeedEnum.NEED_FIN
    return ret
  def prepareWorker(self, *args):
    ret = self._logged_transition(IInfoRegister.prepareWorker, "prepareWorker", args)
    if ret:
      self.need = FinalizeNeedEnum.NEED_FINWORKER
    return ret
  def finalizeWorker(self, *args):
    ret = self._logged_transition(IInfoRegister.finalizeWorker, "finalizeWorker", args)
    if ret:
      self.need = FinalizeNeedEnum.NEED_FIN
    return ret
  def finalize(self, *args):
    ret = self._logged_transition(IInfoRegister.finalize, "finalize", args)
    if ret:
      self.need = FinalizeNeedEnum.NONEED
    return ret

class online_infrastructure(infrastructure):
  # constants
  NAME = "online infrastructure"
  TDAQ_PARTITION = 'part_athenaHLT_mon'
  
  # Signals for which we want a custom handler
  sigs = [signal.SIGFPE, signal.SIGHUP, signal.SIGQUIT, signal.SIGSEGV, 
          signal.SIGTERM]
  ipc_ref_file = None     # so that the attributes are found even when obj init 
  ipc_ref_created = False # has not been completed  
  
  def __init__(self, config):
    infrastructure.__init__(self, config)
    self.pgid = 0
    self.run_index = -1
    self._register_handlers()
    os.environ['TDAQ_PARTITION'] = self.TDAQ_PARTITION
    self._setup_ipc()
    self._start_subprocesses()
    self.mon = AutoFinIInfoRegister(self.config.get_mon_finalizeWorker_ptree(),
                                    self.config.get_mon_finalize_ptree(),
                                    config['info-service'])
  def __del__(self):
    # we delete this now, while required external processes are still around
    del self.mon
    self.mon = None # so __del__ can be called again
    # No more OHRootProvider expecting external procs. We can get rid of them
    self._terminate_processes()
    self._cleanup_ipc()
  def _setup_ipc(self):
    if not self.config['user-ipc']:
      self.ipc_ref_file = os.path.join(os.getcwd(), 'ipc_init.ref')
      self.ipc_ref_created= False if os.path.exists(self.ipc_ref_file) else True
      os.environ['TDAQ_IPC_INIT_REF'] = 'file:%s' % self.ipc_ref_file
    ipc_init() # this happened automatically in the past when running
               # athenaMT/PT with OH, because the AppControl would be used and
               # take care of initialization. This is no longer the case...
  def _cleanup_ipc(self):
    if (self.ipc_ref_file and self.ipc_ref_created and 
        os.path.exists(self.ipc_ref_file)):
      logging.info('Removing IPC initial reference (%s)...' % self.ipc_ref_file)
      rm_file(self.ipc_ref_file)
  def _start_subprocess(self, target):
    if target == SubpEnum.INITIAL_IPC:
      subp = self.initial_ipc_server = initial_ipc_server(self.pgid)
    elif target == SubpEnum.IPC:
      subp = self.ipc_server = ipc_server(self.TDAQ_PARTITION, self.pgid)
    elif target == SubpEnum.IS:
      subp = self.is_server = is_server(self.TDAQ_PARTITION, 
                                        self.config['ohserver'],
                                        self.pgid)
    else: # target == SubpEnum.OH_DISPLAY
      subp = self.oh_display = oh_display(self.pgid)
    if subp and not self.pgid: # if it was initialized, update the group id
      self.pgid = subp.pid
  def _start_subprocesses(self):
    logging.info('Starting online infrastructure subprocesses')
    # Here we put child processes in a new group (with the pgid of the initial 
    # ipc). This way, the new processes don't receive signals sent to the group 
    # of the parent process (athenaHLT). For instance, this prevents Ctrl+C from 
    # being sent to child processes. That way, athenaHLT is the only responsible
    # for handling the signal, finishing the processes in due time if necessary.
    # Without this approach, servers would quit immediately on Ctrl+C and 
    # athenaHLT wouldn't succeed in saving the histograms. In the case of 
    # SIGSEGV, athenaHLT cannot guarantee full cleanup (that is part of the 
    # nature of a crash). Child processes may linger in that case.
    self._start_subprocess(SubpEnum.INITIAL_IPC)
    self._start_subprocess(SubpEnum.IPC)
    self._start_subprocess(SubpEnum.IS)
    if self.config['oh-display']:
      self._start_subprocess(SubpEnum.OH_DISPLAY)
  def _terminate_processes(self):
    for pnam in ['oh_display', 'is_server', 'ipc_server', 'initial_ipc_server']:
      proc = getattr(self, pnam, None)
      if proc:
        exitcode = proc.poll()
        if exitcode is not None:
          # the process already terminated
          logging.info('The %s already exited, with code %d'
                          % (pnam, exitcode))
        else:
          logging.info('Terminating the %s (pid=%d)...' % (pnam, proc.pid))
          terminate_process(proc)          
  def _register_handlers(self):
    self.prehandlers = {}
    for s in self.sigs:
      self.prehandlers[s] = signal.getsignal(s)
      signal.signal(s, self._handle_quit)
  def _handle_quit(self, signum, frame):
    logging.error("Caught signal %d. Trying to clean the infrastructure and "
                  "exit cleanly" % signum)
    self.__del__() # this doesn't delete the object, only executes __del__
    # execute the previous handler as well:
    prehandler = signal.SIG_DFL
    if signum in self.prehandlers:
      prehandler = self.prehandlers[signum]
      del self.prehandlers[signum]
    signal.signal(signum, prehandler)
    os.kill(os.getpid(), signum)
  def configure(self):
    infrastructure.configure(self)
    return self.mon.configure(self.config.get_mon_config_ptree())
  def prepareForRun(self):
    infrastructure.prepareForRun(self)
    return self.mon.prepareForRun(self.config.get_mon_prepare_ptree())
  def prepareWorker(self):
    infrastructure.prepareWorker(self)
    return self.mon.prepareWorker(self.config.get_mon_prepareWorker_ptree())
  def run(self):
    self.run_index += 1
    return infrastructure.run(self)
  def stopRun(self):
    infrastructure.stopRun(self)
    ret = self.mon.finalizeWorker(self.config.get_mon_finalizeWorker_ptree())
    ret &= self.mon.finalize(self.config.get_mon_finalize_ptree())
    oh_cp(self.TDAQ_PARTITION, self.config['ohserver'], 
          self.run_index if self.run_index != -1 else 0)
    return ret
  def unconfigure(self):
    return infrastructure.unconfigure(self)


################################################################################
#################################### Tests #####################################
################################################################################

import unittest

def rm_dir(name):
  """
  Remove the directory with the provided name, along with all its contents
  """
  shutil.rmtree(name)

class test_file_and_dir_creation_and_deletion(unittest.TestCase):
  def setUp(self):
    self.filename = "%s.test" % type(self)
  def test_create(self):
    new_file(self.filename)
    self.assert_(os.path.exists(self.filename), 
                 "File %s was not correctly created" % self.filename)
    self.assertEquals(os.path.getsize(self.filename), 0, 
                      "File %s was not correctly created" % self.filename)
    rm_file(self.filename)
  def test_rm_file(self):
    new_file(self.filename)
    with open(self.filename, 'w') as f:
      print >> f, "stuff"
    rm_file(self.filename)
    self.assertFalse(os.path.exists(self.filename), 
                     "File %s was not correctly removed" % self.filename)
  def test_rm_dir(self):
    os.mkdir(self.filename)
    new_file("%s/%s" % (self.filename, self.filename))
    rm_dir(self.filename)
    self.assertFalse(os.path.exists(self.filename), 
                     "Directory %s was not correctly removed" % self.filename)

class test_output_wait(unittest.TestCase):
  """
  Tests wait_for_output
  """
  dirname = "test_output_tmp"
  proc_name = "fake_proc"
  def setUp(self):
    os.mkdir(self.dirname)
    os.chdir(self.dirname)
    new_file('%s.out' % self.proc_name).close()
    new_file('%s.err' % self.proc_name).close()
  def test_timeout(self):
    t0 = time.time()
    ret = wait_for_output(self.proc_name, 1, 0.3)
    delta = time.time() - t0
    self.assert_(delta < 2)
    self.assertEquals(ret, 1)
  def test_out(self):
    self.assertEquals(self.base_test_output('out'), 0)
  def test_err(self):
    self.assertEquals(self.base_test_output('err'), -1)
  def base_test_output(self, extension):
    t0 = time.time()
    f = open('%s.%s' % (self.proc_name, extension), 'w')
    f.write('output')
    f.close()
    ret = wait_for_output(self.proc_name, 10, 0.3)
    delta = time.time() - t0
    self.assert_(delta < 0.1)
    return ret
  def tearDown(self):
    os.chdir('..')
    rm_dir(self.dirname)
    
class online_infrastructure_transitions_test(infrastructure_transitions_test):
  def setUp(self):
    infrastructure_transitions_test.setUp(self)
    self.cli_args = ["-M"] + self.cli_args
    
class test_online_subprocesses(unittest.TestCase):
  """
  Test that online subprocesses are correctly started and killed
  """
  def setUp(self):
    self.sleep = 0.5
    self.part_name = 'part_athenaHLT_test'
    self.server_name = 'dummy_is_server'
    self.ipc_ref = os.path.join(os.getcwd(), 'ipc_init.ref')
    self.old_ipc = os.environ['TDAQ_IPC_INIT_REF']
    os.environ['TDAQ_IPC_INIT_REF'] = 'file:' + self.ipc_ref
    self.sps = []
  def tearDown(self):
    os.environ['TDAQ_IPC_INIT_REF'] = self.old_ipc
    for sp in self.sps[::-1]: # kill in FILO order
      logging.warn('Leftover process with PID %d. Killing it.' % sp.pid)
      sp.kill()
  def testall(self):
    pgid = 0
    initipcs = self.create_sp(initial_ipc_server, pgid)
    pgid = initipcs.pid
    ipcs = self.create_sp(ipc_server, self.part_name, pgid)
    iss = self.create_sp(is_server, self.part_name, self.server_name, pgid)
    self.terminate_sp(iss, 'IS server')
    self.terminate_sp(ipcs, 'IPC server')
    self.terminate_sp(initipcs, 'initial IPC server')
  def create_sp(self, sp_creator, *args):
    sp = sp_creator(*args)
    time.sleep(self.sleep)
    self.assert_(sp, "Subprocess not correctly created by '%s'" 
                          % sp_creator.__name__)
    self.sps.append(sp)
    logging.info("Created process with PID %d" % sp.pid)
    return sp
  def terminate_sp(self, sp, spname):
    sp.terminate()
    time.sleep(self.sleep)
    ecode = sp.poll()
    # ecode is None if the process didn't exit and 0 if it exited correctly
    if not ecode is None:
      self.sps.remove(sp)
    self.assertFalse(ecode, "Subprocess '%s' terminated with non-zero error "
                            "code: %s" % (spname, ecode))
    self.assertNotEqual(ecode, None, "Subprocess '%s' didn't exit on SIGTERM"
                                     % spname)
    
class test_online_infrastructure_as_subprocess(unittest.TestCase):
  """
  Run the online infrastructure in a subprocess (which in turn creates 
  subprocesses), and kill it in different places, to confirm that signal 
  handling and cleanup work properly. The infrastructure class that is used
  descends from online_infrastructure but replaces certain methods, to provide
  specific points of signal handling. Also, it doesn't set up any monitoring
  service and it provides no transitions
  """
  def test0(self):
    self._test_base(0)
  def test1(self):
    self._test_base(1)
  def test2(self):
    self._test_base(2)
  def test3(self):
    self._test_base(3)
  def test4(self):
    self._test_base(4)
  def _test_base(self, n):
    import multiprocessing, time
    print '### Testing with n =', n
    p = multiprocessing.Process(target=self.pausing_online_infrastructure)
    p.start()
    for i in range(n):
      time.sleep(0.5)
      os.kill(p.pid, signal.SIGUSR1)
    time.sleep(0.5)
    os.kill(p.pid, signal.SIGTERM)
    p.join()
    self.assertEquals(p.exitcode, -15, "Subprocess exited with an unexpedted " 
                                       "code: %d" % p.exitcode)
    del p
  def test_pausing_online_infrastructure_init_del(self):
    class dont_pause_context(object):
      def __enter__(self):
        self.pause = signal.pause
        signal.pause = lambda: time.sleep(0.5)
      def __exit__(self, *unused_args):
        signal.pause = self.pause
    with dont_pause_context():
      # For some reason I have to call __del__ explicitly (probably due to 
      # issues with del - see 
      # https://docs.python.org/2/reference/datamodel.html#object.__del__
      # Otherwise deletion would be delayed until after the test and any problem
      # with the d'tor would be unnoticed. Notice however that __del__ is called
      # again when the object is actually deleted. This accounts for warning 
      # messages saying that sub processes exited already
      self.pausing_online_infrastructure().__del__()
    
  class pausing_online_infrastructure(online_infrastructure):
    # no transitions
    configure=connect=prepareForRun=run=stopRun=disconnect=unconfigure = None
    
    def __init__(self):
      self.pause_count = 0
      self.part_name = 'part_athenaHLT_test'
      self.server_name = 'dummy_is_server'
      self.ipc_ref_file = os.path.join(os.getcwd(), 'ipc_init.ref')
      os.environ['TDAQ_IPC_INIT_REF'] = 'file:' + self.ipc_ref_file
      self.mon = None
      self._test_servers()
    def _test_servers(self):
      # handlers
      self._register_handlers()
      signal.signal(signal.SIGUSR1, self._test_handler)
      self._pause() # pausing 0
      # initial ipc
      pgid = 0
      self.initial_ipc_server = initial_ipc_server(pgid)
      pgid = self.initial_ipc_server.pid
      self._pause() # pausing 1
      # ipc
      self.ipc_server = ipc_server(self.part_name, pgid)
      self._pause() # pausing 2
      # is
      self.is_server = is_server(self.part_name, self.server_name, pgid)
      self._pause() # pausing 3
      # oh display
      self.oh_display = oh_display(pgid)
      self._pause() # pausing 4
    def _pause(self):
      print 'pausing - ' + str(self.pause_count)
      signal.pause()
      self.pause_count += 1
    def _test_handler(self, signum, frame):
    #def _test_handler(signum, frame):
      print 'test handler received signal', signum

if __name__ == '__main__':
  from HLTTestApps import test_main
  import logging
  logging.getLogger().setLevel(1)
  # we also need a test setup for the infrastructure module, as some tests
  # come from there. We then exclude the tests that are tested in that module
  test_main(more_modules_requiring_setup=['infrastructure'],
            exclude_names=['infrastructure_transitions_test'])
            #include_names=['online_infrastructure_transitions_test'])
  
