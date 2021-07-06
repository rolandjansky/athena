# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# based on https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Control/AthenaServices/python/Configurables.py#0247
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


def buildListOfModifiers(ConfigFlags):
    # migrated from RunDMCFlags.py
    Modifiers = []
    pDicts = ConfigFlags.Input.RunAndLumiOverrideList
    DataRunNumber = ConfigFlags.Digitization.DataRunNumber

    if pDicts:
        for el in pDicts:
            evt_nbr = el.get("evt_nbr", None)
            Modifiers += add_modifier(run_nbr=el["run"], evt_nbr=evt_nbr, time_stamp=el["starttstamp"], lbk_nbr=el["lb"], nevts=el["evts"])
    elif DataRunNumber>0:
        assert DataRunNumber >= 0, (
            "ConfigFlags.Digitization.DataRunNumber %d is negative. "
            "Use a real run number from data." % DataRunNumber)

        # Using event numbers to avoid "some very large number" setting
        totalNumber = 1000000
        if ConfigFlags.Exec.MaxEvents > 0:
            totalNumber = ConfigFlags.Exec.MaxEvents + 1
        if ConfigFlags.Exec.SkipEvents > 0:
            totalNumber += ConfigFlags.Exec.SkipEvents

        InitialTimeStamp = ConfigFlags.IOVDb.RunToTimestampDict.get(DataRunNumber, 1) # TODO fix repeated configuration

        FirstLB = 1
        Modifiers += add_modifier(run_nbr=DataRunNumber, lbk_nbr=FirstLB, time_stamp=InitialTimeStamp, nevts=totalNumber)
    elif ConfigFlags.Input.RunNumber:
        # Behaviour for Simulation jobs. For standard Simulation we
        # override the run number once per job. TODO Still need to deal with the specific case of DataOverlay
        myRunNumber = ConfigFlags.Input.RunNumber[0]
        assert myRunNumber >= 0, (
            "ConfigFlags.Input.RunNumber[0] %d is negative. "
            "Use a real run number from data." % myRunNumber)
        myFirstLB = ConfigFlags.Input.LumiBlockNumber[0]
        myInitialTimeStamp = ConfigFlags.Input.TimeStamp[0]

        # Using event numbers to avoid "some very large number" setting
        totalNumber = 1000000
        if ConfigFlags.Exec.MaxEvents > 0:
            totalNumber = ConfigFlags.Exec.MaxEvents + 1
        if ConfigFlags.Exec.SkipEvents > 0:
            totalNumber += ConfigFlags.Exec.SkipEvents
        Modifiers += add_modifier(run_nbr=myRunNumber, lbk_nbr=myFirstLB, time_stamp=myInitialTimeStamp, nevts=totalNumber)
    return Modifiers


def getMinMaxRunNumbers(ConfigFlags):
    """Get a pair (firstrun,lastrun + 1) for setting ranges in IOVMetaData """
    mini = 1
    maxi = 2147483647
    pDicts = ConfigFlags.Input.RunAndLumiOverrideList
    if pDicts:
        # Behaviour for Digitization jobs using RunAndLumiOverrideList
        allruns = [element['run'] for element in pDicts]
        mini = min(allruns) + 0
        maxi = max(allruns) + 1
    elif ConfigFlags.Digitization.DataRunNumber>0:
        # Behaviour for Digitization jobs using DataRunNumber
        DataRunNumber = ConfigFlags.Digitization.DataRunNumber
        assert DataRunNumber >= 0, (
            "ConfigFlags.Digitization.DataRunNumber %d is negative. "
            "Use a real run number from data." % DataRunNumber)
        mini = DataRunNumber
        maxi = DataRunNumber+1
    elif ConfigFlags.Input.RunNumber:
        # Behaviour for Simulation jobs
        myRunNumber = ConfigFlags.Input.RunNumber[0]
        assert myRunNumber >= 0, (
            "ConfigFlags.Input.RunNumber[0] %d is negative. "
            "Use a real run number from data." % myRunNumber)
        mini = myRunNumber
        maxi = 2147483647
    return (mini,maxi)


def EvtIdModifierSvcCfg(ConfigFlags, name="EvtIdModifierSvc", **kwargs):
    acc = ComponentAccumulator()
    if ConfigFlags.Digitization.PileUp or ConfigFlags.Sim.DoFullChain:
        kwargs.setdefault("EvtStoreName", "OriginalEvent_SG")
    else:
        kwargs.setdefault("EvtStoreName", "StoreGateSvc")

    Modifiers = buildListOfModifiers(ConfigFlags)
    if len(Modifiers) > 0:
        kwargs.setdefault("Modifiers", Modifiers)
    acc.addService(CompFactory.EvtIdModifierSvc(name, **kwargs), create=True)
    iovDbMetaDataTool = CompFactory.IOVDbMetaDataTool()
    iovDbMetaDataTool.MinMaxRunNumbers = getMinMaxRunNumbers(ConfigFlags)
    acc.addPublicTool(iovDbMetaDataTool)
    return acc
