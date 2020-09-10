"""ComponentAccumulator run dependent service configuration

Including features from old style DigitizationFlags.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# FIXME not yet migrated: add_modifier, buildListOfModifiers, getEvtIdModifierSvc

def maxNevtsPerXing(flags):
    """Return the largest minbias pileup value, for PileUpEvtLoopMgr caches"""
    # from DigitizationFlags.py
    pDicts = flags.Digitization.PU.RunAndLumiOverrideList
    return max(element["mu"] for element in pDicts)


def runLumiListAndScaleFactorLists(flags):
    # DigitizationFlags.py
    runLumiList = []
    scaleFactorList = []
    pDicts = flags.Digitization.PU.RunAndLumiOverrideList
    MaxCollisionsPerXing = maxNevtsPerXing(flags)
    for element in pDicts:
        run = element['run']
        lbk = element['lb']
        mu = element['mu']
        iovt = (run << 32) + lbk
        runLumiList += [iovt]
        if MaxCollisionsPerXing > 0:
            scaleFactorList += [mu/MaxCollisionsPerXing]
        else:
            scaleFactorList += [1.0]
    return runLumiList, scaleFactorList


def LumiProfileSvcCfg(flags, name="LumiProfileSvc", **kwargs):
    acc = ComponentAccumulator()
    runLumiList, scaleFactorList = runLumiListAndScaleFactorLists(flags)
    #clear svc properties?
    kwargs.setdefault('RunLumiList', runLumiList)
    kwargs.setdefault('ScaleFactorList', scaleFactorList)
    acc.addService(CompFactory.LumiProfileSvc(name, **kwargs))
    return acc


def NoProfileSvcCfg(name="NoProfileSvc", **kwargs):
    acc = ComponentAccumulator()
    acc.addService(CompFactory.NoProfileSvc(name, **kwargs))
    return acc
