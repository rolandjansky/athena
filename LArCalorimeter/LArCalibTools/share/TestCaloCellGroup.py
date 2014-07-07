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

#Get identifier mapping (needed by LArConditionsContainer)
svcMgr.IOVDbSvc.GlobalTag='COMCOND-006-01'
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 1


from LArCalibTools.LArCalibToolsConf import CaloCellGroup2Ntuple
theCaloCellGroups2Ntuple=CaloCellGroup2Ntuple()
theCaloCellGroups2Ntuple.AddBadChannelInfo=False
theCaloCellGroups2Ntuple.group=["0, 21.1",
                                "[EMB,EMECIW,EMECOW/C/*/*/*/*] 1, 22.2", #
                                "[FCAL/A/*/*/*/*] 2, 24.4", #1
                                "[EMB/C/1/0/0-32/*] 3, 32.2", #2
                                "[EMECIW/*/1/0/3/4] 4, 42.2", #3
                                ]
topSequence+=theCaloCellGroups2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='CellGroup.root' OPT='NEW'" ]


#from GaudiSvc.GaudiSvcConf import THistSvc
#svcMgr+=THistSvc()
#svcMgr.THistSvc.Output =  [ "FILE1 DATAFILE='CellGroup.root' OPT='RECREATE'" ]

#svcMgr.DetectorStore.Dump=True
#svcMgr.MessageSvc.OutputLevel = DEBUG

