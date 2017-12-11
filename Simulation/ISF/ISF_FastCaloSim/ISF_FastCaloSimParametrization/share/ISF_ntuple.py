from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

import os
import sys
from glob import glob
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
#specify input file here
#athenaCommonFlags.FilesInput = glob( "ESDpion_eta1.pool.root" )
#athenaCommonFlags.FilesInput = glob( "ESD_calo__211__E10000_10000__eta20_20_Evts0-1000_z03350.pool.root" )
#athenaCommonFlags.FilesInput = glob( "ESD_*root" )
#athenaCommonFlags.FilesInput = ["/afs/cern.ch/user/c/cmills/public/pions20GeV_fulldet.ESD.pool.root"]
#athenaCommonFlags.FilesInput = ["/afs/cern.ch/user/c/cmills/public/pions20GeV_z0150_fulldet.ESD.pool.root"]
athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/user/z/zhubacek/FastCaloSim/ForMichael/ESD_evgen_calo__211_E50000_50000_eta20_25_Evts0-5500_vz_0_origin_calo.pool.root"]

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
#include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import ISF_HitAnalysis
topSequence += ISF_HitAnalysis()

#This can be used to text dump the TruthEvent container
#from TruthExamples.TruthExamplesConf import DumpMC
#dumpMC = DumpMC();
#dumpMC.MCEventKey = "TruthEvent"
#topSequence += DumpMC(McEventKey = "TruthEvent")

ISF_HitAnalysis = ISF_HitAnalysis()
ISF_HitAnalysis.NtupleFileName = 'ISF_HitAnalysis'

##############################
#ISF_HitAnalysis.CaloBoundaryR = [ 0., 1148., 1148., 0. ]
#ISF_HitAnalysis.CaloBoundaryZ = [ -3475., -3475., 3475., 3475. ]

ISF_HitAnalysis.CaloBoundaryR = 1148.0
ISF_HitAnalysis.CaloBoundaryZ = 3549.5 #before: 3475.0
ISF_HitAnalysis.CaloMargin=100 #=10cm
ISF_HitAnalysis.NTruthParticles = 1 # Copy only one truth particle to the ntuples for now
#ISF_HitAnalysis.OutputLevel = WARNING
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

#from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimGeometryHelper
#FCSgeoHelper=FastCaloSimGeometryHelper()
#ToolSvc+=FCSgeoHelper
#ISF_HitAnalysis.CaloGeometryHelper=FCSgeoHelper
fcsgeohelper= getPublicTool('FastCaloSimGeometryHelper')
ToolSvc+=fcsgeohelper
ISF_HitAnalysis.CaloGeometryHelper=fcsgeohelper

ISF_HitAnalysis.FastCaloSimCaloExtrapolation= getPublicTool('FastCaloSimCaloExtrapolation')
#ISF_HitAnalysis.FastCaloSimCaloExtrapolation.OutputLevel = DEBUG
#ToolSvc.FastCaloSimCaloExtrapolation.OutputLevel = DEBUG
ISF_HitAnalysis.FastCaloSimCaloExtrapolation.OutputLevel = ERROR
ToolSvc.FastCaloSimCaloExtrapolation.OutputLevel = ERROR


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
#name the output file here
OutputName="ESD_output_test.root"
OutputName=OutputName.replace("ESD","ISF_HitAnalysis")
print OutputName
#Use this to automatically name the output file (rename ESD->ISF_HitAnalysis)
ServiceMgr.THistSvc.Output += [ "ISF_HitAnalysis DATAFILE='"+OutputName+"' OPT='RECREATE'" ]
from AthenaCommon.GlobalFlags import jobproperties
ServiceMgr.THistSvc.Output += [ "ISF_Geometry DATAFILE='output_geo.root' OPT='RECREATE'" ]

#ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.OutputLevel = ERROR
ServiceMgr.MessageSvc.defaultLimit = 9999999

#All events by default
#theApp.EvtMax = -1
theApp.EvtMax = 100

ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE
