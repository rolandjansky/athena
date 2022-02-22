# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def isOverlay():
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay():
        return True
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
                    from OverlayCommonAlgs.OverlayFlags import overlayFlags
                    if overlayFlags.doLAr():
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
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
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
    kwargs.setdefault("RandomSeedOffset", digitizationFlags.rndmSeedOffset1.get_Value() + digitizationFlags.rndmSeedOffset2.get_Value())
    if isOverlay():
        # Some noise needs to be added during MC Overlay
        # No noise should be added during Data Overlay
        kwargs.setdefault('NoiseOnOff', not overlayFlags.isDataOverlay())
    else:
        kwargs.setdefault('NoiseOnOff', digitizationFlags.doCaloNoise.get_Value() ) # For other jobs go with the noise flag setting.
    kwargs.setdefault('DoDigiTruthReconstruction',digitizationFlags.doDigiTruth())

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault('FirstXing', -751 )
        kwargs.setdefault('LastXing', 101 )
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.pileup.any_on():
        kwargs.setdefault("PileUpMergeSvc", '')
        kwargs.setdefault("OnlyUseContainerName", False)

    kwargs.setdefault('RndmEvtOverlay', isOverlay() )

    if useLArFloat():
        kwargs.setdefault("LArHitContainers",[])
    else:
        kwargs.setdefault("LArHitFloatContainers",[])

    # if doing MC+MC overlay 
    from AthenaCommon.GlobalFlags import globalflags
    if isOverlay() and globalflags.DataSource() == 'geant4':
          kwargs.setdefault('isMcOverlay',True)

    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('Nsamples', larRODFlags.nSamples() )
    kwargs.setdefault('firstSample', larRODFlags.firstSample() )

    if isOverlay() :
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("OnlyUseContainerName", False)
            kwargs.setdefault('InputDigitContainer',  overlayFlags.bkgPrefix() + 'LArDigitContainer_MC' )
        else:
            kwargs.setdefault('InputDigitContainer', 'LArDigitContainer_MC' )

    # CosmicTriggerTimeTool for cosmics digitization
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        from CommissionUtils.CommissionUtilsConf import CosmicTriggerTimeTool
        from AthenaCommon.AppMgr import ToolSvc
        theTriggerTimeTool = CosmicTriggerTimeTool()
        ToolSvc += theTriggerTimeTool
        kwargs.setdefault('UseTriggerTime', True )
        kwargs.setdefault('TriggerTimeToolName', theTriggerTimeTool )


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

    # add XTalk conditions
    from LArRecUtils.LArXTalkWeightCondAlgDefault import LArXTalkWeightCondAlgDefault
    LArXTalkWeightCondAlgDefault()

    return CfgMgr.LArPileUpTool(name, **kwargs)

def getLArHitEMapToDigitAlg(name="LArHitEMapToDigitAlgDefault", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'LArHitEMapToDigitAlgDefault:' )
    mlog.info(" ---- in getLArHitEMapToDigitAlg " )
    # the LAr and Calo detector description package
    ## FIXME includes to be replaced by confGetter configuration.
    if not isOverlay():
        from AthenaCommon.Resilience import protectedInclude
        protectedInclude( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
        protectedInclude( "LArDetDescr/LArDetDescr_joboptions.py" )
        protectedInclude("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
    else:
        if overlayFlags.isDataOverlay():
            #Shape taken from real-data DB has a different SG key
            kwargs.setdefault('ShapeKey',"LArShape")
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("RandomSeedOffset", digitizationFlags.rndmSeedOffset1.get_Value() + digitizationFlags.rndmSeedOffset2.get_Value())
    if isOverlay():
         # Some noise needs to be added during MC Overlay
         # No noise should be added during Data Overlay
         kwargs.setdefault('NoiseOnOff', not overlayFlags.isDataOverlay() )
    else :
         kwargs.setdefault('NoiseOnOff', digitizationFlags.doCaloNoise.get_Value() )
    kwargs.setdefault('DoDigiTruthReconstruction',digitizationFlags.doDigiTruth())

    from LArDigitization.LArDigitizationFlags import jobproperties
    # check if using high gain for Fcal or not
    if  (not jobproperties.LArDigitizationFlags.useFcalHighGain()) and (not isOverlay()):
        mlog.info("do not use high gain in Fcal digitization ")
        kwargs.setdefault('HighGainThreshFCAL', 0 ) ## commented out for now
    else:
        mlog.info("use high gain in Fcal digitization or overlay job")

    # check if using high gain for EMEC IW or not
    if (not jobproperties.LArDigitizationFlags.useEmecIwHighGain()) and (not isOverlay()):
       mlog.info("do not use high gain in EMEC IW digitization ")
       kwargs.setdefault('HighGainThreshEMECIW',0) ## commeted out for now

    kwargs.setdefault('RndmEvtOverlay', isOverlay() )

    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault('DigitContainer', overlayFlags.bkgPrefix() + 'LArDigitContainer_MC')
    else:
        kwargs.setdefault('DigitContainer', 'LArDigitContainer_MC') ##FIXME - should not be hard-coded

    # if doing MC+MC overlay
    from AthenaCommon.GlobalFlags import globalflags
    if isOverlay() and globalflags.DataSource() == 'geant4':
          kwargs.setdefault('isMcOverlay',True)

    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('Nsamples', larRODFlags.nSamples() )
    kwargs.setdefault('firstSample', larRODFlags.firstSample() )

    # ADC2MeVCondAlgo
    from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
    LArADC2MeVCondAlgDefault()

    # AutoCorrNoiseCondAlgo
    if not isOverlay():
        from LArRecUtils.LArAutoCorrNoiseCondAlgDefault import LArAutoCorrNoiseCondAlgDefault
        LArAutoCorrNoiseCondAlgDefault()
        kwargs.setdefault('AutoCorrNoiseKey','LArAutoCorr')

    # bad channel masking
    kwargs.setdefault('ProblemsToMask',["deadReadout","deadPhys"])
    # CosmicTriggerTimeTool for cosmics digitization

    return CfgMgr.LArHitEMapToDigitAlg(name, **kwargs)

def getLArDigitMaker(name="digitmaker1" , **kwargs):
    kwargs.setdefault('LArPileUpTool', 'LArPileUpTool')
    return CfgMgr.LArDigitMaker(name, **kwargs)
