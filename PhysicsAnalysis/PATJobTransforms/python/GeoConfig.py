#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def SetFieldForHLT():
    """ setting magnetic field automatically for RAWtoRAWHLT transform, needs athenaCommonFlags.BSRDOInput and globalflags.InputFormat to be set in advance"""
    # by C.Mora August 2009
    from RecExConfig.GetCool import cool
    if cool.toroidCurrent()>100. and cool.solenoidCurrent()>100.:
        setField='All'
    elif cool.solenoidCurrent()>100.:
        setField='Solenoid'
    elif cool.toroidCurrent()>100.:
        setField='Toroid'
    else:
        setField='None'
    return setField

def GetRunNumber(runArgs):
    RunNumber='NONE'
    if hasattr(runArgs,"RunNumber"):
        RunNumber=runArgs.RunNumber
    elif hasattr(runArgs,"inputBSFile") or hasattr(runArgs,"inputESDFile") or hasattr(runArgs,"inputAODFile"):
        #base on a file named like this:
        #/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data
        #or directly
        #daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data

        #from a list of inputFiles, take the first one
        if hasattr(runArgs,"inputBSFile"): fileName=runArgs.inputBSFile[0]
        elif hasattr(runArgs,"inputESDFile"): fileName=runArgs.inputESDFile[0]
        #if several directories, take the last part
        subNames=fileName.split('/')
        lastSub=subNames[len(subNames)-1]
        #from daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data, take the first integer
        #split by dots
        parts=lastSub.split('.')
        Found=False
        for part in parts:            
            if not Found:
                try:
                    RunNumber=int(part)
                    Found=True
                except:
                    pass

    if RunNumber is 'NONE':            
        raise RuntimeError("could not find RunNumber from inputFile nor from direct argument")
    else:
        print "The RunNumber is",RunNumber
    return RunNumber

