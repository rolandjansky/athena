#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## 
# @file CoolDozer/python/__init__.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Custom duty.

import sys
import os
if ( sys.hexversion <= 0x20501f0 ):
    raise RuntimeError("too old python version to run CoolDozer API! use python >= 2.5!")
try:
    os.environ["RTT"] or sys.rtt or thisIsRTT
    
    apiPath = "/afs/cern.ch/atlas/project/RTT/prod/Results/dozer/api/latest"
    xmlPath = "/afs/cern.ch/atlas/project/RTT/prod/Results/dozer/api/xml"
    
    sys.path = [ apiPath ] + sys.path 
    if ( os.environ["XMLPATH"] ):
        os.environ["XMLPATH"] = xmlPath + ":" + os.environ["XMLPATH"]
    else:
        os.environ["XMLPATH"] = xmlPath
    if ( os.environ["CORAL_AUTH_PATH"] ):
        os.environ["CORAL_AUTH_PATH"] = xmlPath + ":" + os.environ["CORAL_AUTH_PATH"]
    else:
        os.environ["CORAL_AUTH_PATH"] = xmlPath
    if ( os.environ["CORAL_DBLOOKUP_PATH"] ):
        os.environ["CORAL_DBLOOKUP_PATH"] = xmlPath + ":" + os.environ["CORAL_DBLOOKUP_PATH"]
    else:
        os.environ["CORAL_DBLOOKUP_PATH"] = xmlPath 
    
    print "running from rtt prod"
    print "apiPath = " + apiPath
    print "XMLPATH " + str(os.environ["XMLPATH"])
    print "CORAL_AUTH_PATH " + str(os.environ["CORAL_AUTH_PATH"])
    print "CORAL_DBLOOKUP_PATH " + str(os.environ["CORAL_DBLOOKUP_PATH"])

except (AttributeError, KeyError):
    print "running from release"
    xmlPath = "/afs/cern.ch/atlas/project/RTT/prod/Results/dozer/api/xml"
    if ( os.environ["XMLPATH"] ):
        os.environ["XMLPATH"] = xmlPath + ":" + os.environ["XMLPATH"]
    else:
        os.environ["XMLPATH"] = xmlPath
    if ( os.environ["CORAL_AUTH_PATH"] ):
        os.environ["CORAL_AUTH_PATH"] = xmlPath + ":" + os.environ["CORAL_AUTH_PATH"]
    else:
        os.environ["CORAL_AUTH_PATH"] = xmlPath
    if ( os.environ["CORAL_DBLOOKUP_PATH"] ):
        os.environ["CORAL_DBLOOKUP_PATH"] = xmlPath + ":" + os.environ["CORAL_DBLOOKUP_PATH"]
    else:
        os.environ["CORAL_DBLOOKUP_PATH"] = xmlPath 
    pass

from DozerUtils import DozerLogger, DozerError, DozerVersion, DozerSingleton
from DozerObject import DozerObject
from DozerValid import DozerValid
from DozerIOV import DozerIOV
from DozerData import DozerData
from DozerChannel import DozerChannel
from DozerSchemaConfig import DozerSchemaConfig
from DozerSchema import DozerSchema
from DozerRecordSet import DozerRecordSet
from DozerGraph import DozerGraph, DozerStyle
from DozerConfig import DozerConfig, DozerFolder
from DozerDB import DozerDB, DozerRTConfig
from DozerApp import DozerApp
from RTTDozerApp import RTTDozerApp

__all__ = [ "DozerLogger", "DozerError", "DozerVersion", "DozerSingleton",
            "DozerObject", "DozerValid", "DozerIOV",
            "DozerData", "DozerChannel",  "DozerSchemaConfig",
            "DozerSchema", "DozerRecordSet", "DozerGraph", "DozerStyle",
            "DozerConfig", "DozerFolder", "DozerDB", "DozerRTConfig", "DozerApp", "RTTDozerApp" ]


