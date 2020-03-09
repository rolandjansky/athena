fill=True

import AthenaCommon.AtlasUnixGeneratorJob
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-R2-2015-04-00-00')
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

svcMgr.IOVDbSvc.GlobalTag = "CONDBR2-BLKPA-2017-03"

#Get identifier mapping (needed by LArConditionsContainer)
include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

# for LArBadChannelTool, instead of conddb.AddFolder below
#include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py") 

theApp.EvtMax = 1
#conddb.setGlobalTag("COMCOND-006-01") #For id mapping

folder="/LAR/Configuration/DSPThresholdFlat/Templates"

#tag="LARConfigurationDSPThresholdTemplates-Qt5sigma-samp5sigma"

#ModeType="noise" # fixed, group, noise

#fileName=ModeType+"tQThr5sigmasampleThr5sigma"
fileName=ModeType+tag

setName="-".join(tag.split("-")[1:])

from CaloTools.CaloNoiseFlags import jobproperties
#jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*30/8)
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(-1.)

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
theCaloNoiseTool.OutputLevel=INFO
theCaloNoiseTool.RescaleForHV=False
ToolSvc+=theCaloNoiseTool

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArHVCorr = False

conddb.addOverride("/CALO/Ofl/Noise/PileUpNoiseLumi","CALOOflNoisePileUpNoiseLumi-RUN2-UPD1-00")
if 'pileupsqlite' in dir():
   conddb.addMarkup("/CALO/Ofl/Noise/PileUpNoiseLumi","<db>sqlite://;schema="+pileupsqlite+";dbname=CONDBR2</db>")
if 'noisesqlite' in dir():
   if 'noisetag' in dir():
      conddb.addMarkup("/LAR/NoiseOfl/CellNoise","<db>sqlite://;schema="+noisesqlite+";dbname=CONDBR2</db><tag>"+noisetag+"</tag>")
   else:   
      conddb.addMarkup("/LAR/NoiseOfl/CellNoise","<db>sqlite://;schema="+noisesqlite+";dbname=CONDBR2</db>")
if 'RunSince' not in dir():
   RunSince=0

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArBadChannelMasker=LArBadChannelMasker("LArBadChannelMasker")
theLArBadChannelMasker.DoMasking=True
theLArBadChannelMasker.ProblemsToMask=[
    "highNoiseHG","highNoiseMG","highNoiseLG"
    ]
##    "deadReadout","deadPhys","deadCalib","short","almostDead",
ToolSvc+=theLArBadChannelMasker

from LArOnlDbPrep.LArOnlDbPrepConf import LArDSPThresholdFillInline
theLArDSPThresholdFillAlg=LArDSPThresholdFillInline()
theLArDSPThresholdFillAlg.OutputLevel=INFO
theLArDSPThresholdFillAlg.Key=folder
theLArDSPThresholdFillAlg.OutFile=fileName+".txt"
theLArDSPThresholdFillAlg.mode=ModeType
theLArDSPThresholdFillAlg.MaskBadChannels=True
theLArDSPThresholdFillAlg.BadChannelMasker=theLArBadChannelMasker
theLArDSPThresholdFillAlg.NameOfSet=setName
# Set masked channel thresholds lower for diagnostics
#theLArDSPThresholdFillAlg.MaskedtQThreshold=0.
#theLArDSPThresholdFillAlg.MaskedsamplesThreshold=0.

##if ModeType=="fixed":
##    theLArDSPThresholdFillAlg.tQThreshold=10000
##    theLArDSPThresholdFillAlg.samplesThreshold=10000
if ModeType=="fixed":
    theLArDSPThresholdFillAlg.tQThreshold=Qtval
    theLArDSPThresholdFillAlg.samplesThreshold=Sampval
if ModeType=="group":
    theLArDSPThresholdFillAlg.ThresholdsPerCellGroup=["250,1000",
                                                      "[HEC,FCAL/*/*/*/*/*] 500, 1000"]
if ModeType=="noise":
    theLArDSPThresholdFillAlg.sigmaNoiseSamples=Sampval
    theLArDSPThresholdFillAlg.sigmaNoiseQt=Qtval
    theLArDSPThresholdFillAlg.usePileupNoiseSamples=Samppileup
    theLArDSPThresholdFillAlg.usePileupNoiseQt=Qtpileup
    theLArDSPThresholdFillAlg.NoiseTool=theCaloNoiseTool


if fill:
    theLArDSPThresholdFillAlg.Fill=True
    theLArDSPThresholdFillAlg.Dump=True
else:
    theLArDSPThresholdFillAlg.Fill=False
    theLArDSPThresholdFillAlg.Dump=True

topSequence+=theLArDSPThresholdFillAlg


MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = WARNING

if fill:
    OutputList=[ "AthenaAttributeList#"+folder ]
    OutputTagList=[tag]

    WriteIOV=True
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
##    theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LArDSPthresholds_"+fileName+".pool.root",
    theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LArDSPthresholdTemplates.pool.root",
                                               OutputList,OutputTagList,WriteIOV)
    
    theOutputConditionsAlg.Run1=RunSince


    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    svcMgr += IOVRegistrationSvc()
    svcMgr.IOVRegistrationSvc.OutputLevel = WARNING
    ##svcMgr.IOVRegistrationSvc.RecreateFolders = True
    svcMgr.IOVRegistrationSvc.RecreateFolders = False
    svcMgr.IOVRegistrationSvc.OverrideNames += ["tQThr","samplesThr","trigSumThr",]
    svcMgr.IOVRegistrationSvc.OverrideTypes += ["Blob16M","Blob16M","Blob16M",]


else:
    conddb.addFolder("",folder+"<tag>"+tag+"</tag>")

# For LArBadChannelTool
conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannels")
conddb.addFolder(LArDB,"/LAR/BadChannels/MissingFEBs")

svcMgr.PoolSvc.FileOpen = "update"
svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:PoolFileCatalog_LARConfigurationDSPThresholdTemplates.xml"
##svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:PoolFileCatalog_"+fileName+".xml"
print svcMgr.PoolSvc

#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=test.db;dbname=COMP200"
#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+fileName+".db;dbname=COMP200"
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=DSPThresholdTemplates.db;dbname=CONDBR2"

svcMgr.DetectorStore.Dump=True
if 'RunNumber' in dir():
   svcMgr.EventSelector.RunNumber=RunNumber
else:
   svcMgr.EventSelector.RunNumber=0xFFFFFF
