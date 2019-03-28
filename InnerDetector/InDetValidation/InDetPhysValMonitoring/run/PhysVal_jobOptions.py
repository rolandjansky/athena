# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 795792 2017-02-06 14:45:08Z sroe $

# Set up the reading of the input xAOD:
import getpass
FNAME = "AOD.pool.root"
mode = "Fwd" #Set this to "Back" for backtracking
usingTrackSelection = False
#
#options for Max to explore backtracking
if (getpass.getuser())=="mbaugh":
  mode="Back"
  FNAME = "../command/target.pool.root"
  '''
  The following sets an environment variable to enable backtracking debug messages.
  To use in C++:
  const char * debugBacktracking = std::getenv("BACKTRACKDEBUG");
  '''
  os.environ["BACKTRACKDEBUG"] = "1"
  #
include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/InDetPVMPlotDefRun2.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

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
monMan.FileKey = "M_output"
topSequence += monMan


#this works:
'''
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectorTool",
                                                        CutLevel = InDetPrimaryVertexingCuts.TrackCutLevel(),
                                                        minPt = InDetPrimaryVertexingCuts.minPT(),
                                                        maxD0 = InDetPrimaryVertexingCuts.IPd0Max(),
                                                        maxZ0 = InDetPrimaryVertexingCuts.z0Max(),
                                                        maxZ0SinTheta = InDetPrimaryVertexingCuts.IPz0Max(),
                                                        maxSigmaD0 = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                        maxSigmaZ0SinTheta = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                        # maxChiSqperNdf = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(), # Seems not to be implemented?
                                                        maxAbsEta = InDetPrimaryVertexingCuts.etaMax(),
                                                        minNInnermostLayerHits = InDetPrimaryVertexingCuts.nHitInnermostLayer(),
                                                        minNPixelHits = InDetPrimaryVertexingCuts.nHitPix(),
                                                        maxNPixelHoles = InDetPrimaryVertexingCuts.nHolesPix(),
                                                        minNSctHits = InDetPrimaryVertexingCuts.nHitSct(),
                                                        minNTrtHits = InDetPrimaryVertexingCuts.nHitTrt(),
                                                        minNSiHits = InDetPrimaryVertexingCuts.nHitSi(),
                                                        TrackSummaryTool = InDetTrackSummaryTool,
                                                        Extrapolator = InDetExtrapolator)


ToolSvc += InDetTrackSelectorTool
'''
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool=InDet__InDetTrackSelectionTool()
ToolSvc += InDetTrackSelectorTool
InDetTrackSelectorTool.CutLevel = "TightPrimary"

#This section should control TTST  7-12-16                                                        
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
AthTruthSelectionTool = AthTruthSelectionTool()

if mode=="Back":
  # max prod. vertex radius for secondaries [mm]
  AthTruthSelectionTool.minPt = 5000
  AthTruthSelectionTool.maxProdVertRadius = 4000 
  AthTruthSelectionTool.maxBarcode = -1
  AthTruthSelectionTool.hasNoGrandparent = True
  AthTruthSelectionTool.poselectronfromgamma = True
  os.environ["BACKTRACKDEBUG"] = "1"

print AthTruthSelectionTool
ToolSvc += AthTruthSelectionTool

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
tool1.TruthSelectionTool = AthTruthSelectionTool
tool1.useTrackSelection = usingTrackSelection
tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.useTrkSelectPV= False
tool1.FillTrackInJetPlots = True
print tool1
ToolSvc += tool1

monMan.AthenaMonTools += [tool1]

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator, usePixel = True, useSCT= True, CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='M_output.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = -1

