#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: __init__.py 102 2013-07-12 17:17:22Z ricab $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 30-Mar-2007 

from libpyhlttestapps import *
import random, sys, os

# avoids the duplication of ERS exception types...
sys.setdlopenflags(0x100|0x2)

def random_sub_dict(basedict):
  return random_sub_dict_num(basedict, random.randint(1,len(basedict)))

def random_sub_dict_num(basedict, numitems):
  #  Get a sub-dictionary of basedict with a length corresponding to the minimum
  # of numitems and the length of basedict (negative length converted to 0).
  
  basekeys = basedict.keys()
  n = min(len(basekeys), numitems) if numitems > 0 else 0
  
  # create the sub-dictionary
  ret = {}
  for i in range(n):
    k = basekeys.pop(random.randint(0, len(basekeys)-1))
    ret[k] = basedict[k]
    
  # make sure this is properly implemented
  assert len(ret) <= len(basedict)
  assert len(ret) <= numitems
  assert len(ret) == numitems or numitems > len(basedict)
  for k, v in ret.items():
    assert k in basedict and v == basedict[k]
    
  return ret

def hook_debugger():
  """
  Hooks debugger to this process.
  
  Copied from athena.py
  """
  pid = os.spawnvp(os.P_NOWAIT, 'gdb', 
                   [ 'gdb', '-q', 'python', str( os.getpid() ) ] )
  # give debugger some time to attach to the python process
  import time
  time.sleep(1)

def get_test_files():
  files = {}
  files['base_dir'] = d = '/afs/cern.ch/work/r/ricab/datafiles/' # temp solution
  
  f1 = d + '2013-05-22VALAllPT_mcV2-1._0001.data' # 100 events, run 177531
  f2 = d + '2012-05-04VALAllPT_physicsV4-1._0001.data' # 99 events, run 200863
  f3 = d + ('data14_cos.00233343.physics_L1Muon.merge.'
            'RAW._lb0002._SFO-ALL.M4._0001.1.') # 34716 events
  f4 = d + ('data14_cos.00248112.physics_CosmicMuons.merge.'
            'RAW._lb0003._SFO-11._150ev.1') # 150 events
  files['datafiles'] = [f1, f2]
  files['default_filelist'] = [f1, f1, f1, f2] # total of 399 events
  files['extra_files_with_valid_core_filename'] = [f4, f3] # start with smaller
  
  files['verbose_config_tree'] = d + "hltconf.xml"
  files['quiet_config_tree'] = d + "hltconf_quiet.xml"
  
  return files

def remove_duplicate_tests(suite):
  uniqtests, uniqnames = [], []
  for test in suite:
    if test._testMethodName not in uniqnames:
      uniqnames.append(test._testMethodName)
      uniqtests.append(test)
  return uniqtests

def remove_exclude_tests(suite, exclude_names):
  ret = []
  for test in suite:
    tnames = [test.__class__.__name__, test._testMethodName]
    for name in exclude_names:
      # if neither the whole name nor class or method names match
      if (name != '.'.join(tnames) and name not in tnames):
        ret.append(test)
  return ret

def test_setup(mod):
  globs = {} 
  files = get_test_files()
  globs['filelist'] = files['default_filelist']
  globs['datafiles'] = files['datafiles']
  globs['extra_datafiles'] = files['extra_files_with_valid_core_filename']
  globs['configxml'] = (files['verbose_config_tree'] if '-d' in sys.argv 
                        else files['quiet_config_tree'])
  # have stuff declared here available to the tests 
  mod.__dict__.update(globs)
  
def test_main(include_names=[], 
              exclude_names=[],
              remove_duplicates=True,
              more_modules_requiring_setup=[]):
  import unittest
  
  mod = sys.modules["__main__"]
  test_setup(mod)
  for m in more_modules_requiring_setup:
    test_setup(sys.modules[m])
    
  if include_names:
    suite = unittest.TestLoader().loadTestsFromNames(include_names, mod)
  else:
    suite = unittest.TestLoader().loadTestsFromModule(mod)
  
  # flatten suite, then remove unintended tests
  suite = unittest.TestSuite([test for subsuite in suite for test in subsuite])
  if exclude_names:
    suite = unittest.TestSuite(remove_exclude_tests(suite, exclude_names))
  if remove_duplicates:
    suite = unittest.TestSuite(remove_duplicate_tests(suite)) 
  
  result = unittest.TextTestRunner(verbosity=2).run(suite)
  # exit with 0(success)/1(failure)
  # need an explicit int for now: see http://bugs.python.org/issue13854
  sys.exit(int(not result.wasSuccessful()))
  
def script_prepare():
  from AthenaCommon.Logging import log
  log.name = os.path.splitext(os.path.basename(sys.argv[0]))[0]
  sys.path.insert(0, '.')
  sys.ps1 = log.name + '> '
  
def script_main(go):
  script_prepare()
  
  result = 0
  try:
    go(sys.argv[1:]) # we don't need the program name
    
  except SystemExit, e:
    if len(e.args) == 0 or e.args[0] == None:
      result = 0
    elif isinstance(e.args[0], int):
      result = e.args[0]
    else:
      result = 1
        
  except:
    import traceback
    traceback.print_exc()
    result = 1

  sys.exit(result)


def call_external(module, func, args):
  """Loads and executes an external function with the given arguments.

  This method will load function 'func', from module 'module' and will call it
  with 'args' as its sequential arguments, returning the result.

  Keyword arguments:

  module -- This is either a simple or compound module name. For example:
  "mymodule1" or "mymodule2.mysubmodule".

  func -- This is the name of the function inside the module named before, that
  will be called.

  args -- This is a sequential list of arguments that will be (dereferenced
  and) passed to the function 'func'. This must be a list or a tuple. If the
  type of this argument is a dictionary, it is doubly-dereferenced to achieve a
  named-argument style call.

  Returns and raises whatever 'func' does.
  """
  import imp
  mod = module.split('.')
  pymod = None
  pathname = None
  for m in mod: 
    try:
      if pathname: pathname = [pathname]
      (f, pathname, description) = imp.find_module(m, pathname)
      pymod = imp.load_module(m, f, pathname, description)
      if f: f.close()
    except ImportError, e:
      name = '.'.join(mod[0:(mod.index(m)+1)])
      raise ImportError, 'Event modifier module "%s"' % name + \
          ' is not visible from your PYTHONPATH (please check): %s' % str(e)

  # at this point, 'pymod' points to the module you are looking for
  pyfunc = getattr(pymod, func) # handle to the event manipulator
  if type(args) in [list, tuple]: return pyfunc(*args)
  elif type(args) is dict: return pyfunc(**args)
  else:
    raise SyntaxError, 'Parameter "args" should be a list, a tuple or a dict'
