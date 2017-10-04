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
                           
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2017-04"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 500000
svcMgr.EventSelector.InitialTimeStamp=int(time())
dbname="<db>COOLOFL_LAR/CONDBR2</db>"

conddb.addFolder("","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>"+dbname)
conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>"+dbname)
#conddb.addFolder("","/LAR/ElecCalibOfl/PhysCaliTdiff<key>input</key>"+dbname)

## LArFebConfigFolders=[
##     "/LAR/Configuration/FEBConfig/Physics/EMBA1",  
##     "/LAR/Configuration/FEBConfig/Physics/EMBA2",
##     "/LAR/Configuration/FEBConfig/Physics/EMBA3",
##     "/LAR/Configuration/FEBConfig/Physics/EMBA4",
##     "/LAR/Configuration/FEBConfig/Physics/EMBC1",
##     "/LAR/Configuration/FEBConfig/Physics/EMBC2",
##     "/LAR/Configuration/FEBConfig/Physics/EMBC3",
##     "/LAR/Configuration/FEBConfig/Physics/EMBC4", 
##     "/LAR/Configuration/FEBConfig/Physics/EMECA1",
##     "/LAR/Configuration/FEBConfig/Physics/EMECA2",
##     "/LAR/Configuration/FEBConfig/Physics/EMECA3",
##     "/LAR/Configuration/FEBConfig/Physics/EMECC1",
##     "/LAR/Configuration/FEBConfig/Physics/EMECC2",
##     "/LAR/Configuration/FEBConfig/Physics/EMECC3",
##     "/LAR/Configuration/FEBConfig/Physics/FCALA", 
##     "/LAR/Configuration/FEBConfig/Physics/FCALC", 
##     "/LAR/Configuration/FEBConfig/Physics/HECA", 
##     "/LAR/Configuration/FEBConfig/Physics/HECC", 
## ]


## for f in LArFebConfigFolders:
##     conddb.addFolder("LAR_ONL",f)


## from LArRecUtils.LArRecUtils import LArFEBConfigReader
## theLArFebConfigReader=LArFEBConfigReader()
## theLArFebConfigReader.OutputLevel=DEBUG
## theLArFebConfigReader.ListOfFolders=LArFebConfigFolders



from LArRecUtils.LArFEBConfigReaderDefault import LArFEBConfigReaderDefault
theLArFebConfigReader=LArFEBConfigReaderDefault()
theLArFebConfigReader.OutputLevel=DEBUG
ToolSvc+=theLArFebConfigReader

from LArCalibTools.LArCalibToolsConf import LArGainThresholds2Ntuple
theLArGainThresholds2Ntuple=LArGainThresholds2Ntuple()
theLArGainThresholds2Ntuple.OutputLevel=DEBUG
theLArGainThresholds2Ntuple.AddFEBTempInfo=False
theLArGainThresholds2Ntuple.FebConfigReader=theLArFebConfigReader

topSequence+=theLArGainThresholds2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='thresholds.root' OPT='NEW'" ]

#svcMgr.DetectorStore.Dump=True
#svcMgr.MessageSvc.OutputLevel = DEBUG

svcMgr.IOVDbSvc.DBInstance=""
svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml",
                               ]


svcMgr.MessageSvc.debugLimit = 5000000

svcMgr.IOVDbSvc.OutputLevel=DEBUG
