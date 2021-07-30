# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders

# @TODO retire once migration to TrackingGeometry conditions data is complete
from InDetRecExample.TrackingCommon import use_tracking_geometry_cond_alg

def ITkPrdAssociationToolCfg(flags, name='ITkPrdAssociationTool',**kwargs) :
  acc = ComponentAccumulator()
  '''
  Provide an instance for all clients in which the tool is only set in c++
  '''
  the_name = makeName( name, kwargs)

  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
  kwargs.setdefault("addTRToutliers", False)

  ITkPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(the_name, **kwargs)
  acc.addPublicTool(ITkPRD_AssociationToolGangedPixels, primary=True)
  return acc

def ITkPrdAssociationTool_setupCfg(flags, name='ITkPrdAssociationTool_setup',**kwargs) :
  '''
  Provide an instance for all clients which set the tool explicitely
  '''
  kwargs.setdefault("SetupCorrect", True)
  return ITkPrdAssociationToolCfg(flags, name, **kwargs)

def ITkTrigPrdAssociationToolCfg(flags, name='ITkTrigPrdAssociationTool_setup',**kwargs) :
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", "ITkTrigPixelClusterAmbiguitiesMap")
  kwargs.setdefault("addTRToutliers", False)
  return ITkPrdAssociationToolCfg(flags, name, **kwargs)

def ITkTrackSummaryHelperToolCfg(flags, name='ITkSummaryHelper', **kwargs):
  result = ComponentAccumulator()

  the_name = makeName( name, kwargs)
  isHLT=kwargs.pop("isHLT",False)

  if 'AssoTool' not in kwargs :
    if not isHLT:
      ITkPrdAssociationTool_setup = result.getPrimaryAndMerge(ITkPrdAssociationTool_setupCfg(flags))
      kwargs.setdefault("AssoTool", ITkPrdAssociationTool_setup)
    else:
      ITkTrigPrdAssociationTool = result.getPrimaryAndMerge(ITkTrigPrdAssociationToolCfg(flags))
      kwargs.setdefault("AssoTool", ITkTrigPrdAssociationTool)

  if "HoleSearch" not in kwargs:
    ITkTrackHoleSearchTool = result.getPrimaryAndMerge(ITkTrackHoleSearchToolCfg(flags))
    # FIXME: assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    kwargs.setdefault("HoleSearch", ITkTrackHoleSearchTool)

  kwargs.setdefault("TRTStrawSummarySvc", "")
  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.ITk.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("useTRT", False)

  result.addPublicTool(CompFactory.InDet.InDetTrackSummaryHelperTool(the_name, **kwargs), primary=True)
  return result

def ITkBoundaryCheckToolCfg(flags, name='ITkBoundaryCheckTool', **kwargs):
  result = ComponentAccumulator()

  if 'SctSummaryTool' not in kwargs:
    if flags.Detector.EnableITkStrip:
      tmpAcc = ITkStripConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    kwargs.setdefault("PixelLayerTool", result.getPrimaryAndMerge(ITkTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("CheckBadSCT", False)

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
  return result


def ITkTrackHoleSearchToolCfg(flags, name = 'ITkHoleSearchTool', **kwargs):
  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    ITkExtrapolator = result.getPrimaryAndMerge(ITkExtrapolatorCfg(flags))
    kwargs.setdefault("Extrapolator", ITkExtrapolator)

  if 'BoundaryCheckTool' not in kwargs:
    ITkBoundaryCheckTool = result.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags))
    kwargs.setdefault('BoundaryCheckTool', ITkBoundaryCheckTool)

  if flags.Beam.Type == "cosmics" :
    kwargs.setdefault("Cosmics", True)

  kwargs.setdefault( "CountDeadModulesAfterLastHit" , True)

  indet_hole_search_tool = CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs)
  result.addPublicTool(indet_hole_search_tool, primary=True)
  return result


#FIXME: Andi says this and all subtools may become private without needing additional changes
def ITkExtrapolatorCfg(flags, name='ITkExtrapolator', **kwargs) :
    result = ComponentAccumulator()
    # FIXME copied from the old config, also needs fixing on the c++ side.
    if 'Propagators' not in kwargs :
         kwargs.setdefault( "Propagators", [result.getPrimaryAndMerge(ITkPropagatorCfg(flags))  ] )
    propagator= kwargs.get('Propagators')[0].name if kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        kwargs.setdefault( "MaterialEffectsUpdators", [result.getPrimaryAndMerge(ITkMaterialEffectsUpdatorCfg(flags)) ] )
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        ITkNavigator = result.getPrimaryAndMerge(ITkNavigatorCfg(flags))
        kwargs.setdefault( "Navigator", ITkNavigator)

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


def ITkStripConditionsSummaryToolCfg(flags, name = "ITkStripConditionsSummaryTool", **kwargs) :
  result = ComponentAccumulator()

  ConditionsTools = []

  if flags.ITk.doStripModuleVeto :
      from SCT_ConditionsTools.SCT_ModuleVetoConfig import SCT_ModuleVetoCfg
      ITkStripModuleVetoTool = result.popToolsAndMerge( SCT_ModuleVetoCfg(flags) )
      ConditionsTools += [ ITkStripModuleVetoTool ]
      if (flags.ITk.doPrintConfigurables):
          print ( ITkStripModuleVetoTool )

  kwargs.setdefault("ConditionsTools", ConditionsTools)
  ITkStripConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name, **kwargs)
  if (flags.ITk.doPrintConfigurables):
      print (ITkStripConditionsSummaryTool)
  result.setPrivateTools(ITkStripConditionsSummaryTool)
  return result

def ITkStripConfigurationConditionsToolCfg(flags, name="ITkStripConfigurationConditionsTool", **kwargs):
  # Load conditions configuration service and load folders and algorithm for it
  # Load folders that have to exist for both MC and Data
  ITkStripConfigurationFolderPath = getITkStripDAQConfigFolder(flags)

  cond_kwargs = {}
  cond_kwargs["ChannelFolder"] = ITkStripConfigurationFolderPath+("Chip" if flags.IOVDb.DatabaseInstance=="COMP200" else "ChipSlim")
  cond_kwargs["ModuleFolder"] = ITkStripConfigurationFolderPath+"Module"
  cond_kwargs["MurFolder"] = ITkStripConfigurationFolderPath+"MUR"
  cond_kwargs["dbInstance"] = "ITkStrip"
  cond_kwargs["SCT_ConfigurationCondAlgName"] = "ITkStripConfigurationCondAlg"

  result = ComponentAccumulator()

  from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
  result.merge(ITkStripGeometryCfg(flags))

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
  result.merge(ITkStripConfigurationCondAlgCfg(flags, name="ITkStripConfigurationCondAlg", **ConfigCondAlg_kwargs))

  tool = CompFactory.SCT_ConfigurationConditionsTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result

def getITkStripDAQConfigFolder(flags) :
  return '/ITkStrip/DAQ/Config/' if ( flags.IOVDb.DatabaseInstance != "COMP200" ) else '/ITkStrip/DAQ/Configuration/'

def ITkStripConfigurationCondAlgCfg(flags, name="ITkStripConfigurationCondAlg", **kwargs):
  result = ComponentAccumulator()
  config_folder_prefix = getITkStripDAQConfigFolder(flags)
  channelFolder = config_folder_prefix+("Chip" if flags.IOVDb.DatabaseInstance=="COMP200" else "ChipSlim")
  kwargs.setdefault("ReadKeyChannel", channelFolder)
  kwargs.setdefault("ReadKeyModule", config_folder_prefix+"Module")
  kwargs.setdefault("ReadKeyMur", config_folder_prefix+"MUR")

  result.merge(addFoldersSplitOnline(flags,
                                     detDb="ITkStrip",
                                     online_folders=channelFolder,
                                     offline_folders=channelFolder,
                                     className='CondAttrListVec',
                                     splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                     detDb="ITkStrip",
                                     online_folders=config_folder_prefix+"Module",
                                     offline_folders=config_folder_prefix+"Module",
                                     className='CondAttrListVec',
                                     splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                     detDb="ITkStrip",
                                     online_folders=config_folder_prefix+"MUR",
                                     offline_folders=config_folder_prefix+"MUR",
                                     className='CondAttrListVec',
                                     splitMC=True))

  ITkStripCablingTool = result.popToolsAndMerge(ITkStripCablingToolCfg(flags))
  kwargs.setdefault("SCT_CablingTool", ITkStripCablingTool)

  ITkStripReadoutTool = result.popToolsAndMerge(ITkStripReadoutToolCfg(flags))
  kwargs.setdefault("SCT_ReadoutTool", ITkStripReadoutTool)

  result.addCondAlgo(CompFactory.SCT_ConfigurationCondAlg(name, **kwargs))
  return result


def ITkStripReadCalibDataToolCfg(flags, name="ITkStripReadCalibDataTool", cond_kwargs={}, **kwargs):
  result = ComponentAccumulator()

  from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
  result.merge( ITkStripGeometryCfg(flags))

  cond_kwargs.setdefault("NoiseFolder","/ITkStrip/DAQ/Calibration/NoiseOccupancyDefects")
  cond_kwargs.setdefault("GainFolder","/ITkStrip/DAQ/Calibration/NPtGainDefects")
  cond_kwargs.setdefault("ReadCalibDataCondAlgName","ITkStripReadCalibDataCondAlg")

  result.merge(addFoldersSplitOnline(flags,
                                     detDb="ITkStrip",
                                     online_folders=cond_kwargs["NoiseFolder"],
                                     offline_folders=cond_kwargs["NoiseFolder"],
                                     className='CondAttrListCollection',
                                     splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                     detDb="ITkStrip",
                                     online_folders=cond_kwargs["GainFolder"],
                                     offline_folders=cond_kwargs["GainFolder"],
                                     className='CondAttrListCollection',
                                     splitMC=True))

  result.addCondAlgo(CompFactory.SCT_ReadCalibDataCondAlg(name = cond_kwargs["ReadCalibDataCondAlgName"],
                                                          ReadKeyGain = cond_kwargs["GainFolder"],
                                                          ReadKeyNoise = cond_kwargs["NoiseFolder"]))

  ITkStripCablingTool = result.popToolsAndMerge(ITkStripCablingToolCfg(flags))
  kwargs.setdefault("SCT_CablingTool", ITkStripCablingTool)

  result.setPrivateTools(CompFactory.SCT_ReadCalibDataTool(name,**kwargs))
  return result


def ITkStripFlaggedConditionToolCfg(flags, name="ITkStripFlaggedConditionTool", **kwargs):
  result = ComponentAccumulator()

  from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
  result.merge( ITkStripGeometryCfg(flags))

  tool = CompFactory.SCT_FlaggedConditionTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result


def ITkStripMonitorConditionsToolCfg(flags, name="ITkStripMonitorConditionsTool", cond_kwargs={}, **kwargs):
  cond_kwargs.setdefault("Folder", "/ITkStrip/Derived/Monitoring")
  cond_kwargs.setdefault("dbInstance", "ITkStrip_OFL")
  cond_kwargs.setdefault("MonitorCondAlgName", "ITkStripMonitorCondAlg")

  kwargs.setdefault("CondKey", "ITkStripMonitorCondData")

  result = ComponentAccumulator()

  from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
  result.merge( ITkStripGeometryCfg(flags))

  if  "FolderDb" not in cond_kwargs:
    cond_kwargs["FolderDb"] = cond_kwargs["Folder"]
  result.merge(addFolders(flags, cond_kwargs["FolderDb"], cond_kwargs["dbInstance"], className="CondAttrListCollection"))

  result.addCondAlgo( CompFactory.SCT_MonitorCondAlg(name    = cond_kwargs["MonitorCondAlgName"],
                                                     ReadKey = cond_kwargs["Folder"],
                                                     WriteKey = kwargs["CondKey"]))

  tool = CompFactory.SCT_MonitorConditionsTool(name, **kwargs)
  result.setPrivateTools(tool)

  return result


def ITkStripByteStreamErrorsToolCfg(flags, name="ITkStripByteStreamErrorsTool", **kwargs):
  result = ITkStripConfigurationConditionsToolCfg(flags)
  kwargs.setdefault("ConfigTool", result.popPrivateTools())
  tool = CompFactory.SCT_ByteStreamErrorsTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result

def ITkStripCablingToolCfg(flags):
  result = ComponentAccumulator()

  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  result.merge(GeoModelCfg(flags))

  from SCT_Cabling.SCT_CablingConfig import SCT_CablingCondAlgCfg
  result.merge(SCT_CablingCondAlgCfg(flags))

  tool = CompFactory.SCT_CablingTool()
  result.setPrivateTools(tool)
  return result

def ITkStripReadoutToolCfg(flags, name="ITkStripReadoutTool", **kwargs):
  result = ITkStripCablingToolCfg(flags)
  kwargs.setdefault("SCT_CablingTool", result.popPrivateTools())

  tool = CompFactory.SCT_ReadoutTool(**kwargs)
  result.setPrivateTools(tool)
  return result

def ITkStripTdaqEnabledToolCfg(flags, name="ITkStripTdaqEnabledTool", **kwargs):
  if flags.Input.isMC:
    print("Warning: should not setup ITkStripTdaqEnabledCond for MC")
    return

  result = ComponentAccumulator()

  # For SCT_ID used in SCT_TdaqEnabledTool
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  result.merge(GeoModelCfg(flags))

  # Folder
  #FIXME - is there a better way to do this? What about run3?
  folder = '/TDAQ/Resources/ATLAS/ITkStrip/Robins' if (flags.IOVDb.DatabaseInstance == "CONDBR2") else '/TDAQ/EnabledResources/ATLAS/ITkStrip/Robins'
  result.merge( addFolders(flags, [folder], detDb="TDAQ", className="CondAttrListCollection") )

  # Algorithm
  kwargs.setdefault( "SCT_CablingTool", result.popToolsAndMerge(ITkStripCablingToolCfg(flags)) )
  result.addCondAlgo( CompFactory.SCT_TdaqEnabledCondAlg(**kwargs) )

  # Tool
  result.setPrivateTools(CompFactory.SCT_TdaqEnabledTool(name=name))
  return result

def ITkTestPixelLayerToolCfg(flags, name = "ITkTestPixelLayerTool", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags)))

  if 'Extrapolator' not in kwargs :
    kwargs.setdefault("Extrapolator", result.getPrimaryAndMerge(ITkExtrapolatorCfg(flags)))

  kwargs.setdefault("CheckActiveAreas", flags.ITk.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.ITk.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.ITk.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name = the_name, **kwargs)
  result.addPublicTool( tool, primary=True )
  return result

def ITkPatternPropagatorCfg(flags, name='ITkPatternPropagator', **kwargs):
    the_name = makeName( name, kwargs)
    result = ComponentAccumulator()
    tool = CompFactory.Trk.RungeKuttaPropagator(name = the_name, **kwargs)
    result.addPublicTool( tool, primary=True )
    return result

def ITkPropagatorCfg(flags, name='ITkPropagator',**kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
  result.merge(MagneticFieldSvcCfg(flags))

  tool = None
  if flags.ITk.propagatorType == "STEP":
    tool = CompFactory.Trk.STEP_Propagator( name = the_name, **kwargs)
  else:
    if flags.ITk.propagatorType == "RungeKutta":
        kwargs.setdefault("AccuracyParameter", 0.0001)
        kwargs.setdefault("MaxStraightLineStep", .004) # Fixes a failed fit
    tool = CompFactory.Trk.RungeKuttaPropagator( name = the_name, **kwargs)

  result.addPublicTool( tool, primary=True)
  return result

def ITkMaterialEffectsUpdatorCfg(flags, name = "ITkMaterialEffectsUpdator", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if not flags.BField.solenoidOn:
      import AthenaCommon.SystemOfUnits as Units
      kwargs.setdefault(EnergyLoss          = False)
      kwargs.setdefault(ForceMomentum       = True)
      kwargs.setdefault(ForcedMomentumValue = 1000*Units.MeV)

  tool = CompFactory.Trk.MaterialEffectsUpdator( name = the_name, **kwargs)
  result.addPublicTool( tool, primary=True )
  return result

def ITkNavigatorCfg(flags, name='ITkNavigator', **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'TrackingGeometrySvc' not in kwargs :
       if not use_tracking_geometry_cond_alg :
              from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
              kwargs.setdefault("TrackingGeometrySvc", result.getPrimaryAndMerge(TrackingGeometrySvcCfg(flags)))
  if 'TrackingGeometryKey' not in kwargs :
       if use_tracking_geometry_cond_alg :
              from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
              result.merge( TrackingGeometryCondAlgCfg(flags) )
              # @TODO howto get the TrackingGeometryKey from the TrackingGeometryCondAlgCfg ?
              kwargs.setdefault("TrackingGeometryKey", 'AtlasTrackingGeometry')

  tool = CompFactory.Trk.Navigator( name = the_name, **kwargs)
  result.addPublicTool( tool, primary=True )
  return result

def ITkPatternUpdatorCfg(flags, name='ITkPatternUpdator', **kwargs):
    the_name = makeName(name, kwargs)
    result = ComponentAccumulator()
    tool = CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    result.setPrivateTools( tool )
    return result

def ITkUpdatorCfg(flags, name = 'ITkUpdator', **kwargs):
    the_name = makeName( name, kwargs )
    result = ComponentAccumulator()

    tool = None
    if flags.ITk.kalmanUpdator == "fast" :
        tool = CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    elif flags.ITk.kalmanUpdator == "weight" :
        tool = CompFactory.Trk.KalmanWeightUpdator(name = the_name, **kwargs)
    elif flags.ITk.kalmanUpdator == "smatrix" :
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name = the_name, **kwargs)
    elif flags.ITk.kalmanUpdator == "amg" :
        tool = CompFactory.Trk.KalmanUpdatorAmg(name = the_name, **kwargs)
    else :
        tool = CompFactory.Trk.KalmanUpdator(name = the_name, **kwargs)

    result.setPrivateTools( tool )
    return result

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['ITk'] :
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

