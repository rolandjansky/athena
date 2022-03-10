# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#file: TileCondToolConf.py
#author: nils.gollub@cern.ch

import string

from TileConditions.TileCondProxyConf  import getTileCondProxy

#=== check Athena running mode
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
isOnline = athenaCommonFlags.isOnline()

from IOVDbSvc.CondDB import conddb
isUsedDataBaseRun2 = (conddb.GetInstance() == 'CONDBR2')


validRunTypes = ['PHY','LAS', 'GAPLAS','CIS','PED','CISPULSE100','CISPULSE5P2','CISLEAK100','CISLEAK5P2']
validSources = ['COOL','FILE']

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")

from TileConditions.TileConditionsConf import TileCalibCondAlg_TileCalibDrawerFlt_ as TileCalibFltCondAlg

def bookTileCalibCondAlg(calibData, proxy):

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    calibCondAlg = calibData + 'CondAlg'
    if not hasattr(condSequence, calibCondAlg):
        condSequence += TileCalibFltCondAlg( name = calibCondAlg,
                                             ConditionsProxy = proxy,
                                             TileCalibData = calibData)

#
#____________________________________________________________________________
def getTileCondToolEmscale(source = 'FILE', name = 'TileCondToolEmscale', **kwargs):

    if source not in validSources:
        raise(Exception("Invalid source: %s" % source))

    from TileConditions.TileConditionsConf import TileCondToolEmscale

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None

    emScale = 'TileEMScale'
    emScaleCondAlg = emScale + 'CondAlg'

    if not hasattr(condSequence, emScaleCondAlg):

        onlCacheUnit = "Invalid"

        if source == 'COOL':
            #====================================================
            #=== Connect COOL TileCondProxies to the tool
            #====================================================
            from AthenaCommon.GlobalFlags import globalflags
            if (globalflags.DataSource() == 'data'):
                onlCacheUnit = "OnlineMegaElectronVolts"

            if isOnline and isUsedDataBaseRun2:
                oflLasFibProxy = None
            else:
                oflLasFibProxy = getTileCondProxy('COOL', 'Flt', 'oflLasFib', 'TileCondProxyCool_OflLasFib')

            oflCisLinProxy = getTileCondProxy('COOL', 'Flt', 'oflCisFitLin', 'TileCondProxyCool_OflCisLin')
            oflCisNlnProxy = getTileCondProxy('COOL', 'Flt', 'oflCisFitNln', 'TileCondProxyCool_OflCisNln')
            oflLasLinProxy = getTileCondProxy('COOL', 'Flt', 'oflLasLin',    'TileCondProxyCool_OflLasLin')
            oflLasNlnProxy = getTileCondProxy('COOL', 'Flt', 'oflLasNln',    'TileCondProxyCool_OflLasNln')
            oflCesProxy    = getTileCondProxy('COOL', 'Flt', 'oflCes',       'TileCondProxyCool_OflCes'   )
            oflEmsProxy    = getTileCondProxy('COOL', 'Flt', 'oflEms',       'TileCondProxyCool_OflEms'   )
            onlCisProxy    = getTileCondProxy('COOL', 'Flt', 'onlCisLin',    'TileCondProxyCool_OnlCis'   )
            onlLasProxy    = getTileCondProxy('COOL', 'Flt', 'onlLasLin',    'TileCondProxyCool_OnlLas'   )
            onlCesProxy    = getTileCondProxy('COOL', 'Flt', 'onlCes',       'TileCondProxyCool_OnlCes'   )
            onlEmsProxy    = getTileCondProxy('COOL', 'Flt', 'onlEms',       'TileCondProxyCool_OnlEms'   )

        else:
            #========================================================
            #=== Connect FILE TileCondProxies to the tool (default)
            #========================================================
            #--- Undoing "online" calibrations makes no sense here and is disabled
            oflCisLinProxy = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflCisLin')
            oflCisNlnProxy = getTileCondProxy('FILE','Flt','TileDefault.cisFitNln','TileCondProxyFile_OflCisNln')
            oflLasLinProxy = getTileCondProxy('FILE','Flt','TileDefault.lasLin'   ,'TileCondProxyFile_OflLasLin')
            oflLasNlnProxy = getTileCondProxy('FILE','Flt','TileDefault.lasNln'   ,'TileCondProxyFile_OflLasNln')
            oflLasFibProxy = getTileCondProxy('FILE','Flt','TileDefault.lasFib'   ,'TileCondProxyFile_OflLasFib')
            oflCesProxy    = getTileCondProxy('FILE','Flt','TileDefault.ces'      ,'TileCondProxyFile_OflCes'   )
            oflEmsProxy    = getTileCondProxy('FILE','Flt','TileDefault.ems'      ,'TileCondProxyFile_OflEms'   )
            onlCisProxy    = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflCisLin')
            onlLasProxy    = getTileCondProxy('FILE','Flt','TileDefault.lasLin'   ,'TileCondProxyFile_OflLasLin')
            onlCesProxy    = getTileCondProxy('FILE','Flt','TileDefault.ces'      ,'TileCondProxyFile_OflCes'   )
            onlEmsProxy    = getTileCondProxy('FILE','Flt','TileDefault.ems'      ,'TileCondProxyFile_OflEms'   )


        from TileConditions.TileConditionsConf import TileEMScaleCondAlg
        condSequence += TileEMScaleCondAlg( name = emScaleCondAlg,
                                            OnlCacheUnit = onlCacheUnit,
                                            OflCisLinProxy = oflCisLinProxy,
                                            OflCisNlnProxy = oflCisNlnProxy,
                                            OflLasLinProxy = oflLasLinProxy,
                                            OflLasNlnProxy = oflLasNlnProxy,
                                            OflLasFibProxy = oflLasFibProxy,
                                            OflCesProxy = oflCesProxy,
                                            OflEmsProxy = oflEmsProxy,
                                            OnlCisProxy = onlCisProxy,
                                            OnlLasProxy = onlLasProxy,
                                            OnlCesProxy = onlCesProxy,
                                            OnlEmsProxy = onlEmsProxy,
                                            TileEMScale = emScale)


    tool = TileCondToolEmscale(name, TileEMScale = emScale)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileExpertToolEmscale(source='FILE', name='TileExpertToolEmscale', **kwargs):

    if source not in validSources:
        raise(Exception("Invalid source: %s"%source))

    from TileConditions.TileConditionsConf import TileExpertToolEmscale

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None

    emScale = 'TileEMScale'
    emScaleCondAlg = emScale + 'CondAlg'

    if not hasattr(condSequence, emScaleCondAlg):

        if source == 'COOL':
            #====================================================
            #=== Connect COOL TileCondProxies to the tool
            #====================================================
            onlCacheUnit = "OnlineMegaElectronVolts",
            oflCisLinProxy = getTileCondProxy('COOL','Flt','oflCisFitLin','TileCondProxyCool_OflCisLin')
            oflCisNlnProxy = getTileCondProxy('COOL','Flt','oflCisFitNln','TileCondProxyCool_OflCisNln')
            oflLasLinProxy = getTileCondProxy('COOL','Flt','oflLasLin'   ,'TileCondProxyCool_OflLasLin')
            oflLasNlnProxy = getTileCondProxy('COOL','Flt','oflLasNln'   ,'TileCondProxyCool_OflLasNln')
            oflLasFibProxy = getTileCondProxy('COOL','Flt','oflLasFib'   ,'TileCondProxyCool_OflLasFib')
            oflCesProxy    = getTileCondProxy('COOL','Flt','oflCes'      ,'TileCondProxyCool_OflCes'   )
            oflEmsProxy    = getTileCondProxy('COOL','Flt','oflEms'      ,'TileCondProxyCool_OflEms'   )
            onlCisProxy    = getTileCondProxy('COOL','Flt','onlCisLin'   ,'TileCondProxyCool_OnlCis'   )
            onlLasProxy    = getTileCondProxy('COOL','Flt','onlLasLin'   ,'TileCondProxyCool_OnlLas'   )
            onlCesProxy    = getTileCondProxy('COOL','Flt','onlCes'      ,'TileCondProxyCool_OnlCes'   )
            onlEmsProxy    = getTileCondProxy('COOL','Flt','onlEms'      ,'TileCondProxyCool_OnlEms'   )

        else:
            #========================================================
            #=== Connect FILE TileCondProxies to the tool (default)
            #========================================================
            #--- Undoing "online" calibrations makes no sense here and is disabled
            onlCacheUnit = "Invalid",
            oflCisLinProxy = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflCisLin')
            oflCisNlnProxy = getTileCondProxy('FILE','Flt','TileDefault.cisFitNln','TileCondProxyFile_OflCisNln')
            oflLasLinProxy = getTileCondProxy('FILE','Flt','TileDefault.lasLin'   ,'TileCondProxyFile_OflLasLin')
            oflLasNlnProxy = getTileCondProxy('FILE','Flt','TileDefault.lasNln'   ,'TileCondProxyFile_OflLasNln')
            oflLasFibProxy = getTileCondProxy('FILE','Flt','TileDefault.lasFib'   ,'TileCondProxyFile_OflLasFib')
            oflCesProxy    = getTileCondProxy('FILE','Flt','TileDefault.ces'      ,'TileCondProxyFile_OflCes'   )
            oflEmsProxy    = getTileCondProxy('FILE','Flt','TileDefault.ems'      ,'TileCondProxyFile_OflEms'   )
            onlCisProxy    = None
            onlLasProxy    = None
            onlCesProxy    = None
            onlEmsProxy    = None


        from TileConditions.TileConditionsConf import TileEMScaleCondAlg
        condSequence += TileEMScaleCondAlg( name = emScaleCondAlg,
                                            OnlCacheUnit = onlCacheUnit,
                                            OflCisLinProxy = oflCisLinProxy,
                                            OflCisNlnProxy = oflCisNlnProxy,
                                            OflLasLinProxy = oflLasLinProxy,
                                            OflLasNlnProxy = oflLasNlnProxy,
                                            OflLasFibProxy = oflLasFibProxy,
                                            OflCesProxy = oflCesProxy,
                                            OflEmsProxy = oflEmsProxy,
                                            OnlCisProxy = onlCisProxy,
                                            OnlLasProxy = onlLasProxy,
                                            OnlCesProxy = onlCesProxy,
                                            OnlEmsProxy = onlEmsProxy,
                                            TileEMScale = emScale)


    tool = TileExpertToolEmscale(name, TileEMScale = emScale)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolIntegrator(source = 'FILE', name = 'TileCondToolIntegrator', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolIntegrator

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    integrator = 'TileIntegrator'

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        integratorProxy = getTileCondProxy('COOL','Flt','oflIntGain','TileCondProxyCool_Integrator')

    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        integratorProxy = getTileCondProxy('FILE','Flt','TileDefault.int','TileCondProxyFile_Integrator')

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            integratorProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_Integrator')

        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(integrator, integratorProxy)
    tool = TileCondToolIntegrator(name, integrator)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolMuID(source = 'FILE', name = 'TileCondToolMuID', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolMuID

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    muID = 'TileMuID'

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        muIdProxy = getTileCondProxy('COOL','Flt','onlMuID','TileCondProxyCool_MuID')
    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        muIdProxy = getTileCondProxy('FILE','Flt','TileDefault.muid','TileCondProxyFile_MuID')
    else:
        #====================================================
        #=== guess source is file name
        #====================================================
        file_name = find_data_file(source)
        if file_name is not None:
            muIdProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_MuID')
        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(muID, muIdProxy)
    tool = TileCondToolMuID(name, muID)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolTiming(source = 'FILE', runType = 'PHY', online = False, name = 'TileCondToolTiming', **kwargs):

    if runType not in validRunTypes:
        raise(Exception("Invalid run type %s"%runType))

    from TileConditions.TileConditionsConf import TileCondToolTiming

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    timing = 'TileOnlineTiming' if online else 'TileTiming'

    if source == 'COOL':
        sourcePrefix = 'onl' if online else 'ofl'

        if runType == 'PHY' or runType == 'PED':
            adcOffsetProxy = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeCphy','TileCondProxyCool_AdcOffset')
        if runType == 'LAS':
            adcOffsetProxy = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeClas','TileCondProxyCool_AdcOffset')
        if runType == 'GAPLAS':
            adcOffsetProxy = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeCgapLas','TileCondProxyCool_AdcOffset')
        if runType == 'CIS':
            adcOffsetProxy = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeCcis','TileCondProxyCool_AdcOffset')
    elif source == 'FILE':
        #=== create tool
        if runType == 'PHY' or runType == 'PED':
            adcOffsetProxy = getTileCondProxy('FILE','Flt','TileDefault.tcphy','TileCondProxyFile_AdcOffset')
        if runType == 'LAS':
            adcOffsetProxy = getTileCondProxy('FILE','Flt','TileDefault.tclas','TileCondProxyFile_AdcOffset')
        if runType == 'GAPLAS':
            raise(Exception("Invalid source %s " %source))
        if runType == 'CIS':
            adcOffsetProxy = getTileCondProxy('FILE','Flt','TileDefault.tccis','TileCondProxyFile_AdcOffset')

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            adcOffsetProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_AdcOffset')
        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(timing, adcOffsetProxy)
    tool = TileCondToolTiming(name, TileTiming = timing)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolPulseShape(source = 'FILE', runType = 'PHY', name = 'TileCondToolPulseShape', **kwargs):

    if runType not in validRunTypes:
        raise(Exception("Invalid run type %s"%runType))

    from TileConditions.TileConditionsConf import TileCondToolPulseShape

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    pulseShape = 'TilePulseShape'

    if source == 'COOL':
        #=== create tool
        if runType == 'PHY':
            pulseShapeProxy = getTileCondProxy('COOL','Flt','oflPlsPhy','TileCondProxyCool_PulseShapePhy')
        if runType == 'LAS':
            pulseShapeProxy = getTileCondProxy('COOL','Flt','oflPlsLas','TileCondProxyCool_PulseShapeLas')
        if runType == 'CISPULSE100':
            pulseShapeProxy = getTileCondProxy('COOL','Flt','oflPlsCisPl100','TileCondProxyCool_PulseShapeCisPl100')
        if runType == 'CISPULSE5P2':
            pulseShapeProxy = getTileCondProxy('COOL','Flt','oflPlsCisPl5p2','TileCondProxyCool_PulseShapeCisPl5p2')
        if runType == 'CISLEAK100':
            pulseShapeProxy = getTileCondProxy('COOL','Flt','oflPlsCisLk100','TileCondProxyCool_PulseShapeCisLk100')
        if runType == 'CISLEAK5P2':
            pulseShapeProxy = getTileCondProxy('COOL','Flt','oflPlsCisLk5p2','TileCondProxyCool_PulseShapeCisLk5p2')
    elif source == 'FILE':
        #====================================================
        #=== create tool
        #====================================================
        if runType == 'PHY':
            pulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsPhy','TileCondProxyFile_PulseShapePhy')
        if runType == 'LAS':
            pulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsLas','TileCondProxyFile_PulseShapeLas')
        if runType == 'CISPULSE100':
            pulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsCisPulse100','TileCondProxyFile_PulseShapeCisPl100')
        if runType == 'CISPULSE5P2':
            pulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsCisPulse5p2','TileCondProxyFile_PulseShapeCisPl5p2')
        if runType == 'CISLEAK100':
            pulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsCisLeak100','TileCondProxyFile_PulseShapeCisLk100')
        if runType == 'CISLEAK5P2':
            pulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsCisLeak5p2','TileCondProxyFile_PulseShapeCisLk5p2')

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            pulseShapeProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_PulseShape')
        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(pulseShape, pulseShapeProxy)
    tool = TileCondToolPulseShape(name, TilePulseShape = pulseShape)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolMuRcvPulseShape(source = 'FILE', name = 'TileCondToolMuRcvPulseShape', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolPulseShape

    tool = None
    muRcvPulseShape = 'TileMuRcvPulseShape'

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        raise(Exception("Not implemented source: %s" %source ))

    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        muRcvPulseShapeProxy = getTileCondProxy('FILE','Flt','TileDefault.plsMuRcv','TileCondProxyFile_PulseShapeMuRcv')
    else:
        #====================================================
        #=== guess source is file name
        #====================================================
        file_name = find_data_file(source)
        if file_name is not None:
            muRcvPulseShapeProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_PulseShapeMuRcv')
        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(muRcvPulseShape, muRcvPulseShapeProxy)
    tool = TileCondToolPulseShape(name, TilePulseShape = muRcvPulseShape)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileBadChanTool(source = 'FILE', name = 'TileBadChanTool', **kwargs):

    if source not in validSources:
        raise(Exception("Invalid source: %s"%source))

    from TileConditions.TileConditionsConf import TileBadChanTool

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None

    badChannels = 'TileBadChannels'
    badChannelsCondAlg = badChannels + 'CondAlg'

    if not hasattr(condSequence, badChannelsCondAlg):

        if source == 'COOL':
            #====================================================
            #=== Connect COOL TileCondProxies to the tool
            #====================================================
            if isOnline and isUsedDataBaseRun2:
                offlineBadChannelsProxy = None
            else:
                offlineBadChannelsProxy = getTileCondProxy('COOL','Bch','oflStatAdc','TileCondProxyCool_OflBch')

            onlineBadChannelsProxy = getTileCondProxy('COOL','Bch','onlStatAdc','TileCondProxyCool_OnlBch')
        else:
            #========================================================
            #=== Connect FILE TileCondProxies to the tool (default)
            #========================================================
            onlineBadChannelsProxy = getTileCondProxy('FILE','Bch','TileDefault.onlBch','TileCondProxyFile_OnlBch')
            offlineBadChannelsProxy = getTileCondProxy('FILE','Bch','TileDefault.oflBch','TileCondProxyFile_OflBch')


        from TileConditions.TileConditionsConf import TileBadChannelsCondAlg
        condSequence += TileBadChannelsCondAlg( name = badChannelsCondAlg,
                                                OnlBchProxy = onlineBadChannelsProxy,
                                                OflBchProxy = offlineBadChannelsProxy,
                                                TileBadChannels = badChannels)

    tool = TileBadChanTool(name, TileBadChannels = badChannels)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolNoiseSample(source = 'FILE', name = 'TileCondToolNoiseSample', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolNoiseSample

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    sampleNoise = 'TileSampleNoise'

    if isOnline or not isUsedDataBaseRun2:
        onlineSampleNoiseProxy = None
    else:
        onlineSampleNoiseProxy = getTileCondProxy('COOL','Flt','onlNoiseAdc','TileCondProxyCool_OnlineNoiseSample')

    onlineSampleNoise = 'TileOnlineSampleNoise' if onlineSampleNoiseProxy else ''

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        sampleNoiseProxy = getTileCondProxy('COOL','Flt','oflNoiseAdc','TileCondProxyCool_NoiseSample')
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        sampleNoiseProxy = getTileCondProxy('FILE','Flt','TileDefault.ped','TileCondProxyFile_NoiseSample')

    bookTileCalibCondAlg(sampleNoise, sampleNoiseProxy)
    if (onlineSampleNoiseProxy):
        bookTileCalibCondAlg(onlineSampleNoise, onlineSampleNoiseProxy)

    tool = TileCondToolNoiseSample(name,
                                   TileSampleNoise = sampleNoise,
                                   TileOnlineSampleNoise = onlineSampleNoise)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolAutoCr(source = 'FILE', name = 'TileCondToolAutoCr', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolAutoCr

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None

    autoCorrelation = 'TileAutoCorrelation'

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        autoCorrelationProxy = getTileCondProxy('COOL','Flt','oflNoiseAcr','TileCondProxyCool_NoiseAutoCr')

    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        autoCorrelationProxy = getTileCondProxy('FILE','Flt','TileDefault.acr','TileCondProxyFile_NoiseAutoCr')

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            autoCorrelationProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_NoiseAutoCr')

        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(autoCorrelation, autoCorrelationProxy)
    tool = TileCondToolAutoCr(name, TileAutoCorrelation = autoCorrelation)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolNoiseRawChn(source = 'FILE', name = 'TileCondToolNoiseRawChn', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolNoiseRawChn

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None

    rawChannelNoise = 'TileRawChannelNoise'

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        rawChannelNoiseProxy = getTileCondProxy('COOL','Flt','onlNoise1gOfni','TileCondProxyCool_NoiseRawChn')
    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        rawChannelNoiseProxy = getTileCondProxy('FILE','Flt','TileDefault.rchFit','TileCondProxyFile_NoiseRawChn')
    else:
        #====================================================
        #=== guess source is file name
        #====================================================
        file_name = find_data_file(source)
        if file_name is not None:
            rawChannelNoiseProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_NoiseRawChn')
        else:
            raise(Exception("Invalid source: %s" %source ))

    bookTileCalibCondAlg(rawChannelNoise, rawChannelNoiseProxy)
    tool = TileCondToolNoiseRawChn(name, TileRawChannelNoise = rawChannelNoise)

    #=== set the arguments passed and return tool
    for n, v in kwargs.items():
        setattr(tool, n, v)
    return tool


#
#____________________________________________________________________________
def getTileCondToolOfcCool(source = 'FILE', runType = 'PHY', ofcType = 'OF2', name = 'TileCondToolOfcCool', **kwargs):

    if source not in validSources:
        raise(Exception("Invalid source: %s" % source ))

    if runType not in validRunTypes:
        raise(Exception("Invalid run type %s" % runType))

    from TileConditions.TileConditionsConf import TileCondToolOfcCool

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    #do some check for global flag here: if source='' and flag set, adopt flag
    from AthenaCommon.GlobalFlags import globalflags
    isMC = (globalflags.DataSource() != 'data')

    tool = None

    if source == 'COOL':
        # There are OFC for OF1 only in DB used in Run2
        if ofcType == 'OF1' and not (isUsedDataBaseRun2 or isMC):
            return None

        from TileConditions.TileCoolMgr import GetTileOfcCoolSource, AddTileOfcCoolSource, tileCoolMgr

        ofc = 'TileOfc' + ofcType.capitalize()
        ofcCondAlg = ofc + 'CondAlg'

        if not hasattr(condSequence, ofcCondAlg):
            proxySource = GetTileOfcCoolSource(ofcType, runType)
            if not tileCoolMgr.isSourceAvailable(proxySource):
                splitOnline = isUsedDataBaseRun2 and (runType == 'PHY')
                AddTileOfcCoolSource(ofcType, runType, splitOnline)

            proxyName = 'TileCondProxyCool_' + proxySource
            ofcCoolProxy = getTileCondProxy('COOL', 'Ofc', proxySource, proxyName)

            from TileConditions.TileConditionsConf import TileCalibCondAlg_TileCalibDrawerOfc_ as TileCalibOfcCondAlg
            condSequence += TileCalibOfcCondAlg( name = ofcCondAlg,
                                                 ConditionsProxy = ofcCoolProxy,
                                                 TileCalibData = ofc)

        tool = TileCondToolOfcCool(name, TileOfc = ofc)

    else:
        raise(Exception("Invalid source %s" % source))



    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool


#
#____________________________________________________________________________
def getTileCondToolTMDB(source = 'FILE', runType = 'PHY', name = 'TileCondToolTMDB', **kwargs):

    if runType not in validRunTypes:
        raise(Exception("Invalid run type %s" % runType))

    from TileConditions.TileConditionsConf import TileCondToolTMDB

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None

    if not isUsedDataBaseRun2:
        return None

    TMDBThreshold = 'TileTMDBThreshold'
    TMDBDelay = 'TileTMDBDelay'
    TMDBTMF = 'TileTMDBTMF'
    TMDBCalib = 'TileTMDBCalib'

    if source == 'COOL':
        run = string.capwords(runType)
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        TMDBThresholdProxy = getTileCondProxy('COOL', 'Flt', 'onlTmdbThreshold' + run, 'TileCondProxyCool_TMDBThreshold' + run)
        TMDBDelayProxy = getTileCondProxy('COOL', 'Flt', 'onlTmdbDelay' + run, 'TileCondProxyCool_TMDBDelay' + run)
        TMDBTMFProxy = getTileCondProxy('COOL', 'Flt', 'onlTmdbTmf' + run, 'TileCondProxyCool_TMDBTmf' + run)
        TMDBCalibProxy = getTileCondProxy('COOL', 'Flt', 'onlTmdbCalibPhy', 'TileCondProxyCool_TMDBCalibPhy')

    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        raise(Exception("Invalid source %s" % source))

    bookTileCalibCondAlg(TMDBThreshold, TMDBThresholdProxy)
    bookTileCalibCondAlg(TMDBDelay, TMDBDelayProxy)
    bookTileCalibCondAlg(TMDBTMF, TMDBTMFProxy)
    bookTileCalibCondAlg(TMDBCalib, TMDBCalibProxy)

    tool = TileCondToolTMDB(name,
                            TileTMDBThreshold = TMDBThreshold,
                            TileTMDBDelay = TMDBDelay,
                            TileTMDBTMF = TMDBTMF,
                            TileTMDBCalib = TMDBCalib)

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool



#
#____________________________________________________________________________
def getTileCondToolDspThreshold(source = 'FILE', name = 'TileCondToolDspThreshold', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolDspThreshold

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    dspThreshold = 'TileDSP_Threshold'

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        if not isUsedDataBaseRun2:
            return None

        dspThresholdProxy = getTileCondProxy('COOL', 'Flt', 'oflDspThreshold', 'TileCondProxyCool_DspThreshold')
    elif  source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        dspThresholdProxy = getTileCondProxy('FILE', 'Flt', 'TileDefault.dspThreshold', 'TileCondProxyFile_DspThreshold')
    else:
        #====================================================
        #=== guess source is file name
        #====================================================
        file_name = find_data_file(source)
        if file_name is not None:
            dspThresholdProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_DspThreshold')
        else:
            raise(Exception("Invalid source: %s" %source ))


    bookTileCalibCondAlg(dspThreshold, dspThresholdProxy)
    tool = TileCondToolDspThreshold(name, TileDSP_Threshold = dspThreshold)

    #=== set the arguments passed and return tool
    for n, v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def bookTileSamplingFractionCondAlg(source = 'FILE'):

    from TileConditions.TileConditionsConf import TileCondAlg_TileSamplingFraction_TileCalibDrawerFlt_ as TileSamplingFractionCondAlg
    sampFraction = 'TileSamplingFraction'
    sampFractionCondAlg = sampFraction + 'CondAlg'

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    # Override the existing conditions algorithm
    if hasattr(condSequence, sampFractionCondAlg):
        delattr(condSequence, sampFractionCondAlg)

    if not hasattr(condSequence, sampFractionCondAlg):
        if source == 'COOL':
            sampFractionProxy = getTileCondProxy('COOL','Flt','oflSampFrac','TileCondProxyCool_SamplingFraction')
        elif source == 'FILE':
            sampFractionProxy = getTileCondProxy('FILE','Flt','TileDefault.sfr','TileCondProxyFile_SamplingFraction')
        else:
            file_name = find_data_file(source)
            if file_name is not None:
                sampFractionProxy = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_SamplingFraction')
            else:
                raise(Exception("Invalid source: %s" %source ))

        condSequence += TileSamplingFractionCondAlg( name = sampFractionCondAlg,
                                                     ConditionsProxy = sampFractionProxy,
                                                     TileCondData = sampFraction)

    pass


import os
def find_data_file(fname, pathlist = None, access = os.R_OK):
    """the python equivalent to the C++ PathResolver for datafiles.
    """

    if pathlist is None:
        pathlist = [ os.getcwd() ]
        pathlist += os.getenv('DATAPATH').split(os.pathsep)

    for path in pathlist:
        f = os.path.join( path, fname )
        if os.access( f, access ):
            return f

    return None
