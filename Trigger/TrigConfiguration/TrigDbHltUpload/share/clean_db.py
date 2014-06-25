#!/usr/bin/env python
# $Id: clean_db.py,v 1.4 2009-03-13 15:54:35 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 27-Mar-2006
# Maintained by Joerg Stelzer <stelzer@cern.ch>
# Modified by Miroslav Nozicka <nozicka@mail.desy.de>

# Reads an XML description of what has to be inserted into the HLT database,
# connects to the DB and inserts the information making use of the the basic
# primitives

# If you have cx_Oracle PATH problems you might have to use
# export PYTHONPATH=/afs/cern.ch/atlas/project/database/python/cx_Oracle-4.2.1/build/slc4_ia32_gcc34:$PYTHONPATH

import TrigDbHltUpload.sqlutils

import getopt, sys, re
import time



def usage():
  """Prints a help message"""
  print "Usage: %s [database-options]" % \
        sys.argv[0].split('/')[-1]
  print "  -h|-?|--help                Issues this help message"
  print " Database options may be:"
  print "  -p|--password               Asks the user password to the DB"
  print "  -t|--type mysql|oracle      Selects the database backend to use"
  print "  -u|--user <string>          The name of the user in the DB"
  print "  -n|--name <string>          The name of the DB inside the server"
  print "  -m|--host <string>          The name of the host where the DB server is running"
  print "  -c|--dbConn <string>        The connection to the the DB server is running"

def table_names(cursor):
  names = []
  # Test for type of database in use
  oracle_db = False
  mysql_db = False
  if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
  if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True
  
  query=""
  if oracle_db:
    query = "SELECT table_name FROM cat WHERE "
    query += "TABLE_TYPE='TABLE' AND " 
    query += "("
    query += "TABLE_NAME LIKE 'HLT_%' " 
    query += " OR TABLE_NAME LIKE 'L1_%' "
    query += " OR TABLE_NAME='SUPER_MASTER_TABLE' "
    query += " OR TABLE_NAME='TRIGGER_NEXT_RUN' "
    query += " OR TABLE_NAME='TRIGGER_LOG' "
    query += " OR TABLE_NAME='TRIGGER_SCHEMA' "
    query += " OR TABLE_NAME='TRIGGER_ALIAS' "
    query += " OR TABLE_NAME='TT_USERS' "
    query += ")"

  if mysql_db:
    mysql = "SHOW TABLES"
  
  cursor.execute( query )
  result = cursor.fetchall()
  
  if(oracle_db):
    for output in result:
      names.append(output[0])
  
  return names
  
def view_names(cursor):
  names = []
  # Test for type of database in use
  oracle_db = False
  mysql_db = False
  if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
  if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True
  
  query=""
  if oracle_db:
    query = "SELECT table_name FROM cat WHERE "
    query += "TABLE_TYPE='VIEW' AND (TABLE_NAME LIKE 'HLT%' OR TABLE_NAME LIKE 'L1%')"
    query += "ORDER BY TABLE_TYPE DESC"

  cursor.execute( query )
  result = cursor.fetchall()
  
  if(oracle_db):
    for output in result:
      names.append(output[0])
  
  return names

  
if __name__ == '__main__':
    
  short_opt = "h?p:t:u:n:m:c:"
  long_opt = ['help', 'password=', 'type=', 'user=', 'name=', 'host=', 'dbConn=']
  
  if len(sys.argv) == 1:
      print "Missing arguments"
      usage()
      sys.exit(1)
      
  #these are common bootstrap procedures for option processing
  try:
      opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
  except getopt.GetoptError, exc:
      print '\nError: '+exc.msg+'\n'
      usage()
      sys.exit(1)

  # The defaults
  db_type = ''
  db_host = ''
  db_user = ''
  db_name = ''
  db_userpw = ''
  NewStyleXML = False
  release_name = None
  ask_pass = False
  force_full_release = False
  SMK_base = 0;
  MenuOnly = False
  db_conn = ''

  # this map constains all xml documents
  doc_map = {}

  #Read the options
  for o, a in opts:
    if o in ['-h', '-?', '--help']:
      usage()
      sys.exit(0)
    if o in ['-p', '--password']:
      ask_pass = False
      db_userpw = a
    if o in ['-t', '--type']:
      if a not in ('mysql', 'oracle'):
        raise SyntaxError, 'Database backend %s is not implemented' % a
      db_type = a
    if o in ['-u', '--user']:    db_user = a
    if o in ['-n', '--name']:    db_name = a
    if o in ['-m', '--host']:    db_host = a
    if o in ['-c', '--dbConn']:  db_conn = a

  if db_conn:
    print "interpret connection",db_conn
    m = re.match('(.*)/(.*)@(.*)',db_conn)
    if m:
      (db_user, db_userpw, db_host) = m.groups()
      db_type = 'oracle'
      if db_user[-2:]=='_W' or db_user[-2:]=='_R':
        db_name = db_user[:-2]
      else:
        db_name = db_user
      print "db_type  :", db_type
      print "db_host  :", db_host
      print "db_user  :", db_user
      print "db_name  :", db_name
      print "db_userpw:", db_userpw
    else:
      m = re.match('(.*)://(.*);schema=(.*);user=(.*);passwd=(.*);',db_conn)
      if m:
        (db_type, db_host, db_name, db_user, db_userpw) = m.groups()
        print "db_type  :", db_type
        print "db_host  :", db_host
        print "db_user  :", db_user
        print "db_name  :", db_name
        print "db_userpw:", db_userpw
      else:
        print "Can't interpret", db_conn
        sys.exit(0)

  
  fVerbose = False

  # connect to the database
  start = time.time()
  cursor = TrigDbHltUpload.sqlutils.get_cursor(db_host, db_user, db_name, db_type, ask_pass, db_userpw)
  # Test for type of database in use
  oracle_db = False
  mysql_db = False
  if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
  if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True
  
  if oracle_db:
    from cx_Oracle import *
  
  if mysql_db:
    from MySQLdb import *
  
  query=""
  
  table_list = table_names(cursor)
  
  view_list = []
  if oracle_db :
    view_list = view_names(cursor)

  if len(table_list) >0 :
    print "Existing tables:\t", table_list
  else:
    print "No existing table found"
  
  if len(view_list) >0 :
    print "Existing views:\t", view_list
    
  counter = 0
  while len(table_list)>0 or len(view_list)>0:
    counter = counter+1
    print "Attempt: ", counter
    # DROP Views
    for view in view_list:
      query = "DROP VIEW "
      query += view
      cursor.execute( query )
      cursor.execute( "COMMIT" )
      
    # DROP Tables
    for table in table_list:
      query = "DROP TABLE "
      query += table
      try:
        cursor.execute( query )
      except DatabaseError:
        print "Table: ", table, "\t not removed in this step"
      #cursor.execute( "COMMIT" )
    
    table_list = table_names(cursor)
    view_list = view_names(cursor)
  cursor.close()
  end = time.time()
  
  print "Database Cleaned in ", counter , " steps"
  
  sys.exit()

  
  
