# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 607847 2014-07-22 09:52:48Z sroe $

# Set up the reading of the input xAOD:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )


# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "MyPhysVal"
topSequence += monMan

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
tool1.useTrackSelection = False
ToolSvc += tool1
monMan.AthenaMonTools += [tool1]


from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator,usePixel = True,useSCT= True,CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name                                = "InDetDetailedTrackSelectorTool",
                                                               pTMin                               = InDetPrimaryVertexingCuts.minPT(),
                                                               IPd0Max                             = InDetPrimaryVertexingCuts.IPd0Max(),
                                                               IPz0Max                             = InDetPrimaryVertexingCuts.IPz0Max(),
                                                               z0Max                               = InDetPrimaryVertexingCuts.z0Max(),
                                                               sigIPd0Max                          = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                               sigIPz0Max                          = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                               d0significanceMax                   = InDetPrimaryVertexingCuts.d0significanceMax(),
                                                               z0significanceMax                   = InDetPrimaryVertexingCuts.z0significanceMax(),
                                                               etaMax                              = InDetPrimaryVertexingCuts.etaMax(),
                                                               useTrackSummaryInfo                 = InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                               nHitBLayer                          = InDetPrimaryVertexingCuts.nHitBLayer(),
                                                               nHitPix                             = InDetPrimaryVertexingCuts.nHitPix(),
                                                               nHolesPixel                         = InDetPrimaryVertexingCuts.nHolesPix(),
                                                               nHitBLayerPlusPix                   = InDetPrimaryVertexingCuts.nHitBLayerPlusPix(),
                                                               nHitSct                             = InDetPrimaryVertexingCuts.nHitSct(),
                                                               nHitSi                              = InDetPrimaryVertexingCuts.nHitSi(),
                                                               nHitTrt                             = InDetPrimaryVertexingCuts.nHitTrt(),
                                                               nHitTrtHighEFractionMax             = InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                               nHitTrtHighEFractionWithOutliersMax = InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                               useSharedHitInfo                    = InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                               useTrackQualityInfo                 = InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                               fitChi2OnNdfMax                     = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                               TrtMaxEtaAcceptance                 = InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                               # InDetTestBLayerTool                 = InDetRecTestBLayerTool,
                                                               TrackSummaryTool                    = InDetTrackSummaryTool,
                                                               Extrapolator                        = InDetExtrapolator)

        
ToolSvc += InDetTrackSelectorTool




from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["MyPhysVal DATAFILE='MyPhysVal.root' OPT='RECREATE'"]


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = DEBUG
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = 10
