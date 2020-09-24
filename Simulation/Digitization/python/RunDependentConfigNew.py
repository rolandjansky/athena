"""ComponentAccumulator run dependent service configuration

Including features from old style DigitizationFlags.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Auxiliary
def add_modifier(run_nbr=None, evt_nbr=None, time_stamp=None, lbk_nbr=None, nevts=1):
    
    if run_nbr is None:
        modify_run_nbr = 0
        run_nbr = 0
    else:
        modify_run_nbr = 1

    if evt_nbr is None:
        modify_evt_nbr = 0
        evt_nbr = 0
    else:
        modify_evt_nbr = 1

    if time_stamp is None:
        modify_time_stamp = 0
        time_stamp = 0
    else:
        modify_time_stamp = 1

    if lbk_nbr is None:
        modify_lbk_nbr = 0
        lbk_nbr = 0
    else:
        modify_lbk_nbr = 1

    mod_bit = int(0b0000
                  | (modify_run_nbr << 0)
                  | (modify_evt_nbr << 1)
                  | (modify_time_stamp << 2)
                  | (modify_lbk_nbr << 3))

    return [run_nbr, evt_nbr, time_stamp, lbk_nbr, nevts, mod_bit]


def buildListOfModifiers(flags):
    # migrated from RunDMCFlags.py
    Modifiers = []
    pDicts = flags.Digitization.PU.RunAndLumiOverrideList
    DataRunNumber = flags.Digitization.DataRunNumber
    
    if pDicts:
        for el in pDicts:
            evt_nbr = el.get("evt_nbr", None)
            Modifiers += add_modifier(run_nbr=el["run"], evt_nbr=evt_nbr, time_stamp=el["starttstamp"], lbk_nbr=el["lb"], nevts=el["evts"])
    elif DataRunNumber:
        assert DataRunNumber >= 0, (
            "flags.Digitization.DataRunNumber %d is negative. "
            "Use a real run number from data." % DataRunNumber)
        
        # Using event numbers to avoid "some very large number" setting
        totalNumber = 1000000
        if flags.Exec.MaxEvents > 0:
            totalNumber = flags.Exec.MaxEvents + 1
        if flags.Exec.SkipEvents > 0:
            totalNumber += flags.Exec.SkipEvents
            
        InitialTimeStamp = flags.Sim.RunToTimestampDict.get(DataRunNumber, 1)

        FirstLB = 1
        Modifiers += add_modifier(run_nbr=DataRunNumber, lbk_nbr=FirstLB, time_stamp=InitialTimeStamp, nevts=totalNumber)
    return Modifiers


def maxNevtsPerXing(flags):
    """Return the largest minbias pileup value, for PileUpEvtLoopMgr caches"""
    # migrated from DigitizationFlags.py
    pDicts = flags.Digitization.PU.RunAndLumiOverrideList
    return max(element["mu"] for element in pDicts)


def runLumiListAndScaleFactorLists(flags):
    # migrated from DigitizationFlags.py
    runLumiList = []
    scaleFactorList = []
    pDicts = flags.Digitization.PU.RunAndLumiOverrideList
    MaxCollisionsPerXing = maxNevtsPerXing(flags)
    for element in pDicts:
        run = element["run"]
        lbk = element["lb"]
        mu = element["mu"]
        iovt = (run << 32) + lbk
        runLumiList += [iovt]
        if MaxCollisionsPerXing > 0:
            scaleFactorList += [mu/MaxCollisionsPerXing]
        else:
            scaleFactorList += [1.0]
    return runLumiList, scaleFactorList


# Config
def EvtIdModifierSvcCfg(flags, name="EvtIdModifierSvc", **kwargs):
    acc = ComponentAccumulator()
    
    if flags.Sim.DoFullChain:
        kwargs.setdefault("EvtStoreName", "OriginalEvent_SG")
    else:
        kwargs.setdefault("EvtStoreName", "StoreGateSvc")

    Modifiers = buildListOfModifiers(flags)
    if len(Modifiers) > 0:
        kwargs.setdefault("Modifiers", Modifiers)

    acc.addService(CompFactory.EvtIdModifierSvc(name, **kwargs), create=True)
    return acc


def LumiProfileSvcCfg(flags, name="LumiProfileSvc", **kwargs):
    acc = ComponentAccumulator()
    runLumiList, scaleFactorList = runLumiListAndScaleFactorLists(flags)
    #clear svc properties?
    kwargs.setdefault("RunLumiList", runLumiList)
    kwargs.setdefault("ScaleFactorList", scaleFactorList)
    acc.addService(CompFactory.LumiProfileSvc(name, **kwargs))
    return acc


def NoProfileSvcCfg(name="NoProfileSvc", **kwargs):
    acc = ComponentAccumulator()
    acc.addService(CompFactory.NoProfileSvc(name, **kwargs))
    return acc
