# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# A collection of methods to unify/merge list-properties
# ToDo: Define the merging-method when defining the property


def unifySet(prop1,prop2):
    #May want to strip whitespace in case the params are lists of strings
    s1=set(prop1)
    s2=set(prop2)
    su=s1 | s2
    return list(su)

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
               }


def unifyProperty(propname,prop1,prop2):
    if propname not in _propsToUnify:
        from AthenaConfiguration.ComponentAccumulator import DeduplicationFailed
        raise DeduplicationFailed("List property %s defined multiple times with conflicting values.\n " % propname \
                                      +"If this property should be merged, consider adding it to AthenaConfiguration/UnifyProperties.py")
    return _propsToUnify[propname](prop1,prop2)
