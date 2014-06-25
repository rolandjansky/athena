#!/usr/bin/env python

import getopt, sys
import xml.dom.minidom
import time

from xml.dom.minidom import Document
from xml.dom.minidom import DocumentType
from xml.dom.ext import PrettyPrint

def usage():
    """Prints a help message"""
    print "Usage: %s --l2menu <l2menu.xml> --efmenu <efmenu.xml>  [--output <hltmenu.xml>]" % \
          sys.argv[0].split('/')[-1]
    print "  -h|-?|--help                Issues this help message"
    print "     --l2menu <l2menu.xml>    Level 2 Setup (required)"
    print "     --efmenu <efmenu.xml>    Event Filter Setup (required)"
    print "  -o|--output <hltmenu>       output file [default: hltmenu.xml]"
    print "  -n|--name   <string>        name of the new menu [by default it is a copy from the efmenu]"


def readxmlfile(xmlfile):
    start = time.time()
    tmpdoc = xml.dom.minidom.parse(xmlfile)
    end = time.time()
    print "[%s] Reading %s" % (sys.argv[0], a),
    print "took %1.2f second(s)" % (end - start)
    return tmpdoc


if __name__ == '__main__':
    
    short_opt = "h?o:n:"
    long_opt = ['help', 'l2menu=', 'efmenu=', 'output=', 'name=']
    
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

    output = 'hltMenu.xml'
    name   = 'XML'

    doc_map = {}

    for o, a in opts:
        if o in ['-h', '-?', '--help']:
            usage()
            sys.exit(0)
        if o in ['--l2menu']: doc_map[ 'l2menu' ] = readxmlfile(a)
        if o in ['--efmenu']: doc_map[ 'efmenu' ] = readxmlfile(a)
        if o in ['-o', '--output']: output = a
        if o in ['-n', '--name']: name = a

    # check if all necessary xml setup files were supplied
    if not 'l2menu' in doc_map:
        print "\nerror: No Level 2 menu supplied\n"
        usage()
        sys.exit(1)
    if not 'efmenu' in doc_map:
        print "\nerror: No EF menu supplied\n"
        usage()
        sys.exit(1)

    # start the merging

    l2ChainList = doc_map[ 'l2menu' ].getElementsByTagName( "CHAIN_LIST" )[0]
    efChainList = doc_map[ 'efmenu' ].getElementsByTagName( "CHAIN_LIST" )[0]

    l2SeqList   = doc_map[ 'l2menu' ].getElementsByTagName( "SEQUENCE_LIST" )[0]
    efSeqList   = doc_map[ 'efmenu' ].getElementsByTagName( "SEQUENCE_LIST" )[0]


    doc = Document()
    doctype = DocumentType( "HLT_MENU" )
    #doctype.systemId = "hlt_menu.dtd"
    doc.appendChild( doctype );
    hlt_menu = doc.createElement( "HLT_MENU" )
    if name == 'XML':
        efmenu_el = doc_map[ 'efmenu' ].getElementsByTagName( "HLT_MENU" )[0]
        name = efmenu_el.getAttribute("menu_name")
        prescale_set_name = efmenu_el.getAttribute("prescale_set_name")
        hlt_menu.setAttribute( "prescale_set_name", prescale_set_name )
    hlt_menu.setAttribute( "menu_name", name )
    doc.appendChild( hlt_menu )

    # merge sequences
    processedSeq = []
    seqList = doc.createElement("SEQUENCE_LIST")
    hlt_menu.appendChild( seqList )
    for seq in l2SeqList.getElementsByTagName('SEQUENCE') + efSeqList.getElementsByTagName('SEQUENCE'):
        outputTE = seq.getAttribute("output")
        if outputTE in processedSeq: continue
        processedSeq += [ outputTE ]
        seqList.appendChild( seq.cloneNode( True ))

    # merge chains
    processedChains = []
    chList = doc.createElement("CHAIN_LIST");
    hlt_menu.appendChild( chList )
    for ch in l2ChainList.getElementsByTagName('CHAIN') + efChainList.getElementsByTagName('CHAIN'):
        chName = ch.getAttribute("chain_name")
        if chName in processedChains: continue
        processedChains += [ chName ]
        chList.appendChild( ch.cloneNode( True ))



    print "Writing merged menu to %s" % output
    PrettyPrint(doc, open(output, "w"))
