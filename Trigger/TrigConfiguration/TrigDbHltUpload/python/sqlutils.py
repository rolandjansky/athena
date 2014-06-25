#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: sqlutils.py,v 1.10 2007-07-13 17:27:14 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 17-Mar-2006

import sys
from math import floor

def get_cursor (host, user, db="", type="mysql", ask_passwd=True, passwd=""):

  """ Gets a python DB cursor for a given triple: db, host, user. The passwd
  flag can be used to trigger a little trigger interactive password request to
  the user. The type can be either 'mysql' or 'oracle' and that will return one
  of the two different cursor implementations.  """

  if type.lower() == "mysql":
    return _get_mysql_cursor(host, db, user, ask_passwd, passwd)
  elif type.lower() == "oracle":
    return _get_oracle_cursor(host, user, ask_passwd, passwd)

def _get_oracle_cursor (tns, user, ask_passwd=True, passwd=""):
  """Returns an Oracle cursor"""
  try:
    from cx_Oracle import connect
  except ImportError:
    print "ERROR: Have you setup your Oracle environment correctly?"
    print "You have to set these enviroment variables to make it work:"
    print "LD_LIBRARY_PATH: include the directory of the client libraries"
    print "TNS_ADMIN: include the TNS definitions %s" % \
          "(e.g.: /afs/cern.ch/project/oracle/admin)"
    sys.exit(2)
  
  if ask_passwd:
    from getpass import getpass
    passwd = getpass("[Oracle] database password for %s@%s: " % (user, tns))
    connection = connect (user, passwd, tns, threaded=True)
  else:
    connection = connect (user, passwd, tns, threaded=True)
  return connection.cursor()

def _get_mysql_cursor (host, db, user, ask_passwd=True, passwd=""):
  """Returns a MySQL cursor"""

  from MySQLdb import connect
  from getpass import getpass
  
  if ask_passwd:
    passwd = getpass("[MySQL] `%s' database password for %s@%s: " %
                     (db, user, host))
  connection = connect (host=host, user=user, passwd=passwd, db=db, connect_timeout=10)
  return connection.cursor()

def _run_single(cursor, query, verbose=False):
  """Runs a single query and ignore the result"""
  if str(type(cursor)).lower().find('oracle') >= 0:
    query = query.replace(" as ", " ")
    query = query.replace(" AS ", " ")
  if verbose:
    print query
  cursor.execute(str(query))
  return

def _commit(cursor, verbose=True):
  """Sends a commit command"""
  if verbose:
    print "COMMITING"
  cursor.execute("commit")
  return


def _run_func(cursor, query, verbose=True):

  """Runs a single query and ignore the result"""
  lquery = """BEGIN
  :result := %s;
  END;""" % query 
  #if verbose:
  print lquery
  from cx_Oracle import NUMBER
  result = cursor.var(NUMBER)
  cursor.execute(lquery, result=result )
  #if verbose:
  print " ==> function return value: ", result.getvalue()
  return result

def _get_single(cursor, query, verbose=True):

  """Executes a single query that has a single value as output and return that
  value, as converted by the MySQLdb interface"""
  
  _run_single(cursor, query, verbose)
  retval = cursor.fetchall()
  cursor.execute("show warnings")
  result = cursor.fetchall()
  if result!=None and len(result)>0:
    serious = 0
    for warning in result:
      if warning[1] != 1329: serious = 1
    if serious:
      print "Warning(s):"
      for warning in result:
        if warning[1] != 1329: print warning
        print "Caused by query: ",query

  #  cursor.execute("select @@warning_count;")
  #  if cursor.fetchall()[0][0]>0:
  #    print "Cause of warning: ",query 
  if retval == None or len(retval) == 0:
    raise AssertionError, "Query `%s' has returned no matches" % query
  elif len(retval) != 1:
    raise AssertionError, "Query `%s' has returned too many matches" % query
  return retval[0][0]

def _get_tuple(cursor, query):

  """Executes a single query that has a tuple as output and return that
  value, as converted by the MySQLdb interface"""

  _run_single(cursor, query)
  retval = cursor.fetchall()
  if retval == None or len(retval) == 0:
    raise AssertionError, "Query `%s' has returned no matches" % query
  elif len(retval) != 1:
    raise AssertionError, "Query `%s' has returned too many matches" % query
  return retval[0]

def _get_tuples(cursor, query, verbose=False):

  """Executes a single query that has many tuples as output and return those
  values, as converted by the MySQLdb interface"""

  _run_single(cursor, query, verbose)
  return cursor.fetchall()

def boot_setup(cursor, verbose=True):
  """Starts a new setup"""
  _run_single(cursor, 'call HLT_BOOT_SETUP()',verbose)

def shutdown_setup(cursor, verbose=True):
  """Shuts down the setup infrastructure"""
  _run_single(cursor, 'call HLT_SHUTDOWN_SETUP()',verbose)

def insert_parameter(cursor, p, verbose=True):

  """ Inserts a new parameter (or return the row identifer of an exact match)
  via the cursor. The parameter should be dictionary with the following obvious
  entries: name, type, op and value, matching the required entries in the
  HLT_PARAMETER table.  """

  query = 'select HLT_INSERT_PARAMETER(\'%s\', \'%s\', \'%s\')' % \
          (p['name'], p['op'], p['value'].replace( "'", "\\'"))
  return _get_single(cursor, query, verbose)

def select_latest_parameter(cursor, p):
  """Returns the row identifier for the latest entry of a particular property"""
  query = 'select max(HLT_PARAMETER.HPA_ID) from HLT_PARAMETER where'
  query += ' HLT_PARAMETER.HPA_NAME = \'%s\'' % p['name']
  query += ' and HLT_PARAMETER.HPA_VALUE = \'%s\'' % p['value']
  query += ' and HLT_PARAMETER.HPA_OP = \'%s\'' % p['op']
  return _get_single(cursor, query)

def count_parameter(cursor):
  """Counts the number of rows in the HLT_PARAMETER table"""
  query = 'select count(HLT_PARAMETER.HPA_ID) from HLT_PARAMETER'
  return _get_single(cursor, query)

def reset_tmp_parameter(cursor):
  """Resets the temporary parameter table"""
  _run_single(cursor, 'call HLT_RESET_PARAMETER()')

def read_parameter(cursor, parameter_id):
  """Reads the properties of a given parameter from the database"""
  query = 'select HLT_PARAMETER.HPA_NAME' + \
          ', HLT_PARAMETER.HPA_VALUE' + \
          ', HLT_PARAMETER.HPA_OP' + \
          ' from HLT_PARAMETER where HLT_PARAMETER.HPA_ID = %ld' % parameter_id
  return _get_tuple(cursor, query)

def insert_component(cursor, c, verbose=True):

  """Inserts a new component into the HLT_COMPONENT table or return an exact
  match, regarding component name/alias and parameters associated. The
  component should be dictionary with the following entries: alias, name and
  par. The par entry is a list respecting the parameter description constraint
  described on the comments of insert_parameter()"""

  for p in c['parameter']: insert_parameter(cursor, p, verbose)
  query = 'select HLT_INSERT_COMPONENT(\'%s\', \'%s\', %i)' % \
          (c['name'], c['alias'], c['topalg'])
  return _get_single(cursor, query, verbose)

def select_latest_component(cursor, c):

  """Formats a select statement for the row identifier of a component, taking
  into consideration the component name, alias and the topalg flag. Please
  notice that this might return misleading results as this function does not
  verify the component connectivity. This happens because one cannot know the
  exact version number by simply looking at the component name, so we always
  return the latest, meaning the maximum row identifier of the component entry
  which matches the required name/alias/topalg condition"""

  query = 'select max(HLT_COMPONENT.HCP_ID) from HLT_COMPONENT where'
  query += ' HLT_COMPONENT.HCP_NAME = \'%s\'' % c['name']
  query += ' and HLT_COMPONENT.HCP_ALIAS = \'%s\'' % c['alias']
  query += ' and HLT_COMPONENT.HCP_TOPALG = \'%s\'' % c['topalg']
  return _get_single(cursor, query)

def count_component(cursor):
  """Counts the number of rows in the HLT_COMPONENT table"""
  query = 'select count(HLT_COMPONENT.HCP_ID) from HLT_COMPONENT'
  return _get_single(cursor, query)

def count_component_parameter(cursor, component_id):
  """Counts how many parameters, a component with a given ID has"""
  query = 'select HLT_COUNT_CP2PA(%ld)' % component_id
  return _get_single(cursor, query)

def reset_tmp_component(cursor):
  """Resets the temporary parameter table"""
  _run_single(cursor, 'call HLT_RESET_COMPONENT()')

def read_component(cursor, component_id):
  """Reads the properties of a given component from the database"""
  query = 'select HLT_COMPONENT.HCP_NAME' + \
          ', HLT_COMPONENT.HCP_ALIAS' + \
          ', HLT_COMPONENT.HCP_TOPALG' + \
          ' from HLT_COMPONENT where HLT_COMPONENT.HCP_ID = %ld' % component_id
  return _get_tuple(cursor, query)

def read_component_parameter(cursor, component_id):
  """Reads all the parameters for a given component"""
  query = 'select T.HPA_NAME, T.HPA_VALUE, T.HPA_OP' + \
          ' from HLT_CP_TO_PA_VIEW T where T.HCP_ID = %ld' % component_id + \
          ' order by T.HPA_ID'
  return _get_tuples(cursor, query)

def insert_force_dll(cursor, c, verbose=True):
  """Formats a dll in SQL for insertion in the DB via the cursor"""
  _run_single(cursor, 'call HLT_NEW_FORCE_DLL(\'%s\')' % c, verbose)

def count_force_dll(cursor, c, verbose=True):
  """Counts the number of DLLs in the forced DLL table"""
  query = 'select count(HLT_FORCE_DLL.HFD_ID) from HLT_FORCE_DLL'
  return _get_single(cursor, query, verbose)

def reset_tmp_force_dll(cursor):
  """Resets the temporary table for DLLs"""
  _run_single(cursor, 'call HLT_RESET_FORCE_DLL()')

def uniq(l):
  """Creates a unique list of elements from the originating list `l',
  preserving the order in which the componenents in l appear"""
  retval = []
  for v in l:
    if v not in retval: retval.append(v)
  return retval

def insert_setup(cursor, setup, verbose=True):
  """
  Inserts a new setup and return its identifier. The setup is supposed to be a
  dictionary containing the following entries:
  name: The name of the setup
  force_dll: The DLLs that have to forced by this setup
  component: The components, as specified by the insert_component() function
  """
  
  for c in setup['component']: insert_component(cursor, c, verbose)
  # have to calculate uniq() since it is an error inserting the same DLL twice!
  for d in uniq(setup['force_dll']): insert_force_dll(cursor, d, verbose)
  query = 'select HLT_INSERT_SETUP(\'%s\')' % setup['name']
  return _get_single(cursor, query, verbose)

def select_latest_setup(cursor, s):
  """Formats a select statement for the row identifier of a setup, taking into
  consideration the setup name and its version."""

  query = 'select HLT_SETUP.HST_ID from HLT_SETUP where'
  query += ' HLT_SETUP.HST_NAME = \'%s\'' % s['name']
  query += ' and HLT_SETUP.HST_VERSION = \'%s\'' % s['version']
  return _get_single(cursor, query)

def count_setup(cursor):
  """Counts the number of rows in the HLT_SETUP table"""
  query = 'select count(HLT_SETUP.HST_ID) from HLT_SETUP'
  return _get_single(cursor, query)

def count_setup_component(cursor, setup_id):
  """Counts how many components, a setup with a given ID has"""
  query = 'select HLT_COUNT_ST2CP(%ld)' % setup_id
  return _get_single(cursor, query)

def count_setup_force_dll(cursor, setup_id):
  """Counts how many forced DLLs, a setup with a given ID has"""
  query = 'select HLT_COUNT_ST2FD(%ld)' % setup_id
  return _get_single(cursor, query)

def read_setup(cursor, setup_id):
  """Reads the properties of a given component from the database"""
  query = 'select HLT_SETUP.HST_NAME, HLT_SETUP.HST_VERSION' + \
          ' from HLT_SETUP where HLT_SETUP.HST_ID = %ld' % setup_id
  return _get_tuple(cursor, query)

def get_setup_id(cursor, name, version=None):
  """Gets the row identifier of the latest version of a given setup or a
  specific version."""
  if version is None:
    query = 'select MAX(HLT_SETUP.HST_ID)' + \
            ' from HLT_SETUP where HLT_SETUP.HST_NAME = \'%s\'' % name
  else:
    query = 'select HLT_SETUP.HST_ID' + \
            ' from HLT_SETUP where HLT_SETUP.HST_NAME = \'%s\'' % name + \
            ' and HLT_SETUP.HST_VERSION = \'%11.5f\'' % version
  return _get_single(cursor, query)

def get_setups(cursor):
  """Returns all setups"""
  query = 'select HLT_SETUP.HST_NAME, HLT_SETUP.HST_VERSION from HLT_SETUP'
  return _get_tuples(cursor, query)

def read_setup_component_id(cursor, setup_id):
  """Reads all the component identifiers for a given setup"""
  query = 'select HLT_ST_TO_CP.HST2CP_COMPONENT_ID' + \
          ' from HLT_ST_TO_CP where HLT_ST_TO_CP.HST2CP_SETUP_ID = %ld' % \
          setup_id + ' order by HLT_ST_TO_CP.HST2CP_COMPONENT_ID'
  return _get_tuples(cursor, query)

def read_setup_force_dll(cursor, setup_id):
  """Reads all the forced DLL names for a given setup"""
  query = 'select HLT_FORCE_DLL.HFD_DLL_NAME' + \
          ' from HLT_FORCE_DLL, HLT_FD_TO_ST' + \
          ' where HLT_FORCE_DLL.HFD_ID = HLT_FD_TO_ST.HFD2ST_FORCE_DLL_ID AND' + \
          ' HLT_FD_TO_ST.HFD2ST_SETUP_ID = %ld' % \
          setup_id + ' order by HLT_FORCE_DLL.HFD_ID'
  return _get_tuples(cursor, query)

def boot_release(cursor, verbose=True):
  """Starts up a new release"""
  _run_single(cursor, 'call HLT_BOOT_RELEASE()', verbose)

def shutdown_release(cursor, verbose=True):
  """Terminates the release insertion cycle"""
  _run_single(cursor, 'call HLT_SHUTDOWN_RELEASE()', verbose)

def insert_property(cursor, p, verbose=True):

  """ Inserts a new property (or return the row identifer of an exact match)
  via the cursor. The parameter should be dictionary with the following obvious
  entries: name, type and value, matching the required entries in the
  HLT_PROPERTY table.  """

  query = 'select HLT_INSERT_PROPERTY(\'%s\', \'%s\', \'%s\')' % \
          (p['name'], p['type'], p['value'])
  return _get_single(cursor, query, verbose)

def select_latest_property(cursor, p):

  """Returns the row identifier for the latest entry of a particular
  property"""
  
  query = 'select max(HLT_PROPERTY.HPY_ID) from HLT_PROPERTY where'
  query += ' HLT_PROPERTY.HPY_NAME = \'%s\'' % p['name']
  query += ' and HLT_PROPERTY.HPY_VALUE = \'%s\'' % p['value']
  query += ' and HLT_PROPERTY.HPY_TYPE = \'%s\'' % p['type']
  return _get_single(cursor, query)

def count_property(cursor):
  """Counts the number of rows in the HLT_PROPERTY table"""
  query = 'select count(HLT_PROPERTY.HPY_ID) from HLT_PROPERTY'
  return _get_single(cursor, query)

def reset_tmp_property(cursor):
  """Resets the temporary property table"""
  _run_single(cursor, 'call HLT_RESET_PROPERTY()')

def read_property(cursor, property_id):
  """Reads the properties of a given property from the database"""
  query = 'select HLT_PROPERTY.HPY_NAME' + \
          ', HLT_PROPERTY.HPY_TYPE' + \
          ', HLT_PROPERTY.HPY_VALUE' + \
          ' from HLT_PROPERTY where HLT_PROPERTY.HPY_ID = %ld' % property_id
  return _get_tuple(cursor, query)

def insert_source(cursor, s, verbose=True):

  """ Inserts a new source into the HLT_SOURCE table or return an exact match,
  regarding source name/gaudi_type and properties associated. The source should
  be dictionary with the following entries: name, gaudi_type and prop. The prop
  entry is a list respecting the property description constraint described on
  the comments of insert_property() """

  for p in s['property']: insert_property(cursor, p, verbose)
  query = 'select HLT_INSERT_SOURCE(\'%s\', \'%s\')' % (s['name'], s['gaudi_type'])
  return _get_single(cursor, query, verbose)

def select_latest_source(cursor, s):

  """Formats a select statement for the row identifier of a source, taking into
  consideration the source name and Gaudi type. Please notice that this might
  return misleading results as this function does not verify the source
  connectivity. This happens because one cannot know the exact row identifier
  by simply looking at the source name, so we always return the latest, meaning
  the maximum row identifier of the source entry which matches the required
  name/gaudi_type condition"""

  query = 'select max(HLT_SOURCE.HSO_ID) from HLT_SOURCE where'
  query += ' HLT_SOURCE.HSO_NAME = \'%s\'' % s['name']
  query += ' and HLT_SOURCE.HSO_GAUDI_TYPE = \'%s\'' % s['gaudi_type']
  return _get_single(cursor, query)

def count_source(cursor):
  """Counts the number of rows in the HLT_SOURCE table"""
  query = 'select count(HLT_SOURCE.HSO_ID) from HLT_SOURCE'
  return _get_single(cursor, query)

def count_source_property(cursor, source_id):
  """Counts how many properties, a source with a given ID has"""
  query = 'select HLT_COUNT_SO2PY(%ld)' % source_id
  return _get_single(cursor, query)

def reset_tmp_source(cursor):
  """Resets the temporary source table"""
  _run_single(cursor, 'call HLT_RESET_SOURCE()')

def read_source(cursor, source_id):
  """Reads the properties of a given source from the database"""
  query = 'select HLT_SOURCE.HSO_NAME' + \
          ', HLT_SOURCE.HSO_GAUDI_TYPE' + \
          ' from HLT_SOURCE where HLT_SOURCE.HSO_ID = %ld' % source_id
  return _get_tuple(cursor, query)

def read_source_property(cursor, source_id):
  """Reads all the parameters for a given component"""
  query = 'select T.HPY_NAME, T.HPY_TYPE, T.HPY_VALUE' + \
          ' from HLT_SO_TO_PY_VIEW T where T.HSO_ID = %ld' % source_id + \
          ' order by T.HPY_ID'
  return _get_tuples(cursor, query)

def insert_dll(cursor, d, verbose=True):

  """ Inserts a new dll into the HLT_DLL table or return an exact match,
  regarding dll name/cmt_version and sources associated. The dll should be
  dictionary with the following entries: name, cmt_version and source. The
  source entry is a list respecting the source description constraint
  described on the comments of insert_source() """
  
  for s in d['source']: insert_source(cursor, s, verbose)
  query = 'select HLT_INSERT_DLL(\'%s\', \'%s\', \'%s\')' % (d['name'], d['package'],
                                                       d['cmt_version'])
  return _get_single(cursor, query, verbose)

def select_latest_dll(cursor, d, verbose):

  """Formats a select statement for the row identifier of a dll, taking into
  consideration the dll name and its CMT version."""

  query = 'select HLT_DLL.HDL_ID from HLT_DLL where'
  query += ' HLT_DLL.HDL_NAME = \'%s\'' % d['name']
  query += ' and HLT_DLL.HDL_PACKAGE = \'%s\'' % d['package']
  query += ' and HLT_DLL.HDL_CMT_VERSION = \'%s\'' % d['cmt_version']
  return _get_single(cursor, query)

def count_dll(cursor):
  """Counts the number of rows in the HLT_DLL table"""
  query = 'select count(HLT_DLL.HDL_ID) from HLT_DLL'
  return _get_single(cursor, query)

def count_dll_source(cursor, dll_id):
  """Counts how many sources, a dll with a given ID has"""
  query = 'select HLT_COUNT_SO2DL(%ld)' % dll_id
  return _get_single(cursor, query)

def reset_tmp_dll(cursor):
  """Resets the temporary dll table"""
  _run_single(cursor, 'call HLT_RESET_DLL()')

def read_dll(cursor, dll_id):
  """Reads the parameters of a given dll from the database"""
  query = 'select HLT_DLL.HDL_NAME, HLT_DLL.HDL_PACKAGE, HLT_DLL.HDL_CMT_VERSION' + \
          ' from HLT_DLL where HLT_DLL.HDL_ID = %ld' % dll_id
  return _get_tuple(cursor, query)

def read_dll_source_id(cursor, dll_id):
  """Reads all the source identifiers for a given dll"""
  query = 'select HLT_SO_TO_DL.HSO2DL_SOURCE_ID' + \
          ' from HLT_SO_TO_DL where HLT_SO_TO_DL.HSO2DL_DLL_ID = %ld' % \
          dll_id + ' order by HLT_SO_TO_DL.HSO2DL_SOURCE_ID'
  return _get_tuples(cursor, query)

def insert_release(cursor, r, verbose=True):

  """ Inserts a new release into the HLT_RELEASE table or return an exact
  match, regarding release name/version and dlls associated. The release should
  be dictionary with the following entries: name, version and dll. The dll
  entry is a list respecting the dll description constraint described on the
  comments of insert_dll() """
  
  for d in r['dll']: insert_dll(cursor, d, verbose)
  # this works with MySQL
  query = 'select HLT_INSERT_RELEASE(\'%s\', \'%s\')' % (r['name'], r['version'])
  return _get_single(cursor, query, verbose)
  # this will work with ORACLE
  # query = 'HLT_INSERT_RELEASE(\'%s\', \'%s\')' % (str(r['name']), str(r['version']))
  # return _run_func(cursor, query, verbose)

def select_latest_release(cursor, r):

  """Formats a select statement for the row identifier of a release, taking into
  consideration the release name and its version."""

  query = 'select HLT_RELEASE.HRE_ID from HLT_RELEASE where'
  query += ' HLT_RELEASE.HRE_NAME = \'%s\'' % r['name']
  query += ' and HLT_RELEASE.HRE_VERSION = \'%s\'' % r['version']
  return _get_single(cursor, query)

def count_release(cursor):
  """Counts the number of rows in the HLT_RELEASE table"""
  query = 'select count(HLT_RELEASE.HRE_ID) from HLT_RELEASE'
  return _get_single(cursor, query)

def count_release_dll(cursor, release_id):
  """Counts how many DLLs, a release with a given ID has"""
  query = 'select HLT_COUNT_RE2DL(%ld)' % release_id
  return _get_single(cursor, query)

def read_release(cursor, release_id):
  """Reads the parameters of a given release from the database"""
  query = 'select HLT_RELEASE.HRE_NAME, HLT_RELEASE.HRE_VERSION' + \
          ' from HLT_RELEASE where HLT_RELEASE.HRE_ID = %ld' % release_id
  return _get_tuple(cursor, query)

def read_release_dll_id(cursor, release_id):
  """Reads all the dll identifiers for a given release"""
  query = 'select HLT_RE_TO_DL.HRE2DL_DLL_ID' + \
          ' from HLT_RE_TO_DL where HLT_RE_TO_DL.HRE2DL_RELEASE_ID = %ld' % \
          release_id + ' order by HLT_RE_TO_DL.HRE2DL_DLL_ID'
  return _get_tuples(cursor, query)

def get_release_id(cursor, name, version=None):
  """Gets the row identifier of the latest version of a given release or a
  specific version."""
  if version is None:
    query = 'select MAX(HLT_RELEASE.HRE_ID)' + \
            ' from HLT_RELEASE where HLT_RELEASE.HRE_NAME = \'%s\'' % name
  else:
    query = 'select HLT_RELEASE.HRE_ID' + \
            ' from HLT_RELEASE where HLT_RELEASE.HRE_NAME = \'%s\'' % name + \
            ' and HLT_RELEASE.HRE_VERSION = \'%s\'' % version
  return _get_single(cursor, query)

def get_releases(cursor):
  """Returns all releases"""
  query = 'select HLT_RELEASE.HRE_NAME, HLT_RELEASE.HRE_VERSION from HLT_RELEASE'
  return _get_tuples(cursor, query)

def get_job_id(cursor, name, version=None):
  """Gets the row identifier of the latest version of a given job or a
  specific version."""
  if version is None:
    query = 'select MAX(HLT_MASTER_TABLE.HMT_ID)' + \
            ' from HLT_MASTER_TABLE where HLT_MASTER_TABLE.HMT_NAME = \'%s\'' % name
  else:
    query = 'select HLT_MASTER_TABLE.HMT_ID' + \
            ' from HLT_MASTER_TABLE where HLT_MASTER_TABLE.HMT_NAME = \'%s\'' % name + \
            ' and HLT_MASTER_TABLE.HMT_VERSION = \'%11.5f\'' % version
  return _get_single(cursor, query)

def read_job(cursor, job_id):
  """Reads the parameters of a given job from the database"""
  query = 'select HLT_MASTER_TABLE.HMT_NAME, HLT_MASTER_TABLE.HMT_VERSION' + \
          ' from HLT_MASTER_TABLE where HLT_MASTER_TABLE.HMT_ID = %ld' % job_id
  return _get_tuple(cursor, query)

def get_job_setups(cursor, job_id):
  """Returns the jobs attached to a given master key."""

  #First get jobs directly connected:
  query = 'select HLT_MASTER_TABLE.HMT_FORCED_SETUP_ID' + \
          ' from HLT_MASTER_TABLE where HLT_MASTER_TABLE.HMT_ID = %ld' % \
          job_id
  forced_setup = _get_single(cursor, query)
  print forced_setup
  if forced_setup is not None: return [forced_setup]

  #If this guy turns out to be NULL, use the path through the menus
  query = 'select HLT_MK_TO_SETUP_VIEW.HST_ID' + \
          ' from HLT_MK_TO_SETUP_VIEW where' + \
          ' HLT_MK_TO_SETUP_VIEW.HMT_ID = %ld' % job_id
  setups = _get_tuples(cursor, query)
  retval = []
  for s in setups:
    if s[0] not in retval: retval.append(s[0])
  return retval

def get_job_menu(cursor, job_id):
  """Returns the menu id of a given job"""

  #First get jobs directly connected:
  query = 'select HLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID' + \
          ' from HLT_MASTER_TABLE where HLT_MASTER_TABLE.HMT_ID = %ld' % \
          job_id
  return _get_single(cursor, query)

def get_job_menu_and_ps(cursor, job_id):
  """Returns the menu id of a given job"""

  #First get jobs directly connected:
  query = 'select HLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID, HLT_MASTER_TABLE.HMT_PRESCALE_SET_ID' + \
          ' from HLT_MASTER_TABLE where HLT_MASTER_TABLE.HMT_ID = %ld' % \
          job_id
  return _get_tuple(cursor, query)

def getMainKeysFromSmk(cursor, SMK):
  """Return the SM_name, SM_version, lvl1 mk, hlt mk, trigger menu id, prescale set id, and
  release id"""
  query = ( 'select SMT.SMT_NAME, SMT.SMT_VERSION, SMT.SMT_L1_MASTER_TABLE_ID, HMT.HMT_ID, HMT.HMT_TRIGGER_MENU_ID, HMT.HMT_PRESCALE_SET_ID, HMT.HMT_RELEASE_ID'
            ' from SUPER_MASTER_TABLE as SMT, HLT_MASTER_TABLE as HMT where SMT.SMT_ID=%i AND HMT.HMT_ID=SMT.SMT_HLT_MASTER_TABLE_ID'
            % (SMK) )
  return _get_tuple(cursor, query)
  


def read_job_release(cursor, job_id):
  """Returns the job release information."""

  #First get jobs directly connected:
  query = 'select HLT_RELEASE.HRE_NAME, HLT_RELEASE.HRE_VERSION' + \
          ' from HLT_RELEASE, HLT_MASTER_TABLE where' + \
          ' HLT_MASTER_TABLE.HMT_ID = %ld' % job_id  + \
          ' and HLT_MASTER_TABLE.HMT_RELEASE_ID = HLT_RELEASE.HRE_ID'
  return _get_tuple(cursor, query)

def insert_offline_job(cursor, job, verbose=True):
  """Inserts an (offline) job in the HLT database. These jobs are characterized
  by only having a single setup and no HLT menus. This is a good example on how
  the stored procedures could be tranferred into the python world."""
  boot_setup(cursor)
  setup_id = insert_setup(cursor, job['setup'][0], verbose)
  shutdown_setup(cursor)
  
  release_id = get_release_id(cursor, job['release_name'],
                              job['release_version'])
  query = 'select MAX(HLT_MASTER_TABLE.HMT_ID)' + \
          ' from HLT_MASTER_TABLE where' + \
          ' HLT_MASTER_TABLE.HMT_NAME = \'%s\' ' % job['name'] + \
          ' and HLT_MASTER_TABLE.HMT_FORCED_SETUP_ID = \'%ld\' ' % setup_id + \
          ' and HLT_MASTER_TABLE.HMT_RELEASE_ID = \'%ld\' ' % release_id
  try:
    existing_job_id = _get_single(cursor, query)
    if existing_job_id is None: raise AssertionError
    return existing_job_id
  except AssertionError:
    #in which case I have to insert the new entry
    query = 'select MAX(HLT_MASTER_TABLE.HMT_VERSION)' + \
            ' from HLT_MASTER_TABLE where' + \
            ' HLT_MASTER_TABLE.HMT_NAME = \'%s\' ' % job['name']
    try:
      latest_id = _get_single(cursor, query)
      if latest_id is None: raise AssertionError
      next_id = floor(float(latest_id)) + 1
    except AssertionError:
      next_id = 1.0

    query = 'insert into HLT_MASTER_TABLE (HMT_NAME, HMT_VERSION,' + \
            ' HMT_FORCED_SETUP_ID, HMT_RELEASE_ID, HMT_PRESCALE_SET_ID,' + \
            ' HMT_TRIGGER_MENU_ID) values' + \
            '(\'%s\', \'%.5f\', \'%ld\', \'%ld\', NULL, NULL)' % \
            (job['name'], next_id, setup_id, release_id)
    _run_single(cursor, query) #should go smooth after all these checks...
    query = 'select MAX(HLT_MASTER_TABLE.HMT_ID)' + \
            ' from HLT_MASTER_TABLE where' + \
            ' HLT_MASTER_TABLE.HMT_NAME = \'%s\' ' % job['name'] + \
            ' and HLT_MASTER_TABLE.HMT_VERSION = \'%11.5f\' ' % next_id + \
            ' and HLT_MASTER_TABLE.HMT_FORCED_SETUP_ID = \'%ld\' ' % setup_id + \
            ' and HLT_MASTER_TABLE.HMT_RELEASE_ID = \'%ld\' ' % release_id
    return _get_single(cursor, query) #return the row identifier

def insert_trigger_job(cursor, job):
  """Inserts a job in the HLT database. These jobs are characterized by having
  multiple setups and one HLT menu."""
  
  release_id = get_release_id(cursor, job['release_name'],
                              job['release_version'])

  #inserts blindly all the setups
  setup_ids = []
  boot_setup(cursor)
  for s in job.setup: setup_ids.append(s.sql_insert(cursor))
  shutdown_setup(cursor)
  menu_id = job.hltmenu.sql_insert(cursor, setup_ids)

  query = 'select MAX(HLT_MASTER_TABLE.HMT_ID)' + \
          ' from HLT_MASTER_TABLE where' + \
          ' HLT_MASTER_TABLE.HMT_NAME = \'%s\' ' % job['name'] + \
          ' and HLT_MASTER_TABLE.HMT_FORCED_SETUP_ID = NULL ' + \
          ' and HLT_MASTER_TABLE.HMT_RELEASE_ID = \'%ld\' ' % release_id + \
          ' and HLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID = \'%ld\' ' % menu_id
  
  try:
    existing_job_id = _get_single(cursor, query)
    if existing_job_id is None: raise AssertionError
    return existing_job_id
  
  except AssertionError:
    #in which case I have to insert the new entry
    query = 'select MAX(HLT_MASTER_TABLE.HMT_VERSION)' + \
            ' from HLT_MASTER_TABLE where' + \
            ' HLT_MASTER_TABLE.HMT_NAME = \'%s\' ' % job['name']
    try:
      latest_id = _get_single(cursor, query)
      if latest_id is None: raise AssertionError
      next_id = floor(float(latest_id)) + 1
    except AssertionError:
      next_id = 1.0

    query = 'insert into HLT_MASTER_TABLE (HMT_NAME, HMT_VERSION,' + \
            ' HMT_FORCED_SETUP_ID, HMT_RELEASE_ID, HMT_PRESCALE_SET_ID,' + \
            ' HMT_TRIGGER_MENU_ID) values' + \
            '(\'%s\', \'%.5f\', NULL, \'%ld\', NULL, \'%ld\')' % \
            (job['name'], next_id, release_id, menu_id)
    _run_single(cursor, query) #should go smooth after all these checks...
    query = 'select MAX(HLT_MASTER_TABLE.HMT_ID)' + \
            ' from HLT_MASTER_TABLE where' + \
            ' HLT_MASTER_TABLE.HMT_NAME = \'%s\' ' % job['name'] + \
            ' and HLT_MASTER_TABLE.HMT_VERSION = \'%11.5f\' ' % next_id + \
            ' and HLT_MASTER_TABLE.HMT_FORCED_SETUP_ID = NULL ' % setup_id + \
            ' and HLT_MASTER_TABLE.HMT_RELEASE_ID = \'%ld\' ' % release_id + \
            ' and HLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID = \'%ld\' ' % menu_id
    return _get_single(cursor, query) #return the row identifier

def get_jobs(cursor):
  """Returns all jobs"""
  query = 'select HLT_MASTER_TABLE.HMT_NAME, HLT_MASTER_TABLE.HMT_VERSION' \
          ' from HLT_MASTER_TABLE'
  return _get_tuples(cursor, query)

