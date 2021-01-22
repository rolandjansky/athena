# block include this file, many package will want to setup cabling
include.block ('InDetRecExample/InDetRecCabling.py')

from InDetRecExample.InDetJobProperties import InDetFlags

if not ('conddb' in dir()):
  IOVDbSvc = Service("IOVDbSvc")
  from IOVDbSvc.CondDB import conddb

from AthenaCommon.DetFlags import DetFlags

#
# --- Pixel cabling
#
if DetFlags.detdescr.pixel_on() and not 'PixelCabling' in dir():
  from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")

  if not hasattr(condSeq, "PixelConfigCondAlg"):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg

    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
    if (globalflags.DataSource()=='geant4'):
      from PixelDigitization.PixelDigitizationConfig import PixelConfigCondAlg_MC, IdMapping
      condSeq += PixelConfigCondAlg_MC()
      IdMappingDat=IdMapping()

    elif (globalflags.DataSource=='data'):
      from RecExConfig.AutoConfiguration import GetRunNumber
      runNum = GetRunNumber()
      IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
      if (runNum<222222):
        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
      else:
        # Even though we are reading from COOL, set the correct fallback map.
        if (runNum >= 344494):
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
        elif (runNum >= 314940 and runNum < 344494):
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
        elif (runNum >= 289350 and runNum < 314940): # 2016
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
        elif (runNum >= 222222 and runNum < 289350): # 2015
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        else:
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"

      # Data overlay: hard-scatter MC digitization + pileup data configuration
      if (globalflags.isOverlay()):
        from PixelDigitization.PixelDigitizationConfig import PixelConfigCondAlg_MC, IdMapping
        alg = PixelConfigCondAlg_MC()
        alg.CablingMapFileName=IdMappingDat
      else: 
        alg = PixelConfigCondAlg(name="PixelConfigCondAlg", 
                                 CablingMapFileName=IdMappingDat)

      if jobproperties.Beam.beamType() == 'cosmics':
        alg.BarrelTimeJitter=[25.0,25.0,25.0,25.0]
        alg.EndcapTimeJitter=[25.0,25.0,25.0]
        alg.DBMTimeJitter=[25.0,25.0,25.0]
        alg.BarrelNumberOfBCID=[8,8,8,8]
        alg.EndcapNumberOfBCID=[8,8,8]
        alg.DBMNumberOfBCID=[8,8,8]
        alg.BarrelTimeOffset=[100.0,100.0,100.0,100.0]
        alg.EndcapTimeOffset=[100.0,100.0,100.0]
        alg.DBMTimeOffset=[100.0,100.0,100.0]

      condSeq += alg

  #####################
  # Cabling map Setup #
  #####################
  if (conddb.dbdata=="CONDBR2" or (conddb.dbmc=="OFLP200" and geoFlags.isIBL()==True)) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
    conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelHitDiscCnfgAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg
      condSeq += PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg")

  if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
    if not (globalflags.DataSource() == 'geant4'):
      conddb.addFolder("PIXEL", "/PIXEL/ReadoutSpeed", className="AthenaAttributeList")
    else:
      conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList")

  if not hasattr(condSeq, 'PixelReadoutSpeedAlg'):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
    condSeq += PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg")

  if (globalflags.DataSource=='data' and conddb.dbdata == 'CONDBR2'):
    if not conddb.folderRequested("/PIXEL/CablingMap"):
      conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList")

  if not hasattr(condSeq, 'PixelCablingCondAlg'):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
    alg = PixelCablingCondAlg(name="PixelCablingCondAlg")
    if (not conddb.folderRequested("/PIXEL/CablingMap") and not conddb.folderRequested("/PIXEL/Onl/CablingMap")):
      alg.ReadKey = ''
    if (globalflags.DataSource()=='geant4'):
      alg.ReadKey = ''
    if (globalflags.DataSource=='data'):
      from RecExConfig.AutoConfiguration import GetRunNumber
      runNum = GetRunNumber()
      if (runNum<222222):
        alg.ReadKey = ''
      # data-overlay does not need DB for cabling. 
      if (globalflags.isOverlay()):
        alg.ReadKey = ''

    condSeq += alg

  from PixelCabling.PixelCablingConf import PixelCablingSvc
  PixelCablingSvc = PixelCablingSvc()
  ServiceMgr += PixelCablingSvc
  if (InDetFlags.doPrintConfigurables()):
    printfunc  (PixelCablingSvc)
      
#
# --- SCT cabling
#
if DetFlags.detdescr.SCT_on():
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, "SCT_CablingCondAlgFromCoraCool"):
    from AthenaCommon.CfgGetter import getAlgorithm
    SCT_CablingCondAlgFromCoraCool = getAlgorithm("SCT_CablingCondAlgFromCoraCool")
    condSeq += SCT_CablingCondAlgFromCoraCool
    if (InDetFlags.doPrintConfigurables()):
      printfunc  (SCT_CablingCondAlgFromCoraCool)
  
#
# --- TRT cabling
#
if DetFlags.detdescr.TRT_on() and not 'TRT_Cabling' in dir():
  # if bytestream comes from real data
  from TRT_Cabling.TRT_CablingConf import TRT_FillCablingData_DC3
  TRT_Cabling = TRT_FillCablingData_DC3()
  if not (globalflags.DataSource() == 'geant4'): 
    # TRT needs the hack in "the other direction"
    TRT_Cabling.RealData=True
  ToolSvc += TRT_Cabling 
  if (InDetFlags.doPrintConfigurables()):
    printfunc (TRT_Cabling)

  from TRT_Cabling.TRT_CablingConf import TRT_CablingSvc
  TRT_CablingSvc = TRT_CablingSvc() 

  ServiceMgr += TRT_CablingSvc
  if (InDetFlags.doPrintConfigurables()):
    printfunc  (TRT_CablingSvc)
       
