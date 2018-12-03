# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#### Auxiliary Methods
def add_modifier(run_nbr=None, evt_nbr=None, time_stamp=None, lbk_nbr=None,
                 nevts=1):
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

    return [run_nbr, evt_nbr, time_stamp, lbk_nbr,
                       nevts, mod_bit]

def buildListOfModifiers():
    Modifiers = []
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.RunAndLumiOverrideList.get_Value():
        if not digitizationFlags.RunAndLumiOverrideList.is_locked():
            raise RuntimeError( 'You cannot configure the EvtIdModifierSvc with an unlocked JobProperty.' )
        pDicts = digitizationFlags.RunAndLumiOverrideList.get_Value()
        for el in pDicts:
            if 'evt_nbr' in el:
                Modifiers += add_modifier(run_nbr=el['run'], lbk_nbr=el['lb'], time_stamp=el['starttstamp'], nevts=el['evts'], evt_nbr=el['evt_nbr'])
            else:
                Modifiers += add_modifier(run_nbr=el['run'], lbk_nbr=el['lb'], time_stamp=el['starttstamp'], nevts=el['evts'])
    elif digitizationFlags.dataRunNumber.get_Value():
        if digitizationFlags.dataRunNumber.get_Value() < 0:
            raise SystemExit("Given a negative Run Number - please use a real run number from data.")
        #logDigitization_flags.info( 'Overriding run number to be: %s ', digitizationFlags.dataRunNumber.get_Value() )
        myRunNumber = digitizationFlags.dataRunNumber.get_Value()
        myFirstLB = 1
        ## Using event numbers to avoid "some very large number" setting
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        totalNumber = 1000000
        if athenaCommonFlags.EvtMax() is not None and athenaCommonFlags.EvtMax()>0: totalNumber = athenaCommonFlags.EvtMax()+1
        if athenaCommonFlags.SkipEvents() is not None and athenaCommonFlags.SkipEvents()>0: totalNumber += athenaCommonFlags.SkipEvents()
        try:
            from RunDependentSimComps.RunDMCFlags import runDMCFlags
            myInitialTimeStamp = runDMCFlags.RunToTimestampDict.getTimestampForRun(myRunNumber)
            #print "FOUND TIMESTAMP ", str(myInitialTimeStamp)
        except:
            myInitialTimeStamp = 1

        Modifiers += add_modifier(run_nbr=myRunNumber, lbk_nbr=myFirstLB, time_stamp=myInitialTimeStamp, nevts=totalNumber)
    return Modifiers


#### CfgGetter methods

def getEvtIdModifierSvc(name="EvtIdModifierSvc", **kwargs):
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.pileup.any_on():
       kwargs.setdefault("EvtStoreName", "OriginalEvent_SG")
    else:
       kwargs.setdefault("EvtStoreName", "StoreGateSvc")

    Modifiers = buildListOfModifiers()
    if len(Modifiers) > 0:
        kwargs.setdefault("Modifiers", Modifiers)

    from AthenaCommon import CfgMgr
    theEvtIdModifierSvc = CfgMgr.EvtIdModifierSvc(name, **kwargs)
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += theEvtIdModifierSvc
    from AthenaCommon.AppMgr import theApp
    if not "EvtIdModifierSvc" in theApp.CreateSvc:
        theApp.CreateSvc += ["EvtIdModifierSvc"]
    return theEvtIdModifierSvc

def getLumiProfileSvc(name="LumiProfileSvc", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if not(digitizationFlags.RunAndLumiOverrideList.is_locked()):
        raise RuntimeError( 'You cannot configure the EvtIdModifierSvc with an unlocked JobProperty.' )
    runLumiList, scaleFactorList = digitizationFlags.RunAndLumiOverrideList.getRunLumiListAndScaleFactorLists()
    #clear svc properties?
    kwargs.setdefault('RunLumiList', runLumiList)
    kwargs.setdefault('ScaleFactorList', scaleFactorList)
    kwargs.setdefault('OutputLevel', 2) # 2=DEBUG TODO remove
    from AthenaCommon import CfgMgr
    return CfgMgr.LumiProfileSvc(name, **kwargs)

def getNoProfileSvc(name="NoProfileSvc", **kwargs):
    from AthenaCommon import CfgMgr
    return CfgMgr.NoProfileSvc(name, **kwargs)
