# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: LargeD0_jobOptions.py 719703 2016-01-26 03:22:46Z hotono $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset

#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)
import getpass

'''
FNAME = ["/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000001.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000002.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000003.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000004.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000005.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000006.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000007.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000008.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000009.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000010.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000011.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000012.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000013.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000014.pool.root",
         "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000015.pool.root"]
'''

#FNAME = "/afs/cern.ch/work/s/sche/public/MC/xAOD/InDetRecAOD_1500.root"
FNAME = "/afs/cern.ch/work/h/hotono/public/user.kmotohas.mc15_13TeV.402716.PythiaRhad_AUET2BCTEQ6L1_gen_gluino_p1_1200_qq_1070_1ns.retracking_00_StreamAOD/user.kmotohas.7424105.StreamAOD._000006.pool.root"
#FNAME = "/afs/cern.ch/work/h/hotono/public/user.sfujiyam.mc15_13TeV.406000.g1500_w750_b670_Zqq_1ns_pileup.full_signal_retracking.AOD.e192442_s19243_r2015101_r20722_StreamAOD/user.sfujiyam.7368141.StreamAOD._000015.pool.root"

#FNAME = "/afs/cern.ch/work/s/sche/public/20.7.2/InnerDetector/InDetExample/InDetRecExample/run/InDetRecAOD_new.root"
#FNAME = "/afs/cern.ch/work/h/hotono/public/user.kmotohas.data15_13TeV.00267599.physics_MinBias.recon.ESD.r6945.retracking_00_StreamAOD/user.kmotohas.7416502.StreamAOD._002423.pool.root"


if (getpass.getuser())=="mbaugh":
  FNAME = "ESD.05297574._000081.pool.root.1"
  print " Hello, Max"

include( "AthenaPython/iread_file.py" )


# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/inDetPhysValMonitoringPlotDefinitions.hdef"

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg
decorators = InDetPhysValDecoratorAlg()
topSequence += decorators


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "output"
topSequence += monMan

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()

#tool1.useTrackSelection = True
'''
tool1.useTrackSelection = True
#tool1.onlyInsideOutTracks = True
tool1.TrackSelectionTool.CutLevel         = "Loose"
tool1.TrackSelectionTool.UseTrkTrackTools = True
tool1.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
tool1.TrackSelectionTool.Extrapolator     = InDetExtrapolator

'''
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import TrackTruthSelectionTool
truthSelection = TrackTruthSelectionTool()
truthSelection.requireDecayBeforePixel = False
ToolSvc += truthSelection
        
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValLargeD0Tool
tool2 = InDetPhysValLargeD0Tool()
tool2.TruthSelectionTool = truthSelection
#tool2.LongLivedParticle = "Zprime"   # "Zprime", "Wino", "Gluino"
#tool2.LongLivedParticle = "Wino"
tool2.LongLivedParticle = "Gluino"

ToolSvc += tool1
ToolSvc += tool2


monMan.AthenaMonTools += [tool1]
monMan.AthenaMonTools += [tool2]
#monMan.AthenaMonTools += [truthSelection]

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator,usePixel = True,useSCT= True,CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["output DATAFILE='output.root' OPT='RECREATE'"]


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = 10
#theApp.EvtMax = 100
#theApp.EvtMax = -1
