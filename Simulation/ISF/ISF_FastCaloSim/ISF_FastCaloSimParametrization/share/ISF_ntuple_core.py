from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

import os
import sys
from glob import glob
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput() # This is stupid and redundant, but necessary

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# To set up a geometry
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo() # Configure the settings for the geometry
include("RecExCond/AllDet_detDescr.py") # Actually load the geometry

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Forward_setOff()

from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import ISF_HitAnalysis
topSequence += ISF_HitAnalysis()

ISF_HitAnalysis = ISF_HitAnalysis()
ISF_HitAnalysis.NtupleFileName = 'ISF_HitAnalysis'
ISF_HitAnalysis.GeoFileName = 'ISF_Geometry'

##############################

ISF_HitAnalysis.CaloBoundaryR = 1148.0
ISF_HitAnalysis.CaloBoundaryZ = 3549.5 #before: 3475.0
ISF_HitAnalysis.CaloMargin=100 #=10cm
ISF_HitAnalysis.NTruthParticles = 1 # Copy only one truth particle to the ntuples for now
ISF_HitAnalysis.SaveAllBranches = False
ISF_HitAnalysis.DoAllCells = False
ISF_HitAnalysis.DoLayers = True
ISF_HitAnalysis.DoLayerSums = True
ISF_HitAnalysis.DoG4Hits = False
ISF_HitAnalysis.TimingCut = 999999
ISF_HitAnalysis.OutputLevel = ERROR

#############################
##### NEW TRACKING SETUP ####
#############################
mlog = logging.getLogger( 'ISF_HitAnalysis::configure:' )
mlog.info("now configure the non-interacting propagator...")
from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
niPropagator = Trk__STEP_Propagator()
niPropagator.MaterialEffects = False
ToolSvc+=niPropagator
mlog.info("configure nono-interacting propagator finished")

mlog.info("now configure the TimedExtrapolator...")
from TrkExTools.TimedExtrapolator import TimedExtrapolator
timedExtrapolator=TimedExtrapolator()
timedExtrapolator.STEP_Propagator = niPropagator
timedExtrapolator.ApplyMaterialEffects = False
ToolSvc+=timedExtrapolator
mlog.info("configure TimedExtrapolator finished")

from CaloTrackingGeometry.CaloTrackingGeometryConf import CaloSurfaceHelper
caloSurfaceHelper = CaloSurfaceHelper()
ToolSvc+=caloSurfaceHelper

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

ISF_HitAnalysis.CaloEntrance=TrkDetFlags.InDetContainerName()
ISF_HitAnalysis.CaloSurfaceHelper=caloSurfaceHelper
ISF_HitAnalysis.Extrapolator=timedExtrapolator

#############################

from AthenaCommon.CfgGetter import getPublicTool

fcsgeohelper= getPublicTool('FastCaloSimGeometryHelper')
ToolSvc+=fcsgeohelper
ISF_HitAnalysis.CaloGeometryHelper=fcsgeohelper

ISF_HitAnalysis.FastCaloSimCaloExtrapolation= getPublicTool('FastCaloSimCaloExtrapolation')
ISF_HitAnalysis.FastCaloSimCaloExtrapolation.OutputLevel = ERROR
ToolSvc.FastCaloSimCaloExtrapolation.OutputLevel = ERROR

ServiceMgr.MessageSvc.OutputLevel = ERROR
ServiceMgr.MessageSvc.defaultLimit = 9999999

ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE