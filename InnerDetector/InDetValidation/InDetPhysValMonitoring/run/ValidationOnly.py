# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ValidationOnly.py 789797 2016-12-15 06:07:11Z lmijovic $

# Set up the reading of the input xAOD:

FNAME = "tmp.AOD"

include( "AthenaPython/iread_file.py" )


# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

include ('PerfMonGPerfTools/ProfileEventLoop_preInclude.py')
ServiceMgr.ProfilerService.InitEvent=10
#from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doMonitoring = True

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/InDetPVMPlotDefRun2.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"


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
#This section should control TTST  7-12-16                                                        
mode = "FWD" #Set this to "Back" for backtracking
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
truthSelection = AthTruthSelectionTool()
if mode == "Back":
  # max prod. vertex radius for secondaries [mm]
  # < 0 corresponds to : do not require decay before pixel
  truthSelection.maxProdVertRadius = -999.9 
  truthSelection.maxBarcode = -1

ToolSvc += truthSelection

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
tool1.TruthSelectionTool = truthSelection
tool1.useTrackSelection = False
#tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.FillTrackInJetPlots = False


ToolSvc += tool1
monMan.AthenaMonTools += [tool1]
'''
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator, usePixel = True, useSCT= True, CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool
'''
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='M_output.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = -1
