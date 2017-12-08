# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

# Import relevant libs
import os
import commands
import string

#set DB by hand
ConfForMC = True

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
#GlobalFlags.DetGeo.set_atlas()
if (ConfForMC):
    #GlobalFlags.DataSource.set_geant4()
    globalflags.DetGeo.set_Value_and_Lock('geant4')
else :
    #GlobalFlags.DataSource.set_data()
    globalflags.DataSource.set_Value_and_Lock('data')
from IOVDbSvc.CondDB import conddb

###Define ESD location on local disk for merged ESDs; change this to the path to your datasets
#path = "/unix/atlas2/paw/mc08.108407.Pythia_directJpsimu4mu4.recon.ESD.e347_s462_s520_r809_tid092535"
#path = "/unix/atlas2/paw/data10_7TeV.00153599.physics_MuonswBeam.recon.ESD.f251"

ServiceMgr.EventSelector.InputCollections = [ #"/unix/atlas2/paw/mc09_7TeV.108407.Pythia_directJpsimu4mu4.digit.ESD.e477_s765_s767_d300_tid130174_00/1.ESD.pool.root",
                                              #"/unix/atlas2/paw/mc09_7TeV.108407.Pythia_directJpsimu4mu4.digit.ESD.e477_s765_s767_d300_tid130174_00/2.ESD.pool.root",
                                              #"/unix/atlas2/paw/mc09_7TeV.108407.Pythia_directJpsimu4mu4.digit.ESD.e477_s765_s767_d300_tid130174_00/3.ESD.pool.root",
                                              #"/unix/atlas2/paw/mc09_7TeV.108407.Pythia_directJpsimu4mu4.digit.ESD.e477_s765_s767_d300_tid130174_00/4.ESD.pool.root",
                                              #"/unix/atlas2/paw/mc09_7TeV.108407.Pythia_directJpsimu4mu4.digit.ESD.e477_s765_s767_d300_tid130174_00/5.ESD.pool.root",
                                              #"/unix/atlas2/paw/mc09_7TeV.108407.Pythia_directJpsimu4mu4.digit.ESD.e477_s765_s767_d300_tid130174_00/6.ESD.pool.root"
#    "root://eosatlas//eos/atlas/atlasdatadisk/data11_7TeV/AOD/r3365_r3368_p902/data11_7TeV.00190933.Jpsi_JulieKirk.merge.AOD.r3365_r3368_p902_tid735922_00/AOD.735922._000001.pool.root.1"
    "/unix/atlas2/paw/mc11_7TeV.108494.Pythia_directJpsimu2p5mu2p5.merge.AOD.e835_a131_s1353_a140_r2900_tid601752_00/AOD.601752._000001.pool.root.1"
# "/afs/cern.ch/user/p/paw/BugFixing/myESD.pool.root"
#   "/tmp/ESD.158525._000301.pool.root"
#    "/tmp/Bug/myESD_express_0.pool.root"
#    "/tmp/Bug/myESD.pool.root"
#    "/unix/atlas2/paw/mc09_7TeV.106047.PythiaZmumu_no_filter.recon.ESD.e468_s765_s767_r1430_tid158525_00/ESD.158525._000301.pool.root"
#    "/unix/atlas2/paw/data11_7TeV.00180664.physics_Muons.merge.AOD.o8_f375_m830/data11_7TeV.00180664.physics_Muons.merge.AOD.o8_f375_m830._lb0496-lb0498._0001.root"
#    "/unix/atlas2/paw/mc09_7TeV.106047.PythiaZmumu_no_filter.recon.ESD.e468_s765_s767_r1430_tid158525_00/ESD.158525._000301.pool.root"
                                              ]

###code to add an ESD on InputCollections

#status,output = commands.getstatusoutput('ls '+path)
#output = output.splitlines()

#istart=0
#for i in xrange(0,len(output)) :
#    if output[i].count(path) < 1:
#        output[i] = path+"/"+output[i]
#    output[i] = output[i].replace('//','/')

#    ESDfile = output[i]
#    print ESDfile

#    import glob
#    inFile = glob.glob(ESDfile)

#    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#    athenaCommonFlags.FilesInput = inFile

#    from RecExConfig.InputFilePeeker import inputFileSummary

#    import AthenaPoolCnvSvc.ReadAthenaPool
#    ServiceMgr.EventSelector.BackNavigation = True

#    if istart==0:
#        ServiceMgr.EventSelector.InputCollections = [ ESDfile ]
#        istart=1
#    else:
#        ServiceMgr.EventSelector.InputCollections += [ ESDfile ]
        
#import glob
#inFile = glob.glob('/unix/atlas2/paw/mc08.108407.Pythia_directJpsimu4mu4.recon.ESD.e347_s462_s520_r809_tid092535/ESD.092535._000051.pool.root.1')

#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput.set_Value_and_Lock(inFile)

#from RecExConfig.InputFilePeeker import inputFileSummary

#import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.EventSelector.BackNavigation = True

#ServiceMgr.EventSelector.InputCollections = [ "/unix/atlas2/paw/mc08.108407.Pythia_directJpsimu4mu4.recon.ESD.e347_s462_s520_r809_tid092535/ESD.092535._000051.pool.root.1" ]

#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput=["/home/paw/data10_7TeV.00153565.physics_MuonswBeam.merge.AOD.f251_m466._lb0002-lb0022._0001.1"]

#svcMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()

###test
#from RecExConfig.InputFilePeeker import inputFileSummary
#include ("RecExCond/RecExCommon_flags.py")
#include( "RecExCond/AllDet_detDescr.py" )
#include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
###test

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# BeamPos
from AthenaCommon.Include import include
include("InDetBeamSpotService/BeamCondSvc.py")

# list of the algorithms to be executed at run time
#from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigInDetAnalysisSkeleton
#topSequence.CBNT_AthenaAware += TrigInDetAnalysisSkeleton()
#TrigInDetAnalysisSkeleton = TrigInDetAnalysisSkeleton()
#TrigInDetAnalysisSkeleton.OutputLevel = DEBUG

## set up monitoring
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

from TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConf import TrigJpsiMonTool
TestMonTool = TrigJpsiMonTool( name="TestMonTool")
ToolSvc += TestMonTool
HLTMonManager.AthenaMonTools += [ "TrigJpsiMonTool/TestMonTool" ]

##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = INFO

# might be needed for TriggerConfigGetter...
from RecExConfig.RecFlags  import rec
rec.readRDO=False
#rec.readESD=True
rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False

# To read files with trigger config stored as in-file meta-data, i.e. 13.0.40
# and above: ds. To read AOD produced with 13.0.30 you need to change ds to aod:
#from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.configurationSourceList = ['ds']
# set up trigger config service
#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#cfg =  TriggerConfigGetter( "ReadPool" )

#########cut part

TrigJpsiMonTool.matchR = 0.1
TrigJpsiMonTool.pTCutOffline = 4000.0
TrigJpsiMonTool.d0Cut = 1000.0
TrigJpsiMonTool.z0Cut = 2000.0
TrigJpsiMonTool.d0CutOffline = 1000.0
TrigJpsiMonTool.z0CutOffline = 2000.0
#TrigJpsiMonTool.pTCutOffline = 4000.0
#TrigJpsiMonTool.d0Cut = 1000.0
#TrigJpsiMonTool.d0CutOffline = 1000.0
#TrigJpsiMonTool.matchR = 1.0
#TrigJpsiMonTool.z0CutOffline = 2000.0
TrigJpsiMonTool.Chain0 = "EF_2mu4T_Jpsimumu_IDTrkNoCut"
TrigJpsiMonTool.Chain1 = ""
TrigJpsiMonTool.Chain2 = ""
TrigJpsiMonTool.Collection0 = ""
#TrigJpsiMonTool.Collection1 = ""
TrigJpsiMonTool.Collection2 = ""
TrigJpsiMonTool.Collection3 = ""
TrigJpsiMonTool.Collection4 = ""
TrigJpsiMonTool.Collection5 = ""
TrigJpsiMonTool.Collection6 = ""
TrigJpsiMonTool.Collection7 = ""

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='IDTrackingCBNT.aan.root' OPT='RECREATE'" ]
HLTMonManager.FileKey = "AANT"
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'IDTrackingCBNT.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = DEBUG

theApp.EvtMax = -1

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter( "ReadPool" )

#to dump ESD/AOD content (do only if theApp.EvtMax = 1)
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True 

