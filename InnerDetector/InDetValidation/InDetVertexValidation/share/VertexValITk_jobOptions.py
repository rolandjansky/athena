# $Id: PhysVal_jobOptions.py 714189 2015-12-11 17:33:02Z sroe $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset
#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)
#import getpass
#FNAME="root://eosatlas//eos/atlas/atlascerngroupdisk/det-slhc/users/oda/xAODexample/mc12_14TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.e1323_ATLAS-P2-ITK-01-00-00_19.2.3.1_20.1.3.2.pool.root"

#FNAME="/opt/ppd/scratch/glee/data/InDetRecExample/WorkArea/run/Iter_ttbar_8832_000001.root"

import AthenaPoolCnvSvc.ReadAthenaPool
#svcMgr.EventSelector.InputCollections = [ FNAME ]

#include( "AthenaPython/iread_file.py" )

import glob
svcMgr.EventSelector.InputCollections = glob.glob("/afs/cern.ch/work/g/glee/data/Vertex/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.DAOD_IDTRKVALID.e2176_s3142_s3143_r9360/*")
#svcMgr.EventSelector.InputCollections = glob.glob("/opt/ppd/scratch/glee/data/test_ExtBrl4_r7911/OUT.AOD_ExtBrl4_mu200.root")

#svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/work/g/glee/data/data16_13TeV.00310216.physics_MinBias.merge.AOD.f754_m1699/data16_13TeV.00310216.physics_MinBias.merge.AOD.f754_m1699._lb0237._0009.1"]

# Set global flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/inDetPhysValMonitoringPlotDefinitions.hdef"

# Declare tighttrackvertexassotool by do first PV dz recovery(match the association procedure of JVF and JVT, default would be for each PV).
from AthenaCommon import CfgMgr
tighttrackvertexassotool=CfgMgr.CP__TightTrackVertexAssociationTool("TightTrackVertexAssociationTool", dzSinTheta_cut=3, doPV=True)
loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2)

ToolSvc += tighttrackvertexassotool
ToolSvc += loosetrackvertexassotool

#from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchTool
#tool0 = InDetVertexTruthMatchTool()
#tool0.OutputLevel=DEBUG
#ToolSvc += tool0

# Add the truthmatching algorithm:
#from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchAlgorithm
#alg = InDetVertexTruthMatchAlgorithm()
#topSequence += alg

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "tuning"
topSequence += monMan

from InDetVertexValidation.InDetVertexValidationConf import InDetVertexValidationTool
tool1 = InDetVertexValidationTool()
tool1.MaxLayoutEta = 4.0
tool1.TightTrackVertexAssoTool=tighttrackvertexassotool
tool1.LooseTrackVertexAssoTool=loosetrackvertexassotool

#tool1.useTrackSelection = True
'''
#tool1.useTrackSelection = True
#tool1.onlyInsideOutTracks = True
tool1.TrackSelectionTool.CutLevel         = "Loose"
tool1.TrackSelectionTool.UseTrkTrackTools = True
tool1.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
tool1.TrackSelectionTool.Extrapolator     = InDetExtrapolator
'''

ToolSvc += tool1
monMan.AthenaMonTools += [tool1]

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["tuning DATAFILE='tuning.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 100
theApp.EvtMax = -1
