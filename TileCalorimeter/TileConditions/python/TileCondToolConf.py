# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#file: TileCondToolConf.py
#author: nils.gollub@cern.ch

import string

from TileConditions.TileCondProxyConf  import *
from AthenaCommon.Constants import INFO

#=== check Athena running mode
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
isOnline = athenaCommonFlags.isOnline()

from IOVDbSvc.CondDB import conddb
isUsedDataBaseRun2 = (conddb.GetInstance() == 'CONDBR2')


validRunTypes = ['PHY','LAS', 'GAPLAS','CIS','PED','CISPULSE100','CISPULSE5P2','CISLEAK100','CISLEAK5P2']
validSources = ['COOL','FILE']

#
#____________________________________________________________________________
def getTileCondToolEmscale(source='FILE', name='TileCondToolEmscale', **kwargs):

    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileCondToolEmscale

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        from AthenaCommon.GlobalFlags import globalflags
        if (globalflags.DataSource() != 'data'): onlunit = "Invalid"
        else: onlunit = "OnlineMegaElectronVolts"

        if isOnline and isUsedDataBaseRun2: oflLasFibProxy = None
        else: oflLasFibProxy = getTileCondProxy('COOL', 'Flt', 'oflLasFib', 'TileCondProxyCool_OflLasFib')

        tool = TileCondToolEmscale(name
                                   , OnlCacheUnit = onlunit
                                   , ProxyOflCisLin = getTileCondProxy('COOL', 'Flt', 'oflCisFitLin', 'TileCondProxyCool_OflCisLin')
                                   , ProxyOflCisNln = getTileCondProxy('COOL', 'Flt', 'oflCisFitNln', 'TileCondProxyCool_OflCisNln')
                                   , ProxyOflLasLin = getTileCondProxy('COOL', 'Flt', 'oflLasLin',    'TileCondProxyCool_OflLasLin')
                                   , ProxyOflLasNln = getTileCondProxy('COOL', 'Flt', 'oflLasNln',    'TileCondProxyCool_OflLasNln')
                                   , ProxyOflLasFib = oflLasFibProxy
                                   , ProxyOflCes    = getTileCondProxy('COOL', 'Flt', 'oflCes',       'TileCondProxyCool_OflCes'   )
                                   , ProxyOflEms    = getTileCondProxy('COOL', 'Flt', 'oflEms',       'TileCondProxyCool_OflEms'   )
                                   , ProxyOnlCis    = getTileCondProxy('COOL', 'Flt', 'onlCisLin',    'TileCondProxyCool_OnlCis'   )
                                   , ProxyOnlLas    = getTileCondProxy('COOL', 'Flt', 'onlLasLin',    'TileCondProxyCool_OnlLas'   )
                                   , ProxyOnlCes    = getTileCondProxy('COOL', 'Flt', 'onlCes',       'TileCondProxyCool_OnlCes'   )
                                   , ProxyOnlEms    = getTileCondProxy('COOL', 'Flt', 'onlEms',       'TileCondProxyCool_OnlEms'   ))
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        #--- Undoing "online" calibrations makes no sense here and is disabled
        tool = TileCondToolEmscale(name
                                   , OnlCacheUnit = "Invalid"
                                   , ProxyOflCisLin = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflCisLin')
                                   , ProxyOflCisNln = getTileCondProxy('FILE','Flt','TileDefault.cisFitNln','TileCondProxyFile_OflCisNln')
                                   , ProxyOflLasLin = getTileCondProxy('FILE','Flt','TileDefault.lasLin'   ,'TileCondProxyFile_OflLasLin')
                                   , ProxyOflLasNln = getTileCondProxy('FILE','Flt','TileDefault.lasNln'   ,'TileCondProxyFile_OflLasNln')
                                   , ProxyOflLasFib = getTileCondProxy('FILE','Flt','TileDefault.lasFib'   ,'TileCondProxyFile_OflLasFib')
                                   , ProxyOflCes    = getTileCondProxy('FILE','Flt','TileDefault.ces'      ,'TileCondProxyFile_OflCes'   )
                                   , ProxyOflEms    = getTileCondProxy('FILE','Flt','TileDefault.ems'      ,'TileCondProxyFile_OflEms'   )
                                   , ProxyOnlCis    = None
                                   , ProxyOnlLas    = None
                                   , ProxyOnlCes    = None
                                   , ProxyOnlEms    = None)
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileExpertToolEmscale(source='FILE', name='TileExpertToolEmscale', **kwargs):
    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileExpertToolEmscale

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileExpertToolEmscale(name,
                                   OnlCacheUnit = "OnlineMegaElectronVolts",
                                   ProxyOflCisLin = getTileCondProxy('COOL','Flt','oflCisFitLin','TileCondProxyCool_OflCisLin'),
                                   ProxyOflCisNln = getTileCondProxy('COOL','Flt','oflCisFitNln','TileCondProxyCool_OflCisNln'),
                                   ProxyOflLasLin = getTileCondProxy('COOL','Flt','oflLasLin'   ,'TileCondProxyCool_OflLasLin'),
                                   ProxyOflLasNln = getTileCondProxy('COOL','Flt','oflLasNln'   ,'TileCondProxyCool_OflLasNln'),
                                   ProxyOflLasFib = getTileCondProxy('COOL','Flt','oflLasFib'   ,'TileCondProxyCool_OflLasFib'),
                                   ProxyOflCes    = getTileCondProxy('COOL','Flt','oflCes'      ,'TileCondProxyCool_OflCes'   ),
                                   ProxyOflEms    = getTileCondProxy('COOL','Flt','oflEms'      ,'TileCondProxyCool_OflEms'   ),
                                   ProxyOnlCis    = getTileCondProxy('COOL','Flt','onlCisLin'   ,'TileCondProxyCool_OnlCis'   ),
                                   ProxyOnlLas    = getTileCondProxy('COOL','Flt','onlLasLin'   ,'TileCondProxyCool_OnlLas'   ),
                                   ProxyOnlCes    = getTileCondProxy('COOL','Flt','onlCes'      ,'TileCondProxyCool_OnlCes'   ),
                                   ProxyOnlEms    = getTileCondProxy('COOL','Flt','onlEms'      ,'TileCondProxyCool_OnlEms'   ))
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        #--- Undoing "online" calibrations makes no sense here and is disabled
        tool = TileExpertToolEmscale(name,
                                   OnlCacheUnit = "Invalid",
                                   ProxyOflCisLin = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflCisLin'),
                                   ProxyOflCisNln = getTileCondProxy('FILE','Flt','TileDefault.cisFitNln','TileCondProxyFile_OflCisNln'),
                                   ProxyOflLasLin = getTileCondProxy('FILE','Flt','TileDefault.lasLin'   ,'TileCondProxyFile_OflLasLin'),
                                   ProxyOflLasNln = getTileCondProxy('FILE','Flt','TileDefault.lasNln'   ,'TileCondProxyFile_OflLasNln'),
                                   ProxyOflLasFib = getTileCondProxy('FILE','Flt','TileDefault.lasFib'   ,'TileCondProxyFile_OflLasFib'),
                                   ProxyOflCes    = getTileCondProxy('FILE','Flt','TileDefault.ces'      ,'TileCondProxyFile_OflCes'   ),
                                   ProxyOflEms    = getTileCondProxy('FILE','Flt','TileDefault.ems'      ,'TileCondProxyFile_OflEms'   ),
                                   ProxyOnlCis    = None,
                                   ProxyOnlLas    = None,
                                   ProxyOnlCes    = None,
                                   ProxyOnlEms    = None)
        
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
    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolIntegrator(name,
                                       ProxyIntegrator = getTileCondProxy('COOL','Flt','oflIntGain','TileCondProxyCool_Integrator'))
        
    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolIntegrator(name,
                                       ProxyIntegrator = getTileCondProxy('FILE','Flt','TileDefault.int','TileCondProxyFile_Integrator'))

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolIntegrator(name
                                          , ProxyIntegrator = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_Integrator'))

        else:
            raise(Exception("Invalid source: %s" %source ))

        
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
    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolMuID(name
                                , ProxyMuID = getTileCondProxy('COOL','Flt','onlMuID','TileCondProxyCool_MuID'))
        
    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolMuID(name
                                , ProxyMuID = getTileCondProxy('FILE','Flt','TileDefault.muid','TileCondProxyFile_MuID'))
        
    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolMuID(name
                                    , ProxyMuID = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_MuID'))

        else:
            raise(Exception("Invalid source: %s" %source ))


    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolTiming(source = 'FILE', runType = 'PHY', online = False, name = 'TileCondToolTiming', **kwargs):


    if not runType in validRunTypes: raise(Exception("Invalid run type %s"%runType))
    from TileConditions.TileConditionsConf import TileCondToolTiming

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    if source == 'COOL':
        sourcePrefix = 'onl' if online else 'ofl'

        if runType == 'PHY' or runType == 'PED':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeCphy','TileCondProxyCool_AdcOffset'))
        if runType == 'LAS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeClas','TileCondProxyCool_AdcOffset'))
        if runType == 'GAPLAS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeCgapLas','TileCondProxyCool_AdcOffset'))
        if runType == 'CIS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt', sourcePrefix + 'TimeCcis','TileCondProxyCool_AdcOffset'))
    elif source == 'FILE':
        #=== create tool
        if runType == 'PHY' or runType == 'PED':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('FILE','Flt','TileDefault.tcphy','TileCondProxyFile_AdcOffset'))
        if runType == 'LAS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('FILE','Flt','TileDefault.tclas','TileCondProxyFile_AdcOffset'))
        if runType == 'GAPLAS':
            raise(Exception("Invalid source %s " %source))
        if runType == 'CIS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('FILE','Flt','TileDefault.tccis','TileCondProxyFile_AdcOffset'))

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_AdcOffset'))

        else:
            raise(Exception("Invalid source: %s" %source ))
        
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolPulseShape(source = 'FILE', runType = 'PHY', name = 'TileCondToolPulseShape', **kwargs):

    if not runType in validRunTypes: raise(Exception("Invalid run type %s"%runType))
    from TileConditions.TileConditionsConf import TileCondToolPulseShape

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    if source == 'COOL':
        #=== create tool
        if runType == 'PHY':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsPhy','TileCondProxyCool_PulseShapePhy'),
                                          )
        if runType == 'LAS':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsLas','TileCondProxyCool_PulseShapeLas'),
                                          )
        if runType == 'CISPULSE100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisPl100','TileCondProxyCool_PulseShapeCisPl100'),
                                          )
        if runType == 'CISPULSE5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisPl5p2','TileCondProxyCool_PulseShapeCisPl5p2'),
                                          )
        if runType == 'CISLEAK100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisLk100','TileCondProxyCool_PulseShapeCisLk100'),
                                          )
        if runType == 'CISLEAK5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisLk5p2','TileCondProxyCool_PulseShapeCisLk5p2'),
                                          )

    elif source == 'FILE':
        #====================================================
        #=== create tool
        #====================================================
        if runType == 'PHY':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsPhy','TileCondProxyFile_PulseShapePhy'),
                                          )
        if runType == 'LAS':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsLas','TileCondProxyFile_PulseShapeLas'),
                                          )
        if runType == 'CISPULSE100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisPulse100','TileCondProxyFile_PulseShapeCisPl100'),
                                          )
        if runType == 'CISPULSE5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisPulse5p2','TileCondProxyFile_PulseShapeCisPl5p2'),
                                          )
        if runType == 'CISLEAK100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisLeak100','TileCondProxyFile_PulseShapeCisLk100'),
                                          )
        if runType == 'CISLEAK5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisLeak5p2','TileCondProxyFile_PulseShapeCisLk5p2'),
                                          )


    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolPulseShape(name
                                          , ProxyPulseShape = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_PulseShape'))

        else:
            raise(Exception("Invalid source: %s" %source ))

        
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool
    
#
#____________________________________________________________________________
def getTileCondToolMuRcvPulseShape(source = 'FILE', name = 'TileCondToolMuRcvPulseShape', **kwargs):

    from TileConditions.TileConditionsConf import TileCondToolPulseShape

    tool = None
    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        raise(Exception("Not implemented source: %s" %source ))

    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================

        tool = TileCondToolPulseShape(name
                                      , ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsMuRcv','TileCondProxyFile_PulseShapeMuRcv'))


    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolPulseShape(name
                                          , ProxyPulseShape = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_PulseShapeMuRcv'))

        else:
            raise(Exception("Invalid source: %s" %source ))



    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileBadChanTool(source = 'FILE', name = 'TileBadChanTool', **kwargs):

    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileBadChanTool

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        if isOnline and isUsedDataBaseRun2: oflBchProxy = None
        else: oflBchProxy = getTileCondProxy('COOL','Bch','oflStatAdc','TileCondProxyCool_OflBch')

        tool = TileBadChanTool(name
                               , ProxyOnlBch = getTileCondProxy('COOL','Bch','onlStatAdc','TileCondProxyCool_OnlBch')
                               , ProxyOflBch = oflBchProxy)
                               

    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileBadChanTool(name
                               , ProxyOnlBch = getTileCondProxy('FILE','Bch','TileDefault.onlBch','TileCondProxyFile_OnlBch')
                               , ProxyOflBch = getTileCondProxy('FILE','Bch','TileDefault.oflBch','TileCondProxyFile_OflBch'))

        
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

    if isOnline or not isUsedDataBaseRun2: onlNoiseSampleProxy = None
    else: onlNoiseSampleProxy = getTileCondProxy('COOL','Flt','onlNoiseAdc','TileCondProxyCool_OnlineNoiseSample')

    if source == 'COOL': 
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolNoiseSample(name,
                                       ProxyNoiseSample = getTileCondProxy('COOL','Flt','oflNoiseAdc','TileCondProxyCool_NoiseSample'),
                                       ProxyOnlineNoiseSample = onlNoiseSampleProxy)
#                                       ProxyNoiseAutoCr = getTileCondProxy('COOL','Flt','oflNoiseAcr','TileCondProxyCool_NoiseAutoCr'))
        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolNoiseSample(name,
                                       ProxyNoiseSample = getTileCondProxy('FILE','Flt','TileDefault.ped','TileCondProxyFile_NoiseSample'),
                                       ProxyOnlineNoiseSample = None)
#                                       ProxyNoiseAutoCr = getTileCondProxy('FILE','Flt','TileDefault.acr','TileCondProxyFile_NoiseAutoCr'))
       
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
    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolAutoCr(name
                                  , ProxyNoiseAutoCr = getTileCondProxy('COOL','Flt','oflNoiseAcr','TileCondProxyCool_NoiseAutoCr'))
        
    elif source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolAutoCr(name
                                  , ProxyNoiseAutoCr = getTileCondProxy('FILE','Flt','TileDefault.acr','TileCondProxyFile_NoiseAutoCr'))
        

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolAutoCr(name
                                      , ProxyNoiseAutoCr = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_NoiseAutoCr'))

        else:
            raise(Exception("Invalid source: %s" %source ))


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
    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolNoiseRawChn(name
                                       , ProxyNoiseRawChn = getTileCondProxy('COOL','Flt','onlNoise1gOfni','TileCondProxyCool_NoiseRawChn'))
        
    elif  source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolNoiseRawChn(name
                                       , ProxyNoiseRawChn = getTileCondProxy('FILE','Flt','TileDefault.rchFit','TileCondProxyFile_NoiseRawChn'))
        

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolNoiseRawChn(name
                                           , ProxyNoiseRawChn = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_NoiseRawChn'))

        else:
            raise(Exception("Invalid source: %s" %source ))


    #=== set the arguments passed and return tool
    for n, v in kwargs.items():
        setattr(tool, n, v)
    return tool


#
#____________________________________________________________________________
def getTileCellNoiseTool(source = 'FILE', name = 'TileCellNoiseTool', **kwargs):
    
    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileCellNoiseTool
    
    #do some check for global flag here: if source='' and flag set, adopt flag

    #=== create private noise source
    noiseSource = getTileCondToolNoiseRawChn(source, "TileToolNoiseRawChnForNoiseTool")
    tool = TileCellNoiseTool(name, NoiseSource = noiseSource)
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool


#
#____________________________________________________________________________
def getTileCondToolOfcCool(source = 'FILE', runType = 'PHY', ofcType = 'OF2', name = 'TileCondToolOfcCool', **kwargs):

    if not source  in validSources : raise(Exception("Invalid source: %s" % source ))
    if not runType in validRunTypes: raise(Exception("Invalid run type %s" % runType))
    from TileConditions.TileConditionsConf import TileCondToolOfcCool

    #do some check for global flag here: if source='' and flag set, adopt flag
    from AthenaCommon.GlobalFlags import globalflags
    isMC = (globalflags.DataSource() != 'data')

    tool = None
    if source == 'COOL':
        # There are OFC for OF1 only in DB used in Run2
        if ofcType == 'OF1' and not (isUsedDataBaseRun2 or isMC): return None

        from TileCoolMgr import GetTileOfcCoolSource, AddTileOfcCoolSource, tileCoolMgr
        
        proxySource = GetTileOfcCoolSource(ofcType, runType)
        if not tileCoolMgr.isSourceAvailable(proxySource):
            splitOnline = isUsedDataBaseRun2 and (runType == 'PHY')
            AddTileOfcCoolSource(ofcType, runType, splitOnline)
            
        proxyName = 'TileCondProxyCool_' + proxySource

        tool = TileCondToolOfcCool(name, ProxyOfcCool = getTileCondProxy('COOL', 'Ofc', proxySource, proxyName))

    else:
        raise(Exception("Invalid source %s" % source))

        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool


#
#____________________________________________________________________________
def getTileCondToolTMDB(source = 'FILE', runType = 'PHY', name = 'TileCondToolTMDB', **kwargs):

    if not runType in validRunTypes: raise(Exception("Invalid run type %s" % runType))
    from TileConditions.TileConditionsConf import TileCondToolTMDB

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None

    if not isUsedDataBaseRun2: return None
    
    if source == 'COOL': 
        run = string.capwords(runType)
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolTMDB(name,
                                ProxyThreshold = getTileCondProxy('COOL', 'Flt', 'onlTmdbThreshold' + run, 'TileCondProxyCool_TmdbThreshold' + run),
                                ProxyDelay = getTileCondProxy('COOL', 'Flt', 'onlTmdbDelay' + run, 'TileCondProxyCool_TmdbDelay' + run),
                                ProxyTMF = getTileCondProxy('COOL', 'Flt', 'onlTmdbTmf' + run, 'TileCondProxyCool_TmdbTmf' + run),
                                ProxyCalib = getTileCondProxy('COOL', 'Flt', 'onlTmdbCalibPhy', 'TileCondProxyCool_TmdbCalibPhy'))
                                


        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        raise(Exception("Invalid source %s" % source))

       
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

    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================

        if not isUsedDataBaseRun2: return None

        tool = TileCondToolDspThreshold(name,
                                         ProxyDspThreshold = getTileCondProxy('COOL', 'Flt', 'oflDspThreshold', 'TileCondProxyCool_DspThreshold'))
        
    elif  source == 'FILE':
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolDspThreshold(name,
                                         ProxyDspThreshold = getTileCondProxy('FILE', 'Flt', 'TileDefault.dspThreshold', 'TileCondProxyFile_DspThreshold'))
        

    else:
        #====================================================
        #=== guess source is file name
        #====================================================

        file_name = find_data_file(source)
        if file_name is not None:
            tool = TileCondToolDspThreshold(name,
                                             ProxyDspThreshold = getTileCondProxy('FILE', 'Flt', file_name, 'TileCondProxyFile_DspThreshold'))

        else:
            raise(Exception("Invalid source: %s" %source ))


    #=== set the arguments passed and return tool
    for n, v in kwargs.items():
        setattr(tool, n, v)
    return tool




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
