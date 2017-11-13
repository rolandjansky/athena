if 'inputfolder' not in dir():
   inputfolder='/LAR/Configuration/DSPThreshold/Templates' 

if 'inputtag' not in dir():
   inputtag='Qt0-samp0'

if 'outputtag' not in dir():
   outputtag='Qt0-samp0'

if 'outputfolder' not in dir():
   outputfolder="/LAR/Configuration/DSPThresholdFlat/Templates"

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
from time import time
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
globalflags.DatabaseInstance="CONDBR2"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

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
svcMgr.IOVDbSvc.GlobalTag="LARCALIB-000-02"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 500000
#svcMgr.EventSelector.RunNumber = 2147483647
svcMgr.EventSelector.InitialTimeStamp=int(time())
dbname="<db>COOLOFL_LAR/COMP200</db>"

conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>")
conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>")
conddb.addFolder("","/LAR/IdentifierOfl/OnOffIdMap_SC<db>COOLOFL_LAR/OFLP200</db><tag>LARIdentifierOflOnOffIdMap_SC-000</tag>")

#conddb.addFolder("","/LAR/ElecCalibOnl/Pedestal<db>COOLONL_LAR/COMP200</db>")

intag=join(inputfolder.split("/"),'')+"-"+inputtag
#intag=inputtag
conddb.addFolder("","/LAR/Configuration/DSPThreshold/Templates<db>COOLONL_LAR/COMP200</db><tag>"+intag+"</tag>")

from LArCalibTools.LArCalibToolsConf import LArCompleteToFlat
theLArCompleteToFlat=LArCompleteToFlat()
theLArCompleteToFlat.ForceStop=False
theLArCompleteToFlat.DSPThresholdsInput="LArDSPThresholds"
theLArCompleteToFlat.NameOfSet = outputtag
topSequence+=theLArCompleteToFlat


#from LArCalibTools.LArCalibToolsConf import LArDSPThresholds2Ntuple
#LArDSPThresholds2Ntuple=LArDSPThresholds2Ntuple("LArDSPThresholds2Ntuple")
#LArDSPThresholds2Ntuple.DumpFlat=True
#LArDSPThresholds2Ntuple.DumpFlat=True
#topSequence+=LArDSPThresholds2Ntuple


svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:'+'/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml']


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='flatDSPThresholds.root' OPT='NEW'" ]



outtagspec=join(outputfolder.split("/"),'')+"-"+outputtag
folderspec="AthenaAttributeList#"+outputfolder
#outtagspec=join(outputfolder.split("/"),'')+"-"+outputtag
#folderspec="CondAttrListCollection#"+outputfolder

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                        [folderspec],[outtagspec],
                                        True)


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
#svcMgr.IOVRegistrationSvc.OverrideNames += [ "Pedestal","PedestalRMS","OFCa"   ,"OFCb"    ,"TimeOffset","RampVec","Shape","ShapeDer","HVScaleCorr","MphysOverMcal","DAC2uA","uA2MeV"]
svcMgr.IOVRegistrationSvc.OverrideNames += [ "tQThr", "samplesThr", "trigSumThr"]
#svcMgr.IOVRegistrationSvc.OverrideNames += [ "Pedestal", "PedestalRMS"]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=DSP_test.db;dbname=COMP200"


svcMgr.DetectorStore.Dump=True
svcMgr.StoreGateSvc.Dump = True
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.IOVDbSvc.DBInstance=""

