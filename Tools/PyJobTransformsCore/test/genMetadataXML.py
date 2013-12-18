#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
#  genMetadataXML.py
#  
#
#  Created by Alvin on 10/05/2010.
#
from __future__ import with_statement
import sys
import cPickle as pickle

usage = "genMetadataXML.py JOB_REPORT_PICKLE [--new|--old]"
if len( sys.argv ) < 2:
    print usage
    sys.exit( 1 )
with open( sys.argv[ 1 ] ) as f:
    r = pickle.load( f )
try:
    optParam = sys.argv[ 2 ]
except IndexError:
    optParam = '--old'
if optParam == '--new':
    r.writeMetaDataXML_new()
elif optParam == '--old':
    r.writeMetaDataXML_old()
else:
    print usage
    sys.exit(1)
sys.exit(0)
