#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Andreas Hoecker, 10-Apr-2006
#
# hltmenu and auxiliary classes 
# ------------------------------------------------------------------------
#
import hltpopdblib
import hltdumpxmllib
import hltsetup
import sys
import time # for time accounting
import socket # for host information

from hltutils import dbVerbose, traceInsert, dumpSQL, dumpSQLFile, get_lastversion, format_dbstring, get_row_ids, get_new_index, get_column

import xml.dom.minidom
import sqlutils
from xml.dom.minidom import Document
from xml.dom.minidom import DocumentType

from sets import Set

from hltdbclasses import DBTable
from hltdbclasses import Column

#access with: globals()["dbverbose"] = False

# Python time format to modify the date and time for Oracle DB
time_format= 'YYYY-MM-DD HH24:MI:SS'

OldSteering = False

class HLTMenu(object):

    def __init__(self, *args):

        #print "-- in HLTMenu: number of arguments: %i" % len(args)
        
        if len(args) == 2 and isinstance(args[0], xml.dom.minidom.Node ):
            # create menu object from xml files
            (chainNode, sequenceNode) = args
            self.menu = hltpopdblib.parseXML( chainNode, sequenceNode )

        elif len(args) == 3:
            # create menu object from DB
            (cursor, htm_id, ps_id) = args

            # retrieve sequence and signature content tables
            sequences = hltdumpxmllib.get_sequence( cursor, htm_id )
            if len(sequences) == 0:
                print "Error while reading sequences: empty container returned ==> abort"
                sys.exit(1)

            chains = hltdumpxmllib.get_chain( cursor, htm_id, ps_id )
            if len(chains) == 0:
                print "Error while reading signatures: empty container returned ==> abort"
                sys.exit(1)

            signatures = hltdumpxmllib.get_signature( cursor, htm_id, ps_id )
            if len(signatures) == 0:
                print "Error while reading signatures: empty container returned ==> abort"
                sys.exit(1)

            streamtags = hltdumpxmllib.get_streamtag( cursor, htm_id )
            if len(streamtags) == 0:
                print "Error while reading streamtags: empty container returned old version ==> ignore"
                #print "Error while reading streamtags: empty container returned ==> abort"
                #sys.exit(1)

            triggerbits = hltdumpxmllib.get_triggerbit( cursor, htm_id )
            if len(triggerbits) == 0:
                print "Error while reading triggerbits: empty container returned old version ==> ignore"
                #print "Error while reading triggerbits: empty container returned ==> abort"
                #sys.exit(1)

            triggergroups = hltdumpxmllib.get_triggergroup( cursor, htm_id )
            if len(triggerbits) == 0:
                print "Error while reading triggerbits: empty container returned old version ==> ignore"
                #print "Error while reading triggerbits: empty container returned ==> abort"
                #sys.exit(1)

            # create dom documents from content tables
            seq_doc = hltdumpxmllib.createSequenceDoc( sequences  )
            sig_doc = hltdumpxmllib.createSignatureDoc( chains, signatures, streamtags, triggerbits, triggergroups )

            #print sig_doc.toprettyxml()
            
            # finally, create the chain list
            # first: retrieve chain and sequence lists
            dom_chainList = sig_doc.getElementsByTagName( "CHAIN_LIST" )[0]
            dom_seqList   = seq_doc.getElementsByTagName( "SEQUENCE_LIST" )[0]

            doc = Document()
            doctype = DocumentType( "HLT_MENU" );
            doctype.systemId = "hlt_menu.dtd"
            doc.appendChild( doctype );
            hlt_menu = doc.createElement( "HLT_MENU" )
            hlt_menu.setAttribute( "name", "dummy" )
            hlt_menu.setAttribute( "version", "%i" % 1 )
            doc.appendChild( hlt_menu )
            hlt_menu.appendChild( dom_seqList.cloneNode( True ));
            hlt_menu.appendChild( dom_chainList.cloneNode( True ));
            
            self.menu     = hltpopdblib.parseXML( hlt_menu )

        else:
            raise SyntaxError, "Unsupported initialization method for Menu"

    def xml_node(self, doc):
        # materialize self.menu as xml, returns an XML node
        return (self.createElement_sequence(doc), self.createElement_signature(doc))

    def db_node(self, cursor):
        # populate DB with self.menu 

        # defaults
        htm_name    = self.menu.menu_name
        htm_version = self.menu.menu_version

        # retrieve existing menus
        menus = hltpopdblib.get_menu( cursor )

        print '\n-- available trigger menus:'
        found = False
        for record in menus:
            print '--',record[0], float(record[1]), record[2], record[3]
            if record[0] == self.menu.menu_name and (self.menu.menu_version == -1 or \
                                                     float(record[1]) == self.menu.menu_version):
                found       = True
                htm_version = record[1]
                htm_id      = record[2]
                hps_name    = record[3]
                hps_id      = record[4]

        if found == False:
            print " - menu %s, version: %0.1f does not exist" % \
                  (self.menu.menu_name, float(self.menu.menu_version))
            htm_id      = -1
            htm_version = 1
            hps_id      = -1
            hps_name    = "prB_%s" % htm_name
        print ""
        print '-- use menu        : "%s" version: %s with htm_id: %i (query parameter)' \
              % (htm_name, htm_version, htm_id)
        print '-- use prescale_set: "%s" with hps_id: %i' % (hps_name, hps_id)
        print ""

        hltpopdblib.populateDB( cursor, self.menu, htm_id, htm_name, hps_id, hps_name )
        


    def createSequenceDoc( self, doc ):
        # create sequence_list from self

        # comment fields
        C = []
        C.append( doc.createComment( "xml script created %s on %s" \
                                     % (time.asctime(), socket.gethostname()) ) )
        for c in C: doc.appendChild( c )
        doc.appendChild( createElement_sequence(doc) )
        
    def createElement_sequence(self, doc):
        # sequence list
        seqList_doc = doc.createElement( "SEQUENCE_LIST" )
        seqList_doc.setAttribute( "hlt_setup.name", "dummy" )
        seqList_doc.setAttribute( "hlt_setup.version", "%i" % 0.0 )
        doc.appendChild( seqList_doc )

        # sequences
        # first retrieve all sequences in self
        seqs = []
        for ch in self.menu.chainList:
            for sig in ch.sigList:
                for seq in sig.seqList:
                    seqs.append( seq )

        # remove duplicated records (cannot use dictionary since destroys list sorting)
        seqs = hltdumpxmllib.unique( seqs )
        
        for seq in seqs:
            teI = ""
            for te in seq.teInList: teI += te + ' '
            algo = ""
            for alg in seq.algoList:
                algo += alg.longSpecSlashed() + " "

            seq_doc = doc.createElement( "SEQUENCE" )
            seq_doc.setAttribute( "input",     teI.strip() )
            seq_doc.setAttribute( "algorithm", "%s" % algo.strip() )
            seq_doc.setAttribute( "output",    seq.teOutElem )
            seqList_doc.appendChild( seq_doc )
        return seqList_doc
            
    def createSignatureDoc( self, doc ):
        # create chain_list from self

        # comment fields
        C = []
        C.append( doc.createComment( "xml script created %s on %s" \
                                     % (time.asctime(), socket.gethostname()) ) )
        for c in C: doc.appendChild( c )
        doc.appendChild( createElement_signature(doc) )

    def createElement_signature(self, doc):

        # chain list
        chainList_doc = doc.createElement( "CHAIN_LIST" )
        chainList_doc.setAttribute( "hlt_master_table.name",    self.menu.mastertable_name )
        chainList_doc.setAttribute( "hlt_master_table.version", "%0.1f" % self.menu.mastertable_version )
        chainList_doc.setAttribute( "hlt_trigger_menu.name",    self.menu.menu_name )
        chainList_doc.setAttribute( "hlt_trigger_menu.version", "%0.1f" % self.menu.menu_version )
        chainList_doc.setAttribute( "hlt_prescale_set.name",    self.menu.prescale_set_name )
        chainList_doc.setAttribute( "hlt_prescale_set.version", "%0.1f" % self.menu.prescale_set_version )
        
        # loop over chains
        for chain in self.menu.chainList:
            chain_doc = doc.createElement( "CHAIN" )
            chain_doc.setAttribute( "chain_id",             chain.name )
            chain_doc.setAttribute( "chain_counter", "%i" % chain.counter )
            chain_doc.setAttribute( "lower_chain",          chain.lower_chain )
            chain_doc.setAttribute( "level",                chain.level )
            chain_doc.setAttribute( "prescale",      "%i" % chain.prescale )
            chain_doc.setAttribute( "forced_accept", "%i" % chain.forced_acc )
            chainList_doc.appendChild( chain_doc )

            # signatures list
            sigList_doc = doc.createElement( "SIGNATURE_LIST" )
            chain_doc.appendChild( sigList_doc )

            # loop over signatures
            for sig in chain.sigList:
                sig_doc = doc.createElement( "SIGNATURE" )
                sig_doc.setAttribute( "signature_id",      "%s" % sig.id )
                sig_doc.setAttribute( "signature_counter", "%i" % sig.counter )
                sig_doc.setAttribute( "logic",             "%i" % sig.logic )
                sigList_doc.appendChild( sig_doc )
                
                # loop over sequences
                for seq in sig.seqList:
                    te_doc = doc.createElement( "TRIGGERELEMENT" )
                    te_doc.setAttribute( "te_name", seq.teOutElem )
                    sig_doc.appendChild( te_doc )
        return chainList_doc

# ------------------------------------------------------------------------------------------------

class TriggerElement(object):
    def __init__( self, name, id ):
        self.name = name
        self.id   = id
    def __eq__(self, other):       
        if (self.name == other.name and self.id == other.id):
            return True
        else:
            return False

class Algorithm(object):
    def __init__( self, fullstring ):
        self.buildFromSpecSlashed(fullstring)

    def __eq__(self, other):       
        if (self.classname    == other.classname and
            self.instancename == other.instancename and
            self.third        == other.third):
            return True
        else:
            return False

    def aliasSlashed(self):
        if self.third == "":
            return self.instancename
        else:
            return '%s/%s' % (self.instancename, self.third)

    def aliasUScored(self):
        if self.third == "":
            return self.instancename
        else:
            return '%s_%s' % (self.instancename, self.third)

    def longSpecSlashed(self):
        if self.third == "":
            return '%s/%s' % (self.classname, self.instancename)
        else:
            return '%s/%s/%s' % (self.classname, self.instancename, self.third)

    def longSpec(self):
        if self.third == "":
            return '%s/%s' % (self.classname, self.instancename)
        else:
            return '%s/%s_%s' % (self.classname, self.instancename, self.third)

    def longSpecUScored(self):
        if self.third == "":
            return '%s_%s' % (self.classname, self.instancename)
        else:
            return '%s_%s_%s' % (self.classname, self.instancename, self.third)

    def buildFromSpecSlashed(self, spec):
        fsList = spec.split('/')
        if len(fsList)==3:
            self.classname    = fsList[0]
            self.instancename = fsList[1]
            self.third        = fsList[2]
        elif len(fsList)==2:
            self.classname    = fsList[0]
            self.instancename = fsList[1]
            self.third        = ""
        else:
            print "The algorithm specifications should be of the form 'classname/instancename[/specifier]' but is of the form", spec
            sys.exit(1)

        if self.third == "":
            return '%s/%s' % (self.classname, self.instancename)
        else:
            return '%s/%s/%s' % (self.classname, self.instancename, self.third)

        
        
class Sequence(object):
    def __init__( self ):
        self.teInList  = ["~"]
        self.teTopo    = ""
        self.algoList  = []
        self.teOutElem = ""

    def add_teIn( self, teIn ):
        if self.teInList  == ["~"]:
            self.teInList = [ teIn ]
        else:
            self.teInList.append( teIn )

    def set_teInTopo( self, teInTopo ):
        self.teTopo = teInTopo

    def set_teOut( self, teOut ):
        self.teOutElem = teOut

    def __eq__(self, other):
        if (self.teInList  == other.teInList and
            self.teOutElem == other.teOutElem and
            self.algoList  == other.algoList):
            return True
        else:
            return False
    def _print_( self ):        
        tein   = ""
        tetopo = ""
        algo   = ""
        for te in self.teInList:
            tein += te + " "            
        for alg in self.algoList:
            algo += alg.longSpecSlashed() + " "
            
        print "-                   sequence: input TE(s) : %s" % tein
        print "-                             topo TE     : %s" % tetopo
        print "-                             algorithm(s): %s" % algo
        print "-                             output TE   : %s" % self.teOutElem

    def __str__( self ):        
        tein  = self.teInList[0]
        algo  = self.algoList[0].longSpecSlashed()
        for te in self.teInList[1:]: tein += " " + te
        for alg in self.algoList[1:]: algo += " " + alg.longSpecSlashed()
        return "algorithms='%s', input='%s', output='%s'" % (algo, tein, self.teOutElem)


    
class Signature(object):
    def __init__( self, id, counter, logic ):
        self.id      = id
        self.counter = counter
        self.logic   = logic
        self.seqList = []
    def add_seq( self, seq ):
        self.seqList.append( seq )
    def _print_( self ):
        print "-        signature: id=%s counter=%i, logic=%i" \
              % (self.id, self.counter, self.logic)
        for seq in self.seqList:
            seq._print_()
        
class Chain(object):
    def __init__( self, name, counter, lower_chain,
                  level, prescale, pass_through ):
        self.name         = name
        self.counter      = counter
        self.lower_chain  = lower_chain
        self.level        = level
        self.prescale     = prescale
        self.pass_through = pass_through
        self.sigList      = []
        self.streamtags   = []
        self.triggertypes = []
        self.triggergroups= []
        if self.level != "L2" and self.level != "EF":
            print "*** error: trigger level must be 'L2' or 'EF', but for chain '%s' the level '%s' was specified! ==> abort ***" % (self.name,self.level)
            sys.exit(1)

    def add_sig( self, sig ):
        self.sigList.append( sig )

    def add_group( self, group ):
        self.triggergroups.append( group )


    def _print_( self ):        
        print "----------------------------------------------------------------------------" \
              "----"
        print "- chain: name=%s, counter=%s, lower=%s, level=%s, prescale=%g, forced_acc=%g" \
              % (self.name, self.counter, self.lower_chain,
                 self.level, self.prescale, self.forced_acc)
        for sig in self.sigList:
            sig._print_()

class StreamTag(object):
    def __init__( self, name, type, prescale, obeyLB, description ):
        self.name        = name       
        self.type        = type       
        self.prescale    = prescale   
        self.obeyLB      = (obeyLB.lower()=="yes" or obeyLB.lower()=="true" or obeyLB=="1")
        self.description = description

class TriggerType(object):
    def __init__( self, typebit ):
        self.typebit = typebit       

class TriggerGroup(object):
    def __init__( self, name ):
        self.name = name       

class Menu(object):
    def __init__( self, mastertable_name, mastertable_version,
                  menu_name, menu_version, prescale_set_name, prescale_set_version ):
        self.mastertable_name     = mastertable_name
        self.mastertable_version  = mastertable_version
        self.menu_name            = menu_name
        self.menu_version         = int(menu_version)
        self.prescale_set_name    = prescale_set_name
        self.prescale_set_version = prescale_set_version
        self.chainList            = []

    def add_chain( self, chain ):
        self.chainList.append( chain )

    def _print_( self ):
        print "----------------------------------------------------------------------------" \
              "----"
        print "- Menu: name/version         = %s/%s" % (self.menu_name, self.menu_version)
        for chain in self.chainList:
            chain._print_()

    def writeToFile(self, xmlSeqFileName, xmlSigFileName="", offset="  "):
        """write everything to file. If the second filename is empty
        then write only one file"""

        # calculate the number of L2 and EF chains
        chainCounterL2 = len(filter(lambda x:x.level=='L2', self.chainList))
        chainCounterEF = len(self.chainList)-chainCounterL2
        
        if xmlSigFileName=="":
            newStyle=True
        else:
            newStyle=False

        if newStyle:
            header = """<?xml version="1.0" ?>
<!--File is auto-generated by db2xml.py, may be overwritten-->
<!DOCTYPE HLT_MENU SYSTEM 'htl_menu.dtd'>
<!--No. of L2 HLT chains:%i-->
<!--No. of EF HLT chains:%i-->
<HLT_MENU name="%s" version="%i">
""" % (chainCounterL2, chainCounterEF, self.menu_name, int(self.menu_version))
            footer="</HLT_MENU>\n"
        else:
            headerSeq = """<?xml version="1.0" ?>
<!--File is auto-generated by db2xml.py, may be overwritten-->
<!DOCTYPE SEQUENCE_LIST SYSTEM 'seqlist.dtd'>
<!--No. of L2 HLT chains:%i-->
<!--No. of EF HLT chains:%i-->
""" % (chainCounterL2, chainCounterEF)
            footerSeq=""
            headerSig = """<?xml version="1.0" ?>
<!--File is auto-generated by db2xml.py, may be overwritten-->
<!DOCTYPE CHAIN_LIST SYSTEM 'chainlist.dtd'>
<!--No. of L2 HLT chains:%i-->
<!--No. of EF HLT chains:%i-->
""" % (chainCounterL2, chainCounterEF)
            footerSig=""

        if newStyle:
            fout = open( xmlSeqFileName,  "w" )
            fout.write( header )
            self.writeSeqList(fout, offset)
            self.writeChainList(fout, offset)
            fout.write( footer )
            fout.close()
            print "... wrote file %s" % xmlSeqFileName
        else:
            fout = open( xmlSeqFileName,  "w" )
            fout.write(headerSeq)
            self.writeSeqList(fout)
            fout.write(footerSeq)
            fout.close()
            print "... wrote file %s" % xmlSeqFileName
            fout = open( xmlSigFileName, "w" )
            fout.write(headerSig)
            self.writeChainList(fout)
            fout.write(footerSig)
            fout.close()
            print "... wrote file %s" % xmlSigFileName



    def writeSeqList(self, fout, offset=""):
        fout.write( "%s<SEQUENCE_LIST>\n" % (offset) )

        teDict = {}
        for chain in self.chainList:
            for sig in chain.sigList:
                for seq in sig.seqList:
                    if seq.teOutElem in teDict:
                        # compare and exit if same outputTE for in different sequences
                        if not seq == teDict[seq.teOutElem]:
                            print "*** error: found different SEQUENCES with the same output TE %s => about ***" % seq.teOutElem
                            print seq
                            print teDict[seq.teOutElem]
                            sys.exit(1)
                    else:
                        teDict[seq.teOutElem] = seq


        for te in teDict:
            seq = teDict[te]
            topoAttribute = ""
            if seq.teTopo != "":
                topoAttribute=' topo_start_from="%s"' % seq.teTopo
            fout.write('%s<SEQUENCE algorithm="%s"%s input="%s" output="%s"/>\n' \
                       % ( offset+offset, " ".join(["%s" % (alg.longSpec()) for alg in seq.algoList]), \
                           topoAttribute, " ".join(seq.teInList), seq.teOutElem) );
        fout.write( "%s</SEQUENCE_LIST>\n" % (offset) )

    def writeChainList(self, fout, newStyle=False):
        if newStyle:
            offset="  "
        else:
            offset=""
        fout.write( "%s<CHAIN_LIST>\n" % offset )

        for chain in self.chainList:
            fout.write('%s<CHAIN chain_counter="%i" chain_name="%s" level="%s" lower_chain_name="%s" pass_through="%i" prescale="%i">\n' \
                       % (offset+offset, chain.counter, chain.name, chain.level, chain.lower_chain, chain.pass_through, chain.prescale) )
            if newStyle:
                fout.write('%s<TRIGGERTYPE_LIST>\n' % (offset+offset+offset) )
                for triggertype in chain.triggertypes:
                    fout.write('%s<TRIGGERTYPE bit="%s"/>\n'
                               % (offset+offset+offset+offset, triggertype.typebit) )
                fout.write('%s</TRIGGERTYPE_LIST>\n' % (offset+offset+offset) )
                fout.write('%s<STREAMTAG_LIST>\n' % (offset+offset+offset) )
                for stream in chain.streamtags:
                    if stream.obeyLB or stream.obeyLB=="1" or stream.obeyLB=="True" or stream.obeyLB=="true":
                        obeyLbFlag = "yes"
                    else:
                        obeyLbFlag = "no"
                    fout.write('%s<STREAMTAG obeyLB="%s" prescale="%i" stream="%s" type="%s"/>\n'
                               % (offset+offset+offset+offset, obeyLbFlag, stream.prescale, stream.name, stream.type) )
                fout.write('%s</STREAMTAG_LIST>\n' % (offset+offset+offset) )

                if len(chain.triggergroups)==0:
                    fout.write('%s<GROUP_LIST/>\n' % (offset+offset+offset) )
                else:
                    fout.write('%s<GROUP_LIST>\n' % (offset+offset+offset) )
                    for group in chain.triggergroups:
                        fout.write('%s<GROUP name="%s"/>\n'
                                   % (offset+offset+offset+offset, group.name) )
                    fout.write('%s</GROUP_LIST>\n' % (offset+offset+offset) )

            if len(chain.sigList)==0:
                fout.write('%s<SIGNATURE_LIST/>\n' % (offset+offset+offset) )
            else:
                fout.write('%s<SIGNATURE_LIST>\n' % (offset+offset+offset) )
                for sig in chain.sigList:
                    fout.write('%s<SIGNATURE logic="%s" signature_counter="%i">\n'
                               % (offset+offset+offset+offset, sig.logic, sig.counter) )
                    for seq in sig.seqList:
                        fout.write('%s<TRIGGERELEMENT te_name="%s"/>\n'
                                   % ( offset+offset+offset+offset+offset, seq.teOutElem) );

                    fout.write('%s</SIGNATURE>\n' % (offset+offset+offset+offset) )
                fout.write('%s</SIGNATURE_LIST>\n' % (offset+offset+offset) )
            

            fout.write('%s</CHAIN>\n' % (offset+offset) )
        fout.write( '%s</CHAIN_LIST>\n' % (offset) )
        






def check_row( cursor, table_name, table_prefix, columnList, order = ""):
    idlist = get_row_ids(cursor, table_name, table_prefix, columnList, order)
    if len(idlist) > 0:
        if dbVerbose(): print "EXISTS!"
        return idlist[0]
    else:
        if dbVerbose(): print "DOES NOT EXIST!"
        return -1




def insert_row( cursor, table_name, table_prefix, columnList ):
    # Test for type of database in use
    oracle_db = False
    mysql_db = False
    if str(type(cursor)).lower().find('oracle') >= 0: oracle_db = True
    if str(type(cursor)).lower().find('mysql') >= 0: mysql_db = True
    
    # insert new row    

    id = get_new_index( cursor, table_name, "%s_ID" % table_prefix )

    insert = "INSERT INTO %s \t VALUES( %i, " % (table_name, id)
    if  dbVerbose():
        print insert
    for col in columnList:
        # if it's the version, and it should be reset, do so
        if col.type == "S":
            insert += "'%s', " % format_dbstring(col.value)
        elif col.type == "I":
            insert += "%i, " % int(col.value)
        elif col.type == "F":
            insert += "%g, " % float(col.value)
        elif col.type == "T":
            if oracle_db :
                insert += "to_date('%s' , '%s')," % (col.value, time_format)
            else:
                insert += "'%s', " % (col.value)
        else:
            print "*** unknown type in function insert_row ==> abort"
            sys.exit(1)


    insert = insert.rstrip( ", " )
    insert += " )"

    if isinstance(insert, unicode): insert = str(insert)

    if dumpSQL():
        print >>dumpSQLFile(), insert

    cursor.execute( insert )
        
    return int(id)



def matchingConnectorList(cursor,
                          table_name,
                          table_prefix,
                          base_name,
                          reference_name,
                          baseID_list,
                          refID_list,
                          order):
    """ Here we look into a connection table for an existing mapping
    @param ID_list list of table IDs on the  
    """

    #if len(refID_list)==0:
    #    if traceInsert():
    #        print "TRACE: Search for (list<->list) connector in '%s' with %s = %s among %s ==> " % (table_name, reference_name, refID_list, baseID_list),
    #        print baseID_list
    #    return baseID_list

    refId_list = refID_list

    # for later comparison
    if order=="":
        refId_list.sort()

    if len(baseID_list)==0:
        query = "SELECT DISTINCT %s_ID FROM %s WHERE " % (base_name, table_name)
        query += " %s_ID IN " % reference_name
        query += "( " + ", ".join(map(str,refId_list)) + " )"
        if isinstance(query, unicode): query = str(query)
        cursor.execute( query )
        result = cursor.fetchall()
        baseId_list = [int(x[0]) for x in result]
    else:
        baseId_list = baseID_list
    # result now contains all entries of table base_name that are connected with at least one entry in the id_list

    # now we loop over those entries and check if there are the same number of matches
    matchingConnectorEntries = []
    for qr in baseId_list:

        query = "SELECT %s_ID FROM %s WHERE %s_ID = %i" % \
                (reference_name, table_name, base_name, int(qr))

        if order=="":
            query += " ORDER BY %s_ID" % reference_name
        else:
            query += " ORDER BY %s" % order

        if isinstance(query, unicode): query = str(query)
        cursor.execute( query )
        result = [ x[0] for x in cursor.fetchall() ] # a list of tuples

        if result == refId_list:
            matchingConnectorEntries.append( int(qr) )
            
    if traceInsert():
        print "TRACE: Search for (list<->list) connector in '%s' with %s = %s among %s ==> " % (table_name, reference_name, refID_list, baseID_list),
        print matchingConnectorEntries
      
    return matchingConnectorEntries




######################################
##
##        The DB Trigger Element
##
######################################
class DBHltTriggerElement(DBTable):
    TableName = "HLT_TRIGGER_ELEMENT"
    Praefix = "HTE"

    def __init__( self, parents, seq ):
        self.seq      = seq
        self.name     = seq.teOutElem
        (self.mastertable, self.triggermenu, self.triggerchain, self.triggersignature ) = parents
        

    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "TRACE: Prepare to insert TriggerElement '%s'" % self.name
        needsNewThis = False

        colList = []
        colList.append( Column( "HTE_NAME", self.name, "S" ) )
        if self.mastertable.verbose:
            print "Checking TRIGGER_ELEMENT: %s" % self.name
        id_list_matchingTEname = get_row_ids( cursor, self.TableName, self.Praefix, colList )
        #self.dbID = id

        self.componentIdList = self.findComponents(cursor, username, timestamp)
        
        if len(id_list_matchingTEname)==0:  # does not exist yet
            needsNewThis = True
        else:     # exists, so also check for connectors to CPs and input TEs
            matchConnIdtoComps = matchingConnectorList(cursor,
                                                       table_name     = "HLT_TE_TO_CP",
                                                       table_prefix   = "HTE2CP",
                                                       base_name      = "HTE2CP_TRIGGER_ELEMENT",
                                                       reference_name = "HTE2CP_COMPONENT",
                                                       baseID_list    = id_list_matchingTEname,
                                                       refID_list     = self.componentIdList,
                                                       order          = "HTE2CP_ALGORITHM_COUNTER")
            # matchConnIdtoComps should contain all TE's that point to a certain (list of) components

            reducedMatchConnId = self.findTEsWithMatchingInputTEs(matchConnIdtoComps,
                                                                  self.seq.teInList, # this list contains all input TEs (name)
                                                                  self.seq.teTopo,
                                                                  cursor)

            if len(reducedMatchConnId)==0: # connector does not yet exist
                needsNewThis = True
            else:
                if len(reducedMatchConnId)>1:
                    print self.componentIdList
                    print self.seq.teInList
                    print matchConnIdtoComps
                    print "*** error: duplicated triggerelement ",reducedMatchConnId," found => abort ***" 
                    sys.exit(1)
                needsNewThis = False
                self.dbID = reducedMatchConnId[0]

        if needsNewThis:
            if self.mastertable.verbose:
                print "Needs new TRIGGER_ELEMENT: %s" % self.name
            self.dbID = self.dbInsertThis(cursor, username, timestamp) # force insert
            self.insertConnectorsTEtoTE(cursor, username, timestamp)
            self.insertAllCPsAndConnectorsTEtoCP(cursor, username, timestamp)
        else:
            if traceInsert():
                print "TRACE: Return existing TriggerElement '%s' ==> %i" % (self.name, self.dbID)

        return self.dbID


    
    def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HTE_NAME",          self.name, "S" ) )
        colList.append( Column( "HTE_VERSION",       "%i" % 1, "I", False ) )
        colList.append( Column( "HTE_USERNAME",      username, "S", False ) )
        colList.append( Column( "HTE_MODIFIED_TIME", timestamp, "T", False) )
        colList.append( Column( "HTE_USED",          "0", "S", False ) )
        id = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert TriggerElement '%s' ==> %i" % (self.name,id)
        return id



    def findComponents(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        componentIdList = []
        for algo in self.seq.algoList:
            componentIdList.append( self.findComponent(algo, cursor, username, timestamp) )
        return componentIdList

    def findComponent(self, algo, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):

        componentName  = algo.classname
        componentAlias = algo.aliasUScored()
        if dbVerbose():
            print "trying to find component %s/%s" % (componentName, componentAlias)

        # ====
        # this is only for the technical run in May 2007, where we use release 13 schema and upload scripts but a release 12.0.5-HLT setup.
        if OldSteering:
            # old steering (release 12)
            componentAlias += '_%s' % self.triggerchain.level
        # ====
        
        if self.triggerchain.level=="L2":
            setupId = self.mastertable.l2setup_id
        else:
            setupId = self.mastertable.efsetup_id
        hcp_name = "HLT_COMPONENT"
        hstcp_name = "HLT_ST_TO_CP"
        tables = ( "\t %s, %s\n" ) % (hcp_name, hstcp_name)
        criteria = ( '\t %s.HCP_ALIAS = \'%s\' AND \n' % (hcp_name, componentAlias) +
                     '\t %s.HCP_NAME = \'%s\' AND \n' % (hcp_name, componentName) +
                     '\t %s.HST2CP_COMPONENT_ID = %s.HCP_ID AND \n' % (hstcp_name, hcp_name) +
                     '\t %s.HST2CP_SETUP_ID = %i' % (hstcp_name, setupId) )
        query    = "SELECT DISTINCT %s.HCP_ID \n FROM %s WHERE %s" % (hcp_name, tables, criteria)
        
        if isinstance(query, unicode): query = str(query)
        if self.mastertable.verbose:
            print type(query), query
            
        cursor.execute( query )
        result = cursor.fetchall()
        if result==None or len(result)==0:
            print "Warning: Component %s not found in table HLT_COMPONENT in the current setup (setup-key = %i)" \
                  % (algo.longSpecSlashed(), setupId)

            #print "DETAIL: the query was ", query 
            if self.mastertable.forceComponents:
                print "ForceComponents set to True, will do so"
                # here we force a component into the setup (this should be done only in the testing phase)
                compId = hltsetup.insertDummyComponent(cursor, componentName, componentAlias, 0, username, timestamp)
                colList = []
                colList.append( Column( "HST2CP_SETUP_ID",      "%i" % setupId,    "I" ) )
                colList.append( Column( "HST2CP_COMPONENT_ID",  "%i" % compId,     "I" ) )
                colList.append( Column( "HST2CP_USERNAME",      username,          "S" ) )
                colList.append( Column( "HST2CP_MODIFIED_TIME", timestamp,         "T" ) )
                colList.append( Column( "HST2CP_USED",          "0",               "S" ) )
                if traceInsert():
                    print "TRACE: Insert Connector Setup (%i) <-> Component (%i)" % (setupId,compId)
                insert_row( cursor, "HLT_ST_TO_CP", "HST2CP", colList )
                return int(compId)
            
            else:
                raise KeyError, "ForceComponents set to False, will abort"
                #raise KeyError, "Component %s not found in table HLT_COMPONENT in the current setup (setup-key = %i) and forceComponent set to False" % (self.algo.aliasSlashed(), setupId)
        elif len(result)>1:
            raise KeyError, "Found %i (>1) components %s in the current setup (setup-key = %i)" % (len(result),self.algo.aliasSlashed(), setupId)

        return int(result[0][0])


    def findTEsWithMatchingInputTEs(self, matchConnIdtoComps, teInList, teTopo, cursor):
        reducedMatchConnList = []
        for id in matchConnIdtoComps:
            (inputTElist, topo) = self.findInputTEList(id, cursor)
            if inputTElist==teInList and topo==teTopo:
                reducedMatchConnList.append(id)
        return reducedMatchConnList

    def findInputTEList(self, id, cursor):
        query = ( "select HTE2TE_TE_INP_ID, HTE2TE_TE_INP_TYPE, HTE2TE_TE_COUNTER"
                  " from HLT_TE_TO_TE"
                  " where HTE2TE_TE_ID = '%i'"
                  " order by HTE2TE_TE_INP_TYPE ASC, HTE2TE_TE_COUNTER ASC" % id )
        if isinstance(query, unicode): query = str(query)
        cursor.execute(query)
        result = cursor.fetchall()
        inputTEdict = {}
        inputTopo = ""
        for line in result:
            if line[1]=="topo":
                inputTopo = line[0]
            else:
                inputTEdict[line[2]]=line[0]
        inputTElist = [inputTEdict[x] for x in range(len(inputTEdict))]
        return (inputTElist, inputTopo)

        
        

    def insertConnectorsTEtoTE(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        for c, inpTE in enumerate(self.seq.teInList):
            self.insertConnectorTEtoTE(inpTE, c, "input", cursor, username, timestamp)
        if self.seq.teTopo != "":
            self.insertConnectorTEtoTE(self.seq.teTopo, c, "topo", cursor, username, timestamp)

    def insertConnectorTEtoTE(self, inpTE, counter, type, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HTE2TE_TE_ID",      int(self.dbID), "I" ) )
        colList.append( Column( "HTE2TE_TE_INP_ID",  str(inpTE),     "S" ) )
        colList.append( Column( "HTE2TE_TE_INP_TYPE",str(type),      "S" ) )
        colList.append( Column( "HTE2TE_TE_COUNTER", counter,        "I" ) )
        colList.append( Column( "HTE2TE_USERNAME",                username,       "S", False ) )
        colList.append( Column( "HTE2TE_MODIFIED_TIME",           timestamp,      "T", False ) )
        colList.append( Column( "HTE2TE_USED",                    "0",            "S", False ) )
        insert_row( cursor, "HLT_TE_TO_TE", "HTE2TE", colList )


    
    def insertAllCPsAndConnectorsTEtoCP(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        for i, cpId in enumerate(self.componentIdList):
            self.insertConnectorTEtoCP(cpId, i, cursor, username, timestamp)

    def insertConnectorTEtoCP(self, hcp_id, algCounter, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HTE2CP_TRIGGER_ELEMENT_ID", int(self.dbID),  "I" ) )
        colList.append( Column( "HTE2CP_COMPONENT_ID",       int(hcp_id),     "I" ) )
        colList.append( Column( "HTE2CP_ALGORITHM_COUNTER",  int(algCounter), "I" ) )
        conn_Id = check_row( cursor, "HLT_TE_TO_CP", "HTE2CP", colList )
        if(conn_Id<0): # insert
            colList.append( Column( "HTE2CP_USERNAME", username, "S", False ) )
            colList.append( Column( "HTE2CP_MODIFIED_TIME", timestamp, "T", False) )
            colList.append( Column( "HTE2CP_USED", "0", "S", False ) )                        
            hte2cp_id = insert_row( cursor, "HLT_TE_TO_CP", "HTE2CP", colList )






    
######################################
##
##     The DB Trigger Signature
##
######################################
class DBHltTriggerSignature(DBTable):
    def __init__( self, parents, sig ):
        self.sig = sig
        self.seqList = []
        (self.mastertable, self.triggermenu, self.triggerchain ) = parents
        for seq in self.sig.seqList:
            self.seqList.append( DBHltTriggerElement( parents+(self,) , seq) )
            
    def sig_counter(self):
        return self.sig.counter
        
    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "TRACE: Prepare to insert Signature '%i'" % self.sig.counter
        self.seqIdList = []
        for dbseq in self.seqList:    # insert all Triggerelements
            self.seqIdList.append( dbseq.dbInsert(cursor, username, timestamp) )


        # Find a connector in HLT_TS_TO_TE
        needsNewConnector = False
        needsNewThis = False
        matchConnId = -1

        # check for existence of the entry itself
        colList = []
        colList.append( Column( "HTS_LOGIC", "%i" % self.sig.logic, "I" ) )
        id_list = get_row_ids( cursor, self.TableName, self.Praefix, colList )

        if len(id_list)==0:  # does not exist yet
            needsNewThis = True
        else:     # exists, so also check for signature <-> TR connector
            matchConnIdList = matchingConnectorList(cursor,
                                                    table_name     = "HLT_TS_TO_TE",
                                                    table_prefix   = "HTS2TE",
                                                    base_name      = "HTS2TE_TRIGGER_SIGNATURE",
                                                    reference_name = "HTS2TE_TRIGGER_ELEMENT",
                                                    baseID_list    = [],
                                                    refID_list     = self.seqIdList,
                                                    order          = "HTS2TE_ELEMENT_COUNTER")
            if len(matchConnIdList) > 0 :
              matchConnId = matchConnIdList[0]
            else:
              matchConnId = -1
            if matchConnId<0: # connector does not exist yet
                needsNewThis = True
            else:
                self.dbID = matchConnId

        if needsNewThis:
            self.dbID = self.dbInsertThis(cursor, username, timestamp) # force insert
            self.insertConnectorTStoTE(cursor, username, timestamp)
        else:
            if traceInsert():
                print "TRACE: Return existing Signature '%i' ==> %i" % (self.sig.counter, self.dbID)
            
        return self.dbID


    def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HTS_LOGIC",         "%i" % self.sig.logic, "I" ) )
        colList.append( Column( "HTS_USERNAME",      username,              "S", False ) )
        colList.append( Column( "HTS_MODIFIED_TIME", timestamp,             "T", False ) )
        colList.append( Column( "HTS_USED",          "0",                   "S", False ) )                            
        id = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert Signature '%i' ==> %i" % (self.sig.counter,id)
        return id

    def insertConnectorTStoTE(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        for idx,seqID in enumerate(self.seqIdList):
            colList = []
            colList.append( Column( "HTS2TE_TRIGGER_SIGNATURE_ID",  "%i" % self.dbID , "I" ) )
            colList.append( Column( "HTS2TE_TRIGGER_ELEMENT_ID", "%i" % seqID, "I" ) )
            colList.append( Column( "HTS2TE_ELEMENT_COUNTER",    "%i" % idx, "I" ) )
            colList.append( Column( "HTS2TE_USERNAME",         username, "S", False ) )
            colList.append( Column( "HTS2TE_MODIFIED_TIME",    timestamp, "T", False) )
            colList.append( Column( "HTS2TE_USED",             "0", "S", False ) )
            insert_row( cursor, "HLT_TS_TO_TE", "HTS2TE", colList )
            idx += 1


    TableName = "HLT_TRIGGER_SIGNATURE"
    Praefix   = "HTS"
    


######################################
##
##     The DB Trigger StreamTag
##
######################################
class DBHltTriggerStreamTag(DBTable):
    TableName = "HLT_TRIGGER_STREAM"
    Praefix   = "HTR"

    def __init__( self, parents, streamtag ):
        self.streamtag = streamtag  # StreamTag object
        (self.mastertable, self.triggermenu, self.triggerchain ) = parents

    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "TRACE: Prepare to insert StreamTag '%s, %s' for chain '%s'" % (self.streamtag.name, self.streamtag.type, self.triggerchain.name)

        desc = self.streamtag.description
        if self.streamtag.description == "":
            desc = "~"
            
        # check for existence of the streamtag itself
        colList = []
        colList.append( Column( "HTR_NAME",        str(self.streamtag.name), "S" ) )
        colList.append( Column( "HTR_DESCRIPTION", str(desc),                "S" ) )
        colList.append( Column( "HTR_TYPE",        str(self.streamtag.type), "S" ) )
        colList.append( Column( "HTR_OBEYLB",      int(self.streamtag.obeyLB), "I" ) )
        id = check_row( cursor, self.TableName, self.Praefix, colList )
        if id<0:  # does not exist yet
            needsNewThis = True
        else:     # exists, so also check for connector
            needsNewThis = False
            self.dbID = int(id)

        if needsNewThis:
            self.dbID = self.dbInsertThis(cursor, username, timestamp) # force insert
        else:
            if traceInsert():
                print "TRACE: Return existing StreamTag '%s, %s' ==> %i" % (self.streamtag.name, self.streamtag.type, self.dbID)
            
        return self.dbID

    def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        desc = self.streamtag.description
        if self.streamtag.description == "":
            desc = "~"
        colList = []
        colList.append( Column( "HTR_NAME",               str(self.streamtag.name),        "S" ) )
        colList.append( Column( "HTR_DESCRIPTION",        str(desc),                       "S" ) )
        colList.append( Column( "HTR_TYPE",               str(self.streamtag.type),        "S" ) )
        colList.append( Column( "HTR_OBEYLB",             int(self.streamtag.obeyLB),      "I" ) )
        colList.append( Column( "HTS_USERNAME",           username,                        "S", False ) )
        colList.append( Column( "HTS_MODIFIED_TIME",      timestamp,                       "T", False ) )
        colList.append( Column( "HTS_USED",               "0",                             "S", False ) )                            
        id = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert StreamTag '%s, %s'" % (self.streamtag.name,self.streamtag.type)
        return id




######################################
##
##        The DB Trigger Chain
##
######################################
class DBHltTriggerChain(DBTable):
    TableName = "HLT_TRIGGER_CHAIN"
    Praefix = "HTC"
    def __init__( self, parents, chain ):
        self.name = chain.name
        self.chain = chain
        self.level = self.chain.level
        self.sigList = []
        self.streamList = []
        self.triggerbits = []
        self.triggergroups = []
        (self.mastertable, self.triggermenu ) = parents
        #print "In constructor of DBChain %s I find %i signatures" % (self.name, len(self.chain.sigList))
        for sig in self.chain.sigList:
            self.sigList.append(DBHltTriggerSignature(parents+(self,), sig))
        for stream in self.chain.streamtags:
            self.streamList.append(DBHltTriggerStreamTag(parents+(self,), stream))
        for tbit in self.chain.triggertypes:
            self.triggerbits.append(tbit.typebit)
        for tgroup in self.chain.triggergroups:
            self.triggergroups.append(tgroup.name)
        self.triggerbits.sort()
        self.triggergroups.sort()
        
    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "TRACE: Prepare to insert Chain '%s'" % self.name
        self.sigIdList = []
        self.streamtagIdList = []
        for dbsig in self.sigList:
            self.sigIdList.append( dbsig.dbInsert(cursor, username, timestamp) )
        for dbstreamtag in self.streamList:
            self.streamtagIdList.append( dbstreamtag.dbInsert(cursor, username, timestamp) )
            
        needsNewConnector = False
        needsNewThis = False
        matchConnId = -1
        
        colList = []
        colList.append( Column( "HTC_NAME",              self.chain.name,         "S" ) )
        if self.chain.lower_chain=="":
            colList.append( Column( "HTC_LOWER_CHAIN_NAME",  "none",  "S" ) )
        else:
            colList.append( Column( "HTC_LOWER_CHAIN_NAME",  self.chain.lower_chain,  "S" ) )
        colList.append( Column( "HTC_L2_OR_EF",          self.level,              "S" ) )
        colList.append( Column( "HTC_PASS_THROUGH_RATE", self.chain.pass_through ,"I" ) )
        if self.chain.level=="L2":
            colList.append( Column( "HTC_SETUP_ID",         "%i" % self.mastertable.l2setup_id, "I" ) )
        elif self.chain.level=="EF":
            colList.append( Column( "HTC_SETUP_ID",         "%i" % self.mastertable.efsetup_id, "I" ) )
        else:
            print "Trigger Level of Chain %s not specified" % self.chain.name
            sys.exit(1)
        id_list = get_row_ids( cursor, self.TableName, self.Praefix, colList )

        if len(id_list)==0:  # does not exist yet
            needsNewThis = True
        else:     # exists, so check for connector
            matchConnIdtoSigs    = matchingConnectorList(cursor,
                                                         table_name     = "HLT_TC_TO_TS",
                                                         table_prefix   = "HTC2TS",
                                                         base_name      = "HTC2TS_TRIGGER_CHAIN",
                                                         reference_name = "HTC2TS_TRIGGER_SIGNATURE",
                                                         baseID_list    = id_list,
                                                         refID_list     = self.sigIdList,
                                                         order          = "HTC2TS_SIGNATURE_COUNTER" )

            matchConnIdtoStreams = matchingConnectorList(cursor,
                                                         table_name     = "HLT_TC_TO_TR",
                                                         table_prefix   = "HTC2TR",
                                                         base_name      = "HTC2TR_TRIGGER_CHAIN",
                                                         reference_name = "HTC2TR_TRIGGER_STREAM",
                                                         baseID_list    = matchConnIdtoSigs,
                                                         refID_list     = self.streamtagIdList,
                                                         order          = "" )
            
            if len(matchConnIdtoStreams) == 0:
                print 'Warning - No Connectors to HLT_TRIGGER_STREAM found for chain %s' % self.chain.name
                matchConnIds = []
            else :
                matchConnIds = filter(lambda x:x in matchConnIdtoSigs, matchConnIdtoStreams)

            # in theory len(matchConnId) should be 0 or 1
            if len(matchConnIds)==0: # connector does not yet exist
                needsNewThis = True
            else:
                # a pair of connectors already exists (maybe even multiple pairs)
                # they need to be check if the triggerbits pointing to this chain are identical
                matchConnIdsCopy = []
                for chainConnId in matchConnIds:
                    query = "SELECT HLT_TRIGGER_TYPE.HTT_TYPEBIT FROM HLT_TRIGGER_TYPE WHERE HLT_TRIGGER_TYPE.HTT_TRIGGER_CHAIN_ID=%i" \
                            % int(chainConnId)
                    if isinstance(query, unicode): query = str(query)
                    cursor.execute( query )
                    result = cursor.fetchall()

                    triggerBitList = [int(line[0]) for line in result]
                    triggerBitList.sort()

                    if triggerBitList == self.triggerbits:
                        matchConnIdsCopy.append(chainConnId)

                matchConnIds = matchConnIdsCopy
                if len(matchConnIds)==0:
                    needsNewThis = True
                else:
                    matchConnIdsCopy = []
                    for chainConnId in matchConnIds:
                        query = "SELECT HTG_NAME FROM HLT_TRIGGER_GROUP WHERE HTG_TRIGGER_CHAIN_ID=%i" \
                                % int(chainConnId)
                        if isinstance(query, unicode): query = str(query)
                        cursor.execute( query )
                        result = cursor.fetchall()

                        groupNameList = [str(line[0]) for line in result]
                        groupNameList.sort()

                        if groupNameList == self.triggergroups:
                            matchConnIdsCopy.append(chainConnId)

                    matchConnIds = matchConnIdsCopy
                    if len(matchConnIds)==0:
                        needsNewThis = True
                    else:
                        if len(matchConnIds)>1:
                            needsNewThis = False
                            print "Warning: Multiple (%i) chain entries with identical signature, stream, triggerbit, and trigger group definition, will use the first one I found" \
                                  % len(matchConnIds)
                        self.dbID = matchConnIds[0]
                        needsNewThis = False
                
        if needsNewThis:
            self.dbID = self.dbInsertThis(cursor, username, timestamp) # force insert (= new version)
            self.insertConnectorTC2TS(cursor, username, timestamp)
            self.insertConnectorTC2TR(cursor, username, timestamp)
            self.insertTriggerTypes(cursor, username, timestamp)
            self.insertTriggerGroups(cursor, username, timestamp)
        else:
            if traceInsert():
                print "TRACE: Return existing Chain '%s' ==> %i" % (self.name,self.dbID)
                
        return self.dbID

    
    def dbInsertThis(self,cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HTC_NAME", self.chain.name, "S" ) )
        newVersion = get_lastversion(cursor, self.TableName, self.Praefix, colList) +1
        
        colList = []
        colList.append( Column( "HTC_NAME",             self.chain.name,         "S" ) )
        colList.append( Column( "HTC_VERSION",          "%i" % newVersion,       "I" ) )
        colList.append( Column( "HTC_CHAIN_COUNTER",    self.chain.counter,      "I" ) )
        if self.chain.lower_chain=="":
            colList.append( Column( "HTC_LOWER_CHAIN_NAME", "none",                  "S" ) )
        else:
            colList.append( Column( "HTC_LOWER_CHAIN_NAME", self.chain.lower_chain , "S" ) )
        colList.append( Column( "HTC_L2_OR_EF",         self.chain.level ,       "S" ) )
        colList.append( Column( "HTC_PASS_THROUGH_RATE",self.chain.pass_through, "I" ) )
        if self.chain.level=="L2":
            colList.append( Column( "HTC_SETUP_ID",         "%i" % self.mastertable.l2setup_id, "I" ) )
        elif self.chain.level=="EF":
            colList.append( Column( "HTC_SETUP_ID",         "%i" % self.mastertable.efsetup_id, "I" ) )
        colList.append( Column( "HTC_USERNAME",         username,                "S", False) )
        colList.append( Column( "HTC_MODIFIED_TIME",    timestamp,               "T", False) )
        colList.append( Column( "HTC_USED",             "0", "S", False ) )
        id = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert Chain '%s' ==> %i" % (self.name,id)
        return id

    def insertConnectorTC2TS(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        for sig in self.sigList:
            colList = []
            colList.append( Column( "HTC2TS_TRIGGER_CHAIN_ID",     "%i" % self.dbID , "I" ) )
            colList.append( Column( "HTC2TS_TRIGGER_SIGNATURE_ID", "%i" % sig.dbID, "I" ) )
            colList.append( Column( "HTC2TS_SIGNATURE_COUNTER",    "%i" % sig.sig_counter(), "I" ) )
            colList.append( Column( "HTC2TS_USERNAME",             username,     "S", False ) )
            colList.append( Column( "HTC2TS_MODIFIED_TIME",        timestamp,    "T", False) )
            colList.append( Column( "HTC2TS_USED",                 "0",          "S", False ) )
            if traceInsert():
                print "TRACE: Insert Connecter Chain (%i) <-> Signature (%i)" % (self.dbID,sig.dbID)
            insert_row( cursor, "HLT_TC_TO_TS", "HTC2TS", colList )

    def insertConnectorTC2TR(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        for dbstream in self.streamList:
            colList = []
            colList.append( Column( "HTC2TR_TRIGGER_CHAIN_ID",        int(self.dbID),                   "I" ) )
            colList.append( Column( "HTC2TR_TRIGGER_STREAM_ID",       int(dbstream.dbID),               "I" ) )
            colList.append( Column( "HTC2TR_TRIGGER_STREAM_PRESCALE", int(dbstream.streamtag.prescale), "I" ) )
            colList.append( Column( "HTC2TR_USERNAME",                username,                         "S", False ) )
            colList.append( Column( "HTC2TR_MODIFIED_TIME",           timestamp,                        "T", False) )
            colList.append( Column( "HTC2TR_USED",                    "0",                              "S", False ) )
            if traceInsert():
                print "TRACE: Insert Connecter Chain '%s' <-> Stream '%s'" % (self.name,dbstream.streamtag.name)
            insert_row( cursor, "HLT_TC_TO_TR", "HTC2TR", colList )

    def insertTriggerTypes(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "insertTriggerTypes", self.triggerbits
        for tbit in self.triggerbits:
            colList = []
            colList.append( Column( "HTT_TRIGGER_CHAIN_ID",           int(self.dbID), "I" ) )
            colList.append( Column( "HTT_TYPEBIT",                    int(tbit),      "I" ) )
            colList.append( Column( "HTC2TR_USERNAME",                username,       "S", False ) )
            colList.append( Column( "HTC2TR_MODIFIED_TIME",           timestamp,      "T", False) )
            colList.append( Column( "HTC2TR_USED",                    "0",            "S", False ) )
            if traceInsert():
                print "TRACE: Insert triggerbit '%i' for chain '%s'" % (tbit, self.name)
            insert_row( cursor, "HLT_TRIGGER_TYPE", "HTT", colList )

    def insertTriggerGroups(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "insertTriggerGroups", self.triggergroups
        for tgroupname in self.triggergroups:
            colList = []
            colList.append( Column( "HTG_TRIGGER_CHAIN_ID",           int(self.dbID), "I" ) )
            colList.append( Column( "HTG_NAME",                       str(tgroupname),"S" ) )
            colList.append( Column( "HTG_USERNAME",                   username,       "S", False ) )
            colList.append( Column( "HTG_MODIFIED_TIME",              timestamp,      "T", False) )
            colList.append( Column( "HTG_USED",                       "0",            "S", False ) )
            if traceInsert():
                print "TRACE: Insert trigger group '%s' for chain '%s'" % (tgroupname, self.name)
            insert_row( cursor, "HLT_TRIGGER_GROUP", "HTG", colList )




######################################
##
##       The DB Trigger Menu
##
######################################
class DBHltTriggerMenu(DBTable):
    def __init__( self, mastertable, menu ):
        self.menu = menu
        self.chainList = []
        self.mastertable = mastertable
        for chain in menu.chainList:
            self.chainList.append( DBHltTriggerChain((mastertable, self), chain) )

    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "TRACE: Prepare to insert Menu '%s'" % self.menu.menu_name
        self.chainIdList = []
        for dbchain in self.chainList:
            self.chainIdList.append( dbchain.dbInsert(cursor, username, timestamp) )

        createNewVersion = False
        needsNewConnector = False
        needsNewThis = False
        matchConnId = -1
        
        colList = []
        colList.append( Column( "HTM_NAME",          self.menu.menu_name, "S" ) )
        colList.append( Column( "HTM_PHASE",         "lumi",              "S" ) )
        colList.append( Column( "HTM_CONSISTENT",    "0",                 "S" ) )
        id_list = get_row_ids( cursor, self.TableName, self.Praefix, colList )

        if len(id_list)==0:  # the menu with the requested name does not exist yet
            needsNewThis = True
        else:     # exists, so check for connector
            matchConnIdList = matchingConnectorList(cursor,
                                                    table_name     = "HLT_TM_TO_TC",
                                                    table_prefix   = "HTM2TC",
                                                    base_name      = "HTM2TC_TRIGGER_MENU",
                                                    reference_name = "HTM2TC_TRIGGER_CHAIN",
                                                    baseID_list    = id_list,
                                                    refID_list     = self.chainIdList,
                                                    order          = "" )
            if len(matchConnIdList) > 0:
                self.dbID = matchConnIdList[0]
            elif len(self.chainIdList)==0:
                # that means that there are no chains in the menu
                # although this is strange, we perhaps should give a warning, we don't abort
                self.dbID = id_list[0]
                if self.mastertable.verbose:
                    print "WARNING: no chains given for menu"
            else: # connector does not exist yet
                needsNewThis = True
                createNewVersion = True



        if needsNewThis:
            self.dbID = self.dbInsertThis(cursor, username, createNewVersion, timestamp) # force insert (= new version)
            self.insertConnector(cursor, username, timestamp)
        else:
            if traceInsert():
                print "TRACE: Return existing Menu '%s' ==> '%i'" % (self.menu.menu_name, self.dbID)


        print "==============> HLT Menu for this configuration: %i (name='%s', version='%i')" % \
              (self.dbID, self.menu.menu_name, int(self.menu.menu_version) )
        return self.dbID
        
    def dbInsertThis(self, cursor, username, createNewVersion, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        newVersion = 1
        if createNewVersion:
            colList = []
            colList.append( Column( "HTM_NAME",      self.menu.menu_name, "S" ) )
            newVersion = get_lastversion(cursor, self.TableName, self.Praefix, colList) +1

        colList = []
        colList.append( Column( "HTM_NAME",          self.menu.menu_name, "S" ) )
        colList.append( Column( "HTM_VERSION",       "%f" % newVersion,   "F", False ) )
        colList.append( Column( "HTM_PHASE",         "lumi",              "S" ) )
        colList.append( Column( "HTM_SHIFT_SAFE",    "0",                 "S" ) )
        colList.append( Column( "HTM_USERNAME",      username,            "S", False ) )
        colList.append( Column( "HTM_MODIFIED_TIME", timestamp,           "T", False ) )
        colList.append( Column( "HTM_USED",          "0",                 "S", False ) )
        dbID = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert Menu '%s' ==> %i" % (self.menu.menu_name,dbID)
        return dbID



    def insertConnector(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        for dbchain in self.chainList:
            colList = []
            colList.append( Column( "HTM2TC_TRIGGER_MENU_ID",  "%i" % self.dbID , "I" ) )
            colList.append( Column( "HTM2TC_TRIGGER_CHAIN_ID", "%i" % dbchain.dbID, "I" ) )
            #colList.append( Column( "HTM2TC_CHAIN_COUNTER",    "%i" % dbchain.chain.counter, "I" ) )
            colList.append( Column( "HTM2TC_USERNAME",         username, "S", False ) )
            colList.append( Column( "HTM2TC_MODIFIED_TIME",    timestamp, "T", False) )
            colList.append( Column( "HTM2TC_USED",             "0", "S", False ) )
            if traceInsert():
                print "TRACE: Insert Connecter Menu (%i) <-> Chain (%i)" % (self.dbID, dbchain.dbID)
            insert_row( cursor, "HLT_TM_TO_TC", "HTM2TC", colList )

    TableName = "HLT_TRIGGER_MENU"
    Praefix = "HTM"
    Dependence = { "HLT_TRIGGER_CHAIN": "HLT_TM_TO_TC" } 
    TablesLinkedFrom = ["HLT_TM_TO_TC", "HLT_MASTER_TABLE"] 
    TablesLinkedTo = [] 








######################################
##
##    The DB Prescale Table
##
######################################
class DBHltPrescale(DBTable):
    TableName = "HLT_PRESCALE"
    Praefix   = "HPR"

    def __init__( self, parents, chain ):  # menu is a DBHltTriggerMenu
        (self.mastertable, self.dbPSset) = parents
        self.dbchain = chain

    def level_lt(self,other):
        return self.chain_level()=="L2" and other.chain_level()=="EF"

    def level_eq(self,other):
        return self.chain_level()==other.chain_level()

    def __cmp__(self, other):
        if self.chain_level()==other.chain_level():
            return self.chain_counter().__cmp__(other.chain_counter())
        if self.chain_level()=="L2" and other.chain_level()=="EF":
            return -1
        return +1
                
    def prescale(self):
        return int(self.dbchain.chain.prescale)

    def chain_counter(self):
        return self.dbchain.chain.counter

    def chain_level(self):
        return self.dbchain.chain.level

    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if traceInsert():
            print "TRACE: Prepare to insert Prescale"
        colList = []
        colList.append( Column( "HPR_PRESCALE_SET_ID", "%i" % self.dbPSset.dbID,           "I" ) )
        colList.append( Column( "HPR_CHAIN_COUNTER",   "%i" % self.dbchain.chain.counter,  "I" ) )
        colList.append( Column( "HPR_L2_OR_EF",        "%s" % self.dbchain.chain.level,    "S" ) )
        colList.append( Column( "HPR_PRESCALE",        "%i" % self.dbchain.chain.prescale, "I" ) )
        id_list = get_row_ids( cursor, self.TableName, self.Praefix, colList )
        if len(id_list)==0: # does not exist yet
            needsNewThis = True
        else:
            needsNewThis = False
            self.dbID = id_list[0]
            
        if needsNewThis:
            self.dbID = self.dbInsertThis(cursor, username, timestamp)

        return self.dbID

    def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HPR_PRESCALE_SET_ID", "%i" % self.dbPSset.dbID, "I" ) )
        colList.append( Column( "HPR_CHAIN_COUNTER",   "%i" % self.dbchain.chain.counter,  "I" ) )
        colList.append( Column( "HPR_L2_OR_EF",        "%s" % self.dbchain.chain.level,    "S" ) )
        colList.append( Column( "HPR_PRESCALE",        "%i" % self.dbchain.chain.prescale, "I" ) )
        colList.append( Column( "HPR_USERNAME",        username, "S", False ) )
        colList.append( Column( "HPR_MODIFIED_TIME",   timestamp, "T", False) )
        colList.append( Column( "HPR_USED",            "0", "S", False ) )
        dbID = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert Prescale %i for chain %i in level %s, (key = %i)" % (self.dbchain.chain.prescale,
                                                                                      self.dbchain.chain.counter,
                                                                                      self.dbchain.chain.level,
                                                                                      dbID)
        return dbID








    
######################################
##
##    The DB Prescale Set Table
##
######################################
class DBHltPrescaleSet(DBTable):
    TableName = "HLT_PRESCALE_SET"
    Praefix   = "HPS"

    def __init__( self, mastertable, menu ):  # menu is a DBHltTriggerMenu
        self.dbmenu = menu
        self.name = menu.menu.prescale_set_name
        if self.name == "":
            self.name = menu.menu.menu_name
        self.prescaleList = []
        self.mastertable = mastertable
        for chain in menu.chainList:
            self.prescaleList.append(DBHltPrescale((mastertable, self), chain))
        
    def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        # important for the decision if to create a new PRESCALE_SET table entry are
        # the menu_table_key, the name, and the list of prescales associated with the menu
        # 
        # a new table entry needs to be created in any of
        # the three cases:
        # 1) A new combination of
        #       - name (specified in the signature xml file)
        #       - and trigger_menu_id (foreign key)
        #    is requested
        # 3) The prescales that link to this table have changed 
        #
        # if a new PRESCALE_SET table entry is created, then all prescales need to be renewed
        if traceInsert():
            print "TRACE: Prepare to insert PrescaleSet '%s' and menu key %i" % (self.name, self.dbmenu.dbID)
        needsNewThis = False
        needsNewConnector = False
        matchConnId = -1
        createNewVersion = True
        
        colList = []
        colList.append( Column( "HPS_NAME",            self.name,        "S" ) )
        colList.append( Column( "HPS_TRIGGER_MENU_ID", self.dbmenu.dbID, "I" ) )
        result = get_row_ids( cursor, self.TableName, self.Praefix, colList )
        
        if result==None or len(result)==0: # Combination (Name, Trigger_Menu_Key) does not exist
            if self.mastertable.verbose:
                print "Combination (Name, Trigger_Menu_Key)=(%s,%s) does not exist" % (self.name,self.dbmenu.dbID)
            needsNewThis = True
        else:
            # the combination (Name, Trigger_Menu_Key) exists at least once
            # it needs to be checked if for one of the HPS_ID's in result
            # an exact match of the set of Prescales exists
            coscTuple = []
            self.prescaleList.sort()
            for dbps in self.prescaleList:
                coscTuple += (str(dbps.chain_level()), dbps.chain_counter(), dbps.prescale()),

            #print "That's what we want to insert", coscTuple
            numOfMatches = 0
            for hpsId in result:
                query = 'select HLT_PRESCALE.HPR_L2_OR_EF, HLT_PRESCALE.HPR_CHAIN_COUNTER, HLT_PRESCALE.HPR_PRESCALE from HLT_PRESCALE ' + \
                        'where HLT_PRESCALE.HPR_PRESCALE_SET_ID = %ld ' % hpsId + \
                        'order by HLT_PRESCALE.HPR_L2_OR_EF DESC, HLT_PRESCALE.HPR_CHAIN_COUNTER ASC'
                result2 = list(sqlutils._get_tuples(cursor, query, self.mastertable.verbose))
                # cross-check with what we would like to insert
                if result2 == coscTuple:
                    numOfMatches += 1;
                    self.dbID = int(hpsId)
            if numOfMatches==0:
                needsNewThis = True
                if traceInsert():
                    print "Found no matching prescale sets with same name (%s), same key to the Trigger-Menu (%i), and same prescales:" % (self.name,self.dbmenu.dbID)
            elif numOfMatches>1:
                needsNewThis = False
                if traceInsert():
                    print "Found %i matching prescale sets with same name (%s), same key to the Trigger-Menu (%i), and same prescales:" % (numOfMatches,self.name,self.dbmenu.dbID)
                    print "Will take the last one"
                    for ps in coscTuple:
                        print "... chain %i in level %s with prescale %i" % (ps[1],ps[0],ps[2])
            else: 
                needsNewThis = False
                if traceInsert():
                    print "Found 1 matching prescale sets with same name (%s), same key to the Trigger-Menu (%i), and same prescales:" % (self.name,self.dbmenu.dbID)
                    for ps in coscTuple:
                        print "... chain %i in level %s with prescale %i" % (ps[1],ps[0],ps[2])

        if needsNewThis:
            if traceInsert(): print "Need to insert a new Prescale_Set"
            self.dbID = self.dbInsertThis(cursor, username, createNewVersion, timestamp) # force insert (= new version)
            for dbps in self.prescaleList:
                if traceInsert():
                    print "TRACE: Insert prescale for chain %s" % dbps.dbchain.name
                dbps.dbInsert(cursor,username,timestamp)
        else:
            if traceInsert():
                print "TRACE: Return existing PrescaleSet '%s' ==> %i" % (self.name, self.dbID)
            if self.mastertable.verbose: print "Prescale set already found with HPS_ID=%i" % self.dbID

        print "==============> HLT Prescale set key for this configuration: ", self.dbID
        return self.dbID

    def dbInsertThis(self, cursor, username, createNewVersion, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        if self.mastertable.verbose: print "Running dbInsertThis for PRESCALE_SET_TABLE"
        newVersion = 1
        if createNewVersion:
            colList = []
            colList.append( Column( "HPS_NAME",  self.name, "S" ) )
            newVersion = get_lastversion(cursor, self.TableName, self.Praefix, colList) +1

        colList = []
        colList.append( Column( "HPS_NAME",            self.name,   "S" ) )
        colList.append( Column( "HPS_VERSION",         "%i" % newVersion, "F", False ) )
        colList.append( Column( "HPS_TRIGGER_MENU_ID", self.dbmenu.dbID, "I" ) )
        colList.append( Column( "HPS_USERNAME",        username,    "S", False ) )
        colList.append( Column( "HPS_MODIFIED_TIME",   timestamp,   "T", False ) )
        colList.append( Column( "HPS_USED",            "0",         "S", False ) )
        dbID = insert_row( cursor, self.TableName, self.Praefix, colList )
        if traceInsert():
            print "TRACE: Insert PrescaleSet '%s' ==> %i" % (self.name, dbID)
        return dbID
        



######################################
##
##        The DB Master Table
##
######################################
class DBMasterTable(DBTable):
    def __init__( self, menu, release_id, l2setup_id, efsetup_id, forceComponents ):
        self.name            = menu.mastertable_name
        if self.name == "":
            self.name = menu.menu_name
        self.release_id      = release_id
        self.l2setup_id      = l2setup_id
        self.efsetup_id      = efsetup_id
        self.menu            = DBHltTriggerMenu( self, menu)
        self.prescale_set    = DBHltPrescaleSet( self, self.menu)
        self.forceComponents = forceComponents
        self.verbose         = False

    def dbInsert(self, cursor, verbose = True, username="altrig", timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        dbVerbose(verbose)
        if traceInsert():
            print "TRACE: Prepare to insert MasterTable '%s'" % self.name
        menuId      = self.menu.dbInsert(cursor, username, timestamp)
        prescaleId  = self.prescale_set.dbInsert(cursor, username, timestamp)
        self.dbID   = self.dbInsertThis(cursor, menuId, prescaleId, self.release_id, username, timestamp)
        cursor.execute( "commit" )
        print "==============> HLT Masterkey for this configuration: %i" % self.dbID
        return self.dbID

    def dbInsertThis(self, cursor, menuId, prescaleId, releaseId, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
        colList = []
        colList.append( Column( "HMT_NAME", self.name, "S" ) )
        colList.append( Column( "HMT_PRESCALE_SET_ID", int(prescaleId), "I", True ) )
        colList.append( Column( "HMT_TRIGGER_MENU_ID", int(menuId),     "I", True ) )
        colList.append( Column( "HMT_FORCED_SETUP_ID", int(0),          "I", True ) )
        colList.append( Column( "HMT_RELEASE_ID",      int(releaseId),  "I", True ) )
        id = check_row( cursor, self.TableName, self.Praefix, colList )
        
        if id>0:
            if traceInsert():
                print "TRACE: Return existing HLT Master table ==> ", id
            return int(id)

        colList = []
        colList.append( Column( "HMT_NAME", self.name, "S" ) )
        newVersion = get_lastversion(cursor, self.TableName, self.Praefix, colList)+1

        colList = []
        colList.append( Column( "HMT_NAME", self.name, "S" ) )
        colList.append( Column( "HMT_VERSION", "%f" % newVersion, "F", False ) )
        colList.append( Column( "HMT_PRESCALE_SET_ID", int(prescaleId), "I", False ) )
        colList.append( Column( "HMT_TRIGGER_MENU_ID", int(menuId),     "I", True  ) )
        colList.append( Column( "HMT_FORCED_SETUP_ID", int(0),          "I", False ) )
        colList.append( Column( "HMT_RELEASE_ID",      int(releaseId),  "I", False ) )
        colList.append( Column( "HMT_USERNAME",        username,        "S", False ) )
        colList.append( Column( "HMT_MODIFIED_TIME",   timestamp,       "T", False ) )
        colList.append( Column( "HMT_USED",            "0",             "S", False ) )
        colList.append( Column( "HMT_STATUS",          0,               "I", False ) )

        id = insert_row( cursor, self.TableName, self.Praefix, colList )

        if traceInsert():
            print "TRACE: Insert HLT Master table ==> ", id
        return id
    

    TableName = "HLT_MASTER_TABLE"
    Praefix = "HMT"



######################################
##
##        The DB Super Master Table
##
######################################
def createSMEntry(cursor, newname, HLT_masterKey, SMK_baseKey, verbose, username="altrig", timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
    """Function to create an entry in the Super_Master_Table,
    that links to the given HLT_masterKey, but is otherwise
    identical with the entry given by SMK_baseKey
    If SMK_baseKey==0 then the latest(highest) key in the
    SUPER_MASTER_TABLE is used
    If SMK_baseKey==0 and there is no SMKey entry yet
    a new one will be created
    """

    # first look for identical SM entry (HLT key, name)
    if SMK_baseKey == 0:
        query = "select SMT_ID from SUPER_MASTER_TABLE where SMT_HLT_MASTER_TABLE_ID=%i" % HLT_masterKey
        if isinstance(query, unicode): query = str(query)
        cursor.execute( query )
        result = cursor.fetchall();
        if len(result) != 0:
            SMK_baseKey = result[0][0]
            if traceInsert():
                print "TRACE: Super-Master-Table ", SMK_baseKey, " already links to HLT Mastertable with ID ==> ", HLT_masterKey
            print "==============> Super-Master-Key for this configuration: ", SMK_baseKey
            return SMK_baseKey

    
    print "Trying to create an entry in the supermastertable based on key %i" % SMK_baseKey
    if SMK_baseKey != 0:
        query = "select SMT_NAME, SMT_L1_MASTER_TABLE_ID, SMT_HLT_MASTER_TABLE_ID from SUPER_MASTER_TABLE where SMT_ID=%i" % SMK_baseKey
        if isinstance(query, unicode): query = str(query)
        cursor.execute( query )
        result = cursor.fetchall()
        if len(result)==0:
            print "No Super-Master-Key %i found in the TriggerDB" % SMK_baseKey
            print "Make sure you chose an existing key as base or try --smkeybase 0"
            sys.exit(1)
        else:
            (name, l1_ID, hlt_ID) = result[0];
            if hlt_ID == HLT_masterKey:
                if traceInsert():
                    print "TRACE: Super-Master-Table ", SMK_baseKey, " already links to HLT Mastertable with ID ==> ", HLT_masterKey
                print "==============> Super-Master-Key for this configuration: ", SMK_baseKey
                return SMK_baseKey


    if SMK_baseKey == 0:
        # select the highest tableId as key
        query = "select MAX(SMT_ID) from SUPER_MASTER_TABLE"
        if isinstance(query, unicode): query = str(query)
        cursor.execute( query )
        result = cursor.fetchall()[0][0]
        if result==None: # no supermasterkey at all, should not happen, except during the test phase...
            print "No Super-Master-Key in the database, will try to add an entry into the Super_Master_Table with links to the new HLT setup and to the LVL1 master table with ID 1"
            ( name, l1_ID ) = ( newname, 1 )
        else:
            SMK_baseKey = int(result)
            print "No particular SMK_base specified => will use latest SMT_ID (%i) to insert HLT MasterKey %i" % (SMK_baseKey,HLT_masterKey)
            query = "select SMT_NAME, SMT_L1_MASTER_TABLE_ID, SMT_HLT_MASTER_TABLE_ID from SUPER_MASTER_TABLE where SMT_ID=%i" % SMK_baseKey
            if isinstance(query, unicode): query = str(query)
            cursor.execute( query )
            (name, l1_ID, hlt_ID) = cursor.fetchall()[0];
            if newname!="": name=newname
            if hlt_ID == HLT_masterKey:
                print "TRACE: Super-Master-Table ", SMK_baseKey, " already links to HLT Mastertable with ID ", HLT_masterKey
                print "==============> Super-Master-Key for this configuration: ", SMK_baseKey
                return SMK_baseKey

    # find the latest version with that name
    query = "select SMT_VERSION from SUPER_MASTER_TABLE where SMT_NAME='%s' order by SMT_VERSION DESC" % name
    if isinstance(query, unicode): query = str(query)
    cursor.execute( query )
    result = cursor.fetchall()
    if len(result) == 0:
        newversion = 1;
    else:
        newversion = result[0][0]+1;
        
        
    print "Inserting %s with version %i" % (name, newversion)
    # insert
    colList = []
    colList.append( Column( "SMT_NAME",                name,               "S" ) )
    colList.append( Column( "SMT_VERSION",             "%i" % newversion,  "F", False ) )
    colList.append( Column( "SMT_L1_MASTER_TABLE_ID",  int(l1_ID),         "I", False ) )
    colList.append( Column( "SMT_HLT_MASTER_TABLE_ID", int(HLT_masterKey), "I", True ) )
    colList.append( Column( "SMT_USERNAME",            username,           "S", False ) )
    colList.append( Column( "SMT_MODIFIED_TIME",       timestamp,          "T", False ) )
    colList.append( Column( "SMT_USED",                "0",                "S", False ) )
    colList.append( Column( "SMT_STATUS",              0,                  "I", False ) )

    id = insert_row( cursor, "SUPER_MASTER_TABLE", "SMT", colList )
    cursor.execute( "commit" )
    print "==============> Super-Master-Key for this configuration: %i (%s)" % (id, name)
    return id


######################################
##
##        Insert Dummy Setup
##
######################################
def insertDummySetup(cursor, username="altrig", timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
    colList = []
    colList.append( Column( "HST_NAME",          "dummy", "S" ) )
    colList.append( Column( "HST_VERSION",       "%i" % 0, "I", False ) )
    colList.append( Column( "HST_USERNAME",      username, "S", False ) )
    colList.append( Column( "HST_MODIFIED_TIME", timestamp, "T", False) )
    colList.append( Column( "HST_USED",          "0", "S", False ) )                    
    id = check_row( cursor, "HLT_SETUP", "HST", colList )
    if id<0:
        id = insert_row( cursor, "HLT_SETUP", "HST", colList )
    return id
