import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="geant4"
#globalflags.DataSource="data"
globalflags.InputFormat="bytestream"

from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-00"
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-03-01-00"

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

#Get identifier mapping (needed by LArConditionsContainer)
#svcMgr.IOVDbSvc.GlobalTag="COMCOND-BLKPA-006-07"
#svcMgr.IOVDbSvc.GlobalTag="OFLCOND-MC12-SDR-06"
#svcMgr.IOVDbSvc.GlobalTag="OFLCOND-RUN12-SDR-14"
svcMgr.IOVDbSvc.GlobalTag="OFLCOND-RUN12-SDR-30"

include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theADC2MeVTool = LArADC2MeVToolDefault()
if globalflags.DataSource()=='data':
    theADC2MeVTool.MCSym=False
else:
    theADC2MeVTool.MCSym=True
ToolSvc += theADC2MeVTool

theApp.EvtMax = 1
#svcMgr.EventSelector.RunNumber = 210308
svcMgr.EventSelector.RunNumber = 2147483647

#include( "LArDetDescr/LArDetDescr_joboptions.py" )
include ("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")

conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/OnOffIdMap_SC<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>")

fixedLSB = True;
quantEt = True;
mQuant = 125.
emQuant = 32.
forwardQuant = 250.;

from LArCalibTools.LArCalibToolsConf import LArMCToSC
theLArMCToSC=LArMCToSC(fixedLSB=fixedLSB, quantEt=quantEt, mQuant = mQuant, emQuant=emQuant, forwardQuant=forwardQuant)
topSequence+=theLArMCToSC

svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:'+'/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml']


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='scPedestalOFC.root' OPT='NEW'" ]

folderlist=("/LAR/ElecCalibMCSC/uA2MeV",
            "/LAR/ElecCalibMCSC/DAC2uA",
            "/LAR/ElecCalibMCSC/Noise",
            "/LAR/ElecCalibMCSC/Pedestal",
            "/LAR/ElecCalibMCSC/Ramp",
            "/LAR/ElecCalibMCSC/AutoCorr",
            "/LAR/ElecCalibMCSC/fSampl",
            "/LAR/ElecCalibMCSC/MinBias",
            "/LAR/ElecCalibMCSC/MinBiasAverage",
            )

folderspec=[]
tagspec=[]
for f in folderlist:
    folderspec.append("CondAttrListCollection#"+f)
    tagspec.append("".join(f.split("/"))+"-boohoo")
    pass


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                        #["CondAttrListCollection#/LAR/ElecCalibSC/Pedestal","CondAttrListCollection#/LAR/ElecCalibSC/OFC",],
                                        #["testped","testofc"],
                                        folderspec,tagspec,
                                        True)


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
svcMgr.IOVRegistrationSvc.userTags = False

svcMgr.IOVRegistrationSvc.OverrideNames += [ "Pedestal","PedestalRMS","OFCa"   ,"OFCb",
                                             "TimeOffset","RampVec","Shape","ShapeDer", "fSampl",
                                             "HVScaleCorr","MphysOverMcal","DAC2uA","uA2MeV", "Noise",
                                             "AutoCorrVec", "MinBias", "MinBiasAverage" ]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;
outDBName = "allConstantsSV_";
if fixedLSB:
    outDBName += "Fixed"
    if quantEt:
        outDBName += "EtLSB";
    else:
        outDBName += "ELSB";
    outDBName += "_middle%d.db" % int(round(mQuant,0));
else:
    outDBName += "mV_LSB.db"

svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+outDBName+";dbname=OFLP200"


#svcMgr.DetectorStore.Dump=True
svcMgr.MessageSvc.OutputLevel = INFO

svcMgr.IOVDbSvc.DBInstance=""
