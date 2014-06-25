#!/usr/bin/env python2.3
# $Id: xml2db.py,v 1.12 2007-09-17 10:36:16 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 27-Mar-2006
# Maintained by Joerg Stelzer <stelzer@cern.ch>
# Modified by Miroslav Nozicka <nozicka@mail.desy.de>

# Reads an XML description of what has to be inserted into the HLT database,
# connects to the DB and inserts the information making use of the the basic
# primitives

import TrigDbHltUpload.jobasic
import TrigDbHltUpload.job
import TrigDbHltUpload.sqlutils
import TrigDbHltUpload.hltpopdblib
import TrigDbHltUpload.hltmenu
import TrigDbHltUpload.hltsetup

from TrigDbHltUpload.hltutils import dumpSQL
from TrigDbHltUpload.hltutils import dumpSQLFile

import getopt, sys
import xml.dom.minidom
import time



def usage():
    """Prints a help message"""
    print "Usage: %s [database-options] --l2setup <L2setup.xml> --efsetup <EFsetup.xml>  --smkeybase <signature.xml> <sequence.xml>" % \
          sys.argv[0].split('/')[-1]
    print "  -h|-?|--help                Issues this help message"
    print "     --l2setup L2setup.xml    Level 2 Setup (required)"
    print "     --efsetup efsetup.xml    Event Filter Setup (required)"
    print "     --smkeybase <int>        SuperMasterKey on which this insert is based"
    print "                              ... default is 0 representing the latest SuperMasterKey"
    print "                              ... specifying -1 omits insertion of a new entry in the Super_Master_Table"
    print "  -r|--release <string>       The name of the release if no release.xml is given"
    print "     --menuonly               If specified, only the menu, but no setup or release are expected"
    print " Database options may be:"
    print "  -p|--password               Asks the user password to the DB"
    print "  -t|--type mysql|oracle      Selects the database backend to use"
    print "  -u|--user <string>          The name of the user in the DB"
    print "  -n|--name <string>          The name of the DB inside the server"
    print "  -m|--host <string>          The name of ther host where the DB server is running"



def readxmlfile(xmlfile):
    start = time.time()
    tmpdoc = xml.dom.minidom.parse(xmlfile)
    end = time.time()
    print "[%s] Reading %s" % (sys.argv[0], a),
    print "took %1.2f second(s)" % (end - start)
    return tmpdoc

  
if __name__ == '__main__':
    
    short_opt = "h?p:t:u:n:m:r:"
    long_opt = ['help', 'password=', 'type=', 'user=', 'name=', 'host=',
                'release=', 'l2setup=', 'efsetup=', 'smkeybase=', 'menuonly', 'dumpSQL=']
    
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
    db_type = 'mysql'
    db_host = 'localhost'
    db_user = 'triguser'
    db_name = 'TestDBJoerg'
    db_userpw = 'trigconfig'
    NewStyleXML = False
    release_name = None
    ask_pass = False
    force_full_release = False
    SMK_base = 0;
    MenuOnly = False


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
        if o in ['-r', '--release']: release_name = a
        if o in ['--l2setup']:       doc_map[ 'l2setup' ] = readxmlfile(a)
        if o in ['--efsetup']:       doc_map[ 'efsetup' ] = readxmlfile(a)
        if o in ['--smkeybase']:     SMK_base = a
        if o in ['--menuonly']:      MenuOnly = True
        if o in ['--dumpSQL']:       dumpSQL(True); dumpSQLFile(a)

    # check if all necessary xml setup files were supplied
    if not 'l2setup' in doc_map and not MenuOnly:
        print "\nerror: No Level 2 setup supplied\n"
        usage()
        sys.exit(1)
    if not 'efsetup' in doc_map and not MenuOnly:
        print "\nerror: No EF setup supplied\n"
        usage()
        sys.exit(1)

    for a in args:
        tmpdoc = readxmlfile(a)
        doc_map[ tmpdoc.documentElement.tagName.lower() ] = tmpdoc

    # check if all other necessary xml files were supplied
    if not 'release' in doc_map and release_name == None and not MenuOnly:
        print "No release file supplied"
        usage()
        sys.exit(1)    
    if 'hlt_menu' in doc_map:
        NewStyleXML = True
        print "New style configuration xml"
    else:
        if not 'chain_list' in doc_map:
            print "\nerror: No signature file supplied\n"
            usage()
            sys.exit(1)
        if not 'sequence_list' in doc_map:
            print "\nerror: No sequence file supplied\n"
            usage()
            sys.exit(1)

    
    fVerbose = False
    if fVerbose:
        for key in ('release', 'l2setup', 'efsetup', 'chain_list', 'sequence_list'):
            if key in doc_map: print doc_map[key].toxml()



    # connect to the database
    cursor = TrigDbHltUpload.sqlutils.get_cursor(db_host, db_user, db_name, db_type, ask_pass, db_userpw)


    start = time.time()

    verbose = False

    print "----------------------------------------"

    # load release
    release_vers = '12.34.56'
    if 'release' in doc_map:
        release_obj = TrigDbHltUpload.jobasic.Release(doc_map['release'].documentElement)
        release_name = release_obj.name
        release_vers = release_obj.version
    elif not release_name == None:
        release_vers = '12.34.56'
    else :
        release_name = 'dummy'
    
    modtime_db = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
    colList = []
    colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_NAME",           release_name,     "S",  True ) )
    colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_VERSION",        release_vers,  "S",  True ) )
    release_id = TrigDbHltUpload.hltpopdblib.check_row( cursor, "HLT_RELEASE", "HRE", colList )

    if release_id <=0 :
        colList = []
        colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_NAME",           release_name,     "S",  True ) )
        colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_VERSION",        release_vers,  "S",  True ) )
        colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_USERNAME",       db_user,              "S",  True ) )
        colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_MODIFIED_TIME",  modtime_db,           "T",  True ) )
        colList.append( TrigDbHltUpload.hltmenu.Column( "HRE_USED",           "0",                  "S",  True ) )
        release_id = TrigDbHltUpload.hltpopdblib.insert_row( cursor, "HLT_RELEASE", "HRE", colList )
    
    if isinstance(release_id, tuple) : release_id = release_id[0]
    print "- Release:", release_id

    # load Level 2 setup  
    if not MenuOnly and 'l2setup' in doc_map:
        print "L2 setup:"
        print "\tStart time:", time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
        l2setup = TrigDbHltUpload.hltsetup.Setup(doc_map['l2setup'].documentElement)
        l2setuptable = TrigDbHltUpload.hltsetup.DBHltSetup(l2setup)
        #l2setup_modtime_db = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
        l2setup_id = l2setuptable.dbInsert(cursor,
                                           username=db_user,
                                           timestamp=modtime_db,
                                           verbose=verbose)
        print "\tStop time:", time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
        print "\tL2 setup ID:", l2setup_id
    else:
        print "- L2 setup: dummy"
        l2setup_id = TrigDbHltUpload.hltmenu.insertDummySetup( cursor )
    
    # load Event Filter setup
    if not MenuOnly and 'efsetup' in doc_map:
        print "EF setup:"
        print "\tStart time:", time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
        efsetup = TrigDbHltUpload.hltsetup.Setup(doc_map['efsetup'].documentElement)
        efsetuptable = TrigDbHltUpload.hltsetup.DBHltSetup(efsetup)
        #efsetup_modtime_db = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
        efsetup_id = efsetuptable.dbInsert(cursor,
                                           username=db_user,
                                           timestamp=modtime_db,
                                           verbose=verbose)
        print "\tStop time:", time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
        print "\tEF setup ID:", efsetup_id
    else:
        print "- EF setup: dummy"
        efsetup_id = TrigDbHltUpload.hltmenu.insertDummySetup( cursor )

    # load menu
    if NewStyleXML:
        menu = TrigDbHltUpload.hltpopdblib.parseXML( doc_map['hlt_menu'] )
    else:
        menu = TrigDbHltUpload.hltpopdblib.parseXML( doc_map['chain_list'], doc_map['sequence_list'] )

    # load the HLT master table
    mastertable = TrigDbHltUpload.hltmenu.DBMasterTable(menu=menu,
                                                        release_id=release_id,
                                                        l2setup_id=l2setup_id,
                                                        efsetup_id=efsetup_id,
                                                        forceComponents = MenuOnly)
    HLT_masterKey = mastertable.dbInsert(cursor,
                                         verbose,
                                         username=db_user,
                                         timestamp=modtime_db)

    # load or link the super master table
    newSMK = 0;
    if int(SMK_base)>=0:
        newSMK = TrigDbHltUpload.hltmenu.createSMEntry(cursor=cursor,
                                                       newname=menu.mastertable_name,
                                                       HLT_masterKey=HLT_masterKey,
                                                       SMK_baseKey=int(SMK_base),
                                                       verbose=verbose,
                                                       username=db_user,
                                                       timestamp=modtime_db)

    dumpSQLFile() # closes the dump SQL file

    sys.exit(newSMK)
