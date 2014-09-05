# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#file: TileCondToolConf.py
#author: nils.gollub@cern.ch

from TileConditions.TileCondProxyConf  import *
from AthenaCommon.Constants import INFO

#=== check Athena running mode
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
isOnline=athenaCommonFlags.isOnline()

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
def getTileCondToolIntegrator(source='FILE', name='TileCondToolIntegrator', **kwargs):
    
    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileCondToolIntegrator
    
    #do some check for global flag here: if source='' and flag set, adopt flag
    
    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolIntegrator(name,
                                       ProxyIntegrator = getTileCondProxy('COOL','Flt','oflIntGain','TileCondProxyCool_Integrator'))
        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolIntegrator(name,
                                       ProxyIntegrator = getTileCondProxy('FILE','Flt','TileDefault.int','TileCondProxyFile_Integrator'))
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolMuID(source='FILE', name='TileCondToolMuID', **kwargs):
    
    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileCondToolMuID
    
    #do some check for global flag here: if source='' and flag set, adopt flag
    
    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolMuID(name,
                                       ProxyMuID = getTileCondProxy('COOL','Flt','onlMuID','TileCondProxyCool_MuID'))
        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolMuID(name,
                                       ProxyMuID = getTileCondProxy('FILE','Flt','TileDefault.muid','TileCondProxyFile_MuID'))
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolTiming(source = 'FILE', runType = 'PHY', name = 'TileCondToolTiming', **kwargs):

    if not source  in validSources : raise(Exception("Invalid source: %s" %source ))
    if not runType in validRunTypes: raise(Exception("Invalid run type %s"%runType))
    from TileConditions.TileConditionsConf import TileCondToolTiming

    #do some check for global flag here: if source='' and flag set, adopt flag
    tool = None
    if source == 'COOL':
        if runType == 'PHY' or runType == 'PED':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt','oflTimeCphy','TileCondProxyCool_AdcOffset'))
        if runType == 'LAS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt','oflTimeClas','TileCondProxyCool_AdcOffset'))
        if runType == 'GAPLAS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt','oflTimeCgapLas','TileCondProxyCool_AdcOffset'))
        if runType == 'CIS':
            tool = TileCondToolTiming(name
                                      , ProxyAdcOffset = getTileCondProxy('COOL','Flt','oflTimeCcis','TileCondProxyCool_AdcOffset'))
    else:
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
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolPulseShape(source='FILE', runType='PHY', name='TileCondToolPulseShape', **kwargs):

    if not source  in validSources : raise(Exception("Invalid source: %s" %source ))
    if not runType in validRunTypes: raise(Exception("Invalid run type %s"%runType))
    from TileConditions.TileConditionsConf import TileCondToolPulseShape

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    if source=='COOL':
        #=== create tool
        if runType=='PHY':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsPhy','TileCondProxyCool_PulseShapePhy'),
                                          )
        if runType=='LAS':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsLas','TileCondProxyCool_PulseShapeLas'),
                                          )
        if runType=='CISPULSE100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisPl100','TileCondProxyCool_PulseShapeCisPl100'),
                                          )
        if runType=='CISPULSE5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisPl5p2','TileCondProxyCool_PulseShapeCisPl5p2'),
                                          )
        if runType=='CISLEAK100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisLk100','TileCondProxyCool_PulseShapeCisLk100'),
                                          )
        if runType=='CISLEAK5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('COOL','Flt','oflPlsCisLk5p2','TileCondProxyCool_PulseShapeCisLk5p2'),
                                          )

    else:
        #=== create tool
        if runType=='PHY':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsPhy','TileCondProxyFile_PulseShapePhy'),
                                          )
        if runType=='LAS':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsLas','TileCondProxyFile_PulseShapeLas'),
                                          )
        if runType=='CISPULSE100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisPulse100','TileCondProxyCool_PulseShapeCisPl100'),
                                          )
        if runType=='CISPULSE5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisPulse5p2','TileCondProxyCool_PulseShapeCisPl5p2'),
                                          )
        if runType=='CISLEAK100':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisLeak100','TileCondProxyCool_PulseShapeCisLk100'),
                                          )
        if runType=='CISLEAK5P2':
            tool = TileCondToolPulseShape(name,
                                          ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsCisLeak5p2','TileCondProxyCool_PulseShapeCisLk5p2'),
                                          )
        
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool
    
#
#____________________________________________________________________________
def getTileCondToolMuRcvPulseShape(source='FILE', name='TileCondToolMuRcvPulseShape', **kwargs):

    if not source  in validSources : raise(Exception("Invalid source: %s" %source ))
    from TileConditions.TileConditionsConf import TileCondToolPulseShape

    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        raise(Exception("Not implemented source: %s" %source ))

    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================

        tool = TileCondToolPulseShape(name
                                      , ProxyPulseShape = getTileCondProxy('FILE','Flt','TileDefault.plsMuRcv','TileCondProxyFile_PulseShapeMuRcv'))

    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileBadChanTool(source='FILE', name='TileBadChanTool', **kwargs):

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
def getTileCondToolNoiseSample(source='FILE', name='TileCondToolNoiseSample', **kwargs):
    
    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileCondToolNoiseSample
    
    #do some check for global flag here: if source='' and flag set, adopt flag
    
    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolNoiseSample(name,
                                       ProxyNoiseSample = getTileCondProxy('COOL','Flt','oflNoiseAdc','TileCondProxyCool_NoiseSample'))
#                                       ProxyNoiseAutoCr = getTileCondProxy('COOL','Flt','oflNoiseAcr','TileCondProxyCool_NoiseAutoCr'))
        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolNoiseSample(name,
                                       ProxyNoiseSample = getTileCondProxy('FILE','Flt','TileDefault.ped','TileCondProxyFile_NoiseSample'))
#                                       ProxyNoiseAutoCr = getTileCondProxy('FILE','Flt','TileDefault.acr','TileCondProxyFile_NoiseAutoCr'))
       
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolAutoCr(source='FILE', name='TileCondToolAutoCr', **kwargs):
    
    if not source in validSources: raise(Exception("Invalid source: %s"%source))
    from TileConditions.TileConditionsConf import TileCondToolAutoCr
    
    #do some check for global flag here: if source='' and flag set, adopt flag
    
    tool = None
    if source=='COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolAutoCr(name,
                                       ProxyNoiseAutoCr = getTileCondProxy('COOL','Flt','oflNoiseAcr','TileCondProxyCool_NoiseAutoCr'))
        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolAutoCr(name,
                                       ProxyNoiseAutoCr = getTileCondProxy('FILE','Flt','TileDefault.acr','TileCondProxyFile_NoiseAutoCr'))
        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool

#
#____________________________________________________________________________
def getTileCondToolNoiseRawChn(source = 'FILE', name = 'TileCondToolNoiseRawChn', **kwargs):
    
    if not source in validSources: raise(Exception("Invalid source: %s" %source))
    from TileConditions.TileConditionsConf import TileCondToolNoiseRawChn
    
    #do some check for global flag here: if source='' and flag set, adopt flag
    
    tool = None
    if source == 'COOL':
        #====================================================
        #=== Connect COOL TileCondProxies to the tool
        #====================================================
        tool = TileCondToolNoiseRawChn(name
                                       , ProxyNoiseRawChn = getTileCondProxy('COOL','Flt','onlNoise1gOfni','TileCondProxyCool_NoiseRawChn'))
        
    else:
        #========================================================
        #=== Connect FILE TileCondProxies to the tool (default)
        #========================================================
        tool = TileCondToolNoiseRawChn(name
                                       , ProxyNoiseRawChn = getTileCondProxy('FILE','Flt','TileDefault.rchFit','TileCondProxyFile_NoiseRawChn'))
        
    #=== set the arguments passed and return tool
    for n, v in kwargs.items():
        setattr(tool, n, v)
    return tool


#
#____________________________________________________________________________
def getTileCellNoiseTool(source='FILE', name='TileCellNoiseTool', **kwargs):
    
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
def getTileCondToolOfcCool(source='FILE', runType='PHY', name='TileCondToolOfcCool', **kwargs):

    if not source  in validSources : raise(Exception("Invalid source: %s" %source ))
    if not runType in validRunTypes: raise(Exception("Invalid run type %s"%runType))
    from TileConditions.TileConditionsConf import TileCondToolOfcCool

    #do some check for global flag here: if source='' and flag set, adopt flag

    tool = None
    if source=='COOL':
        #=== create tool
        if runType=='PHY':
            tool = TileCondToolOfcCool(name,
                                          ProxyOfcCool = getTileCondProxy('COOL','Ofc','onlOfcPhy','TileCondProxyCool_OfcPhy'),
                                          )
        if runType=='LAS':
            tool = TileCondToolOfcCool(name,
                                          ProxyOfcCool = getTileCondProxy('COOL','Ofc','onlOfcLas','TileCondProxyCool_OfcLas'),
                                          )
        if runType=='CISPULSE100':
            tool = TileCondToolOfcCool(name,
                                          ProxyOfcCool = getTileCondProxy('COOL','Ofc','onlOfcCisPl100','TileCondProxyCool_OfcCisPl100'),
                                          )
        if runType=='CISPULSE5P2':
            tool = TileCondToolOfcCool(name,
                                          ProxyOfcCool = getTileCondProxy('COOL','Ofc','onlOfcCisPl5p2','TileCondProxyCool_OfcCisPl5p2'),
                                          )

    else:
        raise(Exception("Invalid source %s"%source))

        
    #=== set the arguments passed and return tool
    for n,v in kwargs.items():
        setattr(tool, n, v)
    return tool


