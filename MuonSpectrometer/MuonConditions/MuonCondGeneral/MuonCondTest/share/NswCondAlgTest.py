# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

run = "mc" ## "data" or "mc"

from AthenaCommon.GlobalFlags import GlobalFlags, globalflags
if run == "data":
	globalflags.DetGeo.set_Value_and_Lock('atlas') 
	globalflags.DataSource.set_Value_and_Lock('data')
	globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")
elif run == "mc":
	globalflags.DetGeo.set_Value_and_Lock('atlas') 
	globalflags.DataSource.set_Value_and_Lock('data')
#	globalflags.DatabaseInstance.set_Value_and_Lock("OFLP200")



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


if run=="data":
	#svcMgr.IOVDbSvc.DBInstance=""

	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/TIME/SIDEA"   , "MmTdoSideA-TEST-Rnd"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/TIME/SIDEC"   , "MmTdoSideC-TEST-Rnd"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/CHARGE/SIDEA" , "MmPdoSideA-TEST-Rnd"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/CHARGE/SIDEC" , "MmPdoSideC-TEST-Rnd"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/TIME/SIDEA"  , "sTgcTdoSideA-TEST-Rnd", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/TIME/SIDEC"  , "sTgcTdoSideC-TEST-Rnd", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/CHARGE/SIDEA", "sTgcPdoSideA-TEST-Rnd", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/CHARGE/SIDEC", "sTgcPdoSideC-TEST-Rnd", forceData=True, className='CondAttrListCollection');


elif run=="mc":

	svcMgr.IOVDbSvc.DBInstance="OFLP200"
	
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/TIME/SIDEA"   , "MmTdoSideA-Const-3p73"  , forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/TIME/SIDEC"   , "MmTdoSideC-Const-3p73"  , forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/CHARGE/SIDEA" , "MmPdoSideA-Const-9p0"   , forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/CHARGE/SIDEC" , "MmPdoSideC-Const-9p0"   , forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/THR/SIDEA"    , "MmThrSideA-Const-55p4"  , forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/THR/SIDEC"    , "MmThrSideC-Const-55p4"  , forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/TIME/SIDEA"  , "sTgcTdoSideA-Const-0p27", forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/TIME/SIDEC"  , "sTgcTdoSideC-Const-0p27", forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/CHARGE/SIDEA", "sTgcPdoSideA-Const-0p78", forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/CHARGE/SIDEC", "sTgcPdoSideC-Const-0p78", forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/THR/SIDEA"   , "sTgcThrSideA-Const-52p7", forceMC=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/THR/SIDEC"   , "sTgcThrSideC-Const-52p7", forceMC=True, className='CondAttrListCollection');





##--------------------------------------------------------------
## NEW Data Base Algorithms
##--------------------------------------------------------------

from MuonCondAlg.MuonCondAlgConf import NswCalibDbAlg
alg = NswCalibDbAlg("NswCalibDbAlg")
alg.OutputLevel = DEBUG
alg.isOnline    = False
alg.isData      = True

if "mc" in run:
	alg.isData = False

condSequence += alg



##--------------------------------------------------------------
## NEW Test Algorithm
##--------------------------------------------------------------
from MuonCondTest.MuonCondTestConf import NswCondTestAlg
job += NswCondTestAlg()



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
