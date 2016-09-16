#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Ter 27 Nov 2007 10:41:16 CET 

import logging, types
from ast import literal_eval
from EventApps.myopt import Parser
from optspec import * 
from HLTTestApps import random_sub_dict

def file_optcheck(option_spec, kwargs, extra):
  """Checks if the options passed make sense all together."""
  
  nfiles = len(kwargs['file'])
  if nfiles == 0:
    raise BadOptionSet, 'Cannot process without any input files.'
  elif nfiles > 1 and not kwargs['oh-monitoring']:
    raise BadOptionSet, ('Cannot have multiple input files without '
                         '--oh-monitoring (see --help for explanation)')
  
  # do checks that are common to both emon and file based runs
  common_optcheck(option_spec, kwargs, extra)
    

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
  oh_optcheck(option_spec, kwargs)
  skip_events_optcheck(option_spec, kwargs)
  save_output_optcheck(option_spec, kwargs)
  diverse_optcheck(option_spec, kwargs)
  
def diverse_optcheck(option_spec, kwargs):
  if kwargs['timeout']['timeout'] < 0:
    raise BadOptionSet, 'You cannot set the timeout value to a negative integer'

  if (kwargs['timeout']['warn_fraction'] < 0 or 
      kwargs['timeout']['warn_fraction'] > 1):
    raise BadOptionSet, ('The warn_fraction should be a number in the interval '
                         '[0,1]')

  if kwargs['interactive'] and kwargs['debug']:
    raise BadOptionSet, 'You cannot run GDB over an interactive session'

  if kwargs['leak-check']:
    allowed = option_spec['leak-check']['allowed']
    if kwargs['leak-check'].lower() not in allowed:
      raise BadOptionSet, 'Allowed values for leak-check are %s' % str(allowed)

  if kwargs['delete-check']:
    allowed = option_spec['delete-check']['allowed']
    if not kwargs['delete-check'].lower() in allowed:
      raise BadOptionSet, 'Allowed values for delete are %s' % str(allowed)
  
  if kwargs['debug']:
    allowed = option_spec['debug']['allowed']
    if not kwargs['debug'] in allowed:
      raise BadOptionSet, 'Allowed values for debug are %s' % str(allowed)

  if kwargs['use-compression']:
    # check compression level meaningful
    if kwargs['use-compression'] not in range(1,6):
      raise BadOptionSet, ('Invalid range for output compression level. Needs '
                           'to be in range [1-5]')
    # compression requires saving output
    elif not (kwargs['save-output'] or kwargs['save-output-conventional']):
      raise BadOptionSet, ('Cannot --use-compression without '
                           '--save-output(-conventional)')
      
def save_output_optcheck(option_spec, kwargs):
  # check not both saves
  if kwargs['save-output'] and kwargs['save-output-conventional']:
    raise BadOptionSet, ('Cannot simultaneously --save-output and '
                         '--save-output-conventional')
  
  # convenience
  convd = kwargs['save-output-conventional']
  allowedd = option_spec['save-output-conventional']['allowed']
  convkset = set(convd.keys())
  allowedkset = set(allowedd.keys())
  
  # check save-output-conventional keys are a subset of allowed keys
  if not convkset.issubset(allowedkset):
    raise BadOptionSet, ('Unsupported keys in --save-output-conventional: %s'
                         % list(convkset - allowedkset))
  
  # check save-output-conventional values have proper type
  for k in convkset:
    required_type = type(allowedd[k])
    if not isinstance(convd[k], required_type):
      raise BadOptionSet, ('Value for key "%s" in --save-output-conventional '
                           'must be an instance of %s' % (k, required_type))
        
def skip_events_optcheck(option_spec, kwargs):
  skip = kwargs['skip-events']
  if skip:
    if(skip < 0):
      raise BadOptionSet, 'You cannot skip a negative number of events'
    if kwargs['interactive']:
      raise BadOptionSet, 'Skipping events in interactive mode is not supported'
    if kwargs['rewind']:
      raise BadOptionSet, 'Skipping events in rewind mode is not supported'
        
def oh_optcheck(option_spec, kwargs):
  if not kwargs['oh-monitoring']:
    ohopts = [o for o in option_spec if option_spec[o]['group'] == 
                                        'Online Histogramming']
    ohopts.append('interactive')
    for o in ohopts:
      if o in kwargs and kwargs[o] and kwargs[o] != option_spec[o]['default']:
        justify = (' (see help for explanation).' 
                   if o == 'interactive' else '.')
        raise BadOptionSet, ("Option --%s cannot be specified without " + 
                             "--oh-monitoring%s") % (o, justify)

def db_optcheck(option_spec, kwargs, extra):
  if kwargs['use-database']:
    if extra:
      logging.error("use-database specified simultaneously to job options. "
                    "The latter will be ignored.")
      raise BadOptionSet, "Cannot configure from both DB and JobOptions"
    check_db_type(option_spec, kwargs)
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

import unittest, random, sys

def get_arg_list_from_option_dict(d):
  #  Get a list with the command line options that would generate the dictionary 
  # d. First get a list of pairs for each option plus (isolated) extra arguments. 
  # The string representation is used for each option value and extra argument. 
  # Then flatten the list out. For instance, if we had 
  # 'd={'a': 1, 'b': 2, 'extra': 3}' this method will return the list
  # '['--a', '1', '--b', '2', '3']
  l = [(str(v),) if k == 'extra' else ['--%s' % k] + ([] if v is True 
                                                         else [str(v)]) 
       for k, v in d.iteritems()]
  return [x for sublist in l for x in sublist]

class option_tests_base(unittest.TestCase):
  def setUp(self):
    self.diff = set(auto_options)
    self.option_spec = file_opt_spec
    self.__setup_unsupported()
    self.parser = gen_parser(file_opt_spec, True)
  def _check_arg_set(self, kwargs):
    diff = set(kwargs.keys()) - set(self.option_spec.keys())
    self.assert_(diff.issubset(self.diff), "%s is not a subset of %s" 
                                           % (diff, self.diff))
  def _check_arg_values(self, kwargs, overwritten={}):
    for k, v in kwargs.iteritems():
      if not k in self.diff:
        expect = (overwritten[k] if k in overwritten 
                                 else self.option_spec.get_default(k))
        self.assert_(v == expect or str(v) == str(expect),
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
    self._check_arg_set(kwargs)
    self._check_arg_values(kwargs)
  def test_explicit_maintain(self):
    kwargs, extra = self.parser.parse(["-z", "0"])
    self._check_arg_set(kwargs)
    self._check_arg_values(kwargs)
  def test_explicit(self):
    kwargs, extra = self.parser.parse(["-z", "1"])
    self._check_arg_set(kwargs)
    self._check_arg_values(kwargs, {"use-compression": 1})
  def test_explicit_flag(self):
    kwargs, extra = self.parser.parse(["-H"])
    self._check_arg_set(kwargs)
    self._check_arg_values(kwargs, {"perfmon": True})
  def test_no_extra(self):
    parser = gen_parser(self.option_spec, False)
    self.assertRaises(SyntaxError, parser.parse, ["extra_argument"])
  def test_extra(self):
    argv = ["extra_arg1", "extra_arg2"]
    kwargs, extra = self.parser.parse(argv)
    self._check_arg_set(kwargs)
    self._check_arg_values(kwargs)
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
  def test_explicitly_supported(self):
    self._aux_test_explicitly_supported(["run-number"], 
                                        ["-R", "1234"] + 
                                        self._get_required_args())
  def test_explicitly_supported_flag(self):
    self._aux_test_explicitly_supported(["leak-check-execute"],
                                        ["-Q"] + self._get_required_args())
  def test_explicitly_supported_extra(self):
    self._aux_test_explicitly_supported(["extra_argument"])
  def _check_opt_allowed(self, optn, optv=None):
    # leave the value argument empty for flags
    kwargs, extra = self.parser.parse(self._get_required_args() + 
                                      ["--%s" % optn] + ([] if optv is None
                                                            else [str(optv)]))
    self._check_arg_set(kwargs)
    d = dict(self.required.items() + [(optn,True if optv is None else optv)])
    self._check_arg_values(kwargs, d)
    self.option_spec.optcheck(kwargs, extra)
  def _check_opt_disallowed(self, optn, optv=None):
    # we don't check the value when the option is not allowed to begin with
    with self.assertRaises(BadOptionSet):
      self._check_opt_allowed(optn, optv)
      print >> sys.stderr, ("We did not raise exception with optn='%s' and "
                            "optv='%s'" % (optn, optv))  
  def _aux_test_explicitly_supported(self, sup_args=[], cmd_args=None):
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

class option_diverse_specific_tests(option_consistency_tests):
  def test_leak_check_allowed(self):
    opt = 'leak-check'
    for value in self.option_spec[opt]['allowed']:
      self._check_opt_allowed(opt, value)
  def test_leak_check_disallowed(self):
    disallowed = ('afga', 'star', 0, 123, -.321, self)
    for value in disallowed:
      self._check_opt_disallowed('leak-check', value)
  def test_delete_check_allowed(self):
    opt = 'delete-check'
    for value in self.option_spec[opt]['allowed']:
      self._check_opt_allowed(opt, value)
  def test_delete_check_disallowed(self):
    disallowed = ('afga', 'star', 0, 123, -.321, self)
    for value in disallowed:
      self._check_opt_disallowed('delete-check', value)
  def test_debug_allowed(self):
    opt = 'debug'
    for value in self.option_spec[opt]['allowed']:
      self._check_opt_allowed(opt, value)
  def test_debug_disallowed(self):
    disallowed = ('afga', 'star', 0, 123, -.321, self)
    for value in disallowed:
      self._check_opt_disallowed('debug', value)   
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
  def test_interactive_requires_oh(self):
    self._check_opt_disallowed('interactive')
  def test_multiple_files_require_oh(self):
    multiple_files = '[\'fake_file\', \'another_fake_file\']'
    del self.required['file']
    self._check_opt_disallowed('file', multiple_files)
  def test_oh_display_ok_with_oh(self):
    self._aux_test_explicitly_supported(sup_args=["--oh-display", 
                                                  "--oh-monitoring"])
  def test_user_ipc_ok_with_oh(self):
    self._aux_test_explicitly_supported(sup_args=["--user-ipc", 
                                                  "--oh-monitoring"])
  def test_info_service_ok_with_oh(self):
    self._aux_test_explicitly_supported(sup_args=["--info-service", "bla",
                                                  "--oh-monitoring"])
  def test_histogram_publishing_interval_ok_with_oh(self):
    sargs=["--histogram-publishing-interval", "5", "--oh-monitoring"]
    self._aux_test_explicitly_supported(sup_args=sargs)
  def test_histogram_include_ok_with_oh(self):
    sargs = ['--histogram-include', '.*', '--oh-monitoring']
    self._aux_test_explicitly_supported(sup_args=sargs)
  def test_histogram_exclude_ok_with_oh(self):
    sargs = ['--histogram-exclude', ' ', '--oh-monitoring']
    self._aux_test_explicitly_supported(sup_args=sargs)
  def test_intaractive_ok_with_oh(self):
    sargs = ['--interactive', '--oh-monitoring']
    self._aux_test_explicitly_supported(sup_args=sargs)
  def test_multiple_files_ok_with_oh(self):
    del self.required['file']
    sargs = ['--oh-monitoring', '--file', '["fake_file", "another_fake_file"]']
    self._aux_test_explicitly_supported(sup_args=sargs)
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
                  ('db-extra', {'a':'b'})]
    for args in disallowed:
      self._check_opt_disallowed(*args)
  def test_db_and_job_options_disallowed(self):
    assert "extra" in self.required
    self._check_opt_disallowed('use-database')

class option_save_output_tests(option_consistency_tests):
  def setUp(self):
    super(option_save_output_tests, self).setUp()
  def test_save_output_allowed(self):
    self._check_opt_allowed('save-output', '/tmp/somedir/somefile')
  def test_save_output_conventional_allowed(self):
    # allowed is a dictionary of the allowed keys mapped to their default values
    optvbase = self.option_spec['save-output-conventional']['allowed']
    # create a random combination out of optvbase
    d = random_sub_dict(optvbase)
    # check that this combination is acceptable
    self._check_opt_allowed('save-output-conventional', d)
  def test_save_output_conventional_disallowed_keys(self):
    wrong_dict = {123: 321, 'abc': 'cba'}
    for k, v in wrong_dict.items():
      self._check_opt_disallowed('save-output-conventional', {k: v})
  def test_save_output_conventional_disallowed_values(self):
    # allowed keys
    goodkeys = self.option_spec['save-output-conventional']['allowed'].keys()
    # values with wrong type (None):
    wrong_dict_base = {k: None for k in goodkeys}
    # get a random combination of wrong items
    wrongd = random_sub_dict(wrong_dict_base)
    # check that this combination is not acceptable
    self._check_opt_disallowed('save-output-conventional', wrongd)
  def test_save_output_and_conventional_disallowed(self):
    # allowed is a dictionary of the allowed keys mapped to their default values
    optv = self.option_spec['save-output-conventional']['allowed']
    # this will also be parsed
    self.required['save-output'] = '/tmp/somedir/somefile'
    self._check_opt_disallowed('save-output-conventional', optv)
  def test_compression_requires_save_output_somehow(self):
    compv = random.randint(1,5)
    # conv is a dictionary of the allowed keys mapped to their default values
    conv = self.option_spec['save-output-conventional']['allowed']
    # compression allowed with save-output:
    self.required['save-output'] = '/tmp/somedir/somefile'
    self._check_opt_allowed('use-compression', compv)
    # compression allowed with save-output-conventional
    del self.required['save-output']
    self.required['save-output-conventional'] = conv
    self._check_opt_allowed('use-compression', compv)
    # compression disallowed without save-output or save-output-conventional
    del self.required['save-output-conventional']
    self._check_opt_disallowed('use-compression', compv)

class option_skip_events_tests(option_consistency_tests):
  def setUp(self):
    super(option_skip_events_tests, self).setUp()
    self.required['skip-events'] = 123
  def test_skip_events_interactive_disallowed(self):
    self._check_opt_disallowed('interactive')
  def test_skip_events_rewind_disallowed(self):
    self._check_opt_disallowed('rewind')
  def test_skip_events_negative_disallowed(self):
    del self.required['skip-events']
    self._check_opt_disallowed('skip-events', -1)
  def test_skip_events_explicitly_supported(self):
    self._aux_test_explicitly_supported(cmd_args=self._get_required_args())

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

if __name__ == '__main__':
  from HLTTestApps import test_main
  test_main()
  
