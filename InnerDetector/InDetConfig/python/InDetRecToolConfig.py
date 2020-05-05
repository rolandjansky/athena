# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders

def InDetTrackSummaryHelperToolCfg(flags, name='InDetTrackSummaryHelperTool', **kwargs):
  result = ComponentAccumulator()
  
  if "HoleSearch" not in kwargs:
    acc = InDetTrackHoleSearchToolCfg(flags)
    # FIXME: assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    kwargs.setdefault("HoleSearch", acc.getPrimary())
    result.merge(acc)

  from InDetOverlay.TRT_ConditionsConfig import TRT_StrawStatusSummaryToolCfg
  tmpAcc = TRT_StrawStatusSummaryToolCfg(flags)
  kwargs.setdefault("TRTStrawSummarySvc", tmpAcc.getPrimary()) 
  result.merge(tmpAcc)

  result.addPublicTool(CompFactory.InDet.InDetTrackSummaryHelperTool(name, **kwargs), primary=True)
  return result

def InDetTrackHoleSearchToolCfg(flags, name = 'InDetHoleSearchTool', **kwargs):
  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    tmpAcc =  InDetExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", tmpAcc.getPrimary())
    result.merge(tmpAcc)

  if ('PixelSummaryTool' not in kwargs):
    if flags.Detector.PixelOn:
      tmpAcc = InDetPixelConditionsSummaryToolCfg(flags)
      kwargs.setdefault("PixelSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("PixelSummaryTool", None)

  if ('SctSummaryTool' not in kwargs):
    if flags.Detector.SCTOn:
      tmpAcc = InDetSCT_ConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    tmpAcc = InDetTestPixelLayerToolCfg(flags)
    kwargs.setdefault("PixelLayerTool", tmpAcc.getPrimary())
    result.merge(tmpAcc)

  if flags.Beam.Type == "cosmics" :
    kwargs.setdefault("Cosmics", True)

  kwargs.setdefault( "usePixel"                     , flags.Detector.PixelOn)
  kwargs.setdefault( "useSCT"                       , flags.Detector.SCTOn)
  kwargs.setdefault( "CountDeadModulesAfterLastHit" , True)

  indet_hole_search_tool = CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs)
  result.addPublicTool(indet_hole_search_tool, primary=True)
  return result


#FIXME: Andi says this and all subtools may become private without needing additional changes
def InDetExtrapolatorCfg(flags, name='InDetExtrapolator', **kwargs) :
    result = ComponentAccumulator()
    # FIXME copied from the old config, also needs fixing on the c++ side.
    if 'Propagators' not in kwargs :
        tmpAcc = InDetPropagatorCfg(flags)
        kwargs.setdefault( "Propagators", [tmpAcc.getPrimary()  ] ) # [ InDetPropagator, InDetStepPropagator ],
        result.merge(tmpAcc)
    propagator= kwargs.get('Propagators')[0].name if kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        tmpAcc = InDetMaterialEffectsUpdatorCfg(flags)
        kwargs.setdefault( "MaterialEffectsUpdators", [tmpAcc.getPrimary() ] )
        result.merge(tmpAcc)
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        tmpAcc = InDetNavigatorCfg(flags)
        kwargs.setdefault( "Navigator"               , tmpAcc.getPrimary())
        result.merge(tmpAcc)

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

    kwargs.setdefault("SubPropagators"          , sub_propagators)
    kwargs.setdefault("SubMEUpdators"           , sub_updators)

    extrapolator = CompFactory.Trk.Extrapolator(name, **kwargs)
    result.addPublicTool(extrapolator, primary=True)
    return result

def InDetPixelConditionsSummaryToolCfg(flags, name = "InDetPixelConditionsSummaryTool", **kwargs):
    kwargs.setdefault( "UseByteStream", not flags.Input.isMC)

    if flags.InDet.usePixelDCS:
        kwargs.setdefault( "IsActiveStates", [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ] )
        kwargs.setdefault( "IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ] )
    
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name, **kwargs))
    return result


def InDetSCT_ConditionsSummaryToolCfg(flags, name = "InDetSCT_ConditionsSummaryTool", **kwargs) :
  result = ComponentAccumulator()
  
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
      print ('*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****')
      SCTConfigurationFolderPath=''

  cond_kwargs={"ChannelFolder" : SCTConfigurationFolderPath+"Chip",
               "ModuleFolder"  : SCTConfigurationFolderPath+"Module",
               "MurFolder"     : SCTConfigurationFolderPath+"MUR"}
  cfgCondToolAcc = SCT_ConfigurationConditionsToolCfg(flags,name, cond_kwargs=cond_kwargs)
  SCT_ConfigurationConditionsTool = cfgCondToolAcc.popPrivateTools()
  result.merge(cfgCondToolAcc)
  if (flags.InDet.doPrintConfigurables):
      print (SCT_ConfigurationConditionsTool)

  # Load calibration conditions tool
  calDataAcc = SCT_ReadCalibDataToolCfg(flags)
  SCT_ReadCalibDataTool = calDataAcc.popPrivateTools()
  result.merge(calDataAcc)
  if (flags.InDet.doPrintConfigurables):
      print (SCT_ReadCalibDataTool)
  
  # Load flagged condition tool
  withFlaggedCondTool=kwargs.pop("withFlaggedCondTool",True)

  if withFlaggedCondTool:
    flCondToolAcc = SCT_FlaggedConditionToolCfg(flags)
    SCT_FlaggedConditionTool = flCondToolAcc.popPrivateTools()
    result.merge(flCondToolAcc)
    if (flags.InDet.doPrintConfigurables):
      print (SCT_FlaggedConditionTool)
  
  # Load conditions Monitoring tool
  if not flags.Common.isOnline:
      monCondAcc = SCT_MonitorConditionsToolCfg(flags)
      SCT_MonitorConditionsTool = monCondAcc.popPrivateTools()
      result.merge(monCondAcc)
      if (flags.InDet.doPrintConfigurables):
          print (SCT_MonitorConditionsTool)

# FIXME: can this be deleted? tool is not used, other consumers of implicitly setup condAlg are not setup here (other consumer only LinkMaskingTool?)
#  if flags.InDet.doSCTModuleVeto:
#      from SCT_ConditionsTools.SCT_ModuleVetoToolSetup import SCT_ModuleVetoToolSetup
#      sct_ModuleVetoToolSetup = SCT_ModuleVetoToolSetup()
#      sct_ModuleVetoToolSetup.setup()
#      InDetSCT_ModuleVetoTool = sct_ModuleVetoToolSetup.getTool()
#      if (flags.InDet.doPrintConfigurables):
#          print (InDetSCT_ModuleVetoTool)

  # Load bytestream errors tool (use default instance without "InDet")
  SCT_BSToolAcc = SCT_ByteStreamErrorsToolCfg(flags, **{"ConfigTool" : SCT_ConfigurationConditionsTool})
  SCT_ByteStreamErrorsTool = SCT_BSToolAcc.popPrivateTools()
  result.merge(SCT_BSToolAcc)
  if (flags.InDet.doPrintConfigurables):
      print (SCT_ByteStreamErrorsTool)
  
  if flags.InDet.useSctDCS:
      from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg # FIXME this doesn't seem to have the UseDefaultHV hack from the old config?
      SCT_DCSCondAcc = SCT_DCSConditionsCfg(flags)
      SCT_DCSConditionsTool = SCT_DCSCondAcc.popPrivateTools()
      result.merge(SCT_DCSCondAcc)
      if (flags.InDet.doPrintConfigurables):
          print (SCT_DCSConditionsTool)
  
  if not flags.Input.isMC :
      print ("Conditions db instance is ", flags.IOVDb.DatabaseInstance)
      TdaqToolAcc = SCT_TdaqEnabledToolCfg(flags)
      SCT_TdaqEnabledTool = TdaqToolAcc.popPrivateTools()
      result.merge(TdaqToolAcc)
      if (flags.InDet.doPrintConfigurables):
          print (SCT_TdaqEnabledTool)
      
      # Configure summary tool
      ConditionsTools =  [SCT_ConfigurationConditionsTool,]
      if withFlaggedCondTool:
        ConditionsTools.append(SCT_FlaggedConditionTool)
      ConditionsTools+= [SCT_ByteStreamErrorsTool,
                         SCT_ReadCalibDataTool,
                         SCT_TdaqEnabledTool]

      if not flags.Common.isOnline:
          ConditionsTools += [ SCT_MonitorConditionsTool ]

      if flags.InDet.useSctDCS:
          ConditionsTools += [ SCT_DCSConditionsTool ]
     
  # switch conditions off for SLHC usage
  elif flags.InDet.doSLHC:
      ConditionsTools= []
    
  else :
      ConditionsTools= [ SCT_ConfigurationConditionsTool,
                         SCT_MonitorConditionsTool,
                         SCT_ReadCalibDataTool]
      if withFlaggedCondTool:
        ConditionsTools.append(SCT_FlaggedConditionTool)
      if flags.InDet.useSctDCS:
          ConditionsTools += [ SCT_DCSConditionsTool ]

  if flags.InDet.doSCTModuleVeto:
      ConditionsTools += [ SCT_MonitorConditionsTool ]
  
  kwargs.setdefault("ConditionsTools", ConditionsTools)
  InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name, **kwargs)
  if (flags.InDet.doPrintConfigurables):
      print (InDetSCT_ConditionsSummaryTool)
  result.setPrivateTools(InDetSCT_ConditionsSummaryTool)
  return result

def SCT_ConfigurationConditionsToolCfg(flags, name="SCT_ConfigurationConditionsTool", cond_kwargs={}, **kwargs):
  cond_kwargs.setdefault("ChannelFolder","/SCT/DAQ/Config/Chip")
  cond_kwargs.setdefault("ModuleFolder","/SCT/DAQ/Config/Module")
  cond_kwargs.setdefault("MurFolder","/SCT/DAQ/Config/MUR")
  cond_kwargs.setdefault("dbInstance","SCT")
  cond_kwargs.setdefault("SCT_ConfigurationCondAlgName","SCT_ConfigurationCondAlg")

  result = ComponentAccumulator()
  if 'ChannelFolderDB' not in cond_kwargs:
    result.merge(addFoldersSplitOnline(flags,
                                             detDb=cond_kwargs['dbInstance'],
                                             online_folders=cond_kwargs["ChannelFolder"],
                                             offline_folders=cond_kwargs["ChannelFolder"],
                                             className='CondAttrListVec',
                                             splitMC=True))
  else:
    result.merge(addFolders(flags, [cond_kwargs["ChannelFolderDB"]], detDb = cond_kwargs['dbInstance'], className='CondAttrListVec'))
  if 'ModuleFolderDB' not in cond_kwargs:
    result.merge(addFoldersSplitOnline(flags,
                                             detDb=cond_kwargs['dbInstance'],
                                             online_folders=cond_kwargs["ModuleFolder"],
                                             offline_folders=cond_kwargs["ModuleFolder"],
                                             className='CondAttrListVec',
                                             splitMC=True))
  else:
    result.merge(addFolders(flags, [cond_kwargs["ModuleFolderDB"]], detDb = cond_kwargs['dbInstance'], className='CondAttrListVec'))
  if 'MurFolderDB' not in cond_kwargs:
    result.merge(addFoldersSplitOnline(flags,
                                             detDb=cond_kwargs['dbInstance'],
                                             online_folders=cond_kwargs["MurFolder"],
                                             offline_folders=cond_kwargs["MurFolder"],
                                             className='CondAttrListVec',
                                             splitMC=True))
  else:
    result.merge(addFolders(flags, [cond_kwargs["MurFolderDB"]], detDb = cond_kwargs['dbInstance'],  className='CondAttrListVec'))

  ConfigCondAlg_kwargs={}
  ConfigCondAlg_kwargs["ReadKeyChannel"]=cond_kwargs["ChannelFolder"]
  ConfigCondAlg_kwargs["ReadKeyModule"] =cond_kwargs["ModuleFolder"]
  ConfigCondAlg_kwargs["ReadKeyMur"]    =cond_kwargs["MurFolder"]
  result.merge(SCT_ConfigurationCondAlgCfg(flags,name="SCT_ConfigurationCondAlg", **ConfigCondAlg_kwargs))

  tool = CompFactory.SCT_ConfigurationConditionsTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result

def getSCTDAQConfigFolder(flags) :
  if flags.InDet.ForceCoolVectorPayload and flags.InDet.ForceCoraCool :
        raise Exception('SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected')  
  return '/SCT/DAQ/Config/' if (    flags.InDet.ForceCoolVectorPayload
                                      or (flags.IOVDb.DatabaseInstance != "COMP200"
                                          and not flags.InDet.ForceCoraCool)) else '/SCT/DAQ/Configuration/'


def SCT_ConfigurationCondAlgCfg(flags, name="SCT_ConfigurationCondAlg", **kwargs):
  result = ComponentAccumulator()
  config_folder_prefix = getSCTDAQConfigFolder(flags)
  kwargs.setdefault("ReadKeyChannel", config_folder_prefix+"Chip")
  kwargs.setdefault("ReadKeyModule", config_folder_prefix+"Module")
  kwargs.setdefault("ReadKeyMur", config_folder_prefix+"MUR")

  result.merge(addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           online_folders=config_folder_prefix+"Chip",
                                           offline_folders=config_folder_prefix+"Chip",
                                           className='CondAttrListVec',
                                           splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           online_folders=config_folder_prefix+"Module",
                                           offline_folders=config_folder_prefix+"Module",
                                           className='CondAttrListVec',
                                           splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           online_folders=config_folder_prefix+"MUR",
                                           offline_folders=config_folder_prefix+"MUR",
                                           className='CondAttrListVec',
                                           splitMC=True))
  result.addCondAlgo(CompFactory.SCT_ConfigurationCondAlg(name, **kwargs))
  return result


def SCT_ReadCalibDataToolCfg(flags, name="SCT_ReadCalibDataTool", cond_kwargs={}, **kwargs):
  result = ComponentAccumulator()

  cond_kwargs.setdefault("NoiseFolder","/SCT/DAQ/Calibration/NoiseOccupancyDefects")
  cond_kwargs.setdefault("GainFolder","/SCT/DAQ/Calibration/NPtGainDefects")
  cond_kwargs.setdefault("ReadCalibDataCondAlgName","SCT_ReadCalibDataCondAlg")

  result.merge(addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           online_folders=cond_kwargs["NoiseFolder"],
                                           offline_folders=cond_kwargs["NoiseFolder"],
                                           className='CondAttrListVec',
                                           splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                           detDb="SCT",
                                           online_folders=cond_kwargs["GainFolder"],
                                           offline_folders=cond_kwargs["GainFolder"],
                                           className='CondAttrListVec',
                                           splitMC=True))

  result.addCondAlgo(CompFactory.SCT_ReadCalibDataCondAlg(
                                        name = cond_kwargs["ReadCalibDataCondAlgName"],
                                        ReadKeyGain = cond_kwargs["GainFolder"],
                                        ReadKeyNoise = cond_kwargs["NoiseFolder"]))

  result.setPrivateTools(CompFactory.SCT_ReadCalibDataTool(name,**kwargs))
  return result


def SCT_FlaggedConditionToolCfg(flags, name="SCT_FlaggedConditionTool", **kwargs):
  tool = CompFactory.SCT_FlaggedConditionTool(name, **kwargs)
  result = ComponentAccumulator()
  result.setPrivateTools(tool)
  return result

#<<<<<<< HEAD
#def SCT_MonitorConditionsToolCfg(flags, name="InDetSCT_MonitorConditionsTool", **kwargs):
#  acc = ComponentAccumulator()
#  folder="/SCT/Derived/Monitoring"

#  acc.merge(addFolders(flags,folder, "SCT_OFL", className="CondAttrListCollection"))

#  acc.addCondAlgo(CompFactory.SCT_MonitorCondAlg( "SCT_MonitorCondAlg",ReadKey = folder))
#=======

def SCT_MonitorConditionsToolCfg(flags, name="InDetSCT_MonitorConditionsTool", cond_kwargs={}, **kwargs):
  cond_kwargs.setdefault("Folder", "/SCT/Derived/Monitoring")
  cond_kwargs.setdefault("dbInstance", "SCT_OFL")
  cond_kwargs.setdefault("MonitorCondAlgName", "SCT_MonitorCondAlg")

  result = ComponentAccumulator()

  if  "FolderDb" not in cond_kwargs:
    cond_kwargs["FolderDb"] = cond_kwargs["Folder"]
  result.merge(addFolders(flags, cond_kwargs["FolderDb"], cond_kwargs["dbInstance"], className="CondAttrListCollection"))

  result.addCondAlgo( CompFactory.SCT_MonitorCondAlg(name    = cond_kwargs["MonitorCondAlgName"],
                                                     ReadKey = cond_kwargs["Folder"] ))
#>>>>>>> robert/inDetRecToolsConfig

  tool = CompFactory.SCT_MonitorConditionsTool(name, **kwargs)
  result.setPrivateTools(tool)

  return result


def SCT_ByteStreamErrorsToolCfg(flags, name="SCT_ByteStreamErrorsTool", **kwargs):
  result = SCT_ConfigurationConditionsToolCfg(flags)
  kwargs.setdefault("ConfigTool", result.popPrivateTools())
  tool = CompFactory.SCT_ByteStreamErrorsTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result

def SCT_TdaqEnabledToolCfg(flags):
  # Copied from https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetConditions/SCT_ConditionsTools/python/SCT_TdaqEnabledToolSetup.py
  result = SCT_TdaqEnabledCondAlg(flags)
  tool = CompFactory.SCT_CablingTool()
  result.setPrivateTools(tool)
  return result

def SCT_TdaqEnabledCondAlg(flags):
  if flags.Input.isMC:
    print("Warning: should not setup SCT_TdaqEnabledCond for MC")
    return
  result = ComponentAccumulator()

  #FIXME - is there a better way to do this? What about run3?
  folder = '/TDAQ/Resources/ATLAS/SCT/Robins' if (flags.IOVDb.DatabaseInstance == "CONDBR2") else '/TDAQ/EnabledResources/ATLAS/SCT/Robins'

  result.merge( addFolders(flags, [folder], detDb="TDAQ", className="CondAttrListCollection") )

  # Think there's no need to configure the SCT_CablingTool - the default is fine. 
  result.addCondAlgo( CompFactory.SCT_TdaqEnabledCondAlg() )
  return result

def InDetTestPixelLayerToolCfg(flags, name = "InDetTestPixelLayerTool", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
      tmpAcc = InDetPixelConditionsSummaryToolCfg(flags)
      kwargs.setdefault( "PixelSummaryTool", tmpAcc.getPrimary())
      result.merge(tmpAcc)

  kwargs.setdefault("CheckActiveAreas", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.InDet.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name = the_name, **kwargs)
  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def InDetPropagatorCfg(flags, name='InDetPropagator',**kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  tool = None
  if flags.InDet.propagatorType == "STEP":
    tool = CompFactory.Trk.STEP_Propagator( name = the_name, **kwargs)
  else:
    if flags.InDet.propagatorType == "RungeKutta":
        kwargs.setdefault("AccuracyParameter", 0.0001)
        kwargs.setdefault("MaxStraightLineStep", .004) # Fixes a failed fit
    tool = CompFactory.Trk.RungeKuttaPropagator( name = the_name, **kwargs)

  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def InDetMaterialEffectsUpdatorCfg(flags, name = "InDetMaterialEffectsUpdator", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if not flags.BField.solenoidOn:
      import AthenaCommon.SystemOfUnits as Units
      kwargs.setdefault(EnergyLoss          = False)
      kwargs.setdefault(ForceMomentum       = True)
      kwargs.setdefault(ForcedMomentumValue = 1000*Units.MeV)

  tool = CompFactory.Trk.MaterialEffectsUpdator( name = the_name, **kwargs)
  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def InDetNavigatorCfg(flags, name='InDetNavigator', **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'TrackingGeometrySvc' not in kwargs :
      from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
      tmpAcc = TrackingGeometrySvcCfg(flags)
      kwargs.setdefault("TrackingGeometrySvc", tmpAcc.getPrimary())
      result.merge(tmpAcc)

  tool = CompFactory.Trk.Navigator( name = the_name, **kwargs)
  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['InDet','InDetTrig'] :
        if name[0:len(prefix)] == prefix :
            name=name[len(prefix):]
            default_prefix=prefix
            break
    return default_prefix,name

def makeName( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

def makeNameGetPreAndSuffix( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix,namePrefix,nameSuffix

