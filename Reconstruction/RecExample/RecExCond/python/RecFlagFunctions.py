#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##=============================================================================
## Name:        RecFlagFunctions.py
## Author:      David Cote (DESY)
## Created:     July 2008
## Description: This is a collection of utility functions related to rec flags
##=============================================================================

def inputFileNames():
    from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

    if athenaCommonFlags.FilesInput()!=[]:
        return athenaCommonFlags.FilesInput()
    from RecExConfig.RecFlags import rec
    inFiles=None

    # highest flag have precedence
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
        raise RutimeError("Unable to determine input file")

    return inFiles
    

