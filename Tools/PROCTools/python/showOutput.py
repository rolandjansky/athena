#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re
import sys
import six

def getOutputDictFromAMI(tag):
    try:
        from pyAMI.pyAMI import AMI
        amires = AMI().execute(["ListConfigurationTag","-configTag=%s" % tag])
        d = amires.getDict()['rowset_%s'%tag][tag]
        outDict = eval(d.get('outputs','None'))
    except Exception as e:
        print ("Failed to get OutputDict from AMI")
        print (e)
        return None
    return outDict


if __name__=='__main__':
    if len(sys.argv)!=3:
        print ("Syntax:")
        print (sys.argv[0],"<ami-tag> <input dataset name>")
        sys.exit(-1)

    tag=sys.argv[1]
    dsname=sys.argv[2]
    print ("Getting output for",dsname,"with AMI configuration tag",tag )

    outDict=getOutputDictFromAMI(tag) 
    if outDict is None:
        sys.exit(-1)

    print ("Outputs:")

    for name,outDef in six.iteritems (outDict):
        if 'ifMatch' in outDef:
            if re.match(outDef['ifMatch'],dsname):
                print ("\t",name)
