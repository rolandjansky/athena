#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys,os

def main():

    if len(sys.argv)<=1:
        print("Usage:\n%s TestGeomety.xml" % sys.argv[0])
        return 1

    xmlfile = sys.argv[1]

    checkResult = os.system("get_files -xmls -symlink MUCTPIGeometry.dtd")
    if checkResult != 0:
        print("ERROR Could not find MUCTPIGeometry.dtd in $DATAPATH")
        return 1
    
    checkResult = os.system("xmllint --noout --dtdvalid MUCTPIGeometry.dtd %s" % xmlfile)
    if checkResult == 0:
        print("XML file %s is conform with MUCTPIGeometry.dtd" % xmlfile)
    else:
        print("ERROR the XML does not follow the document type definition MUCTPIGeometry.dtd")
        return 1

    return 0

if __name__=="__main__":
    sys.exit(main())
