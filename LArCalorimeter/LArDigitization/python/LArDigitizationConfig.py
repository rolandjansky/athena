# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def isOverlay():
    # steer random overlay configuration
    from AthenaCommon.AppMgr import ServiceMgr
    if hasattr(ServiceMgr,'PileUpMergeSvc') :
        if hasattr(ServiceMgr.PileUpMergeSvc, 'ReturnTimedData') :
            if (not ServiceMgr.PileUpMergeSvc.ReturnTimedData) :
                from AthenaCommon.DetFlags import DetFlags
                checkOverlay = getattr(DetFlags, 'overlay', None)
                if checkOverlay is not None:
                    if DetFlags.overlay.LAr_on():
                        return True
                else: #fallback for older releases
                    from OverlayCommonAlgs.OverlayFlags import OverlayFlags
                    if OverlayFlags.doLAr():
                        return True
    return False

def useLArFloat():
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.doXingByXingPileUp() or isOverlay():
        # temporary, remapping to LArHitFloat does not seeem to work
        # with this scheme... => larger memory usage
        return False
    # check for fast chain, running digitisation from hits in memory
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.DoFullChain:
        return False
    # use simplified transient LArHit with float E,time
    return True


def getLArRangeEM(name="LArRangeEM", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', -751 )
    kwargs.setdefault('LastXing', 101 )
    if useLArFloat():
        kwargs.setdefault('ItemList', ["LArHitFloatContainer#LArHitEMB",
                                       "LArHitFloatContainer#LArHitEMEC"] )
    else:
        kwargs.setdefault('ItemList', ["LArHitContainer#LArHitEMB",
                                       "LArHitContainer#LArHitEMEC"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def getLArRangeHEC(name="LArRangeHEC", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', -676 )
    kwargs.setdefault('LastXing', 101 )
    if useLArFloat():
        kwargs.setdefault('ItemList', ["LArHitFloatContainer#LArHitHEC"] )
    else:
        kwargs.setdefault('ItemList', ["LArHitContainer#LArHitHEC"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def getLArRangeFCAL(name="LArRangeFCAL", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', -551 )
    kwargs.setdefault('LastXing', 101 )
    if useLArFloat():
        kwargs.setdefault('ItemList', ["LArHitFloatContainer#LArHitFCAL"] )
    else:
        kwargs.setdefault('ItemList', ["LArHitContainer#LArHitFCAL"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def getLArPileUpTool(name='LArPileUpTool', **kwargs): ## useLArFloat()=True,isOverlay()=False,outputKey='LArDigitContainer_MC'):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'LArPileUpToolDefault:' )
    mlog.info(" ---- in getLArPileUpTool " )
    # the LAr and Calo detector description package
    ## FIXME includes to be replaced by confGetter configuration.
    if not isOverlay():
        from AthenaCommon.Resilience import protectedInclude
        protectedInclude( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
        protectedInclude( "LArDetDescr/LArDetDescr_joboptions.py" )
        protectedInclude("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('NoiseOnOff', digitizationFlags.doCaloNoise.get_Value() )
    kwargs.setdefault('RndmSvc', digitizationFlags.rndmSvc.get_Value() )
    digitizationFlags.rndmSeedList.addSeed("LArDigitization", 1234, 5678 )
    from RecExConfig.RecFlags import rec
    kwargs.setdefault('DoDigiTruthReconstruction',rec.doDigiTruth())

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault('FirstXing', -751 )
        kwargs.setdefault('LastXing', 101 )

    from LArDigitization.LArDigitizationFlags import jobproperties
    # check if using high gain for Fcal or not
    if  (not jobproperties.LArDigitizationFlags.useFcalHighGain()) and (not isOverlay()):
        mlog.info("do not use high gain in Fcal digitization ")
        kwargs.setdefault('HighGainThreshFCAL', 0 )
    else:
        mlog.info("use high gain in Fcal digitization or overlay job")

    # check if using high gain for EMEC IW or not
    if (not jobproperties.LArDigitizationFlags.useEmecIwHighGain()) and (not isOverlay()):
       mlog.info("do not use high gain in EMEC IW digitization ")
       kwargs.setdefault('HighGainThreshEMECIW',0)

    kwargs.setdefault('RndmEvtOverlay', isOverlay() )
    kwargs.setdefault('DigitContainer', 'LArDigitContainer_MC' ) ##FIXME - should not be hard-coded

    # if doing MC+MC overlay
    from AthenaCommon.GlobalFlags import globalflags
    if isOverlay() and globalflags.DataSource() == 'geant4':
        kwargs.setdefault('isMcOverlay',True)

    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('Nsamples', larRODFlags.nSamples() )
    kwargs.setdefault('firstSample', larRODFlags.firstSample() )

    if  isOverlay() :
         kwargs.setdefault('RandomDigitContainer', 'LArDigitContainer_MC' )

    # ADC2MeVTool
    mlog.info(" ----  set LArADC2MeVToolDefault")
    kwargs.setdefault('ADC2MeVTool', 'LArADC2MeVToolDefault')

    # Tool for noise autocorrelation generation
    kwargs.setdefault('AutoCorrNoiseTool', 'LArAutoCorrNoiseToolDefault')

    # bad channel masking
    from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
    theLArBadChannelTool=LArBadChanTool()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+=theLArBadChannelTool
    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
    theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
    theLArRCBMasker.TheLArBadChanTool = theLArBadChannelTool
    theLArRCBMasker.DoMasking=True
    theLArRCBMasker.ProblemsToMask=[
         "deadReadout","deadPhys"]
    ToolSvc+=theLArRCBMasker
    kwargs.setdefault('MaskingTool', theLArRCBMasker )
    kwargs.setdefault('BadChannelTool', theLArBadChannelTool )

    # CosmicTriggerTimeTool for cosmics digitization
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        from CommissionUtils.CommissionUtilsConf import CosmicTriggerTimeTool
        theTriggerTimeTool = CosmicTriggerTimeTool()
        ToolSvc += theTriggerTimeTool
        kwargs.setdefault('UseTriggerTime', True )
        kwargs.setdefault('TriggerTimeToolName', theTriggerTimeTool )


    # pileup configuration "algorithm" way
    if not digitizationFlags.doXingByXingPileUp():
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.pileup.LAr_on() or isOverlay():
            kwargs.setdefault('PileUp', True )

    kwargs.setdefault('useLArFloat', useLArFloat() )
    if useLArFloat():
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from SGComps.SGCompsConf import AddressRemappingSvc
        AddressRemappingSvc = AddressRemappingSvc()
        svcMgr += AddressRemappingSvc
        from AthenaCommon.ConfigurableDb import getConfigurable
        svcMgr += getConfigurable( "ProxyProviderSvc" )()
        svcMgr.ProxyProviderSvc.ProviderNames += [ "AddressRemappingSvc" ]
        svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps = [ "LArHitContainer#LArHitEMB->LArHitFloatContainer#LArHitEMB" ,
                                                            "LArHitContainer#LArHitEMEC->LArHitFloatContainer#LArHitEMEC",
                                                            "LArHitContainer#LArHitHEC->LArHitFloatContainer#LArHitHEC",
                                                            "LArHitContainer#LArHitFCAL->LArHitFloatContainer#LArHitFCAL"]
        svcMgr.AddressRemappingSvc.ProxyDict="ActiveStoreSvc"


    return CfgMgr.LArPileUpTool(name, **kwargs)

def getLArDigitMaker(name="digitmaker1" , **kwargs):
    print "in getLArDigitMaker "
    kwargs.setdefault('LArPileUpTool', 'LArPileUpTool')
    return CfgMgr.LArDigitMaker(name, **kwargs)
