# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__  = 'Javier Montejo'
__version__="$Revision: 2.0 $"
__doc__="Interface to retrieve lists of unprescaled triggers according to types and periods"

import os.path
import json
from TriggerMenuMT.TriggerAPI.TriggerInfo import TriggerInfo, TriggerChain
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod
from PathResolver import PathResolver
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.TriggerAPI' )
privateJsonFile = "TriggerInfo.json"

def dump(db, f=privateJsonFile):
    dumpJson(db, f)

def load():
    centralJsonFile = PathResolver.FindCalibFile("TriggerMenu/TriggerInfo_20210302.json")
    if centralJsonFile: 
        log.info("Found json file:"+centralJsonFile)
        centralJsonFile = os.path.realpath(centralJsonFile)
    else: log.error("Couldn't find central json file")

    if centralJsonFile:
        try:
            db = loadJson(centralJsonFile)
        except (json.JSONDecodeError, ValueError):
            log.info("Reading cached information failed")
            db = {}
    else:
        db = {}
    try:
        privatedb = loadJson(privateJsonFile)
        db.update(privatedb)
    except (json.JSONDecodeError, ValueError):
        log.error("Error loading the private json file")
    except IOError:
        pass
    return db

def dumpJson(db, jsonFile):
    with open(jsonFile, "w") as f:
        log.info("Dumping cached information to: "+jsonFile)
        json.dump( toJsonDump(db) , f, cls=TriggerAPIEncoder)

def loadJson(jsonFile):
    with open(jsonFile) as f:
        dct = json.load(f)
    dct = fromJsonDump(dct)
    return dct

def fromJsonDump(thelist):
    #Rebuild objects and dictionary structure from json
    toret = {}
    for key,thedict in thelist:
        pk, grl = key
        pk = TriggerPeriod.fromName( pk )
        key = (pk, grl)
        triggerinfo = TriggerInfo()
        triggerinfo.period  = TriggerPeriod.fromName( thedict["period"] )
        #key and internal period value should be consistent
        assert triggerinfo.period==pk, (triggerinfo.period, pk)
        triggerinfo.totalLB = thedict["totalLB"]
        triggerinfo.triggerChains = []
        for tc in thedict["triggerChains"]:
            tc = TriggerChain(tc["name"],tc["l1seed"],tc["livefraction"],tc["activeLB"],tc["hasRerun"])
            triggerinfo.triggerChains.append(tc)
        toret[key] = triggerinfo
    return toret
        
def toJsonDump(dct):
    #tuple keys are not allowed, convert to list of pairs
    #Dump the enum name instead of the value to allow flexibility in the future
    return [ ((TriggerPeriod.toName(p) ,grl ),v) for (p,grl),v in dct.items()]

class TriggerAPIEncoder(json.JSONEncoder):
    def default(self, obj):
        if hasattr(obj,'toJSON'):
            return obj.toJSON()
        else:
            return json.JSONEncoder.default(self, obj)
