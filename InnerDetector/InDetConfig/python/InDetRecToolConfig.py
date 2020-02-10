import IOVDbSvc.IOVDbSvcConfig as iovDbSvc

def InDetTrackSummaryHelperToolCfg(flags, name='InDetTrackSummaryHelperTool', **kwargs):
  acc = InDetTrackHoleSearchToolCfg(flags)
  
  #FIXME - need InDet to provide configuration for PixelConditionsSummaryTool
  # Also assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
  kwargs.setdefault("HoleSearch", acc.getPrimary())
  acc.addPublicTool(CompFactory.InDet__InDetTrackSummaryHelperTool(name, **kwargs), primary=True)

def InDetTrackHoleSearchToolCfg(flags, name = 'InDetHoleSearchTool', **kwargs):
  from I
  acc = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    tmpAcc =  InDetExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator"     = tmpAcc.getPrimary())
    acc.merge(tmpAcc)

  if ('PixelSummaryTool' not in kwargs):
    if flags.Detector.haveRIO.pixel_on:
      tmpAcc = InDetPixelConditionsSummaryToolCfg(flags)
      kwargs.setdefault("PixelSummaryTool" = tmpAcc.popPrivateTools())
      acc.merge(tmpAcc)
    else:
      kwargs.setdefault("PixelSummaryTool" = None)

  if ('SctSummaryTool' not in kwargs):
    if flags.Detector.haveRIO.SCT_on:
      tmpAcc = InDetSCT_ConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool" = tmpAcc.popPrivateTools())
      acc.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool" = None)

  if 'PixelLayerTool' not in kwargs :
    tmpAcc = InDetTestPixelLayerToolCfg(flags)
    kwargs.setdefault("PixelLayerTool"   = tmpAcc.getPrimary())
    acc.merge(tmpAcc)

  if flags.InDet.doCosmics :
    kwargs.setdefault("Cosmics" = True)

  kwargs.setdefault( "usePixel"                     = flags.Detector.haveRIO.pixel_on,
                     "useSCT"                       = flags.Detector.haveRIO.SCT_on,
                     "CountDeadModulesAfterLastHit" = True)

  indet_hole_search_tool = CompFactory.InDet__InDetTrackHoleSearchTool(name, **kwargs)
  acc.addPublicTool(indet_hole_search_tool, primary=True)
  return acc

#FIXME: Andi says this and all subtools may become private without needing additional changes
def InDetExtrapolatorCfg(flags, name='InDetExtrapolator', **kwargs) :
    acc = ComponentAccumulator()
    # FIXME copied from the old config, also needs fixing on the c++ side.
    if 'Propagators' not in kwargs :
        tmpAcc = InDetPropagatorCfg(flags)
        kwargs.setdefault( "Propagators" = [tmpAcc.getPrimary()  ] ) # [ InDetPropagator, InDetStepPropagator ],
        acc.merge(tmpAcc)
    propagator= kwargs.get('Propagators')[0].name() if  kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        tmpAcc = InDetMaterialEffectsUpdatorCfg(flags)
        kwargs.setdefault( "MaterialEffectsUpdators" = [tmpAcc.getPrimary() ] )
        acc.merge(tmpAcc)
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name() if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        tmpAcc = InDetNavigatorCfg(flags)
        kwargs.setdefault( "Navigator"               = tmpAcc.getPrimary())
        acc.merge(tmpAcc)

    sub_propagators = []
    sub_updators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators    += [ material_updator ]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs.setdefault("SubPropagators"          = sub_propagators,
                      "SubMEUpdators"           = sub_updators)

    extrapolator = CompFactory.Trk__Extrapolator(name, **kwargs)
    acc.addPublicTool(extrapolator, primary=True)
    return acc

def InDetPixelConditionsSummaryToolCfg(flags, name = "InDetMaterialEffectsUpdator", **kwargs) :
    if not flags.InDet.solenoidOn:
        import AthenaCommon.SystemOfUnits as Units
        kwargs.setdefault(   EnergyLoss          = False,
                             ForceMomentum       = True,
                             ForcedMomentumValue = 1000*Units.MeV )
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk__MaterialEffectsUpdator(name, **kwargs))
    return acc



def InDetSCT_ConditionsSummaryToolCfg(flags, name = "InDetSCT_ConditionsSummaryTool", **kwargs)
  acc = ComponentAccumulator()
  
  # Load conditions configuration service and load folders and algorithm for it
  # Load folders that have to exist for both MC and Data
  SCTConfigurationFolderPath='/SCT/DAQ/Config/'
  #if its COMP200, use old folders...
  if (flags.IOVDb.DatabaseInstance == "COMP200"):
      SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
  #...but now check if we want to override that decision with explicit flag (if there is one)
  if flags.InDet.ForceCoraCool:
      SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
  if flags.InDet.ForceCoolVectorPayload:
      SCTConfigurationFolderPath='/SCT/DAQ/Config/'
  
  if (flags.InDet.ForceCoolVectorPayload and flags.InDet.ForceCoraCool):
      printfunc ('*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****')
      SCTConfigurationFolderPath=''

  cond_kwargs={"ChannelFolder" = SCTConfigurationFolderPath+"Chip",
               "ModuleFolder"  = SCTConfigurationFolderPath+"Module",
               "MurFolder"     = SCTConfigurationFolderPath+"MUR"}
  cfgCondToolAcc = SCT_ConfigurationConditionsToolCfg(flags,name, cond_kwargs=cond_kwargs)
  acc.merge(cfgCondToolAcc)
  SCT_ConfigurationConditionsTool = acc.popPrivateTools()
  if (flags.InDet.doPrintConfigurables):
      printfunc (SCT_ConfigurationConditionsTool)

  # Load calibration conditions tool
  calDataAcc = SCT_ReadCalibDataToolCfg(flags)
  acc.merge(calDataAcc)
  SCT_ReadCalibDataTool = calDataAcc.popPrivateTools()
  if (flags.InDet.doPrintConfigurables):
      printfunc (SCT_ReadCalibDataTool)
  
  # Load flagged condition tool
  flCondToolAcc = SCT_FlaggedConditionToolCfg(flags)
  acc.merge(flCondToolAcc)
  SCT_FlaggedConditionTool = flCondToolAcc.popPrivateTools()
  if (flags.InDet.doPrintConfigurables):
      printfunc (SCT_FlaggedConditionTool)
  
  # Load conditions Monitoring tool
  if not flags.Common.isOnline:
      monCondAcc = SCT_MonitorConditionsToolCfg(flags,{OutputLevel=INFO})
      acc.merge(monCondAcc)
      SCT_MonitorConditionsTool = monCondAcc.popPrivateTools()
      if (flags.InDet.doPrintConfigurables):
          printfunc (SCT_MonitorConditionsTool)

# FIXME: can this be deleted? tool is not used, other consumers of implicitly setup condAlg are not setup here (other consumer only LinkMaskingTool?)
#  if flags.InDet.doSCTModuleVeto:
#      from SCT_ConditionsTools.SCT_ModuleVetoToolSetup import SCT_ModuleVetoToolSetup
#      sct_ModuleVetoToolSetup = SCT_ModuleVetoToolSetup()
#      sct_ModuleVetoToolSetup.setup()
#      InDetSCT_ModuleVetoTool = sct_ModuleVetoToolSetup.getTool()
#      if (flags.InDet.doPrintConfigurables):
#          printfunc (InDetSCT_ModuleVetoTool)

  # Load bytestream errors tool (use default instance without "InDet")
  SCT_BSToolAcc = SCT_ByteStreamErrorsToolCfg(flags, {ConfigTool=SCT_ConfigurationConditionsTool})
  acc.merge(SCT_BSToolAcc)
  SCT_ByteStreamErrorsTool = SCT_BSToolAcc.popPrivateTools()
  if (flags.InDet.doPrintConfigurables):
      printfunc (SCT_ByteStreamErrorsTool)
  
  if flags.InDet.useSctDCS:
      DCSCondAlg_kwargs = {}
      if flags.InDet.useHVForSctDCS:
        DCSCondAlg_kwargs["UseDefaultHV"] = True #Hack to use ~20V cut for SCT DCS rather than ChanStat for startup
      DCSCondToolAcc = SCT_DCSConditionsToolCfg(flags, DCSCondAlg_kwargs=DCSCondAlg_kwargs)
      SCT_DCSConditionsTool = DCSCondToolAcc.popPrivateTools()
      if (flags.InDet.doPrintConfigurables):
          printfunc (SCT_DCSConditionsTool)
  
  if (flags.Input.isMC == False):
      printfunc ("Conditions db instance is ", flags.IOVDb.DatabaseInstance)
      # Load Tdaq enabled tools for data only and add some to summary tool for data only
      tdaqFolder = '/TDAQ/EnabledResources/ATLAS/SCT/Robins'
      if ( flags.IOVDb.DatabaseInstance == "CONDBR2"):
          tdaqFolder = '/TDAQ/Resources/ATLAS/SCT/Robins'
      # Load TdaqEnabled tool
      TdaqCondAlg_kwargs={tdaqFolder=tdaqFolder}
      TdaqToolAcc = SCT_TdaqEnabledToolCfg(flags, TdaqCondAlg_kwargs=TdaqCondAlg_kwargs)
      acc.merge(TdaqToolAcc)
      SCT_TdaqEnabledTool = TdaqToolAcc.popPrivateTools()
      if (flags.InDet.doPrintConfigurables):
          printfunc (SCT_TdaqEnabledTool)
      
      # Configure summary tool
      ConditionsTools =  [SCT_ConfigurationConditionsTool,
                          SCT_FlaggedConditionTool,
                          SCT_ByteStreamErrorsTool,
                          SCT_ReadCalibDataTool,
                          SCT_TdaqEnabledTool]
      if not athenaCommonFlags.isOnline():
          ConditionsTools += [ SCT_MonitorConditionsTool ]

      if flags.InDet.useSctDCS:
          ConditionsTools += [ SCT_DCSConditionsTool ]
     
  # switch conditions off for SLHC usage
  elif flags.InDet.doSLHC:
      ConditionsTools= []
    
  else :
      ConditionsTools= [ SCT_ConfigurationConditionsTool,
                         SCT_FlaggedConditionTool,
                         SCT_MonitorConditionsTool,
                         SCT_ReadCalibDataTool]
      if flags.InDet.useSctDCS:
          ConditionsTools += [ SCT_DCSConditionsTool ]

  if flags.InDet.doSCTModuleVeto:
      ConditionsTools += [ SCT_MonitorConditionsTool ]
  
  kwargs.setdefault("ConditionsTools", ConditionsTools)
  InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name, **kwargs)
  if (flags.InDet.doPrintConfigurables):
      printfunc (InDetSCT_ConditionsSummaryTool)

  acc.setPrivateTools(InDetSCT_ConditionsSummaryTool)

def SCT_ConfigurationConditionsToolCfg(flags, name="SCT_ConfigurationConditionsTool", cond_kwargs, **kwargs):

  cond_kwargs.setdefault("ChannelFolder","/SCT/DAQ/Config/Chip")
  cond_kwargs.setdefault("ModuleFolder","/SCT/DAQ/Config/Module")
  cond_kwargs.setdefault("MurFolder","/SCT/DAQ/Config/MUR")
  cond_kwargs.setdefault("dbInstance","SCT")
  cond_kwargs.setdefault("SCT_ConfigurationCondAlgName","SCT_ConfigurationCondAlg")
  cond_kwargs.setdefault("ChannelFolderDB",None)
  cond_kwargs.setdefault("ModuleFolderDB",None)
  cond_kwargs.setdefault("MurFolderDB",None)

  acc = ComponentAccumulator()
  if not cond_kwargs['ChannelFolderDB']:
    acc.merge(iovDbSvc.addFoldersSplitOnline(flags,
                                             detDb=cond_kwargs['dbInstance'],
                                             cond_kwargs["ChannelFolder"],
                                             cond_kwargs["ChannelFolder"],
                                             className='CondAttrListVec',
                                             splitMC=True))
  else:
    acc.merge(iovDbSvc.addFolders(detDb = cond_kwargs['dbInstance'], [cond_kwargs["ChannelFolderDB"]], className='CondAttrListVec'))
  if not cond_kwargs['ModuleFolderDB']:
    acc.merge(iovDbSvc.addFoldersSplitOnline(flags,
                                             detDb=cond_kwargs['dbInstance'],
                                             cond_kwargs["ModuleFolder"],
                                             cond_kwargs["ModuleFolder"],
                                             className='CondAttrListVec',
                                             splitMC=True))
  else:
    acc.merge(iovDbSvc.addFolders(detDb = cond_kwargs['dbInstance'], [cond_kwargs["ModuleFolderDB"]], className='CondAttrListVec'))
  if not cond_kwargs['MurFolderDB']:
    acc.merge(iovDbSvc.addFoldersSplitOnline(flags,
                                             detDb=cond_kwargs['dbInstance'],
                                             cond_kwargs["MurFolder"],
                                             cond_kwargs["MurFolder"],
                                             className='CondAttrListVec',
                                             splitMC=True))
  else:
    acc.merge(iovDbSvc.addFolders(detDb = cond_kwargs['dbInstance'], [cond_kwargs["MurFolderDB"]], className='CondAttrListVec'))

  ConfigCondAlg_kwargs={}
  ConfigCondAlg_kwargs["ReadKeyChannel"]=cond_kwargs["ChannelFolder"]
  ConfigCondAlg_kwargs["ReadKeyModule"] =cond_kwargs["ModuleFolder"]
  ConfigCondAlg_kwargs["ReadKeyMur"]    =cond_kwargs["MurFolder"]
  acc.merge(SCT_ConfigurationCondAlgCfg(flags,name="SCT_ConfigurationCondAlg", **ConfigCondAlg_kwargs))

  tool = CompFactory.SCT_ConfigurationConditionsTool(name, **kwargs)
  acc.setPrivateTools(tool)

  return acc


def SCT_ReadCalibDataToolCfg(flags, name="SCT_ReadCalibDataTool",addPublicTool cond_kwargs, **kwargs):
  acc = ComponentAccumulator()

  cond_kwargs.setdefault("NoiseFolder","/SCT/DAQ/Calibration/NoiseOccupancyDefects")
  cond_kwargs.setdefault("GainFolder","/SCT/DAQ/Calibration/NPtGainDefects")
  cond_kwargs.setdefault("ReadCalibDataCondAlgName","SCT_ReadCalibDataCondAlg")

  acc.merge(iovDbSvc.addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           cond_kwargs["NoiseFolder"],
                                           cond_kwargs["NoiseFolder"],
                                           className='CondAttrListVec',
                                           splitMC=True))
  acc.merge(iovDbSvc.addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           cond_kwargs["GainFolder"],
                                           cond_kwargs["GainFolder"],
                                           className='CondAttrListVec',
                                           splitMC=True))

  acc.merge(SCT_ReadCalibDataCondAlgCfg(flags,
                                        name = cond_kwargs["ReadCalibDataCondAlgName"],
                                        ReadKeyGain = cond_kwargs["GainFolder"],
                                        ReadKeyNoise = cond_kwargs["NoiseFolder"]))

  acc.setPrivateTools(CompFactory.SCT_ReadCalibDataTool(name,**kwargs))
  return acc


def SCT_FlaggedConditionToolCfg(flags, name="SCT_FlaggedConditionTool", **kwargs):
  tool = CompFactory.SCT_FlaggedConditionTool(name, **kwargs)
  acc = ComponentAccumulator()
  acc.setPrivateTools(tool)
  return acc

def SCT_MonitorConditionsToolCfg(flags, name="InDetSCT_MonitorConditionsTool", **kwargs)
  acc.setPrivateTools(tool)
  return acc

def SCT_ByteStreamErrorsToolCfg
  acc.setPrivateTools(tool)
  return acc

def SCT_DCSConditionsToolCfg(flags):
  acc.setPrivateTools(tool)
  return acc

def SCT_TdaqEnabledToolCfg(flags):
  acc.setPrivateTools(tool)
  return acc

def InDetTestPixelLayerToolCfg(flags):
  acc.addPublicTool(tool, primary=True)


def InDetPropagatorCfg(flags):
  acc.addPublicTool(tool, primary=True)

def InDetMaterialEffectsUpdatorCfg(flags):
  acc.addPublicTool(tool, primary=True)

def InDetNavigatorCfg(flags):
  acc.addPublicTool(tool, primary=True)

def TRT_StrawStatusSummaryToolCfg(flags, name='TRT_StrawStatusSummaryTool', **kwargs):
  offlineFolders = []
  offlineFolders.append("/TRT/Cond/StatusHT")
  onlineFolders = []
  onlineFolders.append("/TRT/Onl/Cond/StatusHT")
  acc.merge(iovDbSvc.addFoldersSplitOnline(flags, detDb='TRT', onlineFolders, offlineFolders, className='TRTCond::StrawStatusMultChanContainer') )

