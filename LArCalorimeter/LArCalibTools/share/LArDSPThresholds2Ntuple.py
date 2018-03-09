if 'inputfolder' not in dir():
   inputfolder="/LAR/Configuration/DSPThresholdFlat/Thresholds"

if 'inputtag' not in dir():
   inputtag=""

if 'inputdb' not in dir():
   inputdb="COOLONL_LAR/CONDBR2"

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
from time import time
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

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
                           
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2017-05"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 500000
svcMgr.EventSelector.InitialTimeStamp=int(time())
dbname="<db>COOLOFL_LAR/CONDBR2</db>"

conddb.addFolder("","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>"+dbname)
conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>"+dbname)
conddb.addFolder("",inputfolder+"<tag>"+inputtag+"</tag><db>"+inputdb+"</db>")


## for f in LArFebConfigFolders:
##     conddb.addFolder("LAR_ONL",f)


from LArCalibTools.LArCalibToolsConf import LArDSPThresholds2Ntuple
theLArDSPThresholds2Ntuple=LArDSPThresholds2Ntuple()
theLArDSPThresholds2Ntuple.OutputLevel=DEBUG
theLArDSPThresholds2Ntuple.AddFEBTempInfo=False
theLArDSPThresholds2Ntuple.OffId=True
if "Flat" in inputfolder:
   theLArDSPThresholds2Ntuple.DumpFlat=True
   theLArDSPThresholds2Ntuple.FlatFolder=inputfolder

topSequence+=theLArDSPThresholds2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='DSPthresholds.root' OPT='NEW'" ]

#svcMgr.DetectorStore.Dump=True
svcMgr.MessageSvc.OutputLevel = WARNING

svcMgr.IOVDbSvc.DBInstance=""
svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml",
                               ]


svcMgr.MessageSvc.debugLimit = 5000000

#svcMgr.IOVDbSvc.OutputLevel=DEBUG
