# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/work/c/cheidegg/ec/AthenaDev/test.HITS.pool.root"]

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
	print("fooooooo")
	svcMgr.IOVDbSvc.DBInstance="CONDBR2"
	print(svcMgr.IOVDbSvc.DBInstance)

	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MDT;dbname=CONDBR2;user=ATLAS_COOLOFL_MDT_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MDT/MM/TIME/SIDEA"    , "MmTdoSideA-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MDT;dbname=CONDBR2;user=ATLAS_COOLOFL_MDT_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MDT/MM/TIME/SIDEC"    , "MmTdoSideC-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MDT;dbname=CONDBR2;user=ATLAS_COOLOFL_MDT_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MDT/MM/CHARGE/SIDEA"  , "MmPdoSideA-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MDT;dbname=CONDBR2;user=ATLAS_COOLOFL_MDT_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MDT/MM/CHARGE/SIDEC"  , "MmPdoSideC-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MDT;dbname=CONDBR2;user=ATLAS_COOLOFL_MDT_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MDT/MM/VMM/SIDEA"     , "MmVmmSideA-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MDT;dbname=CONDBR2;user=ATLAS_COOLOFL_MDT_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MDT/MM/VMM/SIDEC"     , "MmVmmSideC-Rnd-TEST",className='CondAttrListCollection')

	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_TGC;dbname=CONDBR2;user=ATLAS_COOLOFL_TGC_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/TGC/NSW/TIME/SIDEA"  , "sTgcTdoSideA-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_TGC;dbname=CONDBR2;user=ATLAS_COOLOFL_TGC_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/TGC/NSW/TIME/SIDEC"  , "sTgcTdoSideC-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_TGC;dbname=CONDBR2;user=ATLAS_COOLOFL_TGC_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/TGC/NSW/CHARGE/SIDEA", "sTgcPdoSideA-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_TGC;dbname=CONDBR2;user=ATLAS_COOLOFL_TGC_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/TGC/NSW/CHARGE/SIDEC", "sTgcPdoSideC-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_TGC;dbname=CONDBR2;user=ATLAS_COOLOFL_TGC_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/TGC/NSW/VMM/SIDEA"   , "sTgcVmmSideA-Rnd-TEST",className='CondAttrListCollection')
	#conddb.addFolderWithTag("", "<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_TGC;dbname=CONDBR2;user=ATLAS_COOLOFL_TGC_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/TGC/NSW/VMM/SIDEC"   , "sTgcVmmSideC-Rnd-TEST",className='CondAttrListCollection')

	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/TIME/SIDEA"   , "MmTdoSideA-Rnd-TEST"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/TIME/SIDEC"   , "MmTdoSideC-Rnd-TEST"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/CHARGE/SIDEA" , "MmPdoSideA-Rnd-TEST"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/CHARGE/SIDEC" , "MmPdoSideC-Rnd-TEST"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/VMM/SIDEA"    , "MmVmmSideA-Rnd-TEST"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("MDT_OFL", "/MDT/MM/VMM/SIDEC"    , "MmVmmSideC-Rnd-TEST"  , forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/TIME/SIDEA"  , "sTgcTdoSideA-Rnd-TEST", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/TIME/SIDEC"  , "sTgcTdoSideC-Rnd-TEST", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/CHARGE/SIDEA", "sTgcPdoSideA-Rnd-TEST", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/CHARGE/SIDEC", "sTgcPdoSideC-Rnd-TEST", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/VMM/SIDEA"   , "sTgcPdoSideA-Rnd-TEST", forceData=True, className='CondAttrListCollection');
	conddb.addFolderWithTag("TGC_OFL", "/TGC/NSW/VMM/SIDEC"   , "sTgcPdoSideC-Rnd-TEST", forceData=True, className='CondAttrListCollection');





##--------------------------------------------------------------
## NEW Data Base Algorithms
##--------------------------------------------------------------

from MuonCondAlg.MuonCondAlgConf import NswCalibDbAlg
alg = NswCalibDbAlg("NswCalibDbAlg")
alg.OutputLevel = VERBOSE
alg.isOnline = False
alg.isData   = True

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
