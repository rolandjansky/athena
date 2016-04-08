#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PyJobTransformsCore.VTimer

def getTimerInfo(vtimercontent):
    d=vtimercontent.toDict()
    #For subtraction we only want the deltas and only the times, not the absolute endpoints:
    out={}
    for k,v in d.items():
        if k.endswith('Time'):
            out[k]=v
    if 'wallTime' in out.keys() and 'cpuTime' in out.keys():
        out['cpu2wallRatio']=out['cpuTime']/out['wallTime']
    return out

def transformTimerFormatOk(transformTimerDict):
    keys=transformTimerDict.keys()
    if not (len(keys)>=3 and 'name' in keys and 'trfList' in keys and 'vTimerContent' in keys):
        return False
    for subinfo in transformTimerDict['trfList']:
        if not transformTimerFormatOk(subinfo):
            return False
    return True

def extractExclusiveInfo(info,out=None,nameAccum=None):
    if not out:
        out={}
        out['measurements']=[]
        out['steps']=[]
        out['stepinfo']={}
    name=info['name']
    nameAccum = name if not nameAccum else nameAccum+'/'+name
    subtrfs=info['trfList']
    if subtrfs:
        for subtrf in subtrfs:
            extractExclusiveInfo(subtrf,out,nameAccum)
    else:
        stepname=nameAccum.replace(' ','_')# if not nameAccum.endswith('/Other') else nameAccum.replace('/Other','/Exclusive')
        out['steps']+=[stepname]
        stepinfo=getTimerInfo(info['vTimerContent'])
        out['stepinfo'][stepname]=stepinfo
        for meas in stepinfo.keys():
            if not meas in out['measurements']:
                out['measurements']+=[meas]
    return out

def getDictFromPickleFile(filename):
    fh=None
    if filename.endswith('.gz'):
        import gzip
        fh=gzip.open(filename)
    else:
        fh=open(filename)
    if not fh:
        return
    import cPickle as pickle
    d=pickle.load(fh)
    return d

def getExclusiveInfoFromPickleFile(filename):
    d=getDictFromPickleFile(filename)
    if not d:
        return
    return extractExclusiveInfo(d)
