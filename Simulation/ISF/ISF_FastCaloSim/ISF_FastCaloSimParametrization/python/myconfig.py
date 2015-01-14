# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


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
athenaCommonFlags.FilesInput = glob( "ESD_*root" )

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
include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

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
ISF_HitAnalysis.CaloBoundaryZ = 3550.0 #before: 3475.0
ISF_HitAnalysis.CaloMargin=100 #=10cm
ISF_HitAnalysis.NTruthParticles = 1 # Copy only one truth particle to the ntuples for now

from TrackToCalo.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory
	   
theFSElectronExtrapolTrackToCaloTool        =ExtrapolTrackToCaloToolFactory(depth="middle",straightLine=False)
ISF_HitAnalysis.electronExtrapolTool=theFSElectronExtrapolTrackToCaloTool.getFullName()
ToolSvc+=theFSElectronExtrapolTrackToCaloTool

theFSElectronExtrapolTrackToCaloToolEntrance=ExtrapolTrackToCaloToolFactory(depth="entrance",straightLine=False)
ISF_HitAnalysis.electronExtrapolToolEntrance=theFSElectronExtrapolTrackToCaloToolEntrance.getFullName()
ToolSvc+=theFSElectronExtrapolTrackToCaloToolEntrance

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theExtrapolator=AtlasExtrapolator()
ToolSvc+=theExtrapolator

ISF_HitAnalysis.ExtrapolatorName=theExtrapolator.getType()
ISF_HitAnalysis.ExtrapolatorInstanceName=theExtrapolator.getName()

from CaloTrackingGeometry.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
theCaloSurfaceBuilderMiddle=CaloSurfaceBuilderFactory(depth="middle")
ISF_HitAnalysis.CalosurfMiddleInstanceName=theCaloSurfaceBuilderMiddle.getName()
ToolSvc+=theCaloSurfaceBuilderMiddle

theCaloSurfaceBuilderEntrance=CaloSurfaceBuilderFactory(depth="entrance")
ISF_HitAnalysis.CalosurfEntranceInstanceName=theCaloSurfaceBuilderEntrance.getName()
ToolSvc+=theCaloSurfaceBuilderEntrance

#############################

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
#name the output file here
OutputName=athenaCommonFlags.FilesInput()[0]
OutputName=OutputName.replace("ESD","ISF_HitAnalysis")
print OutputName
#Use this to automatically name the output file (rename ESD->ISF_HitAnalysis)
ServiceMgr.THistSvc.Output += [ "ISF_HitAnalysis DATAFILE='"+OutputName+"' OPT='RECREATE'" ]
from AthenaCommon.GlobalFlags import jobproperties
ServiceMgr.THistSvc.Output += [ "ISF_Geometry DATAFILE='"+jobproperties.Global.DetDescrVersion.get_Value()+".root' OPT='RECREATE'" ]

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

#All events by default
theApp.EvtMax = -1
#theApp.EvtMax = 10

ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE


