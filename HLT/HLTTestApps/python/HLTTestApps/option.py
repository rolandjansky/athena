#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Ter 27 Nov 2007 10:41:16 CET 

import logging, types
from ast import literal_eval
from EventApps.myopt import Parser
from optspec import * 

def file_optcheck(option_spec, kwargs, extra):
  """Checks if the options passed make sense all together."""
  
  # do checks that are common to both emon and file based runs
  common_optcheck(option_spec, kwargs, extra)
  
  if len(kwargs['file']) == 0:
    raise BadOptionSet, 'Cannot process without any input files.'

  if kwargs['timeout']['timeout'] < 0:
    raise BadOptionSet, 'You cannot set the timeout value to a negative integer'

  if kwargs['timeout']['warn_fraction'] < 0 or kwargs['timeout']['warn_fraction'] > 1:
    raise BadOptionSet, 'The warn_fraction should be a number in the interval [0,1]'

  if kwargs['interactive'] and kwargs['debug']:
    raise BadOptionSet, 'You cannot run GDB over an interactive session'

  if kwargs['leak-check']:
    allowed = [ 'all', 'initialize', 'start', 'beginrun', 'execute', 'finalize', 'endrun', 'stop' ]
    if not kwargs['leak-check'].lower() in allowed:
      raise BadOptionSet, 'Allowed values for leak-check are %s' % allowed

  if kwargs['delete-check']:
    allowed = [ 'all', 'initialize', 'start', 'beginrun', 'execute', 'finalize', 'endrun', 'stop' ]
    if not kwargs['delete-check'].lower() in allowed:
      raise BadOptionSet, 'Allowed values for delete are %s' % allowed

  if ((kwargs['use-compression'] not in range(1,6)) and (kwargs['use-compression'] != 0)):
    raise BadOptionSet, 'Invalid range for output compression level. Needs to be in range [1-5]'
  
  if ((len(kwargs['save-output']) == 0) and (kwargs['use-compression'] in range(1,6))):
    raise BadOptionSet, 'Option to write compressed output file given, but no output file name specified'

  if ((len(kwargs['save-output']) == 0) and (len(kwargs['use-raw-file-convention']) != 0)):
    raise BadOptionSet, 'Option to write Atlas compliant output file names can only be specified together with the --save-output option'

  if ((len(kwargs['use-raw-file-convention']) != 0) and (len(kwargs['use-raw-file-convention']) != 1) and (len(kwargs['use-raw-file-convention']) != 6)):
    raise BadOptionSet, 'For the option --use-raw-file-convention either only the production step name has to be specified or the full set of all 6 parameters has to be provided'

def emon_optcheck(option_spec, kwargs, extra):
  """Checks if the options passed make sense all together."""
  
  raise CurrentlyNotSupported, "Emon input is currently not supported"
  
def common_optcheck(option_spec, kwargs, extra):
  """
    Checks if the options passed make sense all together, in both emon or file
  based runs.
  """
  unsupported_optcheck(option_spec, kwargs)
  db_optcheck(option_spec, kwargs, extra)
        
  if not kwargs['oh-monitoring']:
    if kwargs['oh-display']:
      raise BadOptionSet, 'You cannot launch an OH display if OH monitoring is not enabled.'
    if kwargs['user-ipc']:
      raise BadOptionSet, 'You cannot use the user ipc ref file if OH monitoring is not enabled.'
    if kwargs['info-service'] != option_spec['info-service']['default']:
      raise BadOptionSet, 'You cannot choose an IInfoRegister if OH monitoring is not enabled.'
    if kwargs['histogram-publishing-interval'] != option_spec['histogram-publishing-interval']['default']:
      raise BadOptionSet, 'You cannot choose a histogram publishing interval if OH monitoring is not enabled.'
    if kwargs['histogram-include'] != option_spec['histogram-include']['default']:
      raise BadOptionSet, 'You cannot choose the histogram inclusion regexp if OH monitoring is not enabled.'
    if kwargs['histogram-exclude'] != option_spec['histogram-exclude']['default']:
      raise BadOptionSet, 'You cannot choose the histogram exclusion regexp if OH monitoring is not enabled.'

def db_optcheck(option_spec, kwargs, extra):
  if kwargs['use-database']:
    check_db_type(option_spec, kwargs)
    check_frontier(kwargs)
    check_smkey(kwargs)
    check_hltpskey(kwargs)
  elif not extra:
    raise BadOptionSet, 'No job options specified'
  else:
    check_disallowed_db_options(option_spec, kwargs)

def unsupported_optcheck(option_spec, kwargs):
  """
    Checks for options that are currently not supported. This shall be removed
  when all options are supported again.
  """
  for k, v in kwargs.iteritems():
    if k not in auto_options and v != option_spec.get_default(k):
      if not k in option_spec.supported and not (k, v) in option_spec.supported:
        raise CurrentlyNotSupported, ("Option '%s=%s' is currently not "
                                      "supported" % (k, v))

def check_disallowed_db_options(option_spec, kwargs):
  dbopts = [o for o in option_spec if option_spec[o]['group'] == 'Database']
  for o in dbopts:
    if o in kwargs and kwargs[o]:
      raise BadOptionSet, ("Option --%s cannot be specified without " + 
                           "--use-database") % o
def check_db_type(option_spec, kwargs):
  if(kwargs['db-type'] != None and 
     kwargs['db-type'] not in option_spec['db-type']['allowed']):
    raise BadOptionSet, ('Unknown db-type "%s". Allowed values are %s' % 
                         (kwargs['db-type'], 
                          option_spec['db-type']['allowed']))
def check_frontier(kwargs):
  if kwargs['use-frontier'] and kwargs['db-type'] not in ['Coral', None]:
    raise BadOptionSet, ("Option --use-frontier can only be used with "
                         "--db-type Coral")    
def check_smkey(kwargs):
  if kwargs['db-smkey'] != None and not check_smkey_aux(kwargs['db-smkey']):
    raise BadOptionSet, ("db-smkey not correct. Please check the " + 
                         "corresponding help message for allowed values")
def check_hltpskey(kwargs):
  if (kwargs['db-hltpskey'] != None and 
      not check_hltpskey_aux(kwargs['db-hltpskey'])):
    raise BadOptionSet, ("db-hltpskey not correct. Please check the " + 
                         "corresponding help message for allowed values")
def check_smkey_aux(s):
  try:
    i = literal_eval(s)
    return isinstance(i, int) and i > 0
  except (ValueError, SyntaxError):
    return False
def check_hltpskey_aux(s):
  if not check_smkey_aux(s):
    try:
      l = literal_eval(s)
      if not isinstance(l, list) or not len(l) > 0: 
        return False # ensure non-empty list
      for count, elem in enumerate(l):
        if not isinstance(elem, tuple) or not len(elem) == 2:
          return False # ensure pair
        for i in elem:
          if not isinstance(i, int) or not i > 0:
            return False # ensure positive integers
        if count > 0 and l[count - 1][0] >= elem[0]:
          return False # ensure strict growing order of the first element
    except (ValueError, SyntaxError):
      return False
  return True

def get_default(option_spec, option_key):
  """
    Get the default value of an option. Raise KeyError if the option is not 
  found
  """
  o = option_spec[option_key]
  return o['default'] if o['arg'] else False

def warn_verbosity(verbosity):
    if verbosity != common['verbosity']['default']:
        logging.warning('Custom verbosity specified. '
                        'Make sure not to overwrite it in the job '
                        'options.')

def gen_parser(option_spec, extra_args):
  """
    Generate an option parser for the options specified by option_spec
  """ 
  parser = Parser(extra_args=extra_args)
  for k, v in option_spec.iteritems():
    parser.add_option(k, v['short'], v['description'], v['arg'], v['default'],
                      v['group'])
  return parser

def parse_commands(c):
  """Parse pre and post commands for trailing or multiple ';'"""
  import re
  c = c.strip()
  c = re.sub(';+',';', c)
  c = re.sub('^;','', c)
  return re.sub(';$','', c)

class option_specification(dict):
  def get_default(self, arg):
    if arg in self.constants:
      return self.constants[arg]
    return get_default(self, arg)
  def optcheck(self):
    raise NotImplementedError, ("no optcheck method was provided for this "
                                "option specification")

# file options specification
file_opt_spec = common.copy()
file_opt_spec.update(fileinput)
file_opt_spec = option_specification(file_opt_spec)
file_opt_spec.optcheck = types.MethodType(file_optcheck, file_opt_spec)
file_opt_spec.supported = auto_options + supported
file_opt_spec.constants = common_constants

# emon options specification
emon_opt_spec = common.copy()
emon_opt_spec.update(emoninput)
emon_opt_spec = option_specification(emon_opt_spec)
emon_opt_spec.optcheck = types.MethodType(emon_optcheck, emon_opt_spec)
emon_opt_spec.supported = auto_options + supported
emon_opt_spec.constants = common_constants

################################################################################
#                                   Tests                                      #
################################################################################

import unittest

def get_arg_list_from_option_dict(d):
  #  Get a list with the command line options that would generate the dictionary 
  # d. First get a list of pairs for each option plus (isolated) extra arguments. 
  # The string representation is used for each option value and extra argument. 
  # Then flatten the list out. For instance, if we had 
  # 'd={'a': 1, 'b': 2, 'extra': 3}' this method will return the list
  # '['--a', '1', '--b', '2', '3']
  l = [(str(v),) if k == 'extra' else (('--%s' % k), str(v)) 
       for k, v in d.iteritems()]
  return [x for sublist in l for x in sublist]

class option_tests_base(unittest.TestCase):
  def setUp(self):
    self.diff = set(auto_options)
    self.option_spec = file_opt_spec
    self.__setup_unsupported()
    self.parser = gen_parser(file_opt_spec, True)
  def check_arg_set(self, kwargs):
    diff = set(kwargs.keys()) - set(self.option_spec.keys())
    self.assert_(diff.issubset(self.diff), "%s is not a subset of %s" 
                                           % (diff, self.diff))
  def check_arg_values(self, kwargs, overwritten={}):
    for k, v in kwargs.iteritems():
      if not k in self.diff:
        expect = (overwritten[k] if k in overwritten 
                                 else self.option_spec.get_default(k))
        self.assertEquals(str(v), str(expect), 
                          "Option '%s' has a wrong value: expected '%s' but "
                          "got '%s'" % (k, expect, v))
  def __setup_unsupported(self):
    self.option_spec['unsupported'] = {'short': '', 'arg': True, 
                                       'default': None, 'group': 'Test', 
                                       'description': 'unsupported option'}
    self.option_spec['unsupported-flag'] = {'short': '', 'arg': False, 
                                            'default': None, 'group': 'Test', 
                                            'description': 'unsupported flag'}

class option_basic_tests(option_tests_base):
  def test_default(self):
    kwargs, extra = self.parser.parse([])
    self.check_arg_set(kwargs)
    self.check_arg_values(kwargs)
  def test_explicit_maintain(self):
    kwargs, extra = self.parser.parse(["-z", "0"])
    self.check_arg_set(kwargs)
    self.check_arg_values(kwargs)
  def test_explicit(self):
    kwargs, extra = self.parser.parse(["-z", "1"])
    self.check_arg_set(kwargs)
    self.check_arg_values(kwargs, {"use-compression": 1})
  def test_explicit_flag(self):
    kwargs, extra = self.parser.parse(["-H"])
    self.check_arg_set(kwargs)
    self.check_arg_values(kwargs, {"perfmon": True})
  def test_no_extra(self):
    parser = gen_parser(self.option_spec, False)
    self.assertRaises(SyntaxError, parser.parse, ["extra_argument"])
  def test_extra(self):
    argv = ["extra_arg1", "extra_arg2"]
    kwargs, extra = self.parser.parse(argv)
    self.check_arg_set(kwargs)
    self.check_arg_values(kwargs)
    self.assertEquals(set(extra), set(argv))
    
class option_consistency_tests(option_tests_base):
  def setUp(self):
    super(option_consistency_tests, self).setUp()
    self.required = {'file': ['fake_file'], 
                     'extra': "fake_joboptions"}
  def test_emon_not_allowed(self):
    self.assertRaises(CurrentlyNotSupported, emon_optcheck, None, None, None)
  def test_unsupported_flag(self):
    kwargs, extra = self.parser.parse(self._get_required_args() + 
                                      ["--unsupported-flag"])
    self.assertRaises(CurrentlyNotSupported, self.option_spec.optcheck, 
                      kwargs, extra)
  def test_unsupported(self):
    kwargs, extra = self.parser.parse(self._get_required_args() + 
                                      ["--unsupported", "123"])
    self.assertRaises(CurrentlyNotSupported, self.option_spec.optcheck, 
                      kwargs, extra)
  def test_explicitely_supported(self):
    self._aux_test_explicitely_supported(["run-number"], 
                                        ["-R", "1234"] + 
                                        self._get_required_args())
  def test_explicitely_supported_flag(self):
    self._aux_test_explicitely_supported(["leak-check-execute"],
                                        ["-Q"] + self._get_required_args())
  def test_explicitely_supported_extra(self):
    self._aux_test_explicitely_supported(["extra_argument"])
  def _check_opt_allowed(self, optn, optv=None):
    # leave the value argument empty for flags
    kwargs, extra = self.parser.parse(self._get_required_args() + 
                                      ["--%s" % optn, 'True' if optv is None 
                                                             else str(optv)])
    self.check_arg_set(kwargs)
    d = dict(self.required.items() + [(optn,True if optv is None else optv)])
    self.check_arg_values(kwargs, d)
    self.option_spec.optcheck(kwargs, extra)
  def _check_opt_disallowed(self, optn, optv=None):
    # we don't check the value since there are no guarantees when the value is
    # not allowed to begin with
    self.assertRaises(BadOptionSet, self._check_opt_allowed, optn, optv)
  def _aux_test_explicitely_supported(self, sup_args=[], cmd_args=None):
    # sup_args are extra arguments, but they are also added to the list of 
    # supported arguments, so that currently unsupported options can still be
    # tested (e.g. test that oh-display is supported with oh-monitoring before
    # oh-display is actually supported) ) 
    kwargs, extra = self.parser.parse(cmd_args if cmd_args 
                                      else sup_args + self._get_required_args())
    self.option_spec.supported += sup_args
    self.option_spec.optcheck(kwargs, extra)
  def _get_required_args(self):
    return get_arg_list_from_option_dict(self.required)

class option_specific_tests(option_consistency_tests):
  def test_oh_display_requires_oh(self):
    self._check_opt_disallowed('oh-display')
  def test_user_ipc_requires_oh(self):
    self._check_opt_disallowed('user-ipc')
  def test_info_service_requires_oh(self):
    self._check_opt_disallowed('info-service', 'bla')
  def test_histogram_publishing_interval_requires_oh(self):
    self._check_opt_disallowed('histogram-publishing-interval', 123)
  def test_histogram_include_requires_oh(self):
    self._check_opt_disallowed('histogram-include', 'abc*')
  def test_histogram_exclude_requires_oh(self):
    self._check_opt_disallowed('histogram-exclude', 'abc*')    
  def test_oh_display_ok_with_oh(self):
    self._aux_test_explicitely_supported(["oh-display"],
                                         ["--oh-display", "--oh-monitoring"] +
                                         self._get_required_args())
  def test_user_ipc_ok_with_oh(self):
    self._aux_test_explicitely_supported(cmd_args=["--user-ipc", 
                                                   "--oh-monitoring"]
                                         + self._get_required_args())
  def test_info_service_ok_with_oh(self):
    self._aux_test_explicitely_supported(cmd_args=["--info-service", "bla",
                                                   "--oh-monitoring"]
                                         + self._get_required_args())
  def test_histogram_publishing_interval_ok_with_oh(self):
    cargs=["--histogram-publishing-interval", "5", "--oh-monitoring"]
    cargs += self._get_required_args()
    self._aux_test_explicitely_supported(cmd_args=cargs)
  def test_histogram_include_ok_with_oh(self):
    cargs = ['--histogram-include', '.*', '--oh-monitoring']
    cargs += self._get_required_args()
    self._aux_test_explicitely_supported(cmd_args=cargs)
  def test_histogram_exclude_ok_with_oh(self):
    cargs = ['--histogram-exclude', ' ', '--oh-monitoring']
    cargs += self._get_required_args()
    self._aux_test_explicitely_supported(cmd_args=cargs)
  def test_sor_time_allowed(self):
    allowed = ['now', 1386355338658000000, '13/3/13  08:30:00.123',
               '4/4/04 4:4:4.444444', -123]
    for sor in allowed:
      self.option_spec['sor-time']['parse'](sor)
  def test_sor_time_disallowed(self):
    disallowed = ['tomorrow', 'yesterday', 'blablebli',
                  '13/03/2013  08:30:00.123', '4/4/04', '4:4:04.444444',
                  '4/4/04 4:4:4', '4/13:04 4:4:4.444444', '4/4/4 4:4:4.444444']
    for sor in disallowed:
      self.assertRaises(BadOptionSet, self.option_spec['sor-time']['parse'],sor)
  def test_db_options_no_usedb_disallowed(self):
    disallowed = [('db-type', 'Coral'), ('db-server', 'mehhh'), 
                  ('db-smkey', 84930), ('db-hltpskey', 40000), 
                  ('db-extra', {'a':'b'}), ('use-frontier',)]
    for args in disallowed:
      self._check_opt_disallowed(*args)

class option_database_tests(option_consistency_tests):
  def setUp(self):
    super(option_database_tests, self).setUp()
    del self.required['extra']
    self.required['use-database'] = True
  def test_db_type_disallowed(self):
    self._check_opt_disallowed('db-type', 'wrong')
  def test_db_types_allowed(self):
    for type in ["Oracle", "SQLite", "MySQL"]:
      self._check_opt_allowed('db-type', type)
  def test_db_smkey_disallowed(self):
    for key in [0, -123, 1.23, 'blaa', [(1,2)]]:
      self._check_opt_disallowed('db-smkey', key)
  def test_db_smkey_allowed(self):
    self._check_opt_allowed('db-smkey', 123)
  def test_db_server_allowed(self):
    self._check_opt_allowed('db-server', 'foobar')
  def test_db_hltpskey_disallowed(self):
    bad = [-1, 0, 5.4, 1.0, "1j", "afsg", "[(1,a)]", "[(1,4j)]", [], [1], 
           [(0, 123), (100, 0)], [(1, 123), (100, 0)], [(3,3),(2,2)], 
           [(3,3),(2,2),(1,1)], [(1,1),(3,3),(2,2)], [(1,1), (1,2)]]
    for x in bad:
      self._check_opt_disallowed('db-hltpskey', x)
  def test_db_hltpskey_allowed(self):
    good = [1, [(1,2)], [(1,123), (100,321)], [(1, 123), (100, 22)],
            [(1, 123), (100, 22), (444, 123)], [(1,1),(2,2),(3,1),(44,555)]]
    for x in good:
      self._check_opt_allowed('db-hltpskey', x)
  def test_use_frontier_disallowed_without_coral(self):
    for t in self.option_spec['db-type']['allowed']:
      if t not in ["Coral", None]:
        kwds, extra = self.parser.parse(self._get_required_args() + 
                                        ["--db-type", t, "--use-frontier"])
        self.assertRaises(BadOptionSet, self.option_spec.optcheck, kwds, extra)

if __name__ == '__main__':
  from HLTTestApps import test_main
  test_main()
  
