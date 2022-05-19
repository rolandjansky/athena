# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

run = "datatest" ## "mc" or "dataR1" or "dataR2"

from AthenaCommon.GlobalFlags import GlobalFlags, globalflags
if run == "datatest":
	globalflags.DetGeo.set_Value_and_Lock('atlas') 
	globalflags.DataSource.set_Value_and_Lock('data')
	#globalflags.DatabaseInstance.set_Value_and_Lock("OFLP200")
	globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

from AthenaCommon.JobProperties import jobproperties

# GeoModel is needed to make a test build of the volumes - in case of bugs it crashes
include("MuonGeoModel/MuonGeoModel_MinimalSetup.py")
# after the above include, GeoModelSvc should be setup, now use a recent layout to start from
GeoModelSvc.AtlasVersion='ATLAS-R3S-2021-01-00-02'
# since we have chosen a symmetric NSW layout and we execute this jobOptions file outside of a 
# standard job transform, set the MuonDetectorTool and DetDescrCnvSvc properties by hand
GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasCSC = False
GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasSTgc = True
GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasMM = True
if hasattr(svcMgr,'DetDescrCnvSvc'):
    svcMgr.DetDescrCnvSvc.HasCSC=False
    svcMgr.DetDescrCnvSvc.HasSTgc=True
    svcMgr.DetDescrCnvSvc.HasMM=True


#--------------------------------------------------------------
# Setup Athena
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AppMgr import ServiceMgr,athCondSeq
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/work/c/cheidegg/ec/_testFiles/test.HITS.pool.root"]

# use auditors
from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()




##--------------------------------------------------------------
## Data Base Services
##--------------------------------------------------------------

from AthenaCommon.AlgSequence import AthSequencer

condSequence = AthSequencer("AthCondSeq")

# Conditions Service for reading conditions data in serial and MT Athena
from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc()

# Conditions data access infrastructure for serial and MT Athena
from IOVSvc.IOVSvcConf import CondInputLoader
condSequence += CondInputLoader( "CondInputLoader")

import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

from IOVDbSvc.CondDB import conddb


if run=="datatest":
	svcMgr.IOVDbSvc.DBInstance=""#CONDBR2"

	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/PASSIVATION", "MmPassiv2022Feb26", forceData=True, className='CondAttrListCollection');





##--------------------------------------------------------------
## NEW Data Base Algorithms
##--------------------------------------------------------------

from MuonCondAlg.MuonCondAlgConf import NswPassivationDbAlg
alg = NswPassivationDbAlg("NswPassivationDbAlg")
alg.OutputLevel = DEBUG
condSequence += alg



##--------------------------------------------------------------
## NEW Test Algorithm
##--------------------------------------------------------------
from MuonCondTest.MuonCondTestConf import NswPassivationTestAlg
job += NswPassivationTestAlg()



##--------------------------------------------------------------
## General Stuff
##--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
#ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                              =  2 

ServiceMgr.MessageSvc.Format      = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE
