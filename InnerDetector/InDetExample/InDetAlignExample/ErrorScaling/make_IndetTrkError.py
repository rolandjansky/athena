#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Write out error scalings that are read in from python fragments.
# The fragment should define dataset and tag
# If the keyword "nominal" is passed as one of the arguments then
# nominal error scalings (a=1,c=0) will be writen.
# See usage:

import TrkErrorScalingDB
import sys

nominal_tag = 'IndetTrkErrorScaling_nominal'
nominal_dataset=[ ("PixPhi Barrel",1.0,0.0),
                  ("PixEta Barrel",1.0,0.0),
                  ("PixPhi Endcap",1.0,0.0),
                  ("PixEta Endcap",1.0,0.0),
                  ("SCT Barrel",1.0,0.0),
                  ("SCT Endcap",1.0,0.0),
                  ("TRT Barrel",1.0,0.0),
                  ("TRT Endcap",1.0,0.0)
                  ]

foldername="/Indet/TrkErrorScaling"
#connect="sqlite://X;schema=mycool.db;dbname=COMP200"
connect="sqlite://X;schema=mycool.db;dbname=OFLP200"


args = sys.argv[1:]

if (len(args) > 0):
    for inputfile in args:
        if (inputfile == "nominal"):
            print "Writing nominal error scalings"
            TrkErrorScalingDB.make(nominal_dataset, nominal_tag, foldername, connect)
        else:
            if "dataset" in dir(): del dataset
            if "tag" in dir(): del tag
            print "Reading in",inputfile
            execfile(inputfile)
            if (not "dataset" in dir()) or (not "tag" in dir()):
                print "ERROR: dataset or tag not defined in",inputfile
            else:
                TrkErrorScalingDB.make(dataset, tag, foldername, connect)
        
else:
    print "usage:",sys.argv[0],"[param1.py [param2.py [...]]]"
    print '  If the string "nominal" is used as an argument then nominal'
    print '  scalings will be written (a=1,c=0)'


