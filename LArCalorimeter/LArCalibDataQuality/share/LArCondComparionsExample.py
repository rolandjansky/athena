from string import *

if "refDB" not in dir():
  refDB="COOLOFL_LAR/COMP200"

if "valDB" not in dir():
  valDB="sqlite://;schema=freshConstants_complete.db;dbname=COMP200"


if "refRN" not in dir():
  refRN=2147483647

if "valRN" not in dir():
  valRN=2147483647
  
if "tagSuffix" not in dir():
  TagSuffix="-UPD3-00"

if "Object" not in dir():
  Object="Pedestal"

DoFebAverages=True
PrintFailedPattern=True
DoNtuple=False

from LArConditionsCommon.LArExtendedFTGrouping import LArExtendedFTGrouping
if 'Partitions' in dir() or 'Gains' in dir():
  if 'Partitions' not in dir():
    Partitions=['EMB','EC'] #means all
  if 'Gains' not in dir():
    Gains=[0,1,2]
  g=LArExtendedFTGrouping()
  sel=g.makeRange(Partitions,Gains)
  ccs="<channelSelection>"+sel+"</channelSelection>"

elif "selection" in dir():
  ccs="<channelSelection>"+selection+"</channelSelection>"
else:
  ccs=""




def addFolderAndTag(folder,key):
  if refDB.startswith("sqlite") and len(TagSuffix)>0:
    tag="<tag>"+join(split(folder, '/'),'') + TagSuffix+"</tag>"
  else:
    tag=""
  keyTag="<key>"+key+"Ref</key>"
  frnTag="<forceRunNumber>"+str(refRN)+"</forceRunNumber>"
  conddb.addFolder("","<db>"+refDB+"</db>"+folder+tag+frnTag+keyTag+ccs)

  if valDB.startswith("sqlite") and len(TagSuffix)>0:
    tag="<tag>"+join(split(folder, '/'),'') + TagSuffix+"</tag>"
  else:
    tag=""
  keyTag="<key>"+key+"</key>"
  frnTag="<forceRunNumber>"+str(valRN)+"</forceRunNumber>"
  conddb.addFolder("","<db>"+valDB+"</db>"+folder+tag+frnTag+keyTag+ccs)
  
   




import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  



from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

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
from AtlasGeoModel import SetupRecoGeometry

#Get identifier mapping (needed by LArConditionsContainer)
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

conddb.addFolder("LAR","/LAR/BadChannels/BadChannels")
conddb.addFolder("LAR","/LAR/BadChannels/MissingFEBs")
theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = max(refRN,valRN)
svcMgr.IOVDbSvc.GlobalTag="LARCALIB-000-00"

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if "Catalog" in dir():
  svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:"+Catalog]


## from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
## theLArPedACValBCMask=LArBadChannelMasker("PedValBCMask",
##                                          DoMasking=False,
##                                          ProblemsToMask=[]
##                                          )
## ToolSvc+=theLArPedACValBCMask
## theLArRampValBCMask=LArBadChannelMasker("RampValBCMask",
##                                         DoMasking=True,
##                                         ProblemsToMask=[
##   "deadReadout","deadCalib","deadPhys","almostDeadBit",
##   "highNoiseHG","highNoiseMG","highNoiseLG"
##   ]
## )
## ToolSvc+=theLArRampValBCMask

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArBCMask=LArBadChannelMasker("BadChannelMask",
                                 DoMasking=True,
                                 ProblemsToMask=["deadReadout","deadCalib","deadPhys","almostDead",
                                                 "highNoiseHG","highNoiseMG","highNoiseLG"]
                                 )
ToolSvc += theLArBCMask



if upper(Object)=="PEDESTAL":
  from LArCalibDataQuality.Thresholds import pedThr,rmsThr, pedThrFEB,rmsThrFEB
  addFolderAndTag("/LAR/ElecCalibOfl/Pedestals/Pedestal","Pedestal")
  
  from LArCalibDataQuality.LArCalibDataQualityConf import LArPedestalValidationAlg
  thePedestalValidationAlg=LArPedestalValidationAlg()
  thePedestalValidationAlg.PedestalTolerance=pedThr
  thePedestalValidationAlg.PedestalRMSTolerance=rmsThr
  thePedestalValidationAlg.PedestalToleranceFEB=pedThrFEB
  thePedestalValidationAlg.PedestalRMSToleranceFEB=rmsThrFEB
  #thePedestalValidationAlg.BadChannelMaskingTool=theLArPedACValBCMask
  thePedestalValidationAlg.ValidationKey="Pedestal"
  thePedestalValidationAlg.ReferenceKey="PedestalRef"
  thePedestalValidationAlg.MsgLevelForDeviations=INFO
  thePedestalValidationAlg.ListOfDevFEBs="pedFebs.txt"
  thePedestalValidationAlg.PrintFailedPattern=PrintFailedPattern
  thePedestalValidationAlg.DoFebAverages=DoFebAverages
  topSequence+=thePedestalValidationAlg

elif upper(Object)=="AUTOCORR":
  from LArCalibDataQuality.Thresholds import acThr, acThrFEB
  addFolderAndTag("/LAR/ElecCalibOfl/AutoCorrs/AutoCorr","AutoCorr")
  from LArCalibDataQuality.LArCalibDataQualityConf import LArAutoCorrValidationAlg
  theAutoCorrValidationAlg=LArAutoCorrValidationAlg()
  theAutoCorrValidationAlg.AutoCorrTolerance=acThr
  theAutoCorrValidationAlg.AutoCorrToleranceFEB=acThrFEB
  theAutoCorrValidationAlg=LArAutoCorrValidationAlg()
  #theAutoCorrValidationAlg.BadChannelMaskingTool=theLArPedACValBCMask
  theAutoCorrValidationAlg.ValidationKey="AutoCorr"
  theAutoCorrValidationAlg.ReferenceKey="AutoCorrRef"
  theAutoCorrValidationAlg.MsgLevelForDeviations=INFO
  theAutoCorrValidationAlg.ListOfDevFEBs="ACFebs.txt"
  theAutoCorrValidationAlg.DoFebAverages=DoFebAverages
  theAutoCorrValidationAlg.PrintFailedPattern=PrintFailedPattern
  topSequence+=theAutoCorrValidationAlg

elif upper(Object)=="RAMP":
  from LArCalibDataQuality.Thresholds import rampThr, rampThrFEB
  addFolderAndTag("/LAR/ElecCalibOfl/Ramps/RampLinea","Ramp")
  from LArCalibDataQuality.LArCalibDataQualityConf import LArRampValidationAlg
  theRampValidationAlg=LArRampValidationAlg()
  theRampValidationAlg.RampTolerance=rampThr
  theRampValidationAlg.RampToleranceFEB=rampThrFEB
  theRampValidationAlg.BadChannelMaskingTool=theLArBCMask
  theRampValidationAlg.ValidationKey="Ramp"
  theRampValidationAlg.ReferenceKey="RampRef"
  theRampValidationAlg.MsgLevelForDeviations=INFO
  theRampValidationAlg.ListOfDevFEBs="rampFebs.txt"
  theRampValidationAlg.ThinnedValContainer="ThinRampsVal"
  theRampValidationAlg.ThinnedRefContainer="ThinRampsRef"
  theRampValidationAlg.DoFebAverages=DoFebAverages
  theRampValidationAlg.PrintFailedPattern=PrintFailedPattern
  topSequence+=theRampValidationAlg


#Wave
elif upper(Object)=="CALIWAVE" or upper(Object)=="CALIWAVEXTALK":
  if upper(Object)=="CALIWAVE":
    addFolderAndTag("/LAR/ElecCalibOfl/CaliWaves/CaliWave","CaliWave")
  else:
    addFolderAndTag("/LAR/ElecCalibOfl/CaliWaves/CaliWaveXtalkCorr","CaliWave")
  from LArCalibDataQuality.Thresholds import cwAmpThr, cwAmpThrFEB, cwFWHMThr, cwFWHMThrFEB 
  from LArCalibDataQuality.LArCalibDataQualityConf import LArCaliWaveValidationAlg

  theCaliWaveValidationAlg=LArCaliWaveValidationAlg()
  theCaliWaveValidationAlg.BadChannelMaskingTool=theLArBCMask
  theCaliWaveValidationAlg.ValidationKey="CaliWave"
  theCaliWaveValidationAlg.ReferenceKey="CaliWaveRef"
  theCaliWaveValidationAlg.AmplitudeTolerance=cwAmpThr
  theCaliWaveValidationAlg.CaliWaveFWHMTolerance=cwFWHMThr
  theCaliWaveValidationAlg.AmplitudeToleranceFEB=cwAmpThrFEB
  theCaliWaveValidationAlg.CaliWaveFWHMToleranceFEB=cwFWHMThrFEB
  theCaliWaveValidationAlg.MsgLevelForDeviations=INFO
  theCaliWaveValidationAlg.ListOfDevFEBs="caliWaveFebs.txt"
  theCaliWaveValidationAlg.DoFebAverages=DoFebAverages
  theCaliWaveValidationAlg.PrintFailedPattern=PrintFailedPattern
  theCaliWaveValidationAlg.OutputLevel=DEBUG
  topSequence+=theCaliWaveValidationAlg
else:
  print "ERROR: No object specified"
  sys.exit(-1)



## from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
## ValRamps2Ntuple=LArRamps2Ntuple("ValRamps2Ntuple")
## ValRamps2Ntuple.NtupleName = "VALRAMPS"
## ValRamps2Ntuple.RawRamp = False
## ValRamps2Ntuple.RampKey = "ThinRampsVal"
## topSequence+=ValRamps2Ntuple

## RefRamps2Ntuple=LArRamps2Ntuple("RefRamps2Ntuple")
## RefRamps2Ntuple.NtupleName = "REFRAMPS"
## RefRamps2Ntuple.RawRamp = False
## RefRamps2Ntuple.RampKey = "ThinRampsRef"
## topSequence+=RefRamps2Ntuple

## theApp.HistogramPersistency = "ROOT"
## from GaudiSvc.GaudiSvcConf import NTupleSvc
## svcMgr += NTupleSvc()
## svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='RampComparison.root' OPT='NEW'" ]



svcMgr.DetectorStore.Dump=True

for f in svcMgr.IOVDbSvc.Folders:
  print f
  
## svcMgr.MessageSvc.OutputLevel=DEBUG
## from PerfMonComps.PerfMonFlags import jobproperties
## jobproperties.PerfMonFlags.OutputFile = "pm.root"
## jobproperties.PerfMonFlags.doMonitoring = True
## include( "PerfMonComps/PerfMonSvc_jobOptions.py" )

## include( "PerfMonComps/PerfMonSvc_jobOptions.py" )
