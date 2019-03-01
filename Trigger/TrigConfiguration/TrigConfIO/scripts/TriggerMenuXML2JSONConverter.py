#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys

def main():
    if len(sys.argv)<=1:
        print "please use this format:"
        print "%s inputfile.xml [outputfile.json]   ... menu file conversion" % sys.argv[0].rsplit('/')[-1]
        print "%s inputfile.json                    ... json conformity check of any json file" % sys.argv[0].rsplit('/')[-1]
        return 1

    inputfile = sys.argv[1]
    outputfile = sys.argv[2] if len(sys.argv)>2 else None
    
    from TrigConfIO.MenuXML2JSONConverter import XML2JsonConverter
    converter = XML2JsonConverter()

    if inputfile.endswith(".xml"):
        converter.convertFile( inputfile, outputfile )
    elif inputfile.endswith(".json"):
        converter.checkJsonConformity( inputfile )

if __name__=="__main__":
    sys.exit(main())
