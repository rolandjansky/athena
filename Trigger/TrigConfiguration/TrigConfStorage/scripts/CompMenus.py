#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys
from optparse import OptionParser
from xml.dom import expatbuilder



def getFileLvl(filename):
    builder = expatbuilder.ExpatBuilder()
    fp = open(filename, 'rb')
    buff = fp.read().replace('&','&amp;')
    doc = builder.parseString(buff)
    for x in doc.childNodes:
        if x.nodeName == u'TOPO_MENU':
            return 'L1Topo'
        if x.nodeName == u'LVL1Config':
            return 'L1'
        if x.nodeName == u'HLT_MENU':
            return 'HLT'
        if str(x.nodeName).lower() == 'setup':
            return 'SETUP'
    raise RuntimeError("%s is neither L1 nor HLT menu, nor a SETUP file" % filename)



if __name__ == '__main__':
    
    parser = OptionParser(usage="usage: %prog <options> 'Menu1.xml Menu2.xml'",
                          epilog="%prog is a program to compare two menu.xml files, either L1 or HLT" )

    parser.add_option( "-e",
                       "--env",
                       default='default',
                       dest = "excl",
                       help = "The comparison environment e.g. cool" )

    parser.add_option( "-t",
                       "--html",
                       action="store_true",
                       default=False,
                       dest = "html",
                       help = "The output format" )

    parser.add_option( "-v",
                       action="count",
                       dest = "verbose",
                       help = "verbose flag" )



    (options, args) = parser.parse_args()

    if len(args)!=2:
        parser.error("specify xml files")
        sys.exit(1)

    filetype = [getFileLvl(x) for x in args]

    if filetype[0] != filetype[1]:
        print("ERROR: Comparing apples and oranges:")
        print("  First  file: %s" % filetype[0])
        print("  Second file: %s" % filetype[1])
        sys.exit(0)

    if filetype[0] == 'L1Topo':
        from TrigConfStorage.CompL1TopoMenu import CompareL1TopoXML
        compator = CompareL1TopoXML (tuple(args), excl=options.excl, verbose=options.verbose)
    elif filetype[0] == 'L1':
        from TrigConfStorage.CompL1Menu import CompareL1XML
        compator = CompareL1XML (tuple(args), excl=options.excl, verbose=options.verbose)
    elif filetype[0] == 'HLT':
        from TrigConfStorage.CompHLTMenu import CompareHLTXML
        compator = CompareHLTXML(tuple(args), excl=options.excl, verbose=options.verbose)
    else:
        from TrigConfStorage.CompSetup import CompareSetupXML
        compator = CompareSetupXML(tuple(args), excl=options.excl, verbose=options.verbose)
        
    print(compator.diff())
