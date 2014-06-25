#!/usr/bin/env python
# $Id: db2py.py,v 1.1.1.1 2006-07-31 15:09:43 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 27-Mar-2006

# Reads an XML description of what has to be inserted into the HLT database,
# connects to the DB and inserts the information making use of the the basic
# primitives

import jobasic
import job
import sqlutils
import getopt, sys
import xml.dom.minidom
import time
from xml.dom.ext import PrettyPrint

def usage():
  """Prints an help message"""
  print "usage: %s [database-options] [setup|release-options] " % \
        sys.argv[0]
  print "  -h|-?|--help           Issues this help message"
  print "  -o|--output            Saves the XML output into a file"
  print " Selection options are:"
  print "  -j|--job <string>      The name I'll search in the DB for"
  print "  -v|--version <string>  A string that specifies the version of the setup"
  print " Database options may be:"
  print "  -p|--password          Asks the user password to the DB"
  print "  -t|--type mysql|oracle Selects the database backend to use"
  print "  -u|--user <string>     The name of the user in the DB"
  print "  -n|--name <string>     The name of the DB inside the server"
  print "  -m|--host <string>     The name of ther host where the DB server is running"
    
if __name__ == '__main__':

  short_opt = "h?pt:u:n:m:v:o:j:"
  long_opt = ['help', 'password', 'type=', 'user=', 'name=', 'host=',
              'version=', 'output=', 'job=']

  if len(sys.argv) == 1:
      usage()
      sys.exit(1)
  
  #these are common bootstrap procedures for option processing
  try:
      opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
  except getopt.GetoptError:
      usage()
      sys.exit(2)

  #The defaults
  db_type = 'mysql'
  db_host = 'localhost'
  db_user = 'triguser'
  db_name = 'trigconfig'
  ask_pass = False
  job_name = ''
  version = None
  output = None

  #Read the options
  for o, a in opts:
      if o in ('-h', '-?', '--help'):
          usage()
          sys.exit(0)
      if o in ('-p', '--password'): ask_pass = True
      if o in ('-t', '--type'):
          if a not in ('mysql', 'oracle'):
              raise SyntaxError, 'Database backend %s is not implemented' % a
          db_type = a
      if o in ('-u', '--user'): db_user = a
      if o in ('-n', '--name'): db_name = a
      if o in ('-m', '--host'): db_host = a
      if o in ('-j', '--job'): job_name = a
      if o in ('-v', '--version'): version = a
      if o in ('-o', '--output'): output = a

  if len(job_name) == 0: raise AssertionError, 'Input a job name!'

  cursor = sqlutils.get_cursor(db_host, db_user, db_name, db_type, ask_pass)
  start = time.time()

  j = None
  
  try:
    if version is not None: version = float(version)
    row_id = sqlutils.get_job_id(cursor, job_name, version)
    if row_id is None: raise AssertionError
    j = job.Job(cursor, row_id)
  except AssertionError:
    print '[%s] Error: The job %s-%s was not found in the database' % \
          (sys.argv[0], job_name, version)
    sys.exit(1)
      
  end = time.time()
  print "[%s] Reading job from DB..." % sys.argv[0],
  print "took %1.2f second(s)" % (end - start)

  #create python document
  start = time.time()
  release_row = sqlutils.get_release_id(cursor, j.release_name,
                                        j.release_version)
  release = jobasic.Release(cursor, release_row)
  doc = j.py_print(release)
  end = time.time()
  print "[%s] Building python document" % sys.argv[0],
  print "took %1.2f second(s)" % (end - start)

  if output is None: print doc
  else:
    outfile = file(output, 'wt')
    print>>outfile, doc
    outfile.close()
    
  print "[%s] Ok, all done. Bye!" % sys.argv[0]
  sys.exit(0)

