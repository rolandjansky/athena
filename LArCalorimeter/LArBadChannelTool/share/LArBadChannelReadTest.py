# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

FEBFolder = "/LAR/BadChannels/MissingFEBs"
FEBFile = "../share/badfebs.txt"

#No input file -> use MC event selector
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon                       import CfgMgr

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-01"

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

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

theApp.EvtMax = 1

svcMgr.EventSelector.RunNumber         = 1
#svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 1
#svcMgr.EventSelector.InitialTimeStamp  = 0
#svcMgr.EventSelector.TimeStampInterval = 5

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr)

theApp.EvtMax=1

## theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
## svcMgr.EventSelector.Input=["/home/wlampl/LArOFIter/ramp/inputs/daq.Ramp.0029146.No.Streaming.LB0000.EB-EMBA._0001.data"]
   


#That's the dumper, keep for now
from LArCalibTest.LArCalibTestConf import DumpCaloBadChannels
theDumper=DumpCaloBadChannels()
theDumper.FileName="list.txt"
topSequence+=theDumper

#Thats the registration algo
#from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelDBAlg
#topSequence += LArBadChannelDBAlg( "BadChanAlg" )



svcMgr.IOVDbSvc.Folders+=["/LAR/BadChannels/BadChannels<tag>LARBadChannelsBadChannels-M6-01</tag><dbConnection>sqlite://;schema=BadChannels.db;dbname=CONDBR2</dbConnection>" ]
svcMgr.IOVDbSvc.Folders+=["/LAR/BadChannels/MissingFEBs<tag>LARBadChannelsMissingFEBs-M6-01</tag><dbConnection>sqlite://;schema=BadChannels.db;dbname=CONDBR2</dbConnection>" ]
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2014-01"
svcMgr.IOVDbSvc.OutputLevel=DEBUG

#from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
#svcMgr += IOVRegistrationSvc()
#svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
#svcMgr.IOVRegistrationSvc.RecreateFolders = False

svcMgr.DetectorStore.Dump=True




svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

# this preload causes callbacks for read in objects to be activated,
#IOVSvc=Service("IOVSvc")
#IOVSvc.PreLoadData=True

###########################################################################

svcMgr.DetectorStore.Dump=True
