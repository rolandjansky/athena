# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

'''
Created on Jun 14, 2013

@author: ricab
'''


from pausable_istream import pausable_istream
from HLTTestApps import ptree, ers_debug_level, get_ers_debug_level
from HLTTestApps import tdaq_time_str_from_microsec
from HLTTestApps import set_ros2rob_map as set_dc_ros2rob, set_l1r_robs, set_dcm_strategy
from eformat import EventStorage as ES
from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil as CoolUtil
from CoolConvUtilities import AtlCoolLib
from PyCool import cool
from contextlib import contextmanager
from ast import literal_eval
from os import path, sep
from re import match
import logging
import option

class run_number_error(RuntimeError): pass
class run_params_error(RuntimeError): pass


# Ptree constants
base_conf_ptree_path        = ("Configuration.Partition.TriggerConfiguration."
                               "TriggerConfiguration")
base_hlt_ptree_path         = base_conf_ptree_path + ".hlt"
trigdb_con_ptree_path       = base_conf_ptree_path + (".TriggerDBConnection"
                                                      ".TriggerDBConnection")
lvl1_config_ptree_path      = base_conf_ptree_path + (".L1TriggerConfiguration"
                                                      ".L1TriggerConfiguration")
joboptions_ptree_extension  = "HLTImplementationJobOptions"
dbpy_ptree_extension        = "HLTImplementationDBPython" 
common_ptree_extension      = "HLTCommonParameters.HLTCommonParameters"
athenaHLT_ptree_path        = (base_conf_ptree_path + 
                               ".athenaHLTSpecificConfiguration")
mon_config_rule_path        = ("HLTMonInfoImpl.ConfigurationRules."
                               "ConfigurationRuleBundle.Rules."
                               "ConfigurationRule")
monparams_extension         = "Parameters.OHPublishingParameters"
oh_params_path              = mon_config_rule_path + "." + monparams_extension
muoncalbuf_path             = ("Configuration.HLTMPPUApplication."
                               "MuonCalibrationConfig.CircBuffer")
ros2rob_path                = "Configuration.ROS2ROBS"
appName                     = 'athenaHLT'

# prepareWorker ptree keys agreed with Sami in ATR-9669
prepw_pt                    = ptree() 
prepw_pt['appName']         = appName
prepw_pt['workerId']        = '0'
prepw_pt['numberOfWorkers'] = '1'

# DB constants
prescales_db_strs = ('COMP', 'CONDBR2')
runparams_db_strs = ('COOLONL_TDAQ/COMP200', 'COOLONL_TDAQ/CONDBR2')
runparams_paths   = ('/TDAQ/RunCtrl/SOR_Params', '/TDAQ/RunCtrl/SOR')
dblim_rnum        = 236108


class configuration(dict):
  
  def __init__(self, option_spec, cli_args):
    self.dbconn = None
    self.defaults = {k: option_spec.get_default(k) for k in option_spec}
    self.default_convention = option_spec['save-output-conventional']['allowed']
    self.parse_sor = option_spec['sor-time']['parse']

    if not cli_args or not cli_args[0]:
      cli_args = ['-h'] # if no joboptions, print help msg
    parser = option.gen_parser(option_spec, True)
    kwargs, self.extra = parser.parse(cli_args)
    
    self.update(kwargs)
    self.update(option_spec.constants)
    self.__set_verbosity()
    option_spec.optcheck(self, self.extra)
    self.__digest_config()
    self.__log()
    
  def __getattr__(self, attr):
    if attr.startswith('get_') and attr.endswith("_ptree"):
      return lambda: ptree()
    raise AttributeError, ("%s instance has no attribute '%s'" 
                           % (type(self), attr))
    
  def do_save_output(self):
    return self['save-output'] or self['save-output-conventional']
  
  def parsed_out_data_filename(self):
    if self['save-output']:
      return self.__parse_filename()
    else: # do conventional output
      # get the conventional properties
      convention = {k: self._derive_conventional_property(k)
                    for k in self.default_convention}
      # find our returns
      dir = convention.pop('dir') # popped - this is for our own use only
      # the rest go here - would like to use kwargs, but not supported in ES
      fncore = ES.RawFileName(convention['ProjectTag'],
                              convention['RunNumber'],
                              convention['StreamType'],
                              convention['StreamName'],
                              convention['LumiBlockNumber'],
                              appName,
                              convention['ProductionStep']).fileNameCore() 
      return dir, fncore
  
  def get_config_ptree(self):
    pt = ptree()
    self.__add_config_ptree(pt)
    logging.debug('Configure ptree:\n%s' % pt)
    return pt
  
  def get_prepare_ptree(self):
    # we update the run-number again because the user could have provided a 0 
    # run-number in interactive mode on purpose, to force usage of the 
    # run-number from the event stream.
    self.__update_run_number()
    rparams = self.__get_run_params()

    # In case there are no run params use some sensible defaults
    innerpt, pt = ptree(), ptree()
    innerpt['timeSOR']           = self.__get_sor(rparams)
    innerpt['det_mask']          = self.__get_dmask(rparams)
    innerpt['run_number']        = str(self['run-number'])
    innerpt['run_type']          = str(rparams['RunType']) if rparams else 'Physics'
    innerpt['recording_enabled'] = str(rparams['RecordingEnabled']).lower() if rparams else 'true'
    pt.add_child('RunParams', innerpt)
    logging.debug('Prepare ptree:\n%s' % pt)
    return pt
  
  def get_prepareWorker_ptree(self):
    return prepw_pt

  def get_mon_config_ptree(self):
    pt = ptree()
    if self['oh-monitoring']:
      paramspt, rulept = ptree(), ptree()
      paramspt['NumberOfSlots'] = str(self['oh_numslots'])
      paramspt['PublishInterval'] = str(self['histogram-publishing-interval'])
      paramspt['OHServer'] = self['ohserver']
      paramspt['ROOTProvider'] = self['rootprovider']
      rulept.add_child(monparams_extension, paramspt)
      rulept['IncludeFilter'] = self['histogram-include']
      rulept['ExcludeFilter'] = self['histogram-exclude']
      rulept['UID'] = 'GlobalOHRule'
      rulept['Name'] = 'GlobalOHRule'
      pt.add_child(mon_config_rule_path, rulept)
    logging.debug('OH configure ptree:\n%s' % pt)
    return pt
  
  def get_mon_prepareWorker_ptree(self):
    pt = ptree()
    if self['oh-monitoring']:
      pt['appName'] = appName
    logging.debug('OH prepareWorker ptree:\n%s' % pt)
    return pt
  
  def _derive_conventional_property(self, k):
    try:
      return self['save-output-conventional'][k] # try planA
    except KeyError:
      # fall back to planB
      d, core = self.__parse_filename(self.stream.current_filename()) 
      rf = ES.RawFileName(core)
      if rf.hasValidCore():
        if k == 'ProjectTag': # special case for this one (attr name and return)
          return path.basename(rf.project())
        attr = k[:1].lower() + k[1:] # lower the first letter of the key
        if hasattr(rf, attr):
          return getattr(rf, attr)() # call the corresponding function
    # fall back to planC
    return self.default_convention[k]
  
  def __parse_filename(self, fullname=None):
    if not fullname:
      fullname = self['save-output']
    dir, core = ((path.dirname(fullname), 
                  path.basename(fullname)) if fullname.find(sep) != -1
                                           else ('.', fullname))
    if core.endswith('.data'):
      core = core[:-5]
    return dir, core
  
  def __set_verbosity(self):
    option.warn_verbosity(self['verbosity'])
    logging.getLogger().setLevel(self['verbosity'])
    ers_debug_level(self['ers-debug-level'])
  
  def __digest_config(self):
    self.__setup_input()
    self.__setup_sor()
    self.__setup_hlt_implementation()
    self.__setup_monitoring()
    self.__setup_leak_check()
    self.__setup_commands()
    self.__setup_event_mods()
    self.__setup_ros_rob_details()
    self.__setup_miscellaneous()
  
  def __log(self):
    self.__log_number_events()
    
  def __log_number_events(self):
    nevts, skip = self['number-of-events'], self['skip-events']
    tot = nevts + skip
    avail = len(self.stream)
    if nevts == 0:
      logging.warning("0 events will be processed")
      if skip:
        logging.warning("Skipping events makes no sense in this context")
    elif nevts > 0 and \
      len(self.stream) < tot:
      logging.warning("Events will be recycled!")
      logging.info("%d events available on input" % len(self.stream))
      logging.info("User requested %d skipped events and %d processed events" 
                   % (skip, nevts))
      logging.info("A total of %d events have to be consumed" % tot)
      if avail != 0:
        n_file_iter = float(tot)/avail
      else:
        logging.warning("Input stream has no events!")
        n_file_iter = 0.
        self['number-of-events'] = 0
      logging.info("I'll run over the file(s) %.2f times" % (n_file_iter))
      if self['event-modifier']: 
        logging.info("(assuming no events are skipped due to pre-processing)")
    
  def __setup_input(self):
    # we make the stream be part of the configuration, because some of it's 
    # contents may actually impact how things are configured (e.g. run-number)
    self.stream = None
    if 'file' in self:
      self['file'] = [path.expandvars(path.expanduser(f)) for f in self['file']]
      logging.info('Creating event stream from file list %s' % self['file'])
      self.stream = pausable_istream(self['file'])
      if self['skip-events'] >= self.stream.total_events:
        raise option.BadOptionSet, ('Can only skip less events than those '
                                    'provided as input')
    # We can now update the run number from the input stream if necessary
    self.__update_run_number()
    self['precommand'].append('_run_number=%d' % self['run-number'])
  
  def __setup_sor(self):
    self['sor-time'] = self.parse_sor(self['sor-time'])
      
  def __setup_hlt_implementation(self):
    if (self['use-database']):
      if self['joboptionsvc-type'] == self.defaults['joboptionsvc-type']:
        self['joboptionsvc-type'] = 'TrigConf::HLTJobOptionsSvc'
      self.__setup_db_defaults()
    else:
      self['joboptions'] = ' '.join(self.extra)
        
  def __setup_monitoring(self):
    if self['perfmon']:
      self['precommand'].insert(0, 'include("TrigCommon/PerfMon.py")')
    if not self['oh-monitoring']:
      self['precommand'].append('include("TrigServices/OfflineTHistSvc.py")')
      
  def __setup_leak_check(self):
    doLeakCheck = False
    memChkMode = ''
    if self['leak-check-execute']:
      doLeakCheck = [ 'execute' ]
      memChkMode = 'leak-check'
    if self['leak-check']:
      memChkMode = 'leak-check'
      arg = self['leak-check'].lower()
      if arg=='all':  doLeakCheck = []
      else:           doLeakCheck = [arg]
    if self['delete-check']:
      memChkMode = 'delete-check'
      arg = self['delete-check'].lower()
      if arg=='all':  doLeakCheck = []
      else:           doLeakCheck = [arg]
      
    if doLeakCheck != False:
      #early import is needed for proper offloading later
      import Hephaestus.MemoryTracker as memtrack
      if memChkMode == 'delete-check':
        import Hephaestus.DeleteChecker
      self['precommand'].insert(0, 'memChkMode="%s";doLeakCheck=%s;'
                                'include("TrigCommon/LeakTracer.py")' 
                                % (str(memChkMode).replace("'",'"'),
                                   str(doLeakCheck).replace("'",'"')))
  
  def __setup_commands(self):
    self.__setup_pcommands('pre')
    self.__setup_pcommands('post')
    
  def __setup_pcommands(self, p):
    self["%scommand" % p] = [option.parse_commands(c) 
                             for c in self["%scommand" % p]]
    self["%scommand" % p] = filter(lambda c: not match(r'[\s;]*$', c), 
                                   self["%scommand" % p])
  
  def __setup_event_mods(self):
    def process_additional_plugins(p):
      try:
        com = m.__dict__['additional_plugin_%scommand' % p]
        self['%scommand' % p].append(com)
        logging.info('An additional plugin %scommand will be applied: "%s"' 
                     % (p, com))
      except KeyError:
        logging.debug('No additional plugin %scommand will be applied' % p)
        
    self.event_modifiers = []
    for mod in self['event-modifier']:
      m = __import__(mod, globals(), locals(), ['*'])
      
      if "modify_general" in dir(m):
        # m=m to capture m when the function is created (not when it is called)
        # otherwise, when it was called, m would always be the last imported 
        # module
        modify = lambda event, m=m: m.modify_general(configuration=self, 
                                                     event=event)
      else:
        modify = m.modify
         
      self.event_modifiers.append(modify)
      process_additional_plugins('pre')
      process_additional_plugins('post')
      
  def __setup_ros_rob_details(self):
    try: # try getting a literal (in case it's a dict)
      self['ros2rob'] = literal_eval(self['ros2rob'])
    except ValueError: # not a proper dict -> must be a module
      self['ros2rob'] = __import__(self['ros2rob']).ros2rob
    set_dcm_strategy([self['dcm-prefetch-strategy']])
    set_dc_ros2rob(self['ros2rob'])
    set_l1r_robs(self['extra-l1r-robs'])
      
  def __setup_db_defaults(self):
    db_defaults, dbextra_defaults = self.__get_db_defaults()
    for k, v in db_defaults.items():
      if not self[k]:
        self[k] = v if not callable(v) else v()
    for k, v in dbextra_defaults.items():
      if not k in self['db-extra'] or not self['db-extra'][k]:
        self['db-extra'][k] = v if not callable(v) else v()
  
  def __setup_miscellaneous(self):
    if not self['interactive']:
      # Set PyROOT to batch mode
      from sys import argv
      argv.insert(1, '-b')
      
    if not self['python-setup']:
      extrastr = 'Db' if self['use-database'] else ''
      self['python-setup'] = "TrigPSC/TrigPSCPython%sSetup.py" % (extrastr,)
      
    self['libraries'] = ['TrigPSC', 'TrigServices']
    
  def __update_run_number(self):
    # If we have ever got a custom run number, use it; otherwise, retrieve it 
    # from the event stream 
    if not self['run-number']:
      logging.debug('Run number not provided (==0). It will be read from the '
                    'input stream')
      self['run-number'] = self.stream.current_run_number()
    # if no run number was ever given by the user (in the command line or during
    # interactive mode) nor obtained from the event stream, we cannot run
    if not self['run-number']:
      raise run_number_error, ('No run number found. Cannot continue with '
                               'run number 0')
    logging.info('Using run number %d' % self['run-number'])
  
  def __get_sor(self, rparams):    
    tsor = self['sor-time']
    if not tsor:
      tsor = rparams['SORTime']
    return tdaq_time_str_from_microsec(int(tsor / 1e9),
                                       int(tsor % 1e9 / 1e3))
  def __get_dmask(self, rparams):
    if self['detector-mask']:
      dmask = hex(self['detector-mask'])
    elif rparams is not None:
      dmask = (rparams['DetectorMask'] if self['run-number'] >= dblim_rnum
                                       else hex(rparams['DetectorMask']))
    else:
      dmask = hex(0xffffffffffffffff)

    dmask = dmask.lower().replace('0x', '').replace('l', '')
    return '0' * (32 - len(dmask)) + dmask # (pad with 0s)
    
  def __get_db_defaults(self):
    #   Unnecessary db connections to COOL are avoided below by the usage of the 
    # method __get_cool_default_getter (see respective documentation). 
    #   By delaying the connection, we ensure that the run  number being used 
    # does not need to be present in COOL, as long as the user specified all 
    # required keys (so that we never need to get their defaults). If this 
    # wasn't the case and the user chose a run number that wasn't present in 
    # COOL, the current athenaHLT run would be aborted, even if the all required 
    # parameters were already defined - that is, even though no reason to 
    # connect to COOL ever existed.  
    db_defaults = {'db-type'    : 'Coral', # dblookup
                   'db-server'  : 'TRIGGERDB',
                   'db-smkey'   : self.__get_cool_default_getter('smkey'),
                   'db-hltpskey': self.__get_cool_default_getter('hltpskey')}
    dbextra_defaults={'lvl1key' : self.__get_cool_default_getter('lvl1key'),
                      'schema'  : 'ignored (dblookup)',
                      'user'    : 'ignored (dblookup)',
                      'password': 'ignored (dblookup)'}    
    return db_defaults, dbextra_defaults
    
  def __get_cool_default_getter(self, param):
    """
    This method returns a callable that, when called, obtains a parameter from 
    cool for the run number of this configuration. The returned callable takes
    no parameters of its own.
    
    The accepted parameters are 'smkey', 'hltpskey', 'lvl1key'. ValueError is 
    raised on any other parameter.
    
    Notice that no db connection is ever created until the callable returned by 
    this method is called. Notice also that one db connection is created at 
    most, no matter how many getters are called, nor how many times they are 
    called.    
    """
    def db():
      if not self.dbconn:
        i = 1 if self['run-number'] >= dblim_rnum else 0
        self.dbconn = CoolUtil.GetConnection(prescales_db_strs[i])
      return self.dbconn
    run = int(self['run-number'])
    rlst = [[run,run]]
    if param == 'smkey':
      ret = lambda: str(CoolUtil.getHLTConfigKeys(db(), rlst)[run]['SMK'])
    elif param == 'hltpskey':
      def ret():
        hltpskeys = CoolUtil.getHLTPrescaleKeys(db(), rlst)[run]["HLTPSK2"]
        return str([(int(b),int(a)) for a,b,c in hltpskeys])
    elif param == 'lvl1key':
      ret = lambda: str(CoolUtil.getL1ConfigKeys(db(),rlst)[run]['LVL1PSK'][0][0])
    else:
      raise ValueError("Invalid parameter '%s': wouldn't know how to get it "
                       "from COOL" % param)
    
    return ret
  
  def __get_run_params(self):
    i = 1 if self['run-number'] >= dblim_rnum else 0
    #dbcon = AtlCoolLib.indirectOpen(runparams_db_strs[i], True, True, False)
    dbcon = AtlCoolLib.readOpen(runparams_db_strs[i])
    folder = dbcon.getFolder(runparams_paths[i])
    
    # need to keep sor variable while using payload (cannot do the following in
    # one single line nor overwrite sor). Otherwise: 1) GC comes into play; 
    # 2) the object is deleted; 3) since it's a shared_ptr, the internal 
    # cool::IObject also gets deleted; 4) payload is not valid any longer
    try:
      sor = folder.findObject(self['run-number'] << 32, 0)
    except Exception:
      return None        # This can happen for unknown run numbers

    payload = sor.payload()
    return {k: payload[k] for k in payload}
  
  @contextmanager
  def make_dbextra_rw_context(self):
    self['db-extra-rw'] = self['db-extra'].copy() # shallow copy is enough
    try:
      yield
    finally:
      del self['db-extra-rw']
    
  def __get_db_config_ptrees(self):
    with self.make_dbextra_rw_context():
      ipt, atpt, dbpt, l1pt = (ptree(), ptree(), self.__get_trigdb_ptree(), 
                               self.__get_lvl1conf_ptree()) 
      ipt['hltPrescaleKey'] = self['db-hltpskey']
      # by now, all the db-extra-rw parameters that are recognized and have a 
      # dedicated ptree slot should have been removed
      others = 'additionalConnectionParameters.additionalConnectionParameter'
      for k, v in self['db-extra-rw'].items():
        ipt.add(others, "%s=%s" % (k,v))
  
    self.__add_precommands_ptree(ipt)
    self.__add_postcommands_ptree(ipt)
    self.__add_log_levels(atpt)
    atpt['pythonSetupFile'] = self['python-setup']
    return ipt, atpt, dbpt, l1pt
  
  def __get_trigdb_ptree(self):
    dbpt = ptree()
    dbpt['Type'] = self['db-type']
    dbpt['Server'] = self['db-server']
    dbpt['SuperMasterKey'] = self['db-smkey']
    dbpt['User'] = self['db-extra-rw'].pop('user')
    dbpt['Password'] = self['db-extra-rw'].pop('password')
    dbpt['Name'] = self['db-extra-rw'].pop('schema')
    dbpt['Alias'] = self['db-server'] if self['db-type'] == 'Coral' else ''
    return dbpt
  
  def __get_lvl1conf_ptree(self):
    l1pt = ptree()
    l1pt['Lvl1PrescaleKey'] = str(self['db-extra-rw'].pop('lvl1key'))
    return l1pt
      
  def __get_joboptions_config_ptree(self):
    inner = ptree()
    inner['jobOptionsPath'] = self['joboptions']
    inner['pythonSetupFile'] = self['python-setup']
    inner['showInclude'] = str(self['show-includes'])
    inner['tracePattern'] = self['trace']
    inner['evtSel'] = 'NONE'
    self.__add_log_levels(inner)
    self.__add_precommands_ptree(inner)
    self.__add_postcommands_ptree(inner)
    return inner
  
  def __finish_config_ptree(self, pt):
    pt["Configuration.HLTMPPUApplication.UID"] = "athenaHLT"
    pt["Configuration.Partition.UID"] = "NONE"
  
  def __add_config_ptree(self, pt):
    if self['use-database']:
      ptree_extension = dbpy_ptree_extension
      ipt, atpt, dbpt, l1pt = self.__get_db_config_ptrees()
      pt.add_child(trigdb_con_ptree_path, dbpt)
      pt.add_child(lvl1_config_ptree_path, l1pt)
      pt.add_child(athenaHLT_ptree_path, atpt)
    else:
      ptree_extension = joboptions_ptree_extension
      ipt = self.__get_joboptions_config_ptree()
    self.__add_common_ptree(ipt)
    self.__add_muoncal_ptree(pt)
    self.__add_ros2rob_ptree(pt)
    pt.add_child('.'.join([base_hlt_ptree_path, ptree_extension]), 
                 ipt)
    self.__finish_config_ptree(pt)
  
  def __add_libraries_ptree(self, pt):
    inner = ptree()
    for lib in self['libraries']:
      inner.add('library', lib)
    pt.add_child("libraries", inner)
  
  def __add_common_ptree(self, pt):
    self.__add_libraries_ptree(pt)
    
    comm = ptree()
    comm["messageSvcType"] = self['msgsvc-type']
    comm["jobOptionsSvcType"] = self['joboptionsvc-type']
    comm["dllName"] = self['appmgrdll']
    comm["factoryName"] = self['appmgrfactory']
    pt.add_child(common_ptree_extension, comm)
    
  def __add_muoncal_ptree(self, pt):
    if self['muoncal-buffername'] or self['muoncal-buffersize']:
      pt[muoncalbuf_path + ".CircName"] = self['muoncal-buffername']
      pt[muoncalbuf_path + ".CircSize"] = self['muoncal-buffersize']
      
  def __add_ros2rob_ptree(self, pt):
    r2rpt = ptree()
    r2rdict = self['ros2rob']
    if r2rdict:
      for ros, roblist in r2rdict.iteritems():
        rospt = ptree()
        for rob in roblist:
          rospt.add('ROBID', str(rob))
        r2rpt.add_child(ros, rospt)
    pt.put_child(ros2rob_path, r2rpt)
  
  def __add_log_levels(self, pt):
    for logl in self['log-level'].split(','):
      pt.add('logLevels.logLevel', logl)
  
  def __add_precommands_ptree(self, pt):
    self.__add_pcommands_ptree(pt, 'pre')
  def __add_postcommands_ptree(self, pt):
    self.__add_pcommands_ptree(pt, 'post')
  def __add_pcommands_ptree(self, pt, p):
    pcommands = ptree()
    for prec in self['%scommand' % p]:
      pcommands.add("%sCommand" % p, prec)
    pt.add_child("%sCommands" % p, pcommands)
  
 
################################################################################
#                                   Tests                                      #
################################################################################

import unittest
from datetime import datetime as dt
from HLTTestApps import random_sub_dict
from types import ModuleType
import sys

# this is used in other modules
class dummy_configuration(dict):
  __getattr__ = configuration.__getattr__.__func__
  
def get_virtual_module(modname):
  mod = ModuleType(modname)
  sys.modules[modname] = mod
  return mod

class configuration_tests(unittest.TestCase):
  cli_base_args = []
  special_cli_args = []
  regular_config_params = {'number-of-events': '12345',
                         'rewind': 'True',
                         'run-number': '22',
                         'verbosity': '3',
                         'ers-debug-level': '1',
                         'save-output': 'fakeoutfile',
                         'timeout': '{"timeout": 123, '
                                    '"warn_fraction": 0.125}',
                         'perfmon': '',
                         'tcmalloc': '',
                         'use-compression': '4',
                         'max-result-size': '500',
                         'debug': '',
                         'stdcmalloc': ''}

  def test_defaults(self):
    c = configuration(self.opt_spec, 
                      self._gen_complete_args())
    self.assertEquals(set(self.opt_spec.keys()), set(c.defaults.keys()), 
                      "There are missing or unexpected parameters in the "
                      "defaults")
    for k in c.defaults:
      expect = self.opt_spec.get_default(k)
      actual = c.defaults[k]
      self.assertEquals(actual, expect, "Wrong default for '%s'; expected '%s';"
                                        " got '%s'" % (k, expect, actual))
  def test_regular_config_params(self):
    self._test_regular_config_params(self.regular_config_params)
  def test_ptree_dllName(self):
    dllname = "fake_dll_name"
    path = self.hltcomm + ".dllName"
    pt = self._gen_complete_config_ptree(["--appmgrdll", dllname])
    self._test_ptree_value(pt, path, dllname)
  def test_ptree_factoryName(self):
    factname = "fake_factory_name"
    path = self.hltcomm + ".factoryName"
    pt = self._gen_complete_config_ptree(["--appmgrfactory", factname])
    self._test_ptree_value(pt, path, factname)
  def test_ptree_joboptions_svc(self):
    josvc = 'fakejosvc'
    path = self.hltcomm + ".jobOptionsSvcType"
    pt = self._gen_complete_config_ptree(["--joboptionsvc-type", josvc])
    self._test_ptree_value(pt, path, josvc)
  def test_ptree_message_svc(self):
    msgsvc = 'fakemsgsvc'
    path = self.hltcomm + '.messageSvcType'
    pt = self._gen_complete_config_ptree(["--msgsvc-type", msgsvc])
    self._test_ptree_value(pt, path, msgsvc)
  def test_ptree_muoncal(self):
    bufname, bufsize = "foobar", '5'
    pt = self._gen_complete_config_ptree(['--muoncal-buffername', bufname,
                                          '--muoncal-buffersize', bufsize])
    self._test_ptree_value(pt, muoncalbuf_path + ".CircName", bufname)
    self._test_ptree_value(pt, muoncalbuf_path + ".CircSize", bufsize)
  def test_ptree_ros2rob_map(self):
    r2r = {'ROS1': [1, 2, 3], 
           'ROS2': [4, 5, 6], 
           'ROS3': [7, 8, 9]}
    pt = self._gen_complete_config_ptree(['--ros2rob', repr(r2r)])
    def r2r_pred(r2rpt):
      #import pdb; pdb.set_trace()
      # are all keys the same?
      outters = sorted(r2rpt.keys()) == sorted(r2r.keys())
      # given a ros, get the list of rob ids, as ints, from the ptree
      getrobs = lambda ros: [int(x) for x in r2rpt.get_child(ros).values_data()]
      # for each inner ptree (each ROS), is it the same as in the original list?
      inners = [sorted(getrobs(ros)) == sorted(r2r[ros]) for ros in r2r]
      # did we get True everywhere?
      return outters and all(inners)
    # now check the ros2rob ptree
    self._test_ptree_pred(pt, ros2rob_path, r2r_pred)
  def test_verbosity(self):
    level = 'logging.DEBUG'
    c = configuration(self.opt_spec, 
                      self._gen_complete_args(['--verbosity', level]))
    target = eval(level)
    actual = logging.getLogger().getEffectiveLevel()
    self.assertEquals(actual, target, "Logging level not correct. Expected %s "
                                      "but got %s" % (target, actual))
  def test_ers_debug_level(self):
    level = '0'
    c = configuration(self.opt_spec,
                      self._gen_complete_args(['--ers-debug-level', level]))
    actual = str(get_ers_debug_level())
    self.assertEquals(level, actual, "ERS debug level not correct. Expected %s "
                                     "but got %s" % (level, actual))
  def test_pyroot_batch(self):
    from sys import argv
    cli_args = self._gen_complete_args() # not interactive 
    c = configuration(self.opt_spec, cli_args)
    self.assert_(len(argv) >= 2 and argv[1] == '-b', 
                 "Not ready for PyROOT batch mode")  
  
  def test_event_modifier(self):
    mod = get_virtual_module("fakeModifier")
    mod.modify = lambda x: x+1
    mod.additional_plugin_precommand = "'prec'"
    mod.additional_plugin_postcommand = "'postc'"
        
    c = configuration(self.opt_spec,
                      self._gen_complete_args(["--event-modifier", 
                                               "['%s']" % mod.__name__]))
    
    n = len(c.event_modifiers)
    self.assertEquals(n, 1, "Expected 1 event modifier; got %d" % n)
    modified_1 = c.event_modifiers[0](1)
    self.assertEquals(modified_1, 2, "Expected to get 2 as a modified 1; "
                                     "got %s" % modified_1)
    
    def ensurepcommand(p):
      n = len(c['%scommand' % p])
      self.assertEquals(n, 1, "Expected 1 %scommand; got %d" % (p, n))
      pc = eval(c['%scommand' % p][0])
      expect = '%sc' % p
      self.assertEquals(pc, expect, 
                        "Expected the %scommand %s; got %s" % (p, expect, pc))

    c['precommand'] = [x for x in c['precommand'] if 'OfflineTHist' not in x
                                                  and '_run_number' not in x]    
    ensurepcommand('pre')
    ensurepcommand('post')
  
  def test_event_modifier_general(self):
    mod = get_virtual_module("fakeModifierGeneral")
    mod.modify = lambda x: self.assert_(False, 
                                        ("%s.modify called, this should "
                                         "not have happened") % mod.__name__)
    mod.modify_general = lambda **kwargs: 10 * kwargs['event']
     
    c = configuration(self.opt_spec,
                      self._gen_complete_args(["--event-modifier",
                                               "['%s']" % mod.__name__]))

    modified_1 = c.event_modifiers[0](1)
    
    self.assertEquals(modified_1, 10, "Expected to get 10 as a modified 1; "
                                      "got %s" % modified_1)
  
  def test_run_params(self):
    pt = self._gen_complete_prepare_ptree(['--run-number', '177531'])
    expect = {'timeSOR': '13/3/11 17:15:16', 
              'run_number': '177531', 
              'det_mask': '00000000000000000001fffffffffff7',
              'recording_enabled': 'true', 
              'run_type': 'Physics'}
    for k, v in expect.items():
      self._test_ptree_value(pt, 'RunParams.%s' % k, v)
  def test_run_params_run2(self):
    pt = self._gen_complete_prepare_ptree(['--run-number', '238742'])
    expect = {'timeSOR': '8/9/14 21:54:10', 
              'run_number': '238742', 
              'det_mask': '000000000000000000003f8000ffff00',
              'recording_enabled': 'true', 
              'run_type': 'Physics'}
    for k, v in expect.items():
      self._test_ptree_value(pt, 'RunParams.%s' % k, v)
  def test_explicit_detector_mask(self):
    pt = self._gen_complete_prepare_ptree(['--detector-mask', 123])
    expect = '0000000000000000000000000000007b'
    self._test_ptree_value(pt, 'RunParams.det_mask', expect)
  def test_default_detector_mask(self):
    pt = self._gen_complete_prepare_ptree(['--run-number', '177531',
                                           '--detector-mask', '0L'])
    expect = '00000000000000000001fffffffffff7'
    self._test_ptree_value(pt, 'RunParams.det_mask', expect)
  def test_explicit_time_human_readable(self):
    trepr = '10/12/13 20:07:13'
    pt = self._gen_complete_prepare_ptree(['--sor-time', trepr + '.0'])
    self._test_ptree_value(pt, 'RunParams.timeSOR', trepr)
  def test_explicit_time_nanos(self):
    nanos = 1386702433971151000
    pt = self._gen_complete_prepare_ptree(['--sor-time', nanos])
    self._test_ptree_value(pt, 'RunParams.timeSOR', '10/12/13 20:07:13.971151')
  def test_explicit_time_now(self):
    pt = self._gen_complete_prepare_ptree(['--sor-time', 'now'])
    def less_than_5s_ago(val):
      from optspec import common
      tdelta = dt.now() - dt.strptime(val, common['sor-time']['format'])
      return tdelta.total_seconds() < 5
    self._test_ptree_val_pred(pt, 'RunParams.timeSOR', less_than_5s_ago)
  def test_default_time(self):
    pt = self._gen_complete_prepare_ptree(['--sor-time', 0])
    self._test_ptree_value(pt, 'RunParams.timeSOR', '13/3/11 17:15:16')
  def test_mon_offline_config_ptree_empty(self):
    pt = self._gen_complete_mon_config_ptree()
    self.assertEquals(pt.keys(), [], "offline mon config ptree not empty")
  def test_mon_online_config_ptree_paths(self):
    pt = self._gen_complete_mon_config_ptree(['--oh-monitoring'])
    self._test_ptree_path(pt, mon_config_rule_path + ".UID")
    self._test_ptree_path(pt, mon_config_rule_path + ".Name")
    self._test_ptree_path(pt, oh_params_path + ".OHServer")
    self._test_ptree_path(pt, oh_params_path + ".ROOTProvider")
    self._test_ptree_path(pt, oh_params_path + ".PublishInterval")
    self._test_ptree_path(pt, oh_params_path + ".NumberOfSlots")
  def test_mon_online_config_ptree_include_filter(self):
    val = 'abc'
    pt = self._gen_complete_mon_config_ptree(['--oh-monitoring', 
                                             '--histogram-include', val])
    self._test_ptree_value(pt, mon_config_rule_path + ".IncludeFilter", val)
  def test_mon_online_config_ptree_exclude_filter(self):
    val = 'abc'
    pt = self._gen_complete_mon_config_ptree(['--oh-monitoring', 
                                             '--histogram-exclude', val])
    self._test_ptree_value(pt, mon_config_rule_path + ".ExcludeFilter", val)
  def test_mon_online_config_ptree_publish_interval(self):
    val = '10'
    pt = self._gen_complete_mon_config_ptree(['--oh-monitoring', 
                                             '--histogram-publishing-interval', 
                                             val])
    self._test_ptree_value(pt, oh_params_path + ".PublishInterval", val)
  def test_mon_offline_run_ptree_empty(self):
    pt = self._gen_complete_mon_run_ptree()
    self.assertEquals(pt.keys(), [], "offline mon prepare ptree not empty")
  def test_mon_online_run_ptree_appname(self):
    pt = self._gen_complete_mon_run_ptree(['--oh-monitoring'])
    self._test_ptree_value(pt, 'appName', 'athenaHLT')
  def _get_hlt_conf_path(self):
    return '.'.join([base_hlt_ptree_path, self.ptree_extension])
  def _gen_config_ptree(self, cli_args):
    c = configuration(self.opt_spec, cli_args)
    pt = c.get_config_ptree()
    self._test_config_ptree_basic(pt)
    return pt
  def _gen_prepare_ptree(self, cli_args):
    c = configuration(self.opt_spec, cli_args)
    pt = c.get_prepare_ptree()
    self.assertEquals(pt.keys(), ['RunParams'])
    return pt
  def _gen_mon_config_ptree(self, cli_args):
    c = configuration(self.opt_spec, cli_args)
    return c.get_mon_config_ptree()
  def _gen_mon_run_ptree(self, cli_args):
    c = configuration(self.opt_spec, cli_args)
    return c.get_mon_prepareWorker_ptree()
  def _gen_complete_config_ptree(self, additional_opts=[], additional_args=[]):
    return self._gen_config_ptree(self._gen_complete_args(additional_opts, 
                                                          additional_args))
  def _gen_complete_prepare_ptree(self, additional_opts=[], additional_args=[]):
    return self._gen_prepare_ptree(self._gen_complete_args(additional_opts, 
                                                           additional_args))
  def _gen_complete_mon_config_ptree(self, additional_opts=[], 
                                     additional_args=[]):
    return self._gen_mon_config_ptree(self._gen_complete_args(additional_opts, 
                                                              additional_args))
  def _gen_complete_mon_run_ptree(self, additional_opts=[],
                                      additional_args=[]):
    return self._gen_mon_run_ptree(self._gen_complete_args(additional_opts, 
                                                               additional_args))
  def _test_regular_config_params(self, params):
    d = params
    additional_args = sum([["--%s" % k, v] for k, v, in d.items()], [])
    c = configuration(self.opt_spec, 
                      self._gen_complete_args(additional_args))
    for k in d:
      # regular strings need the 1st part of the or, flags need the 2nd part 
      # and literals with more than one valid string representation need the
      # 3rd part of the or 
      self.assert_(str(c[k]) == d[k] or c[k] == True or c[k] == eval(d[k]),
                   "Option '%s' was not correctly taken into account. " 
                   "Expected '%s' but got '%s'" % (k, d[k], c[k]))
  def _test_ptree_path(self, pt, path):
    self.assert_(path in pt, "No node '%s' found in the configuration ptree"
                             % path)
  def _test_ptree_pred(self, pt, path, test):
    self._test_ptree_path(pt, path)
    child = pt.get_child(path)
    msg = ("The sub tree with path '%s' in the configuration ptree, does "
           "not pass the required condition:\n%s" % (path, test))
    if test.__doc__:
      msg += ".__doc__: %s" % test.__doc__
    self.assert_(test(child), msg)
  def _test_ptree_val_pred(self, pt, path, test):
    self._test_ptree_path(pt, path)
    val = pt[path]
    msg = ("The value of the node '%s' in the configuration ptree ('%s') does "
           "not pass the required condition:\n%s" % (path, val, test))
    if test.__doc__:
      msg += ".__doc__: %s" % test.__doc__
    self.assert_(test(val), msg)
  def _test_ptree_value(self, pt, path, value):
    pred = lambda v: v == value
    pred.func_name = "equals_%s" % value
    pred.__doc__ = "checks whether a certain value equals %s" % value
    self._test_ptree_val_pred(pt, path, pred)
  def _test_ptree_default_value(self, pt, path, arg):
    expect = self.opt_spec.get_default(arg)
    self._test_ptree_value(pt, path, expect)
  def _test_config_ptree_basic(self, pt):
    self.assertEquals(pt.keys(), ['Configuration'])
    self._test_ptree_path(pt, self.hltconf)
    self._test_ptree_path(pt, self.hltcomm)
  def _test_ptree_libraries(self, pt):
    lib_path = self.hltconf + ".libraries"
    self._test_ptree_path(pt, lib_path)
    libs = pt.get_child(lib_path)
    for k in libs:
      self.assertEquals(k, "library", 
                        "Node '%s' in '%s' is not a library" % (k, lib_path))
    expect = set(['TrigPSC', 'TrigServices'])
    actual = set([x.data() for x in libs.values()])
    self.assertEquals(actual, expect, 
                      "Unexpected or missing libraries. Expected '%s'; got '%s'"
                      % (expect, actual))
  def _test_ptree_common(self, pt):
    self._test_ptree_libraries(pt)
    self._test_ptree_default_value(pt, self.hltcomm + ".messageSvcType", 
                                   'msgsvc-type')
    self._test_ptree_default_value(pt, self.hltcomm + ".dllName", 
                                   'appmgrdll')
    self._test_ptree_default_value(pt, self.hltcomm + ".factoryName", 
                                   'appmgrfactory')
    
  def _gen_complete_args(self, additional_opts=[], additional_args=[]):
    return (self.cli_base_args + 
            additional_opts + 
            self.special_cli_args + 
            additional_args)
    

class file_based_configuration_tests(configuration_tests):
  def setUp(self):
    super(file_based_configuration_tests, self).setUp()
    self.opt_spec = option.file_opt_spec
    self.cli_base_args = ["-f", datafiles[0]]
    self.hltconf = self._get_hlt_conf_path()
    self.hltcomm = '.'.join([self.hltconf, common_ptree_extension])
  def test_emon_not_allowed(self):
    self.assertRaises(option.CurrentlyNotSupported, configuration, 
                      option.emon_opt_spec, ['fake_arg'])
  def test_file(self):
    from os import environ, path
    tf = datafiles[0]
    environ['TESTVAR'] = path.dirname(tf)
    cli_args = ['--file', '$TESTVAR/%s' % path.basename(tf)] 
    cli_args += self.special_cli_args
    c = configuration(self.opt_spec, cli_args)
    self.assertEquals(c['file'], [tf])
  def test_skip_too_many_events(self):
    self.assertRaises(option.BadOptionSet, configuration,
                      option.file_opt_spec, 
                      self._gen_complete_args(['--skip-events', '10e20']))
    
class save_output_configuration_tests(file_based_configuration_tests):
  # we need these to make things work
  special_cli_args = ['fake_joboptions_needed_for_successful_config']
  ptree_extension = 'needed_but_irrelevant_since_no_ptree_tests'
  
  # To be used by the configuration, so that we don't need a valid file
  class MockPausableIstream(pausable_istream):
    # we also need a data reader for invalid file
    class MockDataReader(object):
      # we only care about the filename
      def __init__(self, fname):
        self.fname = fname
      # our mock istream will need these in inherited current_* methods
      def fileName(self):
        return self.fname
      def runNumber(self):
        return 1234567890
    def __init__(self, filelist):
      # the data reader is the only thing we do need
      self.dr = self.MockDataReader(filelist[0])
      # the rest is just so that our mock works with the configuration
      self.total_events = 1000
  
  def setUp(self):
    super(save_output_configuration_tests, self).setUp()
    self.convd1 = {'ProjectTag': 'ptag',
                  'LumiBlockNumber': 333,
                  'StreamName': 'sname',
                  'StreamType': 'stype',
                  'RunNumber': 999999999,
                  'ProductionStep': 'pstep',
                  'dir': '/tmp'}
    self.convd2 = {'ProjectTag': 'testcase',
                  'LumiBlockNumber': 1,
                  'StreamName': 'FakeStream',
                  'StreamType': 'debug',
                  'RunNumber': 123,
                  'ProductionStep': 'imagine',
                  'dir': '/fakedir'}
    self._setup_cli_args()
    # we replace the global pausable_istream with our mock
    self._replace_pausable_istream()
  def tearDown(self):
    # restore the global pausable_istream for other tests
    super(save_output_configuration_tests, self).tearDown()
    self._restore_pausable_istream()
    
  def test_save_output_plain(self):
    intended_dir = '/a/b/c'
    intended_fncore = 'd'
    outfile = '%s/%s' % (intended_dir, intended_fncore)
    cli_args = self._gen_complete_args(['--save-output', outfile])
    c = configuration(self.opt_spec, cli_args)
    actual_dir, actual_fncore = c.parsed_out_data_filename()
    self.assertEquals(actual_dir, intended_dir, 
                      'Wrong output directory: "%s". Expected "%s"'
                      % (actual_dir, intended_dir))
    self.assertEquals(actual_fncore, intended_fncore,
                      'Wrong output filename core: "%s". Expected "%s"'
                      % (actual_fncore, intended_fncore))
  def test_save_output_conventional_all(self):
    # build expected stuff
    expect_dir = self.convd1['dir']
    # should generate "ptag.999999999.stype_sname.pstep.RAW._lb0333._athenaHLT"
    expect_fncore = self._gen_filename_core(self.convd1)
    # ask the configuration what it produces
    cli_args = self._gen_complete_args(['--save-output-conventional', 
                                        str(self.convd1)])
    c = configuration(self.opt_spec, cli_args)
    actual_dir, actual_fncore = c.parsed_out_data_filename()
    # compare results with expected
    self.assertEquals(actual_dir, expect_dir, 
                      'Wrong output directory: "%s". Expected "%s".'
                      % (actual_dir, expect_dir))
    self.assertEquals(actual_fncore, expect_fncore, 
                      'Wrong output filename core: "%s". Expected "%s"'
                      % (actual_fncore, expect_fncore))
  def test_save_output_conventional_some(self, default=None):
    # input and default dicts
    subconvd = random_sub_dict(self.convd1)
    if not default:
      default = self.convd2
      # the directory should be taken from the default and not the input file
      default['dir']=self.opt_spec['save-output-conventional']['allowed']['dir']
    # take from default all the values not provided in the option
    expectd = default.copy()
    expectd.update(subconvd)
    # generate expected dict taking defaults from input filename
    
    # the expected filename and directory derive from this
    expect_fncore = self._gen_filename_core(expectd)
    expect_dir = expectd['dir']
    # ask the configuration what it produces
    cli_args = self._gen_complete_args(['--save-output-conventional',
                                        str(subconvd)])
    c = configuration(self.opt_spec, cli_args)
    actual_dir, actual_fncore = c.parsed_out_data_filename()
    # compare results with expected
    self.assertEquals(actual_dir, expect_dir, 
                      'Wrong output directory: "%s". Expected "%s".'
                      % (actual_dir, expect_dir))
    self.assertEquals(actual_fncore, expect_fncore, 
                      'Wrong output filename core: "%s". Expected "%s"'
                      % (actual_fncore, expect_fncore))
  def test_save_output_conventional_some_bad_input(self):
    # when the input filename has a invalid core
    self._setup_cli_args(bad_input=True)
    default = self.opt_spec['save-output-conventional']['allowed']
    self.test_save_output_conventional_some(default)
  def _replace_pausable_istream(self):
    global pausable_istream
    pausable_istream, self.real_pausable_istream = (self.MockPausableIstream, 
                                                    pausable_istream)
  def _restore_pausable_istream(self):
    global pausable_istream
    pausable_istream = self.real_pausable_istream
  def _setup_cli_args(self, bad_input=False):
    # fake file, with good or bad core name
    fncore = 'foobar' if bad_input else self._gen_filename_core(self.convd2)
    self.cli_base_args = ["-f", "%s/%s" % (self.convd2['dir'], fncore)]
  def _gen_filename_core(self, conventional_dict):
    return ('%s.%08d.%s_%s.%s.RAW._lb%04d._%s' 
            % (conventional_dict['ProjectTag'],
               conventional_dict['RunNumber'],
               conventional_dict['StreamType'],
               conventional_dict['StreamName'],
               conventional_dict['ProductionStep'],
               conventional_dict['LumiBlockNumber'],
               appName))

class emon_based_configuration_tests(configuration_tests):
  def setUp(self):
    super(emon_based_configuration_tests, self).setUp()
    pass # TODO

class pcommands_tests:
  def __init__(self):
    raise NotImplementedError, ("This class is meant as a simple ABC and " +
                                "should not be used directly")
  def test_ptree_perfmon(self):
    pt = self._gen_complete_config_ptree(['--perfmon'])
    self._test_in_precommands(pt, 'perfmon')
  def test_ptree_offline_histogramming(self):
    pt = self._gen_complete_config_ptree() # no online monitoring 
    self._test_in_precommands(pt, 'include("TrigServices/OfflineTHistSvc.py")')
  def test_ptree_online_histogramming(self):
    pt = self._gen_complete_config_ptree(['--oh-monitoring'])
    cmd = 'include("TrigServices/OfflineTHistSvc.py")'
    self._test_not_in_precommands(pt, cmd)
  def test_ptree_leak_check_execute(self):
    pt = self._gen_complete_config_ptree(["--leak-check-execute"])
    self._test_in_precommands(pt, 'doLeakCheck')
  def test_ptree_leak_check_all(self):
    pt = self._gen_complete_config_ptree(["--leak-check", "all"])
    self._test_in_precommands(pt, 'doLeakCheck')
  def test_ptree_leak_check_initialize(self):
    pt = self._gen_complete_config_ptree(["--leak-check", "initialize"])
    self._test_in_precommands(pt, 'doLeakCheck')
  def test_ptree_delete_check_execute(self):
    pt = self._gen_complete_config_ptree(["--delete-check", "execute"])
    self._test_in_precommands(pt, 'doLeakCheck')
    self._test_in_precommands(pt, 'delete')
  def test_ptree_precommands(self):
    self._test_ptree_pcommands('pre')
  def test_ptree_postcommands(self):
    self._test_ptree_pcommands('post')
  def _test_ptree_pcommands(self, p):
    coms = ["command1", "command2", ";", ";;", "; ;", ";;;", ';;command3', 
            ';command4;', 'command5;']
    target = ["command%d" % (i+1) for i in range(5)]
    
    pt = self._gen_complete_config_ptree(["--%scommand=%s" % (p, str(coms))])
    actual = pt.get_child(self.hltconf + '.%sCommands' % p).values_data()
    actual = filter(lambda c: 'include' not in c and '_run_number' not in c, 
                    actual)
    
    self.assertEquals(actual, target, "%scommands in config ptree don't match "
                      "expected. Expected: %s; Got: %s" % (p, target, actual))
  def _test_in_precommands(self, pt, substr):
    self._test_in_precommands_aux(pt, substr, False)
  def _test_not_in_precommands(self, pt, substr):
    self._test_in_precommands_aux(pt, substr, True)
  def _test_in_precommands_aux(self, pt, substr, negate=False):
    precommands = pt.get_child(self.hltconf + '.preCommands')
    precommands = [prec.data() for prec in precommands.values()]
    precommands = ';'.join(precommands)
    assertion = substr in precommands or substr in precommands.lower()
    msg = ("The string '%s' was %sfound in the precommands" % (substr, "%s"))
    if negate:
      self.assertFalse(assertion, msg % "")
    else:
      self.assert_(assertion, msg % "not ")
    

class jo_configuration_tests(file_based_configuration_tests, pcommands_tests):
  joboptions = 'fakejo'
  special_cli_args = [joboptions]
  ptree_extension = joboptions_ptree_extension
    
  def test_simple_config_ptree(self):
    pt = self._gen_complete_config_ptree()
    self._test_ptree_value(pt, self.hltconf + ".jobOptionsPath", 
                           self.joboptions)
    self._test_ptree_value(pt, self.hltconf + '.pythonSetupFile', 
                           "TrigPSC/TrigPSCPythonSetup.py")
    self._test_ptree_value(pt, self.hltconf + '.evtSel', 'NONE')
    self._test_ptree_default_value(pt, self.hltcomm + ".jobOptionsSvcType", 
                                   'joboptionsvc-type')

    self._test_ptree_common(pt)
          
  def test_ptree_python_setup(self):
    pysetup = 'PSCFakeBootstrap.py'
    pt = self._gen_complete_config_ptree(['--python-setup', pysetup])
    self._test_ptree_value(pt, self.hltconf + '.pythonSetupFile', pysetup)    
  def test_ptree_loglevel(self):
    loglev = "DEBUG,INFO,WARNING,ERROR"
    pt = self._gen_complete_config_ptree(['--log-level', loglev])
    def logsright(lpt):
      ll = loglev.split(',')
      for l in lpt.values_data():
        if not l in ll:
          return False
        ll.remove(l)
      return not ll #(whether list is empty)
    self._test_ptree_pred(pt, self.hltconf + '.logLevels', logsright)
  def test_ptree_trace(self):
    pattern = '.*[aA]thena.*'
    pt = self._gen_complete_config_ptree(['--trace', pattern])
    self._test_ptree_value(pt, self.hltconf + '.tracePattern', pattern)
    
  def test_ptree_flags(self):
    flags = {'--show-includes': '.showInclude'}
    pt_n = self._gen_complete_config_ptree()
    pt_y = self._gen_complete_config_ptree(flags.keys())
    for k, v in flags.items():
      path = self.hltconf + v
      self._test_ptree_value(pt_n, path, 'False')
      self._test_ptree_value(pt_y, path, 'True')
  
class dbpy_configuration_tests(file_based_configuration_tests, pcommands_tests):
  special_cli_args = ['--use-database']
  ptree_extension = dbpy_ptree_extension
  dbcon = trigdb_con_ptree_path
  l1con = lvl1_config_ptree_path
  athlt = athenaHLT_ptree_path
  
  regular_config_params = file_based_configuration_tests.regular_config_params \
                                                                         .copy()
  regular_config_params.update({'db-smkey': '123',
                                'db-hltpskey': '321',
                                'db-extra': "{'lvl1key': 99, 'user':'_',"
                                            " 'password':'_','schema':'_'}"})
                           
  
  def test_simple_config_ptree(self):
    pt = self._gen_complete_config_ptree()
    def notempty(v):
      "Check whether a certain value is nonempty" 
      return bool(v)
    self._test_ptree_val_pred(pt, self.hltconf + ".hltPrescaleKey", 
                          notempty)
    self._test_ptree_value(pt, self.hltcomm + ".jobOptionsSvcType", 
                                   'TrigConf::HLTJobOptionsSvc')
    self._test_ptree_common(pt)
  
  def test_ptree_loglevel(self):
    logl = "DEBUG"
    pt = self._gen_complete_config_ptree(['--log-level', logl])
    self._test_ptree_value(pt, self.athlt + ".logLevels.logLevel", logl)
  def test_ptree_dbhltpskey(self):
    hltk = "[(1,2),(3,4)]"
    pt = self._gen_complete_config_ptree(['--db-hltpskey', hltk])
    self._test_ptree_value(pt, self.hltconf + ".hltPrescaleKey", hltk)
  def test_ptree_dblvl1key(self):
    l1k = '9876'
    ext = {'lvl1key': l1k}
    pt = self._gen_complete_config_ptree(['--db-extra', ext])
    self._test_ptree_value(pt, self.l1con + ".Lvl1PrescaleKey", l1k)
  
  def test_ptree_dbtype(self):
    type = "Oracle"
    pt = self._gen_complete_config_ptree(['--db-type', type])
    self._test_ptree_value(pt, self.dbcon + ".Type", type)
  def test_ptree_dbserver(self):
    server = "mydbserver"
    pt = self._gen_complete_config_ptree(['--db-server', server])
    self._test_ptree_value(pt, self.dbcon + ".Server", server)
  def test_ptree_dbsmkey(self):
    smk = "1234"
    pt = self._gen_complete_config_ptree(['--db-smkey', smk])
    self._test_ptree_value(pt, self.dbcon + ".SuperMasterKey", smk)
  def test_ptree_dbschema(self):
    schema = 'ashnlagfkjba'
    ext = {'schema': schema}
    pt = self._gen_complete_config_ptree(['--db-extra', ext])
    self._test_ptree_value(pt, self.dbcon + ".Name", schema)
  def test_ptree_dbuser(self):
    user = 'posjdbngol'
    ext = {'user': user}
    pt = self._gen_complete_config_ptree(['--db-extra', ext])
    self._test_ptree_value(pt, self.dbcon + ".User", user)
  def test_ptree_dbpasswd(self):
    pwd = 'gnasdiogd'
    ext = {'password': pwd}
    pt = self._gen_complete_config_ptree(['--db-extra', ext])
    self._test_ptree_value(pt, self.dbcon + ".Password", pwd)
  def test_ptree_other_extra(self):
    special = {'user': 'asdfsdfin', 'password': 'fbgntuiw', 'schema': 'gsdfg', 
               'lvl1key': '4411'}
    nonspecial = {'foo': 'bar', 'maxretrials': '11', 'bar': 'foo', 
                  'retrialperiod': '22', 'foobar': 'barfoo'}
    ext = special.copy()
    ext.update(nonspecial)
    pt = self._gen_complete_config_ptree(['--db-extra', ext])
    
    pred1 = lambda acpt: False not in ["%s=%s" % (k, v) in acpt.values_data() 
                                       for k, v in nonspecial.items()]
    pred1.func_name = "allNonSpecialInAdditional"
    pred1.__doc__ = ("Check that the following items are in the "
                     "additionalConnectionParameters: " + 
                     ', '.join(["%s" % k for k in nonspecial]))
    
    pred2 = lambda acpt: True not in [val.startswith("%s=") 
                                      for val in acpt.values_data()] 
    
    pred2.func_name = "noSpecialInAdditional"
    pred2.__doc__ = ("Check that none of the following items are in the "
                     "additionalConnectionParameters: " + 
                     ', '.join(["%s" % k for k in special]))
    
    predall = lambda acpt: pred1(acpt) and pred2(acpt)
    predall.func_name = "checkAdditionalConnectionParameters"
    predall.__doc__ = "\nand\n".join([pred1.__doc__, pred2.__doc__])
    
    self._test_ptree_pred(pt, 
                          self.hltconf + ".additionalConnectionParameters", 
                          predall)


if __name__ == '__main__':
  from HLTTestApps import test_main
  test_main(['jo_configuration_tests',  
             'dbpy_configuration_tests',
             'save_output_configuration_tests'])
  
