# $Id: PhysVal_jobOptions.py 718465 2016-01-19 15:37:04Z sroe $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset
#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)
#import getpass
#FNAME = "/opt/ppd/scratch/glee/data/vertetx_run2/data16_13TeV.00296939.physics_ZeroBias.merge.AOD.f685_m1576/data16_13TeV.00296939.physics_ZeroBias.merge.AOD.f685_m1576._lb0091-lb0100._0001.1"

#include( "AthenaPython/iread_file.py" )

import AthenaPoolCnvSvc.ReadAthenaPool
import glob
#Updated to run over all data in a directory
svcMgr.EventSelector.InputCollections = glob.glob("/opt/ppd/scratch/glee/data/mc16_valid.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.DAOD_IDTRKVALID.e3698_s2995_r8905/*.pool.root.1")
#svcMgr.EventSelector.InputCollections = glob.glob("/opt/ppd/scratch/glee/data/test_run2_data/data16_13TeV.00304008.physics_Main.merge.DAOD_EGAM1.f716_m1620_p2689/*.pool.root.1")

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/inDetPhysValMonitoringPlotDefinitions.hdef"

# Declare tighttrackvertexassotool by do first PV dz recovery(match the association procedure of JVF and JVT, default would be for each PV).
#from AthenaCommon import CfgMgr
#tighttrackvertexassotool=CfgMgr.CP__TightTrackVertexAssociationTool("TightTrackVertexAssociationTool", dzSinTheta_cut=3, doPV=True)
#loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2)

#ToolSvc += tighttrackvertexassotool
#ToolSvc += loosetrackvertexassotool

from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchTool
tool0 = InDetVertexTruthMatchTool()
tool0.OutputLevel=DEBUG
ToolSvc += tool0

# Add the truthmatching algorithm:
from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchAlgorithm
alg = InDetVertexTruthMatchAlgorithm()
topSequence += alg

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "valid3"
topSequence += monMan

from InDetVertexValidation.InDetVertexValidationConf import InDetVertexValidationTool
tool1 = InDetVertexValidationTool()
tool1.MaxLayoutEta = 3.0

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

from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalBeamSpotMonTool
tool2 = InDetGlobalBeamSpotMonTool()
ToolSvc += tool1
monMan.AthenaMonTools += [tool1]

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["valid3 DATAFILE='valid3.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = -1
