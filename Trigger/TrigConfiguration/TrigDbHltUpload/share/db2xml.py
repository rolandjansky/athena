#!/usr/bin/env python
# $Id: db2xml.py,v 1.6 2007-07-13 17:27:14 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 27-Mar-2006

# Reads an XML description of what has to be inserted into the HLT database,
# connects to the DB and inserts the information making use of the the basic
# primitives

import TrigDbHltUpload.jobasic
import TrigDbHltUpload.job
import TrigDbHltUpload.sqlutils
import getopt, sys
import xml.dom.minidom
import time
#from xml.dom.ext import PrettyPrint

def usage():
  """Prints an help message"""
  print "usage: %s [database-options] [setup|release-options] " % \
        sys.argv[0].split('/')[-1]
  print ""
  print "general options"
  print "  -h|-?|--help                    Issues this help message"
  print ""
  print "database options:"
  print "  -t|--type mysql|oracle          Selects the database backend to use [%s]" % db_type
  print "  -m|--host <string>              The name of ther host where the DB server is running [%s]" % db_host
  print "  -u|--user <string>              The name of the user in the DB [%s]" % db_user
  print "  -n|--name <string>              The name of the DB inside the server [%s]" % db_name
  print "  -p|--password                   Asks the user password to the DB [%s]" % db_passwd
  print ""
  print "configuration selection options:"
  print "  -k|--SMK <int>                  The super master key I'll search in the DB for"
  print "     --hltMK <int>                The HLT master key I'll search in the DB for"
  #  print "  -s|--setup <string>    The name I'll search in the DB for"
  #  print "  -r|--release <string>  The name I'll search in the DB for"
  #  print "  -j|--job <string>      The name I'll search in the DB for"
  #  print "  -v|--version <string>  A string that specifies the version of the setup"
  print ""
  print "output options:"
  print "  -s|--singlefile                 Saves the XML output into a single file (format of Release 13)"
  print "  -o|--output <string>            Basename of the output files"
  print "     --output_sequence <string>   The menu sequence xml filename"
  print "     --output_signature <string>  The menu signature xml filename"

def listlongoptions(shortopts, longopts):
  #for o in shortopts.replace(':',''):
  #  print o,
  for o in longopts:
    print '--%s' % o.replace('=',''),

if __name__ == '__main__':

  short_opt = "h?p:slt:u:n:m:o:k:"
  long_opt = ['help', 'password=', 'list', 'SMK=', 'hltMK=', 'type=', 'user=', 'name=', 'host=', 'output=', 'singlefile', 'output_sequence=', 'output_signature=']

  #The defaults
  db_type   = 'mysql'
  db_host   = 'pcatr13'
  db_user   = 'trigtester'
  db_passwd = 'trigtest'
  db_name   = 'TestHLT_Stelzer_150207'
  ask_pass = False
  smkey    = None
  hltkey   = None
  version  = None
  newStyle = False
  output                = None
  output_menu_sequence  = None
  output_menu_signature = None

  
  #these are common bootstrap procedures for option processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
  except getopt.GetoptError, exc:
    print '\nError: '+exc.msg+'\n'
    usage()
    sys.exit(1)

  #Read the options
  print opts
  for o, a in opts:
    if o in ['-l', '--list']:
      listlongoptions(short_opt, long_opt)
      sys.exit(0)
    if o in ['-h', '-?', '--help']:
      usage()
      sys.exit(0)
    if o in ['-p', '--password']: db_passwd = a; ask_pass=False
    if o in ['-t', '--type']:
      if a not in ('mysql', 'oracle'):
        raise SyntaxError, 'Database backend %s is not implemented' % a
      db_type = a
    if o in ['-u', '--user']:   db_user = a
    if o in ['-n', '--name']:   db_name = a
    if o in ['-m', '--host']:   db_host = a
    if o in ['-k', '--SMK']:    smkey = int(a)
    if o in ['--hltMK']:        hltkey = int(a)
    if o in ['-s', '--singlefile']: newStyle = True
    if o in ['-o', '--output']: output = a
    if o in [      '--output_sequence']:  output_menu_sequence  = a
    if o in [      '--output_signature']: output_menu_signature = a

  if smkey is None and hltkey is None:
    print '\nerror: no super- or hlt- master key specified\n'
    usage()
    sys.exit(1)

  print "DB host: %s" % db_host
  print "DB user: %s" % db_user
  print "DB name: %s" % db_name

  cursor = TrigDbHltUpload.sqlutils.get_cursor(db_host, db_user, db_name, db_type, ask_pass, db_passwd)
  start = time.time()

  element = None

  if smkey is not None:
    (smname, smversion, lvl1MK, hltMK, menu_id, ps_id, rel_id) = TrigDbHltUpload.sqlutils.getMainKeysFromSmk(cursor, smkey)
    print "The following keys belong to the super master key", smkey
    print "   Level 1 master key   :", lvl1MK
    print "   HLT master key       :", hltMK
    print "       HLT trigger menu :", menu_id
    print "       HLT prescale set :", ps_id
    print "       HLT release      :", rel_id

    if output==None:
      output = "HLT_%s_V%s" % (smname, smversion)

    if newStyle:
      if output_menu_sequence==None:
        output_menu_sequence = "%s_menu.xml" % (output)
      output_menu_signature = ''
    else:
      if output_menu_sequence==None:
        output_menu_sequence = "%s_sequence.xml" % (output)
      if output_menu_signature==None:
        output_menu_signature = "%s_signature.xml" % (output)

      
    if menu_id is not None and ps_id is not None:
      menu = TrigDbHltUpload.hltmenu.HLTMenu(cursor,menu_id,ps_id).menu
      #create xml document(s)
      start = time.time()
      menu.writeToFile(xmlSeqFileName=output_menu_sequence, xmlSigFileName=output_menu_signature)
      end = time.time()
      print "[%s] Building XML document" % sys.argv[0],
      print "took %1.2f second(s)" % (end - start)
      
      # Get The Setup ID's from menu
      l2setupid = TrigDbHltUpload.hltsetup.getL2SetupIDs(cursor, menu_id)
      if l2setupid > 0:
        print "Building L2 Setup XML document ..."
        L2xmlFileName= "%s_L2setup.xml" % (output)
        l2setup = TrigDbHltUpload.hltsetup.getSetupFromDB(cursor,l2setupid)
        fout = open( L2xmlFileName,  "w" )
        doc = xml.dom.minidom.Document()
        doc.appendChild(l2setup.xml_node(doc))
        fout.write(doc.toprettyxml())
        fout.close()
        print "File %s written"  % L2xmlFileName

      efsetupid = TrigDbHltUpload.hltsetup.getEFSetupIDs(cursor, menu_id)
      if efsetupid > 0:
        print "Building EF Setup XML document ..."
        EFxmlFileName= "%s_EFsetup.xml" % (output)
        efsetup = TrigDbHltUpload.hltsetup.getSetupFromDB(cursor,efsetupid)
        fout = open( EFxmlFileName,  "w" )
        doc = xml.dom.minidom.Document()
        doc.appendChild(efsetup.xml_node(doc))
        fout.write(doc.toprettyxml())
        fout.close()
        print "File %s written"  % EFxmlFileName



  sys.exit(0)























