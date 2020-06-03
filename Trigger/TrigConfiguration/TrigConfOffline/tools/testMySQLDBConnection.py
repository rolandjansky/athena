#!/usr/bin/env python
###########################################################################
#                                          
#    <nozicka@mail.desy.de>                                                             
#
###########################################################################

import getopt, sys

def usage():
  """Prints a help message"""
  print "Usage: %s [database-options]" % \
        sys.argv[0].split('/')[-1]
  print "  -h|-?|--help                Issues this help message"
  print " Database options may be:"
  print "  -p|--password               The user password to the DB"
  print "  -u|--user <string>          The name of the user in the DB"
  print "  -n|--name <string>          The name of the DB inside the server"
  print "  -m|--host <string>          The name of ther host where the DB server is running"


if __name__ == '__main__':
  db_user='atlas_trig_nozicka'
  db_passwd='*******'
  db_host='devdb10'
  db_name = ''

  short_opt = "h?p:u:n:m:"
  long_opt = ['help', 'password=', 'user=', 'name=', 'host=']
  
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
    
  #Read the options
  for o, a in opts:
    if o in ['-h', '-?', '--help']:
      usage()
      sys.exit(0)
    if o in ['-p', '--password']: db_passwd = a
    if o in ['-u', '--user']:     db_user = a
    if o in ['-n', '--name']:     db_name = a
    if o in ['-m', '--host']:     db_host = a
  
  from MySQLdb import connect
  connection = connect (host=db_host, user=db_user, passwd=db_passwd, db=db_name, connect_timeout=10)
  
  cursor = connection.cursor()
  print cursor
  
  prepend =''
  if db_name != db_user and db_name != '':
    prepend = '%s.' % (db_name)
  
  table_names = []
  query =  " SHOW tables "
  cursor.execute(query)
  result = cursor.fetchall()
  for column in result :
    table_names.append(column[0].upper())
  
  print len(table_names),'tables available'
  trigger_keys= []
  # Get super Master keys
  
  if 'SUPER_MASTER_TABLE' not in table_names :
    print 'SUPER_MASTER_TABLE not found in database'
    cursor.close()
    sys.exit(1)
  else :
    query =  " SELECT %sSUPER_MASTER_TABLE.SMT_ID "  % (prepend)
    query += " FROM %sSUPER_MASTER_TABLE " % (prepend)
    cursor.execute(query)
    result = cursor.fetchall()
    for column in result :
      trigger_keys.append([column[0],0,0])
  
  if 'HLT_PRESCALE_SET' not in table_names :
    print 'HLT_PRESCALE_SET not found in database'
  else :
    for i in range(len(trigger_keys)) :
      query  = " SELECT %sHLT_PRESCALE_SET.HPS_ID " % (prepend)
      query += " FROM %sSUPER_MASTER_TABLE " % (prepend)
      query += " JOIN %sHLT_MASTER_TABLE ON (%sSUPER_MASTER_TABLE.SMT_HLT_MASTER_TABLE_ID = %sHLT_MASTER_TABLE.HMT_ID) " % (prepend, prepend, prepend)
      query += " JOIN %sHLT_TM_TO_PS ON (%sHLT_MASTER_TABLE.HMT_TRIGGER_MENU_ID = %sHLT_TM_TO_PS.HTM2PS_TRIGGER_MENU_ID) " % (prepend, prepend, prepend)
      query += " JOIN %sHLT_PRESCALE_SET ON (%sHLT_PRESCALE_SET.HPS_ID = %sHLT_TM_TO_PS.HTM2PS_PRESCALE_SET_ID) " % (prepend, prepend, prepend)
      query += " WHERE %sSUPER_MASTER_TABLE.SMT_ID=:smt_id " % (prepend)
      cursor.execute(query, smt_id=trigger_keys[i][0])
      result = cursor.fetchall()
      for column in result :
        trigger_keys[i][1]= column[0]
        
  if 'L1_PRESCALE_SET' not in table_names :
    print 'L1_PRESCALE_SET not found in database'
  else :
    for i in range(len(trigger_keys)) :
      query  = " SELECT %sL1_PRESCALE_SET.L1PS_ID " % (prepend)
      query += " FROM %sSUPER_MASTER_TABLE " % (prepend)
      query += " JOIN %sL1_MASTER_TABLE ON (%sSUPER_MASTER_TABLE.SMT_L1_MASTER_TABLE_ID = %sL1_MASTER_TABLE.L1MT_ID) " % (prepend, prepend, prepend)
      query += " JOIN %sL1_TM_TO_PS ON (%sL1_MASTER_TABLE.L1MT_TRIGGER_MENU_ID = %sL1_TM_TO_PS.L1TM2PS_TRIGGER_MENU_ID) " % (prepend, prepend, prepend)
      query += " JOIN %sL1_PRESCALE_SET ON (%sL1_PRESCALE_SET.L1PS_ID = %sL1_TM_TO_PS.L1TM2PS_PRESCALE_SET_ID) " % (prepend, prepend, prepend)
      query += " WHERE %sSUPER_MASTER_TABLE.SMT_ID=:smt_id " % (prepend)
      cursor.execute(query, smt_id=trigger_keys[i][0])
      result = cursor.fetchall()
      for column in result :
        trigger_keys[i][2]= column[0]
  
  if len(trigger_keys) > 0 :
    print 'Available trigger keys:'
    print 'SMKey','\t','HLTPSK','\t','LVL1PSK'
    for keys in trigger_keys :
      print keys[0],'\t',keys[1],'\t',keys[2]
  else :
    print 'No Entries in SUPER_MASTER_TABLE'
  
  cursor.close()
  
  
  
