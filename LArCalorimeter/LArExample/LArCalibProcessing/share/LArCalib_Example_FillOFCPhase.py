# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if 'inpFile' not in dir():
   inpFile="OFC_phase.dat"
if 'KeyOutput' not in dir():
   KeyOutput = "LArOFCPhase"
if 'isSC' not in dir():
   isSC=False
if 'hasID' not in dir():
   hasID=False
if 'GroupType' not in dir():   
   GroupType = "ExtendedSubDetector"
if 'Folder' not in dir():
   Folder = "/LAR/ElecCalibOfl/OFCBin/Dummy"
if 'Tag' not in dir():
   Tag="LArOFCPhase-01" 
if 'outSqlite' not in dir():
   outSqlite="OFCPhase.db"
if 'outPool' not in dir():
   outPool="ofc_phase.pool.root"

include ("LArConditionsCommon/LArMinimalSetup.py")

svcMgr.EventSelector.Input = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1']
svcMgr.EventSelector.RunNumber=500000
svcMgr.EventSelector.EventsPerRun=1;
svcMgr.MessageSvc.OutputLevel      = WARNING
theApp.EvtMax = 1


#--------------------------------------------------------------
# 0. load conditions data 
#--------------------------------------------------------------
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
svcMgr.IOVDbSvc.OutputLevel = INFO
svcMgr.IOVDbSvc.GlobalTag = 'CONDBR2-BLKPA-RUN2-09'

#IOVSvc = Service( "IOVSvc" )
#IOVSvc.updateInterval = "JOB"
#IOVSvc.preLoadData=True
#IOVSvc.OutputLevel = DEBUG
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from LArCalibUtils.LArCalibUtilsConf import LArOFPhaseFill
LArOFPhaseFill = LArOFPhaseFill("LArOFPhaseFill")
LArOFPhaseFill.InputFile = inpFile
LArOFPhaseFill.keyOFCBin = KeyOutput
LArOFPhaseFill.isSC = isSC
LArOFPhaseFill.isID = hasID
LArOFPhaseFill.GroupingType = GroupType
LArOFPhaseFill.OutputLevel = DEBUG
topSequence += LArOFPhaseFill
#
# Make an output
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
ObjectSpec = [ "LArOFCBinComplete#"+KeyOutput+"#"+Folder ]
TagSpec = [ Tag ]
OutputDB = "sqlite://X;schema="+outSqlite+";dbname=CONDBR2"
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","./"+outPool, 
                    ObjectSpec,TagSpec,True)
#OutputConditionsAlg.Run1 = 0
#OutputConditionsAlg.Run2 = 0

topSequence += OutputConditionsAlg


from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = False
svcMgr.IOVDbSvc.dbConnection  = OutputDB

#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
#theApp.initialize() 
#theApp.nextEvent()
svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"
   
svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = ERROR)
svcMgr.DetectorStore.OutputLevel  = DEBUG
svcMgr.DetectorStore.Dump=True

