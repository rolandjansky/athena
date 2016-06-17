#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: hltdumpxmllib.py,v 1.5 2007-07-13 17:27:12 stelzer Exp $
#
# Andreas Hoecker, 12-Nov-2005
#
# helper library
# ------------------------------------------------------------------------
#
import sys # for exiting
import time # for time accounting
import socket # for host information
#import MySQLdb # MySQL-python 1.2.0 modules
import xml.dom.minidom # xml parser
import hltutils
import sqlutils
from xml.dom.minidom import Document
from xml.dom.minidom import DocumentType

# sort
op = lambda o,f,p='',t=type:t(o) in [t(()),t(''),t([])] \
     and eval("lambda o,l,t,p:[l.extend(o),l.%s(%s),%s(l)][2]" \
              % (f,['p',''][f=='reverse' or not p],[["''.join","tuple"][t(o)==t(())],''][t(o)==t([])]))(o,[],t,p)

# remove duplicated records (cannot use dictionary since destroys list sorting)
def unique( li ):
    u = []
    for i in xrange(0,len(li)):
        f = False
        for p in xrange(i+1,len(li)):
            if li[i] == li[p]:  f = True
        if not f: u.append( li[i] )    
    return u

# data structs
class IDN:
    id   = 0    
    name = ""
    aux  = ""

def append_IDN( id, name, aux ):
    x      = IDN()
    x.id   = id
    x.name = name
    x.aux  = aux
    return x

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

# get available trigger menus in db
def get_menu( cursor ):

    columns    = "TM.HTM_NAME, TM.HTM_VERSION, TM.HTM_ID, PS.HPS_NAME"
    tables     = "HLT_TRIGGER_MENU AS TM, HLT_PRESCALE_SET AS PS"
    conditions = "PS.HPS_TRIGGER_MENU_ID = TM.HTM_ID"
    order      = "TM.HTM_VERSION ASC"
    query      = "SELECT DISTINCT %s FROM %s WHERE %s ORDER BY %s;" % (columns, tables, conditions, order)    

    cursor.execute( query )
    result = cursor.fetchall()

    return result

def get_sequence_record(which):
    """Define the sequence record"""

    theKeys = ( "te.name"       ,
                "te.inp.name"   ,
                "te.inp.type"   ,
                "te.inp.counter",
                "cp.name"       ,
                "cp.alias"      ,
                "cp.counter"    )


    record    = { "te.name"        :  "TE.HTE_NAME"                          ,
                  "te.inp.name"    :  "TE2TE.HTE2TE_TE_INP_ID"  ,
                  "te.inp.type"    :  "TE2TE.HTE2TE_TE_INP_TYPE",
                  "te.inp.counter" :  "TE2TE.HTE2TE_TE_COUNTER" ,
                  "cp.name"        :  "CP.HCP_NAME"                          ,
                  "cp.alias"       :  "CP.HCP_ALIAS"                         ,
                  "cp.counter"     :  "TE2CP.HTE2CP_ALGORITHM_COUNTER"       }

    if which=="keys":
        return theKeys
    if which=="string":
        return [record[x] for x in theKeys]
    elif which=="index":
        return dict(zip( theKeys, range(len(theKeys))))


# get the sequence information
def get_sequence( cursor, htm_id ):

    columns    = ( " TE.HTE_ID,"
                   " TE.HTE_NAME" )
    tables     = ( " HLT_TM_TO_TC AS M2C,"
                   " HLT_TC_TO_TS AS C2S,"
                   " HLT_TS_TO_TE AS S2TE,"
                   " HLT_TRIGGER_ELEMENT AS TE"
                   )
    conditions = ( " '%i' = M2C.HTM2TC_TRIGGER_MENU_ID"
                   " AND C2S.HTC2TS_TRIGGER_CHAIN_ID     = M2C.HTM2TC_TRIGGER_CHAIN_ID"
                   " AND C2S.HTC2TS_TRIGGER_SIGNATURE_ID = S2TE.HTS2TE_TRIGGER_SIGNATURE_ID"
                   " AND TE.HTE_ID = S2TE.HTS2TE_TRIGGER_ELEMENT_ID"
                   % htm_id )
    order      = "TE.HTE_ID ASC"
    query      = "SELECT DISTINCT %s FROM %s WHERE %s ORDER BY %s " % (columns, tables, conditions, order)
    
    sqlutils._run_single( cursor, query )
    result = cursor.fetchall()

    teIDstring = str(result[0][0])
    for line in result[1:]:
        teIDstring += "," + str(line[0])

    seqItems = get_sequence_record("string");
    
    columns = ", ".join(seqItems)

    tables = ( " HLT_TRIGGER_ELEMENT AS TE,"
               " HLT_TE_TO_CP AS TE2CP,"
               " HLT_COMPONENT AS CP,"
               " HLT_TE_TO_TE AS TE2TE" )
    conditions = ( " TE.HTE_ID in (%s)"
                   " AND TE.HTE_ID = TE2CP.HTE2CP_TRIGGER_ELEMENT_ID"
                   " AND CP.HCP_ID = TE2CP.HTE2CP_COMPONENT_ID"
                   " AND TE.HTE_ID = TE2TE.HTE2TE_TE_ID"
                   % teIDstring )
    order = "TE.HTE_NAME ASC, TE2CP.HTE2CP_ALGORITHM_COUNTER ASC, TE2TE.HTE2TE_TE_COUNTER ASC"
    query = "SELECT %s FROM %s WHERE %s ORDER BY %s" % (columns, tables, conditions, order)

    sqlutils._run_single( cursor, query )

    result = cursor.fetchall()
    return result



def get_chain_record(which):

    theKeys = (   "menu.name"    ,
                  "menu.version" ,
                  "pres.name"    ,
                  "pres.version" ,
                  "chain.id"     ,
                  "chain.name"   ,
                  "chain.lowname",
                  "chain.level"  ,
                  "chain.counter",
                  "chain.pres"   ,
                  "chain.pass"   )


    record    = { "menu.name"     :  "TM.HTM_NAME"                    ,
                  "menu.version"  :  "TM.HTM_VERSION"                 ,
                  "pres.name"     :  "PS.HPS_NAME"                    ,
                  "pres.version"  :  "PS.HPS_VERSION"                 ,
                  "chain.id"      :  "TC.HTC_ID"                      ,
                  "chain.name"    :  "TC.HTC_NAME"                    ,
                  "chain.lowname" :  "TC.HTC_LOWER_CHAIN_NAME"        ,
                  "chain.level"   :  "TC.HTC_L2_OR_EF"                ,
                  "chain.counter" :  "TC.HTC_CHAIN_COUNTER"           ,
                  "chain.pres"    :  "PR.HPR_PRESCALE"                ,
                  "chain.pass"    :  "TC.HTC_PASS_THROUGH_RATE"       }  

    if which=="keys":
        return theKeys
    if which=="string":
        return [record[x] for x in theKeys]
    elif which=="index":
        return dict(zip( theKeys, range(len(theKeys))))

    
def get_chain( cursor, htm_id, hps_id ):

    sigItems = get_chain_record("string");
    
    columns = ", ".join(sigItems)
    
    tables     = ( " HLT_TRIGGER_MENU AS TM,"
                   " HLT_TM_TO_TC AS M2C,"
                   " HLT_TRIGGER_CHAIN AS TC,"
                   " HLT_PRESCALE_SET AS PS, "
                   " HLT_PRESCALE AS PR" )
    conditions = ( " TM.HTM_ID = '%i' AND"
                   " PS.HPS_ID = '%i' AND"
                   " TM.HTM_ID = M2C.HTM2TC_TRIGGER_MENU_ID AND TC.HTC_ID = M2C.HTM2TC_TRIGGER_CHAIN_ID AND"
                   " TM.HTM_ID = PS.HPS_TRIGGER_MENU_ID AND PS.HPS_ID = PR.HPR_PRESCALE_SET_ID AND"
                   " PR.HPR_CHAIN_COUNTER = TC.HTC_CHAIN_COUNTER AND"
                   " PR.HPR_L2_OR_EF = TC.HTC_L2_OR_EF\n"
                   % (htm_id, hps_id) )
    order      = "TC.HTC_L2_OR_EF DESC, TC.HTC_CHAIN_COUNTER"

    query      = "SELECT %s FROM %s WHERE %s ORDER BY %s" % (columns, tables, conditions, order)

    sqlutils._run_single( cursor, query )
    return cursor.fetchall()



def get_signature_record(which):

    theKeys = (   "chain.level"  ,
                  "chain.counter",
                  "sign.counter" ,
                  "sign.logic"   ,
                  "te.name"      ,
                  "te.counter"   )


    record    = { "chain.level"   :  "TC.HTC_L2_OR_EF"                ,
                  "chain.counter" :  "TC.HTC_CHAIN_COUNTER"           ,
                  "sign.counter"  :  "TC2TS.HTC2TS_SIGNATURE_COUNTER" ,
                  "sign.logic"    :  "TS.HTS_LOGIC"                   ,
                  "te.name"       :  "TE.HTE_NAME"                    ,
                  "te.counter"    :  "S2TE.HTS2TE_ELEMENT_COUNTER"    }  

    if which=="keys":
        return theKeys
    if which=="string":
        return [record[x] for x in theKeys]
    elif which=="index":
        return dict(zip( theKeys, range(len(theKeys))))

    
def get_signature( cursor, htm_id, hps_id ):

    sigItems = get_signature_record("string");
    
    columns = ", \n\t".join(sigItems)
    
    tables     = ( "\t HLT_TRIGGER_MENU AS TM, \n"
                   "\t HLT_TM_TO_TC AS M2C, \n"
                   "\t HLT_TRIGGER_CHAIN AS TC, \n"
                   "\t HLT_TC_TO_TS AS TC2TS, \n"
                   "\t HLT_TRIGGER_SIGNATURE AS TS, \n"
                   "\t HLT_TS_TO_TE AS S2TE, \n"
                   "\t HLT_TRIGGER_ELEMENT AS TE, \n"
                   "\t HLT_PRESCALE_SET AS PS,  \n"
                   "\t HLT_PRESCALE AS PR \n" )
    conditions = ( "\t TM.HTM_ID = '%i' AND \n"
                   "\t PS.HPS_ID = '%i' AND \n"
                   "\t TM.HTM_ID = M2C.HTM2TC_TRIGGER_MENU_ID AND TC.HTC_ID = M2C.HTM2TC_TRIGGER_CHAIN_ID AND \n"
                   "\t TC.HTC_ID = TC2TS.HTC2TS_TRIGGER_CHAIN_ID AND TS.HTS_ID = TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID AND \n"
                   "\t TS.HTS_ID = S2TE.HTS2TE_TRIGGER_SIGNATURE_ID AND TE.HTE_ID = S2TE.HTS2TE_TRIGGER_ELEMENT_ID AND \n"
                   "\t TM.HTM_ID = PS.HPS_TRIGGER_MENU_ID AND PS.HPS_ID = PR.HPR_PRESCALE_SET_ID AND \n"
                   "\t PR.HPR_CHAIN_COUNTER = TC.HTC_CHAIN_COUNTER AND \n"
                   "\t PR.HPR_L2_OR_EF = TC.HTC_L2_OR_EF\n"
                   % (htm_id, hps_id) )
    order      = "TC.HTC_L2_OR_EF DESC, TC.HTC_CHAIN_COUNTER, TC2TS.HTC2TS_SIGNATURE_COUNTER ASC, S2TE.HTS2TE_ELEMENT_COUNTER ASC"

    query      = "SELECT %s \n FROM %s \n WHERE %s \n ORDER BY %s" % (columns, tables, conditions, order)

    sqlutils._run_single( cursor, query )
    return cursor.fetchall()


def get_streamtag_record(which):

    theKeys = (   "chain.counter",
                  "chain.level"  ,
                  "stream.name"  ,
                  "stream.type"  ,
                  "stream.obeylb",
                  "stream.pres"  )  

    record    = { "chain.counter" :  "TC.HTC_CHAIN_COUNTER" ,             
                  "chain.level"   :  "TC.HTC_L2_OR_EF"      ,             
                  "stream.name"   :  "TR.HTR_NAME"          ,             
                  "stream.type"   :  "TR.HTR_TYPE"          ,             
                  "stream.obeylb" :  "TR.HTR_OBEYLB"        ,             
                  "stream.pres"   :  "C2R.HTC2TR_TRIGGER_STREAM_PRESCALE"}

    if which=="keys":
        return theKeys
    if which=="string":
        return [record[x] for x in theKeys]
    elif which=="index":
        return dict(zip( theKeys, range(len(theKeys))))

def get_streamtag( cursor, htm_id ):

    streamItems = get_streamtag_record("string");
    
    columns = ", \n\t".join(streamItems)


    tables     = ( "\t HLT_TM_TO_TC AS M2C, \n"
                   "\t HLT_TRIGGER_CHAIN AS TC, \n"
                   "\t HLT_TC_TO_TR AS C2R, \n" 
                   "\t HLT_TRIGGER_STREAM AS TR \n" )
    conditions = ( "\t '%i' = M2C.HTM2TC_TRIGGER_MENU_ID AND \n"
                   "\t M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID AND \n"
                   "\t TC.HTC_ID = C2R.HTC2TR_TRIGGER_CHAIN_ID AND \n"
                   "\t C2R.HTC2TR_TRIGGER_STREAM_ID = TR.HTR_ID \n"
                   % htm_id )
    order      = "TC.HTC_L2_OR_EF DESC, TC.HTC_CHAIN_COUNTER ASC, C2R.HTC2TR_TRIGGER_STREAM_PRESCALE ASC, TR.HTR_NAME ASC"

    query      = "SELECT %s \n FROM %s \n WHERE %s \n ORDER BY %s" % (columns, tables, conditions, order)

    sqlutils._run_single( cursor, query )
    return cursor.fetchall()


def get_triggerbit_record(which):

    theKeys = (   "chain.counter",
                  "chain.level"  ,
                  "triggerbit")

    record    = { "chain.counter" :  "TC.HTC_CHAIN_COUNTER" ,
                  "chain.level"   :  "TC.HTC_L2_OR_EF"      ,
                  "triggerbit"    :  "TT.HTT_TYPEBIT"       }

    if which=="keys":
        return theKeys
    if which=="string":
        return [record[x] for x in theKeys]
    elif which=="index":
        return dict(zip( theKeys, range(len(theKeys))))

def get_triggerbit( cursor, htm_id ):

    triggerbitItems = get_triggerbit_record("string");
    
    columns = ", \n\t".join(triggerbitItems)

    tables     = ( "\t HLT_TM_TO_TC AS M2C, \n"
                   "\t HLT_TRIGGER_CHAIN AS TC, \n"
                   "\t HLT_TRIGGER_TYPE AS TT \n" )
    conditions = ( "\t '%i' = M2C.HTM2TC_TRIGGER_MENU_ID AND \n"
                   "\t M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID AND \n"
                   "\t TT.HTT_TRIGGER_CHAIN_ID = TC.HTC_ID"
                   % htm_id )
    order      = "TC.HTC_L2_OR_EF DESC, TC.HTC_CHAIN_COUNTER ASC, TT.HTT_TYPEBIT ASC"

    query      = "SELECT %s \n FROM %s \n WHERE %s \n ORDER BY %s" % (columns, tables, conditions, order)


    sqlutils._run_single( cursor, query )
    return cursor.fetchall()


def get_triggergroup_record(which):

    theKeys = (   "chain.counter",
                  "chain.level"  ,
                  "group.name")

    record    = { "chain.counter" :  "TC.HTC_CHAIN_COUNTER" ,
                  "chain.level"   :  "TC.HTC_L2_OR_EF"      ,
                  "group.name"    :  "TG.HTG_NAME"          }

    if which=="keys":
        return theKeys
    if which=="string":
        return [record[x] for x in theKeys]
    elif which=="index":
        return dict(zip( theKeys, range(len(theKeys))))

def get_triggergroup( cursor, htm_id ):

    triggergroupItems = get_triggergroup_record("string");
    
    columns = ", \n\t".join(triggergroupItems)

    tables     = ( "\t HLT_TM_TO_TC AS M2C, \n"
                   "\t HLT_TRIGGER_CHAIN AS TC, \n"
                   "\t HLT_TRIGGER_GROUP AS TG \n" )
    conditions = ( "\t '%i' = M2C.HTM2TC_TRIGGER_MENU_ID AND \n"
                   "\t M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID AND \n"
                   "\t TG.HTG_TRIGGER_CHAIN_ID = TC.HTC_ID"
                   % htm_id )
    order      = "TC.HTC_L2_OR_EF DESC, TC.HTC_CHAIN_COUNTER ASC, TG.HTG_NAME ASC"

    query      = "SELECT %s \n FROM %s \n WHERE %s \n ORDER BY %s" % (columns, tables, conditions, order)


    sqlutils._run_single( cursor, query )
    result = cursor.fetchall()
    print result
    return result



def createSequenceDoc( sequences ):

    # create dom document
    doc = Document()

    # comment fields
    C = []
    C.append( doc.createComment( "xml script created %s on %s" % (time.asctime(), socket.gethostname()) ) )
    for c in C: doc.appendChild( c )

    # document type
    doctype = DocumentType( "SEQUENCE_LIST" );
    doctype.systemId = "sequencelistnewformat.dtd"
    doc.appendChild( doctype );
 
    # sequence list
    seqList = doc.createElement( "SEQUENCE_LIST" )
    seqList.setAttribute( "hlt_setup.name", "dummy" )
    seqList.setAttribute( "hlt_setup.version", "%i" % 1 )
    doc.appendChild( seqList )

    # sequences

    # algorithms
    algoDict = {}
    for record in sequences:
        sequence = dict(zip(get_sequence_record("keys"),record))
        if sequence["te.inp.counter"] > 1:
            continue
        algSpec = "%s/%s" % ( sequence["cp.name"], sequence["cp.alias"] )
        if not sequence["te.name"] in algoDict:
            algoDict[sequence["te.name"]] = {}
        algoDict[sequence["te.name"]][sequence["cp.counter"]] = algSpec


    # input TEs
    inpTEdict = {}
    for record in sequences:
        sequence = dict(zip(get_sequence_record("keys"),record))
        outTE = str(sequence["te.name"]) # determines the sequence
        if sequence["te.inp.type"]=="input":
            inpTECount = int(sequence["te.inp.counter"])
        else:
            inpTECount = "topo"
        if not outTE in inpTEdict:
            inpTEdict[outTE] = {}
        if inpTECount in inpTEdict[outTE]:
            if inpTEdict[outTE][inpTECount] != sequence["te.inp.name"]:
                print "*** error: at position %s two different input TEs '%s' and '%s' were encountered => abort ***" \
                      % (str(inpTECount),sequence["te.inp.name"],inpTEdict[outTE][inpTECount])
                sys.exit(1)
        inpTEdict[outTE][inpTECount] = sequence["te.inp.name"]

            

    # put the SEQUENCE together
    for outTE in algoDict:

        thisTEsInpTEs = inpTEdict[outTE]
        hasTopoStartFrom = "topo" in thisTEsInpTEs
        counters = thisTEsInpTEs.keys()
        if hasTopoStartFrom:
            counters.remove("topo")
        counters.sort()
        inpTEStringDict = inpTEdict[outTE][counters[0]]
        for c in counters[1:]:
            inpTEStringDict += " " + inpTEdict[outTE][c]

        thisTEsAlgs = algoDict[outTE]
        algCounters = thisTEsAlgs.keys()
        algCounters.sort()
        algoString = " ".join([thisTEsAlgs[x] for x in algCounters])
        seq = doc.createElement( "SEQUENCE" )
        seq.setAttribute( "input", inpTEStringDict )
        if hasTopoStartFrom:
            seq.setAttribute( "topo_start_from", thisTEsInpTEs["topo"] )
        seq.setAttribute( "algorithm", algoString )
        seq.setAttribute( "output",    outTE )
        seqList.appendChild( seq )

    return doc

def createSignatureDoc( chains, signatures, streamtags, triggerbits, triggergroups ):

    rel13_tag = True
    if len(streamtags) == 0 and len(triggerbits) == 0:
      rel13_tag = False
      
    # create dom document
    doc = Document()

    doctype = DocumentType( "CHAIN_LIST" );
    doctype.systemId = "chainlist.dtd"
    doc.appendChild( doctype );

    # comment fields
    C = []
    C.append( doc.createComment( "xml script created %s on %s" % (time.asctime(), socket.gethostname()) ) )
    for c in C: doc.appendChild( c )

    # chain list
    firstChain = dict(zip(get_chain_record("keys"),chains[0]))
    chainList = doc.createElement( "CHAIN_LIST" )
    chainList.setAttribute( "hlt_master_table.name",        firstChain["menu.name"] )
    chainList.setAttribute( "hlt_master_table.version", str(firstChain["menu.version"]) )
    chainList.setAttribute( "hlt_trigger_menu.name",        firstChain["menu.name"])
    chainList.setAttribute( "hlt_trigger_menu.version", str(firstChain["menu.name"]) )
    chainList.setAttribute( "hlt_prescale_set.name",        firstChain["pres.name"] )
    chainList.setAttribute( "hlt_prescale_set.version", str(firstChain["pres.version"]) )
    doc.appendChild( chainList )

    # chains
    prevLevel = ""
    prevChainCounter = 0
    prevSigCounter = 0
    prevTE = ""
    streamTagRecords = []
    isFirstChain = True

    streamtagDict = {}

    for record in streamtags:
        stream = dict(zip(get_streamtag_record("keys"),record))
        Lvl_ChCount = "%s_%i" % (str(stream["chain.level"]),int(stream["chain.counter"]) ) 
        streamtag = doc.createElement( "STREAMTAG" )
        streamtag.setAttribute( "obeyLB",   str(stream["stream.obeylb"]) )
        streamtag.setAttribute( "prescale", str(stream["stream.pres"  ]) )
        streamtag.setAttribute( "stream",   str(stream["stream.name"  ]) )
        streamtag.setAttribute( "type",     str(stream["stream.type"  ]) )
        if not Lvl_ChCount in streamtagDict:
            streamtagDict[Lvl_ChCount] = []
        streamtagDict[Lvl_ChCount].append(streamtag)


    triggerbitDict = {}
    for record in triggerbits:
        triggerbit = dict(zip(get_triggerbit_record("keys"),record))
        Lvl_ChCount = "%s_%i" % (str(triggerbit["chain.level"]),int(triggerbit["chain.counter"]) ) 
        triggertype = doc.createElement( "TRIGGERTYPE" )
        triggertype.setAttribute( "bit",   str(triggerbit["triggerbit"])     )
        if not Lvl_ChCount in triggerbitDict:
            triggerbitDict[Lvl_ChCount] = []
        triggerbitDict[Lvl_ChCount].append(triggertype)


    triggergroupDict = {}
    for record in triggergroups:
        print record
        group = dict(zip(get_triggergroup_record("keys"),record))
        Lvl_ChCount = "%s_%i" % (str(group["chain.level"]),int(group["chain.counter"]) ) 
        if not Lvl_ChCount in triggergroupDict:
            triggergroupDict[Lvl_ChCount] = []
        triggergroup = doc.createElement( "GROUP" )
        triggergroup.setAttribute( "name", str(group["group.name"])     )
        triggergroupDict[Lvl_ChCount].append(triggergroup)

    signatureDict = {}
    for record in signatures:
        signature = dict(zip(get_signature_record("keys"),record))
        Lvl_ChCount = "%s_%i" % (str(signature["chain.level"]),int(signature["chain.counter"]) ) 
        if not Lvl_ChCount in signatureDict:
            signatureDict[Lvl_ChCount] = {}
        if not signature["sign.counter"] in signatureDict[Lvl_ChCount]:
            sig = doc.createElement( "SIGNATURE" )
            sig.setAttribute( "signature_counter", str(signature["sign.counter"]) )
            sig.setAttribute( "logic",             str(signature["sign.logic"])      )
            signatureDict[Lvl_ChCount][signature["sign.counter"]] = sig
        # trigger elements in signature
        te = doc.createElement( "TRIGGERELEMENT" )
        te.setAttribute( "te_name", signature["te.name"] )
        signatureDict[Lvl_ChCount][signature["sign.counter"]].appendChild( te )


            
    # chains
    for record in chains:
        chain = dict(zip(get_chain_record("keys"),record))
        Lvl_ChCount = "%s_%i" % (str(chain["chain.level"]),int(chain["chain.counter"]) ) 
        
        ch = doc.createElement( "CHAIN" )
        chainList.appendChild( ch )
        ch.setAttribute( "chain_name",           chain["chain.name"] )
        ch.setAttribute( "chain_counter", "%i" % chain["chain.counter"] )
        ch.setAttribute( "lower_chain_name",     chain["chain.lowname"] )
        ch.setAttribute( "level",                chain["chain.level"] )
        ch.setAttribute( "prescale",      "%i" % chain["chain.pres"] )
        ch.setAttribute( "pass_through", "%i" %  chain["chain.pass"] )
        
        if rel13_tag:
            streamtagList = doc.createElement( "STREAMTAG_LIST" )
            ch.appendChild( streamtagList )
            if not Lvl_ChCount in streamtagDict:
                print "*** error: chain %s in level %s has no streamtag defined" \
                      % ( str(signature["chain.name"]), str(signature["chain.level"]) )
                print streamtagDict.keys()
                sys.exit(1)
            else:
                for streamtag in streamtagDict[Lvl_ChCount]:
                    streamtagList.appendChild(streamtag)


            triggertypeList = doc.createElement( "TRIGGERTYPE_LIST" )
            ch.appendChild( triggertypeList )
            if not Lvl_ChCount in triggerbitDict:
                print "*** error: chain %s in level %s has no triggertype defined" \
                      % ( str(signature["chain.name"]), str(signature["chain.level"]) )
                print triggerbitDict.keys()
                sys.exit(1)
            else:
                for triggertype in triggerbitDict[Lvl_ChCount]:
                    triggertypeList.appendChild(triggertype)


            triggergroupList = doc.createElement( "GROUP_LIST" )
            ch.appendChild( triggergroupList )
            if Lvl_ChCount in triggergroupDict:
                for triggergroup in triggergroupDict[Lvl_ChCount]:
                    triggergroupList.appendChild(triggergroup)



        # signatures in chain
        sigList = doc.createElement( "SIGNATURE_LIST" )
        ch.appendChild( sigList )
        if Lvl_ChCount in signatureDict:
            sigInChainDict = signatureDict[Lvl_ChCount]
            sigCounters = sigInChainDict.keys()
            sigCounters.sort();
            for sigC in sigCounters:
                sigList.appendChild( sigInChainDict[sigC] )
    

    return doc

