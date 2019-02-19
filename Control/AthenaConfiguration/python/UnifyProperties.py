# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# A collection of methods to unify/merge list-properties
# ToDo: Define the merging-method when defining the property

from AthenaCommon.Logging import logging
log=logging.getLogger('ComponentAccumulator')

def unifySet(prop1,prop2):
    #May want to strip whitespace in case the params are lists of strings
    s1=set(prop1)
    s2=set(prop2)
    su=s1 | s2
    return list(su)

def unifySetVerbose(prop1,prop2):
    log.debug("In UnifyProperties: unifying sets" )
    log.debug( str(prop1) )
    log.debug( "and" )
    log.debug( str(prop2) )
    return unifySet(prop1,prop2)

def unifySetOfPairs(prop1,prop2):

    def getPairs(seq):
        r=set()
        nPairs=int(len(seq)/2)
        if (2*nPairs!=len(seq)):
            from AthenaConfiguration.ComponentAccumulator import ConfigurationError
            raise ConfigurationError("Expected a sequence with even number of elements")

        for i in range(0,nPairs):
            r.add((seq[2*i],seq[2*i+1]))
        return r

    setOfPairs1=getPairs(prop1)
    setOfPairs2=getPairs(prop2)
    unionOfPairs=(setOfPairs1 | setOfPairs2)
    finallist=[]
    for p in unionOfPairs:
        finallist+=p
    return finallist



_propsToUnify={"GeoModelSvc.DetectorTools":unifySet,
               "CondInputLoader.Load":unifySet,
               "IOVDbSvc.Folders":unifySet,
               "IOVDbSvc.FoldersToMetaData":unifySet,
               "EvtPersistencySvc.CnvServices":unifySet,
               "PoolSvc.ReadCatalog":unifySet,
               "ProxyProviderSvc.ProviderNames":unifySet,
               "TagInfoMgr.ExtraTagValuePairs":unifySetOfPairs,
               "AthenaOutputStream.ItemList":unifySet,
               "AthenaPoolCnvSvc.PoolAttributes":unifySet,
               "*.HypoTools": unifySet,
               "AtDSFMTGenSvc.Seeds": unifySet,
               "AtRanluxGenSvc.Seeds": unifySet,
               "AtRndmGenSvc.Seeds": unifySet,
               }

def setUnificationFunction(key, function):
    _propsToUnify[key] = function

def getUnificationKey(propname):
    if propname in _propsToUnify:
        return propname
    try:
        objectName, variableName = propname.split('.')[-2:]

        matchingByVariable = '*.{}'.format(variableName)
        if matchingByVariable in _propsToUnify:
            return matchingByVariable

        matchingByObject = '{}.*'.format(objectName)
        if matchingByObject in _propsToUnify:
            return matchingByObject

    except Exception:
        pass

    return None


def matchProperty(propname):
    return getUnificationKey(propname) is not None


def getUnificationFunc(propname):
    unificationKey = getUnificationKey(propname)
    if unificationKey is None:
        return None
    return _propsToUnify[unificationKey]


def unifyProperty(propname,prop1,prop2):
    unificationFunc = getUnificationFunc(propname)

    if unificationFunc is None:
        from AthenaConfiguration.ComponentAccumulator import DeduplicationFailed
        raise DeduplicationFailed("List property %s defined multiple times with conflicting values.\n " % propname \
                                      + str(prop1) +"\n and \n" +str(prop2) \
                                      + "\nIf this property should be merged, consider adding it to AthenaConfiguration/UnifyProperties.py")
    return unificationFunc(prop1,prop2)


# self test
import unittest

class BasicTest( unittest.TestCase ):
    def runTest( self ):
        unified = unifyProperty("Alg.HypoTools", ["a", "b"], ["c", "b"])
        self.assertEqual( sorted(unified), sorted(["a", "b", "c"]), "Hypo tools unification failed" )

        setUnificationFunction("*.HypoTools", unifySetVerbose)
        log.setLevel(7)
        unified = unifyProperty("Alg.HypoTools", ["a", "b"], ["c", "b"])
        self.assertEqual( sorted(unified), sorted(["a", "b", "c"]), "Hypo tools unification failed" )

