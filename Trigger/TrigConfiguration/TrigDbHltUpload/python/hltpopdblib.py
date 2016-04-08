#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: hltpopdblib.py,v 1.13 2008-01-28 20:44:51 stelzer Exp $
#
# Andreas Hoecker, 9-Apr-2006
#
# helper library
# ------------------------------------------------------------------------
#
import sys # for exiting
import time # for time accounting
import socket # for host information
#import MySQLdb # MySQL-python 1.2.0 modules
#import cx_Oracle # Python Oracle cursor
import xml.dom.minidom # xml parser
import hltmenu

import hltutils
from hltutils import dumpSQL, dumpSQLFile, dbVerbose, get_column, format_dbstring



# Python time format to modify the date and time for Oracle DB
time_format= 'YYYY-MM-DD HH24:MI:SS'
  
class Column(object):
    def __init__( self, name, value, type, test = True):
        self.name  = name
        self.value = value
        self.type  = type
        self.test  = test
#
# -----------------------------------------------------------------------------------
# stored procedures
#
# each of these procedures checks if a given row exists in table,
# and returns its id if yex, while inserts it and returns the id otherwise

# modification for Oracle; SELECT __ FROM __  AS - option AS is not supported
# Oracle date and time format may differ from the python format, use formatting oracle function
def check_row( cursor, table_name, table_prefix, columnList, order = ""):
    # Test for type of database in use
    oracle_db = False
    mysql_db = False
    if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
    if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True

    query = "SELECT DISTINCT %s.%s_ID FROM %s WHERE " % \
            (table_name, table_prefix, table_name)
    for col in columnList:
        if col.test == False: continue
        if col.name == 'HPA_VALUE' and col.type == "S":
            query += "%s.%s LIKE '%s' AND " % (table_name, col.name, format_dbstring(col.value))
        elif col.type == "S":
            query += "%s.%s = '%s' AND " % (table_name, col.name, format_dbstring(col.value))
        elif col.type == "I":
            query += "%s.%s = %i AND " % (table_name, col.name, int(col.value))
        elif col.type == "F":
            query += "%s.%s = %g AND " % (table_name, col.name, float(col.value))
        elif col.type == "T":
            if oracle_db :
                query += "%s.%s = to_date('%s' , '%s') AND  " % (table_name, col.name, col.value, time_format)
            else:
                query += "%s.%s = '%s' AND " % (table_name, col.name, col.value)
        else:
            print col.type, col.name, col.value
            print "*** unknown type in function check_row ==> abort"
            sys.exit(1)

    query = query.rstrip( "AND " )
    if order != "": query += " ORDER BY %s ASC" % order

    # In case the query is not str or none oracle_db has problems reformat query
    if isinstance(query, unicode): query = str(query)

    if dbVerbose(): print type(query), len(query), query

    cursor.execute( query )
    result = cursor.fetchall()
    if len(result) > 0:
        if dbVerbose(): print "EXISTS!"
        return result[0]
    else:
        if dbVerbose(): print "DOES NOT EXIST!"
        return -1



def insert_row( cursor, table_name, table_prefix, columnList, insertAnyway = False, order = "" ):
  # Test for type of database in use
  oracle_db = False
  mysql_db = False
  if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
  if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True
  
  id = check_row( cursor, table_name, table_prefix, columnList, order )
  
  # insert new row    
  if id < 0 or insertAnyway:
    # if id > 0, this means the menu exists already, and we have to
    # add another version
    if id > 0:
      if dbVerbose(): print "EXISTS but insert anyway"
      forcedInsert = True
    else:
      forcedInsert = False
        
    # retrieve latest PK id
    id = hltutils.get_new_index( cursor, table_name, "%s_ID" % table_prefix )

    insert = "INSERT INTO %s \t VALUES( %i, " % (table_name, id)
    if  dbVerbose(): print insert
    for col in columnList:
        # if it's the version, and it should be reset, do so
        if col.name.rfind( "VERSION" ) >= 0 and forcedInsert:
            col.value = get_column( cursor, table_name, table_prefix, "VERSION", id ) + 1
        if col.type == "S":
            insert += "'%s', " % format_dbstring(col.value)
        elif col.type == "I":
            insert += "%i, " % int(col.value)
        elif col.type == "F":
            insert += "%g, " % float(col.value)
        elif col.type == "T":
            if oracle_db :
                insert += "to_date('%s' , '%s')," % (col.value, time_format)
            else: insert += "'%s', " % (col.value)
        else:
            print "*** unknown type in function insert_row ==> abort"
            sys.exit(1)

    insert = insert.rstrip( ", " )
    insert += " )"
    
    # In case the query is not str or none oracle_db has problems reformat query
    if isinstance(insert, unicode): insert = str(insert)
    if dbVerbose():
        print type(insert), len(insert), insert

    if dumpSQL():
        print >>dumpSQLFile(), insert
    
    cursor.execute( insert )
    
    return id


# -----------------------------------------------------------------------------------
# db, upload and downlod functions
#
# Gets a python DB cursor for given: db, host, user, passwd
#
# Parameters:
# db    : The database name
# host  : The host where the server is
# user  : The user for the DB in question
# passwd: Its password. If not given, it will be asked
def get_cursor( db, host, user, passwd=-1 ):
    
  # Ask for the user password if it is empty
  if passwd == -1:
	passwd = getpass.getpass("DB password for %s@%s: " % (user, host))

  # Connect to the DBMS and make the queries, connection timeout is 10 secs
  connection = MySQLdb.connect (host=host, user=user, passwd=passwd, db=db,
                                connect_timeout=10)
  return connection.cursor()
  
# domChainList can be a list of nodes or a single node
def parseXML( *args ):

    for arg in args:
        if isinstance( arg, xml.dom.minidom.Document):
            node = arg.documentElement            
        elif isinstance( arg, xml.dom.minidom.Node):
            node = arg
        else:
            print "\n*** error: argument is neither an xml document nor an xml node => abort\n"
            sys.exit(1)

        newStyleMenu = 0
        if node.tagName=='HLT_MENU':
            newStyleMenu = 1
            dom_chainLists = node.getElementsByTagName('CHAIN_LIST')
            if len(dom_chainLists)!=1:
                print "*** error: found %i CHAIN_LISTs in the HLT_MENU => abort ***"
                sys.exit(1)
            DOM_ChainList = dom_chainLists[0]

            dom_sequenceLists = node.getElementsByTagName('SEQUENCE_LIST')
            if len(dom_sequenceLists)!=1:
                print "*** error: found %i SEQUENCE_LISTs in the HLT_MENU => abort ***"
                sys.exit(1)
            DOM_SequenceList = dom_sequenceLists[0]
        elif node.tagName=='CHAIN_LIST':
            DOM_ChainList = node
            print "CHAIN LIST:", DOM_ChainList
        elif node.tagName=='SEQUENCE_LIST':
            DOM_SequenceList = node
        else:
            print "\n*** error: dom argument '%s' is neither an CHAIN_LIST nor a SEQUENCE LIST nor a HLT_MENU => abort\n" % (node.tagName)


    #print DOM_ChainList.toprettyxml()


    # retrieve menu and prescale set, and create Menu class object
    if newStyleMenu == 0:
        mastertable_name     = DOM_ChainList.getAttribute( "hlt_master_table.name" )
        mastertable_version  = int(DOM_ChainList.getAttribute( "hlt_master_table.version" ))
        menu_name            = DOM_ChainList.getAttribute( "hlt_trigger_menu.name" )
        menu_version         = int(DOM_ChainList.getAttribute( "hlt_trigger_menu.version" ))
        prescale_set_name    = DOM_ChainList.getAttribute( "hlt_prescale_set.name" )
        prescale_set_version = int(DOM_ChainList.getAttribute( "hlt_prescale_set.version" ))
    else:
        # Version 13 and higher
        mastertable_name     = ""
        mastertable_version  = 0
        menu_name            = node.getAttribute( "name" )
        if node.getAttribute( "version" )=="":
            menu_version     = 0
        else:
            menu_version     = int(node.getAttribute( "version" ))
        prescale_set_name    = ""
        prescale_set_version = 0
    
    menu = hltmenu.Menu(  mastertable_name,  mastertable_version,
                          menu_name, menu_version, prescale_set_name, prescale_set_version )

    menu._print_()

    # retrieve chains and sequences
    dom_chains = DOM_ChainList.getElementsByTagName( "CHAIN" )
    dom_sequences   = DOM_SequenceList.getElementsByTagName( "SEQUENCE" )
    print "- number of chains found   : %i" % len(dom_chains)
    print "- number of sequences found: %i" % len(dom_sequences)

    #loop over chainList
    print "- start chain loop ..."

    chCounters_L2 = []
    chCounters_EF = []

    for dom_chain in dom_chains:

        # create the chain object
        if dom_chain.hasAttribute( "chain_id" ): # old style
            chainName      = dom_chain.getAttribute( "chain_id" )
            lowerChainName = dom_chain.getAttribute( "lower_chain_id" )
        elif dom_chain.hasAttribute( "chain_name" ):
            chainName      = dom_chain.getAttribute( "chain_name" )
            lowerChainName = dom_chain.getAttribute( "lower_chain_name" )
        else:
            print "*** error: no attribute 'chain_name' or 'chain_id' found => abort ***"
            sys.exit(1)
            
        
        chain = hltmenu.Chain( chainName,
                               int(dom_chain.getAttribute( "chain_counter" )),
                               lowerChainName,
                               dom_chain.getAttribute( "level" ),
                               int(dom_chain.getAttribute( "prescale" )),
                               int(dom_chain.getAttribute( "pass_through" )) )

        # get the streaming tag information
        dom_streamTagLists = dom_chain.getElementsByTagName( "STREAMTAG_LIST" )
        if len(dom_streamTagLists)>1:
            print "*** error: more than one STREAMTAG_LIST found in chain %s => abort ***" % (chain.name)
            sys.exit(1)
         
        # get the triggertype information
        dom_triggerTypeLists = dom_chain.getElementsByTagName( "TRIGGERTYPE_LIST" )
        if len(dom_triggerTypeLists)>1:
            print "*** error: more than one TRIGGERTYPE_LIST found in chain %s => abort ***" % (chain.name)
            sys.exit(1)

        # get the triggertype information
        dom_groupLists = dom_chain.getElementsByTagName( "GROUP_LIST" )
        if len(dom_groupLists)>1:
            print "*** error: more than one GROUP_LIST found in chain %s => abort ***" % (chain.name)
            sys.exit(1)

        # Test the release
        rel13_tag = False
        if newStyleMenu == 1:
            rel13_tag = True
            if len(dom_streamTagLists)<1:
                print "*** error: no STREAMTAG_LIST found in chain %s => abort ***" % (chain.name)
                sys.exit(1)
            if len(dom_triggerTypeLists)<1:
                print "*** error: no TRIGGERTYPE_LIST found in chain %s => abort ***" % (chain.name)
                sys.exit(1)
            if len(dom_groupLists)<1:
                print "*** error: no GROUP_LIST found in chain %s => abort ***" % (chain.name)
                sys.exit(1)

            # new xml style must contain the stream tag information
            dom_streamTagList = dom_streamTagLists[0].getElementsByTagName( "STREAMTAG" )
            if len(dom_streamTagList)==0:
                print "*** error: STREAMTAG_LIST for chain %s has no STREAMTAGs (at least one required) => abort ***" % (chain.name)
                sys.exit(1)
            for dom_streamtag in dom_streamTagList:
                chain.streamtags.append( hltmenu.StreamTag( dom_streamtag.getAttribute( "stream" ),
                                                            dom_streamtag.getAttribute( "type" ),
                                                            int(dom_streamtag.getAttribute( "prescale" )),
                                                            dom_streamtag.getAttribute( "obeyLB" ),
                                                            "") )


            # new xml style must contain the trigger type information
            dom_triggerTypeList = dom_triggerTypeLists[0].getElementsByTagName( "TRIGGERTYPE" )
            #if len(dom_triggerTypeList)==0:
            #    print "*** error: TRIGGERTYPE_LIST for chain %s has no TRIGGERTYPEs (at least one required) => abort ***" % (chain.name)
            #    sys.exit(1)
            # always put the chaincounter into the list of triggertypes
            chain.triggertypes.append( hltmenu.TriggerType( int(chain.counter) ) )
            for dom_triggertype in dom_triggerTypeList:
                chain.triggertypes.append( hltmenu.TriggerType( int(dom_triggertype.getAttribute("bit")) ) )

            # new xml style may contain the group information
            dom_groups = dom_groupLists[0].getElementsByTagName( "GROUP" )
            for dom_group in dom_groups:
                chain.triggergroups.append( hltmenu.TriggerGroup( str(dom_group.getAttribute("name")) ) )



        # --- loop over signature and sequence lists and write attributes to list
        dom_sigLists = dom_chain.getElementsByTagName( "SIGNATURE_LIST" )

        allowEmptyChains = True

        # again, sanity check - there can only be one signature_list in a chain
        if not (len(dom_sigLists) == 1 or len(dom_sigLists)==0 and allowEmptyChains):
            print "*** error: number of signature lists in chain %s is not 1 but %i, there has to be exactly 1 ==> abort ***" \
                  % (chain.name, len(dom_sigLists))
            sys.exit(1)

        # check if same counter is specified twice in the same level
        if chain.level=="L2" and chain.counter in chCounters_L2 or chain.level=="EF" and chain.counter in chCounters_EF:
            print "*** error: chain counter '%i' of chain '%s' has already been specified in level '%s' ==> abort ***" \
                  % (chain.counter, chain.name, chain.level)
            sys.exit(1)
        else:
            if chain.level=="L2":
                chCounters_L2.append(chain.counter)
            else:
                chCounters_EF.append(chain.counter)


        if len(dom_sigLists) == 1:
            # retrieve signatures and sequences
            dom_sigL = dom_sigLists[0].getElementsByTagName( "SIGNATURE" )
            #if len(dom_sigL)==0:
            #    print "*** error: No SIGNATURE specified in SIGNATURELIST of chain %s\n ===> abort" % chain.name
            #    sys.exit(1)

            # now start nested loops over sequence and signature lists
            print "- ... found %i signatures in %s chain: %s (chain counter %i)" % (len(dom_sigL), chain.level, chain.name, chain.counter)

            # signature lists
            sigs = [];    
            for isig, dom_sig in enumerate(dom_sigL):

                # create signature id (name) by adding to the chain name the signature counter
                # signature id is not really used anymore
                sig_id = "%s_%i" % (chain.name, int(dom_sig.getAttribute( "signature_counter" )))
                
                # create the signature object            
                sig = hltmenu.Signature( sig_id,
                                         int(dom_sig.getAttribute( "signature_counter" )), 
                                         int(dom_sig.getAttribute( "logic" )) )

                # sanity check
                # the signature counter does not need to go from 1 to N anymore
                #   if int(sig.counter) != isig+1: 
                #       print "*** error: signature counter out of order: %s != %i ==> abort ***" \
                #             % (sig.counter, isig+1)
                #       sys.exit(1)

                # loop over trigger elements
                dom_teList = dom_sig.getElementsByTagName( "TRIGGERELEMENT" )
                if len(dom_teList)==0:
                    print "*** error: No TRIGGERELEMENT specified in signature %s\n ===> ABORT" % sig_id
                    sys.exit(1)

                for ite, dom_te in enumerate(dom_teList):

                    # create an output trigger element
                    te = hltmenu.TriggerElement( dom_te.getAttribute( "te_name" ), ite )

                    # search for the corresponding sequence
                    found = 0
                    for dom_seq in dom_sequences:

                        # find the sequence
                        if dom_seq.getAttribute( "output" ) == te.name:
                            found += 1
                            seq = hltmenu.Sequence()
                            for inpTE in dom_seq.getAttribute( "input" ).split():
                                seq.add_teIn( inpTE )
                            seq.set_teInTopo( dom_seq.getAttribute( "topo_start_from" ) )
                            seq.set_teOut( dom_seq.getAttribute( "output" ) )
                            
                            #algoList      = dom_seq.getAttribute( "algorithm" ).split()
                            #algoList__    = []
                            #for algo in algoList:
                            #    algoList__.append( hltmenu.Algorithm( algo ) )
                            #seq.algoList  = algoList__
                            seq.algoList = [ hltmenu.Algorithm(algo) for algo in dom_seq.getAttribute( "algorithm" ).split() ]
                    if found==0:
                        print "*** error: incosistency discovered: TRIGGERELEMENT %s specified in signature file but not in sequence file ==> abort ***" \
                              % te.name
                        sys.exit(1)


                    # add to sequence list in signature
                    sig.add_seq( seq )

                # add signature to signature list in chain
                chain.add_sig( sig )

        # add chain to list
        menu.add_chain( chain )

    return menu

# get available trigger menus in db
def get_menu( cursor ):

    columns    = "TM.HTM_NAME, TM.HTM_VERSION, TM.HTM_ID, PS.HPS_NAME, PS.HPS_ID"
    tables     = "HLT_TRIGGER_MENU AS TM, HLT_PRESCALE_SET AS PS"
    conditions = "PS.HPS_TRIGGER_MENU_ID = TM.HTM_ID"
    order      = "TM.HTM_VERSION ASC"
    query      = "SELECT DISTINCT %s FROM %s WHERE %s ORDER BY %s;" % \
                 (columns, tables, conditions, order)    
    cursor.execute( query )
    result = cursor.fetchall()

    return result

# -----------------------------------------------------------------------------------
# populate the db from xml file
#
def populateDB( cursor, menu, htm_id, htm_name, hps_id, hps_name ):

    # -----------------------------------------------------
    # define global variables
    username       = "altrig"
    timestamp      = "NULL"
    setupname      = "dummy"

    # -----------------------------------------------------
    # execute some insertions that are required because the release part is missing
    
    # setup
    colList = []
    colList.append( Column( "HST_NAME", setupname, "S" ) )
    colList.append( Column( "HST_VERSION", "1", "F", False ) )
    colList.append( Column( "HST_USERNAME", username, "S", False ) )
    colList.append( Column( "HST_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HST_USED", "0", "S", False ) )
    hst_id = insert_row( cursor, "HLT_SETUP", "HST", colList )

    # components
    colList = []
    colList.append( Column( "HCP_NAME", "dummy", "S" ) )
    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
    colList.append( Column( "HCP_ALIAS", "dummy", "S" ) )
    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HCP_USED", "0", "S", False ) )
    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )

    colList = []
    colList.append( Column( "HCP_NAME", "LVL1", "S" ) )
    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
    colList.append( Column( "HCP_ALIAS", "LVL1", "S" ) )
    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HCP_USED", "0", "S", False ) )
    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )
    # -----------------------------------------------------
    
    # retrieve prescale set corresponding to the wanted trigger menu
    colList = []
    colList.append( Column( "HPS_ID", hps_id, "I" ) )
    colList.append( Column( "HPS_TRIGGER_MENU_ID", htm_id, "I" ) )
    hps_id = check_row( cursor, "HLT_PRESCALE_SET", "HPS", colList, "HPS_VERSION"  )

    # prescale set doesn't yet exist
    if hps_id < 0:
        # need a new menu 
        colList = []
        colList.append( Column( "HTM_NAME", htm_name, "S" ) )
        colList.append( Column( "HTM_VERSION", "1", "F", False ) )
        colList.append( Column( "HTM_PHASE", "lumi", "S" ) )
        colList.append( Column( "HTM_SHIFT_SAFE", "0", "S" ) )
        colList.append( Column( "HTM_USERNAME", username, "S", False ) )
        colList.append( Column( "HTM_MODIFIED_TIME", timestamp, "S", False ) )
        colList.append( Column( "HTM_USED", "0", "S", False ) )
        htm_id = insert_row( cursor, "HLT_TRIGGER_MENU", "HTM", colList, True )
                
        # now insert the prescale set
        colList = []
        colList.append( Column( "HPS_NAME", hps_name, "S" ) )
        colList.append( Column( "HPS_VERSION", "1", "F", False ) )
        colList.append( Column( "HPS_TRIGGER_MENU_ID", htm_id, "I" ) )
        colList.append( Column( "HPS_USERNAME", username, "S", False ) )
        colList.append( Column( "HPS_MODIFIED_TIME", timestamp, "S", False ) )
        colList.append( Column( "HPS_USED", "0", "S", False ) )
        hps_id = insert_row( cursor, "HLT_PRESCALE_SET", "HPS", colList )
        
    # interate over chains and retrieve prescales
    for chain in menu.chainList:

        # check prescales
        colList = []
        colList.append( Column( "HPR_PRESCALE_SET_ID", "%i" % hps_id, "I" ) )
        colList.append( Column( "HPR_CHAIN_COUNTER", "%i" % chain.counter, "I" ) )
        colList.append( Column( "HPR_PRESCALE", "%i" % chain.prescale, "I" ) )
        colList.append( Column( "HPR_USERNAME", username, "S", False ) )
        colList.append( Column( "HPR_MODIFIED_TIME", timestamp, "S", False) )
        colList.append( Column( "HPR_USED", "0", "S", False ) )
        hpr_id = insert_row( cursor, "HLT_PRESCALE", "HPR", colList )
        
    # iterate over chains and fill chain part
    for chain in menu.chainList:

        # add trigger chain
        colList = []
        colList.append( Column( "HTC_NAME", chain.name , "S" ) )
        colList.append( Column( "HTC_VERSION", "1", "I" ) )
        colList.append( Column( "HTC_LOWER_CHAIN_NAME", chain.lower_chain , "S" ) )
        colList.append( Column( "HTC_L2_OR_EF", chain.level , "S" ) )
        colList.append( Column( "HTC_PASS_THROUGH_RATE", 1.0 , "S" ) )
        colList.append( Column( "HTC_USERNAME", username, "S", False ) )
        colList.append( Column( "HTC_MODIFIED_TIME", timestamp, "S", False) )
        colList.append( Column( "HTC_USED", "0", "S", False ) )
        htc_id = insert_row( cursor, "HLT_TRIGGER_CHAIN", "HTC", colList )
        
        # set relation with trigger menu via N:M table
        colList = []
        colList.append( Column( "HTM2TC_TRIGGER_MENU_ID", "%i" % htm_id, "I" ) )
        colList.append( Column( "HTM2TC_TRIGGER_CHAIN_ID", "%i" % htc_id, "I" ) )
        colList.append( Column( "HTM2TC_CHAIN_COUNTER", "%i" % chain.counter, "I" ) )
        colList.append( Column( "HTM2TC_USERNAME", username, "S", False ) )
        colList.append( Column( "HTM2TC_MODIFIED_TIME", timestamp, "S", False) )
        colList.append( Column( "HTM2TC_USED", "0", "S", False ) )
        htm2tc_id = insert_row( cursor, "HLT_TM_TO_TC", "HTM2TC", colList )


        # loop over signatures in chain
        for sig in chain.sigList:

            EXISTS = False

            # loop over sequences in signature
            for iseq, seq in enumerate(sig.seqList):

                # add output trigger elements

                # add trigger element
                colList = []
                colList.append( Column( "HTE_NAME", seq.teOutElem, "S" ) )
                colList.append( Column( "HTE_VERSION", "%i" % 1, "I", False ) )
                colList.append( Column( "HTE_USERNAME", username, "S", False ) )
                colList.append( Column( "HTE_MODIFIED_TIME", timestamp, "S", False) )
                colList.append( Column( "HTE_USED", "0", "S", False ) )                    
                hte_id = insert_row( cursor, "HLT_TRIGGER_ELEMENT", "HTE", colList )

                # loop over input trigger elements and attribute to output te
                for tein in seq.teInList:

                    colList = []
                    colList.append( Column( "HTE2TE_TRIGGER_ELEMENT_ID", "%i" % hte_id, "I" ) )
                    colList.append( Column( "HTE2TE_TRIGGER_ELEMENT_INP_ID", tein, "S" ) )
                    colList.append( Column( "HTE2TE_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTE2TE_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTE2TE_USED", "0", "S", False ) )
                    hte2te_id = insert_row( cursor, "HLT_TE_TO_TE", "HTE2TE", colList )

                # set combined link between trigger element, signature and chain
                # some manual craft is necessary here ...
                columns     = "TS.HTS_ID"
                tables      = "HLT_TC_TO_TS AS TC2TS, HLT_TRIGGER_SIGNATURE AS TS, HLT_TS_TO_TE AS TS2TE"
                conditions  = "TS.HTS_ID = TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID "
                conditions  = "TS.HTS_ID = TS2TE.HTS2TE_TRIGGER_SIGNATURE_ID "
                conditions += "AND TC2TS.HTC2TS_TRIGGER_CHAIN_ID = %i " % htc_id
                conditions += "AND TS2TE.HTS2TE_TRIGGER_ELEMENT_ID = %i " % hte_id
                conditions += "AND TC2TS.HTC2TS_SIGNATURE_COUNTER = %i" % sig.counter
                query       = "SELECT DISTINCT %s FROM %s WHERE %s;" % (columns, tables, conditions)
                cursor.execute( query )
                result      = cursor.fetchall()

                # insert all if not existing
                if len(result) == iseq:

                    if EXISTS == False:

                        # add signature (forced insert)
                        colList = []
                        colList.append( Column( "HTS_LOGIC", "%i" % sig.logic, "I" ) )
                        colList.append( Column( "HTS_USERNAME", username, "S", False ) )
                        colList.append( Column( "HTS_MODIFIED_TIME", timestamp, "S", False) )
                        colList.append( Column( "HTS_USED", "0", "S", False ) )                            
                        hts_id = insert_row( cursor, "HLT_TRIGGER_SIGNATURE", "HTS", colList, True )

                        # set relation with trigger chain via N:M table
                        colList = []
                        colList.append( Column( "HTC2TS_TRIGGER_CHAIN_ID", "%i" % htc_id, "I" ) )
                        colList.append( Column( "HTC2TS_TRIGGER_SIGNATURE_ID", "%i" % hts_id, "I" ) )
                        colList.append( Column( "HTC2TS_SIGNATURE_COUNTER", "%i" % sig.counter, "I" ) )
                        colList.append( Column( "HTC2TS_USERNAME", username, "S", False ) )
                        colList.append( Column( "HTC2TS_MODIFIED_TIME", timestamp, "S", False) )
                        colList.append( Column( "HTC2TS_USED", "0", "S", False ) )                            
                        htc2ts_id = insert_row( cursor, "HLT_TC_TO_TS", "HTC2TS", colList )

                        EXISTS = True                            

                    # set relation of trigger element with trigger signature via N:M table
                    colList = []
                    colList.append( Column( "HTS2TE_TRIGGER_SIGNATURE_ID", "%i" % hts_id, "I" ) )
                    colList.append( Column( "HTS2TE_TRIGGER_ELEMENT_ID", "%i" % hte_id, "I" ) )
                    colList.append( Column( "HTS2TE_ELEMENT_COUNTER", "%i" % hte_id, "I" ) )
                    colList.append( Column( "HTS2TE_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTS2TE_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTS2TE_USED", "0", "S", False ) )                        
                    hts2te_id = insert_row( cursor, "HLT_TS_TO_TE", "HTS2TE", colList )

                # add algorithms in sequence
                for algo in seq.algoList:

                    # fill components
                    colList = []
                    colList.append( Column( "HCP_NAME", algo.classname, "S" ) )
                    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
                    colList.append( Column( "HCP_ALIAS", algo.instancename, "S" ) )
                    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
                    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
                    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
                    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
                    colList.append( Column( "HCP_USED", "0", "S", False ) )
                    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )

                    # set relation between components and trigger elements via N:M table
                    colList = []
                    colList.append( Column( "HTE2CP_TRIGGER_ELEMENT_ID", "%i" % hte_id, "I" ) )
                    colList.append( Column( "HTE2CP_COMPONENT_ID", "%i" % hcp_id, "I" ) )
                    colList.append( Column( "HTE2CP_SETUP_ID", "%i" % hst_id, "I" ) )
                    colList.append( Column( "HTE2CP_ALGORITHM_COUNTER", "1", "I" ) )
                    colList.append( Column( "HTE2CP_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTE2CP_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTE2CP_USED", "0", "S", False ) )                        
                    hte2cp_id = insert_row( cursor, "HLT_TE_TO_CP", "HTE2CP", colList )
                    
                # add input trigger elements (these have no associations in htl_te_to_te table
                for tein in seq.teInList:
                    
                    # add trigger element
                    colList = []
                    colList.append( Column( "HTE_NAME", tein, "S" ) )
                    colList.append( Column( "HTE_VERSION", "%i" % 1, "I", False ) )
                    colList.append( Column( "HTE_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTE_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTE_USED", "0", "S", False ) )                    
                    hte_id = insert_row( cursor, "HLT_TRIGGER_ELEMENT", "HTE", colList )
                    



# -----------------------------------------------------------------------------------
# populate the db from xml file
#
def populateDBTest( cursor, menu, htm_id, htm_name, hps_id, hps_name ):

    # -----------------------------------------------------
    # define global variables
    username       = "altrig"
    timestamp      = "NULL"
    setupname      = "dummy"

    # -----------------------------------------------------
    # execute some insertions that are required because the release part is missing
    
    # setup
    colList = []
    colList.append( Column( "HST_NAME", setupname, "S" ) )
    colList.append( Column( "HST_VERSION", "1", "F", False ) )
    colList.append( Column( "HST_USERNAME", username, "S", False ) )
    colList.append( Column( "HST_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HST_USED", "0", "S", False ) )
    hst_id = insert_row( cursor, "HLT_SETUP", "HST", colList )

    # components
    colList = []
    colList.append( Column( "HCP_NAME", "dummy", "S" ) )
    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
    colList.append( Column( "HCP_ALIAS", "dummy", "S" ) )
    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HCP_USED", "0", "S", False ) )
    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )

    colList = []
    colList.append( Column( "HCP_NAME", "LVL1", "S" ) )
    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
    colList.append( Column( "HCP_ALIAS", "LVL1", "S" ) )
    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HCP_USED", "0", "S", False ) )
    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )
    # -----------------------------------------------------
    
    # retrieve prescale set corresponding to the wanted trigger menu
    colList = []
    colList.append( Column( "HPS_ID", hps_id, "I" ) )
    colList.append( Column( "HPS_TRIGGER_MENU_ID", htm_id, "I" ) )
    hps_id = check_row( cursor, "HLT_PRESCALE_SET", "HPS", colList, "HPS_VERSION"  )

    # prescale set doesn't yet exist
    if hps_id < 0:
        # need a new menu 
        colList = []
        colList.append( Column( "HTM_NAME", htm_name, "S" ) )
        colList.append( Column( "HTM_VERSION", "1", "F", False ) )
        colList.append( Column( "HTM_PHASE", "lumi", "S" ) )
        colList.append( Column( "HTM_SHIFT_SAFE", "0", "S" ) )
        colList.append( Column( "HTM_USERNAME", username, "S", False ) )
        colList.append( Column( "HTM_MODIFIED_TIME", timestamp, "S", False ) )
        colList.append( Column( "HTM_USED", "0", "S", False ) )
        htm_id = insert_row( cursor, "HLT_TRIGGER_MENU", "HTM", colList, True )
                
        # now insert the prescale set
        colList = []
        colList.append( Column( "HPS_NAME", menu.prescale_set_name, "S" ) )
        colList.append( Column( "HPS_VERSION", menu.prescale_set_version, "F", False ) )
        colList.append( Column( "HPS_TRIGGER_MENU_ID", htm_id, "I" ) )
        colList.append( Column( "HPS_USERNAME", username, "S", False ) )
        colList.append( Column( "HPS_MODIFIED_TIME", timestamp, "S", False ) )
        colList.append( Column( "HPS_USED", "0", "S", False ) )
        hps_id = insert_row( cursor, "HLT_PRESCALE_SET", "HPS", colList )
        
    # interate over chains and retrieve prescales
    for chain in menu.chainList:

        # check prescales
        colList = []
        colList.append( Column( "HPR_PRESCALE_SET_ID", "%i" % hps_id, "I" ) )
        colList.append( Column( "HPR_CHAIN_COUNTER", "%i" % chain.counter, "I" ) )  # talk to Andreas
        colList.append( Column( "HPR_PRESCALE", "%i" % chain.prescale, "I" ) )
        colList.append( Column( "HPR_USERNAME", username, "S", False ) )
        colList.append( Column( "HPR_MODIFIED_TIME", timestamp, "S", False) )
        colList.append( Column( "HPR_USED", "0", "S", False ) )
        hpr_id = insert_row( cursor, "HLT_PRESCALE", "HPR", colList )
        

    # my test output
    if 1==0:
        print "=======================================\nMy Testoutput\n"
        for chain in menu.chainList:
            print "----new chain"
            for sig in chain.sigList:
                for seq in sig.seqList:
                    for te in seq.teInList:
                        print "i:", te;
                    print "o:", seq.teOutElem







    # iterate over chains and fill chain part
    for chain in menu.chainList:

        # add trigger chain
        colList = []
        colList.append( Column( "HTC_NAME", chain.name , "S" ) )
        colList.append( Column( "HTC_VERSION", "1", "I" ) )
        colList.append( Column( "HTC_LOWER_CHAIN_NAME", chain.lower_chain , "S" ) )
        colList.append( Column( "HTC_L2_OR_EF", chain.level , "S" ) )
        colList.append( Column( "HTC_PASS_THROUGH_RATE", 1.0 , "S" ) )
        colList.append( Column( "HTC_USERNAME", username, "S", False ) )
        colList.append( Column( "HTC_MODIFIED_TIME", timestamp, "S", False) )
        colList.append( Column( "HTC_USED", "0", "S", False ) )
        htc_id = insert_row( cursor, "HLT_TRIGGER_CHAIN", "HTC", colList )
        
        # set relation with trigger menu via N:M table
        colList = []
        colList.append( Column( "HTM2TC_TRIGGER_MENU_ID", "%i" % htm_id, "I" ) )
        colList.append( Column( "HTM2TC_TRIGGER_CHAIN_ID", "%i" % htc_id, "I" ) )
        colList.append( Column( "HTM2TC_CHAIN_COUNTER", "%i" % chain.counter, "I" ) )
        colList.append( Column( "HTM2TC_USERNAME", username, "S", False ) )
        colList.append( Column( "HTM2TC_MODIFIED_TIME", timestamp, "S", False) )
        colList.append( Column( "HTM2TC_USED", "0", "S", False ) )
        htm2tc_id = insert_row( cursor, "HLT_TM_TO_TC", "HTM2TC", colList )

        # loop over signatures in chain
        for sig in chain.sigList:

            # loop over sequences in signature
            for iseq, seq in enumerate(sig.seqList):


                # add input trigger elements (these have no associations in htl_te_to_te table
                for tein in seq.teInList:
                    # add trigger element
                    colList = []
                    colList.append( Column( "HTE_NAME", tein, "S" ) )
                    colList.append( Column( "HTE_VERSION", "%i" % 1, "I", False ) )
                    colList.append( Column( "HTE_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTE_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTE_USED", "0", "S", False ) )                    
                    hte_id = insert_row( cursor, "HLT_TRIGGER_ELEMENT", "HTE", colList )


                # add output trigger elements
                # add trigger element
                colList = []
                colList.append( Column( "HTE_NAME", seq.teOutElem, "S" ) )
                colList.append( Column( "HTE_VERSION", "%i" % 1, "I", False ) )
                colList.append( Column( "HTE_USERNAME", username, "S", False ) )
                colList.append( Column( "HTE_MODIFIED_TIME", timestamp, "S", False) )
                colList.append( Column( "HTE_USED", "0", "S", False ) )                    
                hte_id = insert_row( cursor, "HLT_TRIGGER_ELEMENT", "HTE", colList )

                # loop over input trigger elements and attribute to output te
                for tein in seq.teInList:

                    colList = []
                    colList.append( Column( "HTE2TE_TRIGGER_ELEMENT_ID", "%i" % hte_id, "I" ) )
                    colList.append( Column( "HTE2TE_TRIGGER_ELEMENT_INP_ID", tein, "S" ) )
                    colList.append( Column( "HTE2TE_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTE2TE_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTE2TE_USED", "0", "S", False ) )
                    hte2te_id = insert_row( cursor, "HLT_TE_TO_TE", "HTE2TE", colList )

                # set combined link between trigger element, signature and chain
                # some manual craft is necessary here ...

                columns     = "TS.HTS_ID"
                tables      = "HLT_TC_TO_TS AS TC2TS, HLT_TRIGGER_SIGNATURE AS TS, HLT_TS_TO_TE AS TS2TE"
                conditions  = "TS.HTS_ID = TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID "
                conditions  = "TS.HTS_ID = TS2TE.HTS2TE_TRIGGER_SIGNATURE_ID "
                #conditions += "AND TC2TS.HTC2TS_TRIGGER_CHAIN_ID = %i " % htc_id
                conditions += "AND TS2TE.HTS2TE_TRIGGER_ELEMENT_ID = %i " % hte_id
                #conditions += "AND TC2TS.HTC2TS_SIGNATURE_COUNTER = %i" % sig.counter
                query       = "SELECT DISTINCT %s FROM %s WHERE %s;" % (columns, tables, conditions)
                cursor.execute( query )
                result      = cursor.fetchall()


                # add signature (forced insert if it does not exists)
                if len(result)==0:
                    colList = []
                    colList.append( Column( "HTS_LOGIC", "%i" % sig.logic, "I" ) )
                    colList.append( Column( "HTS_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTS_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTS_USED", "0", "S", False ) )
                    hts_id = insert_row( cursor, "HLT_TRIGGER_SIGNATURE", "HTS", colList, True )

                    # set relation with trigger chain via N:M table
                    colList = []
                    colList.append( Column( "HTC2TS_TRIGGER_CHAIN_ID", "%i" % htc_id, "I" ) )
                    colList.append( Column( "HTC2TS_TRIGGER_SIGNATURE_ID", "%i" % hts_id, "I" ) )
                    colList.append( Column( "HTC2TS_SIGNATURE_COUNTER", "%i" % sig.counter, "I" ) )
                    colList.append( Column( "HTC2TS_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTC2TS_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTC2TS_USED", "0", "S", False ) )                            
                    htc2ts_id = insert_row( cursor, "HLT_TC_TO_TS", "HTC2TS", colList )
                else:
                    hts_id = result[0]

                # set relation of trigger element with trigger signature via N:M table
                colList = []
                colList.append( Column( "HTS2TE_TRIGGER_SIGNATURE_ID", "%i" % hts_id, "I" ) )
                colList.append( Column( "HTS2TE_TRIGGER_ELEMENT_ID", "%i" % hte_id, "I" ) )
                colList.append( Column( "HTS2TE_ELEMENT_COUNTER", "%i" % iseq, "I" ) )
                #colList.append( Column( "HTS2TE_ELEMENT_COUNTER", "%i" % hte_id, "I" ) )
                colList.append( Column( "HTS2TE_USERNAME", username, "S", False ) )
                colList.append( Column( "HTS2TE_MODIFIED_TIME", timestamp, "S", False) )
                colList.append( Column( "HTS2TE_USED", "0", "S", False ) )                        
                hts2te_id = insert_row( cursor, "HLT_TS_TO_TE", "HTS2TE", colList )



                # add algorithms in sequence
                for algo in seq.algoList:

                    # fill components
                    colList = []
                    colList.append( Column( "HCP_NAME", algo.classname, "S" ) )
                    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
                    colList.append( Column( "HCP_ALIAS", algo.instancename, "S" ) )
                    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
                    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
                    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
                    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
                    colList.append( Column( "HCP_USED", "0", "S", False ) )
                    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )

                    # set relation between components and trigger elements via N:M table
                    colList = []
                    colList.append( Column( "HTE2CP_TRIGGER_ELEMENT_ID", "%i" % hte_id, "I" ) )
                    colList.append( Column( "HTE2CP_COMPONENT_ID", "%i" % hcp_id, "I" ) )
                    colList.append( Column( "HTE2CP_SETUP_ID", "%i" % hst_id, "I" ) )
                    colList.append( Column( "HTE2CP_ALGORITHM_COUNTER", "1", "I" ) )
                    colList.append( Column( "HTE2CP_USERNAME", username, "S", False ) )
                    colList.append( Column( "HTE2CP_MODIFIED_TIME", timestamp, "S", False) )
                    colList.append( Column( "HTE2CP_USED", "0", "S", False ) )                        
                    hte2cp_id = insert_row( cursor, "HLT_TE_TO_CP", "HTE2CP", colList )


                    

def NewPopDB( cursor, menu ):

    # -----------------------------------------------------
    # define global variables
    username       = "altrig"
    timestamp      = "NULL"
    setupname      = "dummy"

    # -----------------------------------------------------
    # execute some insertions that are required because the release part is missing
    
    # setup
    colList = []
    colList.append( Column( "HST_NAME", setupname, "S" ) )
    colList.append( Column( "HST_VERSION", "1", "F", False ) )
    colList.append( Column( "HST_USERNAME", username, "S", False ) )
    colList.append( Column( "HST_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HST_USED", "0", "S", False ) )
    hst_id = insert_row( cursor, "HLT_SETUP", "HST", colList )

    # components
    colList = []
    colList.append( Column( "HCP_NAME", "dummy", "S" ) )
    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
    colList.append( Column( "HCP_ALIAS", "dummy", "S" ) )
    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HCP_USED", "0", "S", False ) )
    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )

    colList = []
    colList.append( Column( "HCP_NAME", "LVL1", "S" ) )
    colList.append( Column( "HCP_VERSION", "1", "F", False ) )
    colList.append( Column( "HCP_ALIAS", "LVL1", "S" ) )
    colList.append( Column( "HCP_TOPALG", "1", "I" ) )
    colList.append( Column( "HCP_FLAG", "flag", "S" ) )
    colList.append( Column( "HCP_USERNAME", username, "S", False ) )
    colList.append( Column( "HCP_MODIFIED_TIME", timestamp, "S", False ) )
    colList.append( Column( "HCP_USED", "0", "S", False ) )
    hcp_id = insert_row( cursor, "HLT_COMPONENT", "HCP", colList )
    # -----------------------------------------------------
    
    
