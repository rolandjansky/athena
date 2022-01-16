#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
##=============================================================================
## Name:        RecoFunctions.py
## Author:      David Cote (DESY)
## Created:     July 2008
## Description: This is a collection of utility functions for RecExCommon
##=============================================================================

from AthenaCommon.GlobalFlags  import globalflags

def RunInnerDetectorOnly():
    from RecExConfig.RecFlags import rec
    rec.doCalo=False
    rec.doMuon=False
    rec.doJetMissingETTag=False
    rec.doEgamma=False
    rec.doMuonCombined=False
    rec.doTau=False
    return


def ListOfTupleToDic(lst):
    if lst is None:
        return {}

    import collections as _c
    dic= _c.defaultdict(list)

    for tup in lst:
        k,v = tup[0], tup[1]
        dic[k].append(v)
    return dict(dic)


def InputFileNames():
    from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
    if athenaCommonFlags.FilesInput()!=[]:
        return athenaCommonFlags.FilesInput()
    from RecExConfig.RecFlags import rec
    inFiles=None

## FIXME
##     if (not athenaCommonFlags.PoolTAGInput.isDefault() or rec.readTAG() ):
##         inFiles=athenaCommonFlags.PoolTAGInput()
##     elif (not athenaCommonFlags.PoolAODInput.isDefault() or rec.readAOD() ):
##         inFiles=athenaCommonFlags.PoolAODInput()
##     elif (not athenaCommonFlags.PoolESDInput.isDefault() or rec.readESD() ):
##         inFiles=athenaCommonFlags.PoolESDInput()
##     elif not athenaCommonFlags.BSRDOInput.isDefault():
##         inFiles=athenaCommonFlags.BSRDOInput()
##     elif not athenaCommonFlags.PoolRDOInput.isDefault():
##         inFiles=athenaCommonFlags.PoolRDOInput()

    if rec.readTAG():
        inFiles=athenaCommonFlags.PoolTAGInput()
    elif rec.readAOD():
        inFiles=athenaCommonFlags.PoolAODInput()
    elif rec.readESD():
        inFiles=athenaCommonFlags.PoolESDInput()

    elif rec.readRDO():
        from AthenaCommon.GlobalFlags  import globalflags
        if globalflags.InputFormat=='bytestream':
            inFiles=athenaCommonFlags.BSRDOInput()
        else:
            inFiles=athenaCommonFlags.PoolRDOInput()
    else:
        raise RuntimeError("Unable to determine input file")

    return inFiles


# This function generates a name e.g. used perfmon in RecExCommon/RecoUtils.py
def OutputFileName(suffix=""):
    from RecExConfig.RecFlags import rec
    OutFileName=""
    if rec.OutputFileNameForRecoStep()=="":
        if rec.readRDO():
            if globalflags.InputFormat()=="bytestream":
                OutFileName="bs"
            else:
                OutFileName="rdo"
        elif rec.readESD():
            OutFileName="esd"
        elif rec.readAOD():
            OutFileName="aod"
        else:
            OutFileName="in"

        OutFileName=OutFileName+"to"
        if rec.doWriteBS():
            OutFileName=OutFileName+"bs"
        elif rec.doWriteRDO():
            OutFileName=OutFileName+"rdo"

        if rec.doESD():
            OutFileName=OutFileName+"esd"
        if rec.doAOD():
            OutFileName=OutFileName+"aod"
        if rec.doDPD():
            OutFileName=OutFileName+"dpd"
        if rec.doWriteTAG():
            OutFileName=OutFileName+"tag"

        if suffix!="":
            OutFileName=OutFileName+"_"+suffix
        print ("Generated OutFileName",OutFileName)
    else:
        OutFileName=rec.OutputFileNameForRecoStep()
        print ("User defined OutFileName",OutFileName)
    return OutFileName

def isDefault(flagInstance):
    currentVal=flagInstance.get_Value()
    defaultVal=flagInstance.__class__.StoredValue
    return currentVal==defaultVal

##-------------------------------------------------------------------------
## ItemInList, AddValidItemToList & RemoveValidItemFromList
## basic helper functions for lists of strings
def ItemInList(item,aList):
    #Make sure aList is a list
    if not isinstance(aList, list):
        raise TypeError("RecoFunctions.ItemInList() does not support aList of type %s"%type(aList))

    isInList=False
    for i in aList:
        if i==item:
            isInList=True
    return isInList


def OverlapLists(List1, List2):
    for i in range(0,List1.__len__()):
        if List1[i] in List2:
            return True
    return False


def AddValidItemToList(item,aList):
    #Recursive loop if item is a list
    if isinstance(item,list):
        for i in item:
            AddValidItemToList(i,aList)
    #Add to list if item is a string and not already in the list
    elif isinstance(item, str):
        if not ItemInList(item,aList):
            aList += [item]
    else:
        raise TypeError("RecoFunctions.AddValidItemToList() does not support item of type %s"%type(item))
    return

def RemoveValidItemFromList(item,aList):
    #Recursive loop if item is a list
    if isinstance(item,list):
        for i in item:
            RemoveValidItemFromList(i,aList)
    #Remove from list if item is a string and already in list
    elif isinstance(item, str):
        if ItemInList(item,aList):
            aList.remove(item)
        else:
            print ("WARNING you asked to remove item '%s' but this item is not present"%item)
    else:
        raise TypeError("RecoFunctions.RemoveValidItemFromList() does not support item of type %s"%type(item))
    return

##-------------------------------------------------------------------------
def ItemInListStartsWith(item_starts, aList):
    for item in aList:
        if item.startswith(item_starts):
            return True
    return False
