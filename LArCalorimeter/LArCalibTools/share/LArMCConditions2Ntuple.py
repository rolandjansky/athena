import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


#Input Parameters:
# PoolFiles: sequence of pool files to read from though CondProxyProvider
# if not given, read from COOL
#
# RunNumber: Input to COOL IOV-DB if reading from 
#
# RootFile: root file for the ntuple
#
# Objects: List of objects written to ntuple (PEDESTAL OFC, RAMP, 
DBTag='OFLCOND-SDR-BS14T-IBL-06'

if not 'InputDB' in dir():
  InputDB="COOLOFL_LAR/OFLP200"

if not "OFCFolder" in dir():
  OFCFolder="5samples1phase"

if not 'RunNumber' in dir():
  RunNumber=2147483647

if not "RootFile" in dir():
  RootFile="LArConditions.root"

if not "Objects" in dir():
  Objects=["PEDESTAL","RAMP","OFC","MPHYSOVERMCAL","SHAPE","UA2MEV"]
    
if not "DBTag" in dir():
  DBTag="LARCALIB-000-01"


def doObj(objName):
  for o in Objects:
    if o.upper().find(objName.upper())!=-1:
      return True
  return False

def getDBFolderAndTag(folder):
  if "TagSuffix" in dir():
    tag="<tag>"+join(split(folder, '/'),'') + TagSuffix+"</tag>"
  else:
    tag=""
  return "<db>"+InputDB+"</db>"+folder+tag

from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="geant4"
globalflags.InputFormat="pool"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-18-01-03"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit 

#Get identifier mapping (needed by LArConditionsContainer)
svcMgr.IOVDbSvc.GlobalTag=DBTag
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = RunNumber

conddb.addFolder("","<db>COOLOFL_LAR/OFLP200</db>/LAR/BadChannels/BadChannels<key>/LAR/BadChannels/BadChannels</key>")
conddb.addFolder("","<db>COOLOFL_LAR/OFLP200</db>/LAR/BadChannels/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>")

conddb.addOverride('/LAR/Identifier/FebRodAtlas','FebRodAtlas-005')
conddb.addOverride('/LAR/Identifier/OnOffIdAtlas','OnOffIdAtlas-012')


if 'PoolFiles' in dir():
  from AthenaCommon.ConfigurableDb import getConfigurable
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  ServiceMgr += getConfigurable( "CondProxyProvider" )()
  svcMgr.CondProxyProvider.InputCollections=PoolFiles

if 'PoolCat' in dir():
  svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:"+PoolCat]

loadCastorCat=False
  
if doObj("PEDESTAL"):
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/Pedestal"))
  from LArCalibTools.LArCalibToolsConf import LArPedestals2Ntuple
  LArPedestals2Ntuple=LArPedestals2Ntuple("LArPedestals2Ntuple")
  LArPedestals2Ntuple.AddFEBTempInfo=False
  topSequence+=LArPedestals2Ntuple


if doObj("AUTOCORR"):
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibOfl/AutoCorrs/AutoCorr"))
  from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
  LArAutoCorr2Ntuple=LArAutoCorr2Ntuple("LArAutoCorr2Ntuple")
  LArAutoCorr2Ntuple.AddFEBTempInfo=False
  topSequence+=LArAutoCorr2Ntuple


if doObj("OFC"):

  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/HVScaleCorr"))
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/Noise"))
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/AutoCorr"))
  from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
  from LArRecUtils.LArAutoCorrNoiseToolDefault import LArAutoCorrNoiseToolDefault
  theLArADC2MeVToolDefault = LArADC2MeVToolDefault()
  ToolSvc += theLArADC2MeVToolDefault
  theLArAutoCorrNoiseToolDefault = LArAutoCorrNoiseToolDefault()
  theLArAutoCorrNoiseToolDefault.NSamples = 5
  ToolSvc += theLArAutoCorrNoiseToolDefault
  from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
  theOFCTool = LArOFCToolDefault()
  theOFCTool.Dump=True
  ToolSvc += theOFCTool
  
  from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
  LArOFC2Ntuple = LArOFC2Ntuple("LArOFC2Ntuple")
  LArOFC2Ntuple.ContainerKey = "LArOFC"
  LArOFC2Ntuple.AddFEBTempInfo=False
  LArOFC2Ntuple.IsMC = True
  LArOFC2Ntuple.OFCTool = theOFCTool
  topSequence+=LArOFC2Ntuple

if (doObj("SHAPE")):
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/Shape"))
  from LArCalibTools.LArCalibToolsConf import LArShape2Ntuple
  LArShape2Ntuple = LArShape2Ntuple("LArShape2Ntuple")
  LArShape2Ntuple.ContainerKey = "LArShape"
  LArShape2Ntuple.AddFEBTempInfo=False
  LArShape2Ntuple.IsMC = True
  topSequence+=LArShape2Ntuple

if doObj("RAMP"):
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/Ramp"))
  from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
  LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
  LArRamps2Ntuple.NtupleName = "RAMPS"
  LArRamps2Ntuple.RawRamp = False
  LArRamps2Ntuple.IsMC = True
  LArRamps2Ntuple.AddFEBTempInfo=False
  topSequence+=LArRamps2Ntuple

if (doObj("UA2MEV")):
  print ('DAC2uA check : ',getDBFolderAndTag("/LAR/ElecCalibMC/DAC2uA"))
  print ('uA2MeV check : ',getDBFolderAndTag("/LAR/ElecCalibMC/uA2MeV"))
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/DAC2uA"))
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/uA2MeV"))
  from LArCalibTools.LArCalibToolsConf import LAruA2MeV2Ntuple
  LAruA2MeV2Ntuple=LAruA2MeV2Ntuple("LAruA2MeV2Ntuple")
  LAruA2MeV2Ntuple.AddFEBTempInfo=False
  topSequence+=LAruA2MeV2Ntuple

if (doObj("MPHYSOVERMCAL")):
  conddb.addFolder("",getDBFolderAndTag("/LAR/ElecCalibMC/MphysOverMcal"))
  from LArCalibTools.LArCalibToolsConf import LArMphysOverMcal2Ntuple
  LArMphysOverMcal2Ntuple=LArMphysOverMcal2Ntuple("LArMphysOverMcal2Ntuple")
  LArMphysOverMcal2Ntuple.AddFEBTempInfo=False
  LArMphysOverMcal2Ntuple.IsMC=True
  topSequence+=LArMphysOverMcal2Ntuple

if loadCastorCat:
  svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:'+'/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml']

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFile+"' OPT='NEW'" ]

svcMgr.MessageSvc.OutputLevel = DEBUG

svcMgr.IOVDbSvc.DBInstance="OFLP200"
