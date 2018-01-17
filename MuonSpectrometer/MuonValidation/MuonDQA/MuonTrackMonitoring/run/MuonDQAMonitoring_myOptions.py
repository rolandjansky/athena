# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#======================================================================
# Offline Muon Monitoring configuration
#======================================================================
# Standalone JobOptions to run MuonDQAMonitoring on ESD/AOD
#======================================================================  

# Set up the reading of the input ESD or xAOD:
FNAME = 'root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00267638/data15_13TeV.00267638.express_express.recon.ESD.x342/data15_13TeV.00267638.express_express.recon.ESD.x342._lb0523._SFO-3._0001.1'
include( "AthenaPython/iread_file.py" )
# import AthenaPoolCnvSvc.ReadAthenaPool
# ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "MyMuonMonManager" )
monMan.ManualDataTypeSetup = True
#monMan.DataType            = "cosmics"
#monMan.DataType            = "monteCarlo"
monMan.DataType            = "collisions"
monMan.Environment         = "tier0ESD"
# monMan.ManualRunLBSetup    = True
# monMan.Run                 = 1
# monMan.LumiBlock           = 1
monMan.FileKey = "MuonMon"
topSequence += monMan

#from MuonRecExample.MuonRecFlags import muonRecFlags

from AthenaMonitoring.DQMonFlags import DQMonFlags  
DQMonFlags.enableLumiAccess=True
DQMonFlags.doMuonRawMon=False
DQMonFlags.doMuonSegmentMon=False
DQMonFlags.doMuonTrackMon=True
DQMonFlags.doMuonAlignMon=False
DQMonFlags.doMuonTrkPhysMon=False

DQMonFlags.monManFileKey="MuonMon"
#include ("AthenaMonitoring/DataQualitySteering_jobOptions.py")

from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
MuonDQADetFlags.doMuonMonitoring = True
# --- MuonPhysics Monitoring options
MuonDQADetFlags.doMuonPhysMon = False
MuonDQADetFlags.doMuTrkMon = False 
MuonDQADetFlags.doZmumuMon = False

muonOutputLevel=INFO
include ("MuonTrackMonitoring/MuonTrackDQA_options.py")

for tool in monMan.AthenaMonTools:
    tool.EnableLumi = True
    
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["MuonMon DATAFILE='TonyMon.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
