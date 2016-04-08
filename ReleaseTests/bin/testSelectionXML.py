#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import xml.etree.ElementTree as ET
import re
import commands

# list = commands.getstatusoutput("cmt show packages /afs/cern.ch/work/s/seuster/sel | gawk '{print $3 \"/\" $1}'")[1].split()
list = commands.getstatusoutput("cmt show packages | gawk '{print $3 \"/\" $1}'")[1].split()
# list = os.getenv("CMTPATH").split(':')

# list = ['/afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasAnalysis/rel_2/Reconstruction/egamma/egammaPhysCalib/egammaPhysCalib/selection.xml']

ErrorList = {}
ErrorContent = {}
GUIDList = {}
GUIDListDup = {}

print "Processing these packages:"
for index, item in enumerate(list):
    print index, item

for path in list:
    #print path
    from os.path import join, getsize
    for root, dirs, files in os.walk(path):
        if os.path.exists(root):
            for name in files:
                path=join(root, name)
                if os.path.exists(path) and name=="selection.xml":
                    #print ("file ", path," size ",getsize(path))
                    with open(path, 'r') as content_file:
                        content = content_file.read()
                    old="different"
                    while old != content:
                        old = content
                        content = re.sub(r"(class +[a-zA-Z]+ *= *\")([a-zA-Z0-9:_* ,&;]+)<([a-zA-Z0-9:_* ,<&;]+)>([a-zA-Z0-9:_* ,>&;]*)", r"\1\2&lt;\3&gt;\4", content)
                        #print "TEST"
                        #print
                        #print content

                    try:
                        tree = ET.fromstring(content)
                        #for child in tree:
                        #    print child.tag, child.attrib
                        for neighbor in tree.findall('./class'):
                            guid=neighbor.get("id")
                            clnm=neighbor.get("name")
                            #print "id: ",clnm,guid
                            if guid != None:
                                if guid in GUIDList:
                                    print "Duplicate ! ", guid, " found twice !!"
                                    print " 1. ", GUIDList[guid]
                                    print " 2. ", path, " (", clnm, ")"
                                    GUIDListDup[guid]=path + " (" + clnm + ")"
                                else:
                                    GUIDList[guid]=path + " (" + clnm + ")"

                    except Exception as e:
                        #print "ERROR: ", type(e)
                        #print "ERROR: ", e.args
                        #print "ERROR: ", content
                        # ErrorList.append(path)
                        ErrorList[path]=e.args
                        ErrorContent[path]=content
                        pass
                   
if len(ErrorList):
    print "Failed reading these selection.xml files:"
    for index, item in enumerate(ErrorList):
        print index, item, ErrorList[item]
        print "  Error was : ", ErrorList[item]
        print "  selection.xml was: "
        print ErrorContent[item]
else:
    print "All selection.xml files in all packages were read successfully"
    
if len(GUIDListDup):
    print "Found following Duplicates in selection.xml:"
    for index, item in enumerate(GUIDListDup):
        print index, item, GUIDListDup[item]
        print "                     also declared in ", GUIDList[item]
        
import sys
sys.exit(len(GUIDListDup))
