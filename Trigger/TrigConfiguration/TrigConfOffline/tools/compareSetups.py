#!/usr/bin/env python
###########################################################################
#                                          
#    <nozicka@mail.desy.de>                                                             
#
###########################################################################

import getopt, sys

from TrigConfOffline.HLTAlgorithm import HLTAlgorithm
from TrigConfOffline.HLTAlgorithm import diffHLTAlgorithms
from TrigConfOffline.HLTSetupLoader import SetupLoader

def usage():
  """Prints a help message"""
  print "Usage: %s [database-options]" % \
        sys.argv[0].split('/')[-1]
  print "  -h|-?|--help                Issues this help message"
  print " Database options may be:"
  print "  -t|   --type                   The database technology (oracle/mysql)"
  print "  -p|   --password               The user password to the DB"
  print "  -u|   --user <string>          The name of the user in the DB"
  print "  -n|   --name <string>          The name of the DB inside the server"
  print "  -m|   --host <string>          The name of ther host where the DB server is running"
  print "  -s|   --smkey1 <int>           The super master key 1"
  print "  -S|   --smkey2 <int>           The super master key 2"
  print "  -l|   --level <string>         Set the Trigger level : L2/EF/BOTH"
  print "  -o|   --menuonly               Compare the menu components only"
  print "  -u|   --setuponly              Compare the menu components only"
  print "  -d|   --diffmask <int>         Set mask for difference"

def getAlgIDs(setupLoader, smkey, cursor) :
  # Get the algorithms for SM Key 
  setupLoader.SMKey = smkey
  hmtkey = setupLoader.getHLTMasterKeys(cursor)
  
  printout = "Super Master Key: %d" % (smkey)
  printout += "\tHLT Master Key: %d" % (hmtkey)
  
  alg_ids = []
  hltL2Alg_ids = []
  if setupLoader.Level.upper() in ('L2', 'BOTH') and not menuOnly:
    hstL2ID = setupLoader.getHLTL2SetupID(cursor, hmtkey)
    printout += "\tL2 Setup ID: %d" % (hstL2ID)
    hltL2Alg_ids.extend(setupLoader.getHLTSetupComponentIDs(cursor, hstL2ID))
    for alg_id in hltL2Alg_ids : 
      if alg_id not in alg_ids : alg_ids.append(alg_id)
  
  hltEFAlg_ids = []
  if setupLoader.Level.upper() in ('EF', 'BOTH') and not menuOnly:
    hstEFID = setupLoader.getHLTEFSetupID(cursor, hmtkey)
    printout += "\tEF Setup ID: %d" % (hstEFID)
    hltEFAlg_ids.extend(setupLoader.getHLTSetupComponentIDs(cursor, hstEFID))
    for alg_id in hltEFAlg_ids :
      if alg_id not in alg_ids : alg_ids.append(alg_id)
      
  hltMenuL2Alg_ids = []
  if setupLoader.Level.upper() in ('L2', 'BOTH') and not setupOnly:
    hltMenuL2Alg_ids.extend(setupLoader.getHLTTEComponentIDs(cursor, 'L2', hmtkey))
    for alg_id in hltMenuL2Alg_ids : 
      if alg_id not in alg_ids : alg_ids.append(alg_id)
  
  hltMenuEFAlg_ids = []
  if setupLoader.Level.upper() in ('EF', 'BOTH') and not setupOnly:
    hltMenuEFAlg_ids.extend(setupLoader.getHLTSetupComponentIDs(cursor, 'EF', hmtkey))
    for alg_id in hltMenuEFAlg_ids : 
      if alg_id not in alg_ids : alg_ids.append(alg_id)
  
  print printout
  return alg_ids

if __name__ == '__main__':
  
  db_user='atlas_trig_nozicka'
  db_passwd='*******'
  db_host='devdb10'
  db_name = ''
  db_type = 'oracle'
  
  level = 'BOTH'
  smkey1 = -1
  smkey2 = -1
  menuOnly = False
  setupOnly= False
  diffmask= 0xffff
  
  short_opt = "h?t:p:u:n:m:s:S:l:d:ou"
  long_opt = ['help', 'type=', 'password=', 'user=', 'name=', 'host=', 'smkey1=', 'smkey2=', 'level=', 'diffmask=', 'menuonly', 'setuponly']
  
  if len(sys.argv) == 1:
      print "Missing arguments"
      usage()
      sys.exit(1)
      
  # These are common bootstrap procedures for option processing
  try:
      opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
  except getopt.GetoptError, exc:
      print '\nError: '+exc.msg+'\n'
      usage()
      sys.exit(1)

  # The defaults
    
  # Read the options
  for o, a in opts:
    if o in ['-h', '-?', '--help']:
      usage()
      sys.exit(0)
    if o in ['-t', '--type']:       db_type = a
    if o in ['-p', '--password']:   db_passwd = a
    if o in ['-u', '--user']:       db_user = a
    if o in ['-n', '--name']:       db_name = a
    if o in ['-m', '--host']:       db_host = a
    if o in ['-s', '--smkey1']:     smkey1 = a
    if o in ['-S', '--smkey2']:     smkey2 = a
    if o in ['-l', '--level']:      level = a
    if o in ['-o', '--menuonly']:   menuOnly = True
    if o in ['-u', '--setuponly']:  setupOnly = True
    if o in ['-d', '--diffmask']:   diffmask = a
  
  smkey1 = eval(smkey1)
  smkey2 = eval(smkey2)
  diffmask = eval(diffmask)
  
  # Setup loader
  from TrigConfOffline.HLTSetupLoader import SetupLoader
  setupLoader = SetupLoader(dbType=db_type, dbHost=db_host, dbUser=db_user, dbPasswd=db_passwd, dbName=db_name, Level=level)
  cursor = setupLoader.connectDB()
  alg1_ids = getAlgIDs(setupLoader, smkey1, cursor)
  alg2_ids = getAlgIDs(setupLoader, smkey2, cursor)
  
  alg1 = {}
  for alg_id in alg1_ids :
    # No need to compare identic algorithms
    if alg_id not in alg2_ids :
      alg=HLTAlgorithm(dbId=alg_id)
      alg.loadFromDB(cursor,setupLoader.dbName)
      alg1[alg.getAlias()] = alg
  
  alg2 = {}
  for alg_id in alg2_ids :
    # No need to compare identic algorithms
    if alg_id not in alg1_ids :
      alg=HLTAlgorithm(dbId=alg_id)
      alg.loadFromDB(cursor,setupLoader.dbName)
      alg2[alg.getAlias()] = alg
  
  cursor.close()
  
  alg2Missing = []
  # Compare identic alias algorithms
  for key in alg1.keys() :
    if key in alg2.keys() :
      out = diffHLTAlgorithms(alg1[key], alg2[key], diffmask, ' ')
      if out.strip() !='' :
        print out
    else :
      alg2Missing.append(key)

  # Find missing algs in setup
  alg1Missing = []
  for key in alg2.keys() :
    if key not in alg1.keys():
      alg1Missing.append(key)

  # Print out of algorithms missing in the SM1
  if len(alg2Missing) >=0 : print "Algorithms missing in the configuration SM=%d" % (smkey2)
  
  for alg in alg2Missing:
    print alg1[alg].algToStr('\t',False)

  # Print out of algorithms missing in the SM1
  if len(alg1Missing) >=0 : print "Algorithms missing in the configuration SM=%d" % (smkey1)
  for alg in alg1Missing:
    print alg2[alg].algToStr('\t',False)
















