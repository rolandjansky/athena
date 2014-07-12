# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import ROOT

def getMetadataKeys(f):
    mddir = f.Get('HanMetadata_')
    if not mddir:
        return []
    else:
        return [x.GetName() for x in mddir.GetListOfKeys()]

def getMetadata(f, key):
    rv = {}
    mddir = f.Get('HanMetadata_')
    if not mddir:
        return rv
    else:
        keydir = mddir.Get(key)
        if not keydir:
            return rv
        else:
            for subkey in keydir.GetListOfKeys():
                subkeydirlist = keydir.Get(subkey.GetName()).GetListOfKeys()
                if len(subkeydirlist) < 1:
                    continue
                rv[subkey.GetName()] = subkeydirlist[0].GetName()
            return rv

def addMetadata(f, key, mapping):
    import ROOT
    mddir = f.Get('HanMetadata_')
    if not mddir:
        mddir = f.mkdir('HanMetadata_')
    keydir = mddir.Get(key)
    if not keydir:
        keydir = mddir.mkdir(key)
    for subkey in mapping:
        if keydir.Get(subkey):
            keydir.rmdir(subkey)
        subkeydir = keydir.mkdir(subkey)
        val = mapping[subkey]
        valstr = ROOT.TObjString(val)
        subkeydir.WriteTObject(valstr, val)
    f.Flush()
