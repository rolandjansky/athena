from __future__ import print_function
#--------------------------------------------------------------------
# Overrides for run,lumi - dependent MC
#--------------------------------------------------------------------
include.block("Digitization/RunNumberOverride.py")
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AppMgr import ServiceMgr
if digitizationFlags.dataRunNumber.get_Value():
    if digitizationFlags.dataRunNumber.get_Value() < 0:
        raise SystemExit("Given a negative Run Number - please use a real run number from data.")
    print ('Overriding run number to be: %s ', digitizationFlags.dataRunNumber.get_Value())
    myRunNumber = digitizationFlags.dataRunNumber.get_Value()
    myFirstLB = 1
    myInitialTimeStamp = 1
    #update the run/event info for each event
    if not hasattr(ServiceMgr,'EvtIdModifierSvc'):
        from AthenaCommon.CfgGetter import getService
        getService("EvtIdModifierSvc")
    else:
        digilog.warning('RunNumberOverride.py :: Will override the settings of the EvtIdModifierSvc that was previously set up!')
    from Digitization.RunDependentConfig import buildListOfModifiers
    # to prevent the same modifiers being added twice by mistake
    newModifiers = buildListOfModifiers()
    if ServiceMgr.EvtIdModifierSvc.Modifiers != newModifiers:
       ServiceMgr.EvtIdModifierSvc.Modifiers += newModifiers
    #fix iov metadata
    if not hasattr(ServiceMgr.ToolSvc, 'IOVDbMetaDataTool'):
        ServiceMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
    ServiceMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = [myRunNumber, myRunNumber+1]#2147483647]
    myInitialTimeStamp=ServiceMgr.EvtIdModifierSvc.Modifiers[2]

    ServiceMgr.EventSelector.OverrideRunNumberFromInput=True
    ServiceMgr.EventSelector.OverrideRunNumber=True
    ServiceMgr.EventSelector.RunNumber=myRunNumber
    ServiceMgr.EventSelector.FirstLB = myFirstLB

    try:
        from RunDependentSimComps.RunDMCFlags import runDMCFlags
        myInitialTimeStamp = runDMCFlags.RunToTimestampDict.getTimestampForRun(myRunNumber)
    except:
        myInitialTimeStamp = 1
    ServiceMgr.EventSelector.InitialTimeStamp = myInitialTimeStamp

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if not athenaCommonFlags.DoFullChain:
        if digitizationFlags.simRunNumber.get_Value() < 0:
            raise SystemExit("Failed to read HIT file Run Number - please check input file for corruption.")

        ##FIXME need to do some error checking at this point
        ServiceMgr.EventSelector.OldRunNumber=digitizationFlags.simRunNumber.get_Value()
