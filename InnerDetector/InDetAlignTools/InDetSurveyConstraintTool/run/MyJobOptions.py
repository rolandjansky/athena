# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Job options file
#
#==============================================================

# Read in Misalignments
doReadDB = True

from AthenaCommon.GlobalFlags import GlobalFlags
#default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
#set defaults
GlobalFlags.DataSource.set_geant4()    
GlobalFlags.InputFormat.set_pool()    
#default is zero luminosity
GlobalFlags.Luminosity.set_zero()

# --- setup Geometry
DetDescrVers = "Rome-Initial"   # DC2 or DC1 or Rome-Initial or CTB
DetDescrVersion = "Rome-Initial"
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
#DetFlags.readRIOPool.TRT_setOff()
DetFlags.Print()


#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

#load relevant libraries
theApp.Dlls += [ "GaudiAlg" ]
theApp.Dlls += [ "GaudiAud"  ]


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# particle property service
include( "PartPropSvc/PartPropSvc.py" )

include("AtlasGeoModel/SetGeometryVersion.py" )
include("AtlasGeoModel/GeoModelInit.py" )

# Get convertors
#include ( "InDetRecExample/InDetPoolConvertors.py" )

include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
include( "InDetEventCnvTools/InDetEventCnvTools_jobOptions.py" )


##theApp.Dlls   += [ "TrkDetDescrTools" ]
##theApp.Dlls   += [ "InDetTrackingGeometry" ]
##theApp.Dlls   += [ "TrkExTools" ]
##theApp.Dlls   += [ "TrkExRungeKuttaPropagator" ]
##theApp.Dlls   += [ 'BFieldAth' ]
##theApp.ExtSvc += [ 'MagFieldAthenaSvc' ]
##theApp.Dlls   += [ 'TrkMagFieldTools' ]
##theApp.Dlls   += [ "MuonEventCnvTools" ]
##theApp.Dlls   += [ "InDetAlignGenTools" ]
##theApp.Dlls   += [ "SiRobustAlignTools" ]

# Needed for database-, Pool- and AlignableTransforms-Handling
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
PoolSvc=Service("PoolSvc")

include ('TrkExTools/BasicExtrapolatorATLAS_RungeKutta.py')
include( "TrkRIO_OnTrackCreator/RIO_OnTrackCreator_InDet.py")

theApp.Dlls+= ["TrkKalmanFitter"]
include ("TrkKalmanFitter/TrkKalmanFitter.py")
KalmanFitter.InstanceForExtrapolation = extrapolatorInstance

if doReadDB:

  ProxyProviderSvc = Service( "ProxyProviderSvc" )
  ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  CondProxyProvider = Service( "CondProxyProvider" )
  
  # set this to the file containing AlignableTransform objects
  CondProxyProvider.InputCollection = "NominalAlignment.root"
  #CondProxyProvider.InputCollection = "MissalignLayer500muRand.root"
  #CondProxyProvider.InputCollection = "Test.root"
  #CondProxyProvider.InputCollection = "Alignment001.root"
  #CondProxyProvider.InputCollection = "Alignment002.root"
  #CondProxyProvider.InputCollection = "Alignment003.root"

  # this preload causes callbacks for read in objects to be activated,
  # allowing GeoModel to pick up the transforms
  IOVSvc=Service("IOVSvc")
  IOVSvc.PreLoadData=True

  # tell SCT/pixel GeoModel to look for alignment constants
  GeoModelSvc = Service( "GeoModelSvc" )
  GeoModelSvc.PixelDetectorTool.Alignable=TRUE
  GeoModelSvc.SCT_DetectorTool.Alignable=TRUE

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel = INFO
MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
# to change the default limit on number of message
MessageSvc.defaultLimit = 9999999  # all messages
# MessageSvc.debugLimit = 10000000   # all debug message etc...
# to get couloured messages (not in emacs though)
# MessageSvc.useColors = true 

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# configure the ELM
multipleELM = Service("MultipleEventLoopMgr")

#ToolSvc.Chi2AlignNextPassTool.NPasses = 10

#services that need to be reinit
#multipleELM.ToBeReinitialized = ["StoreGateSvc", "DetectorStore"]

#theApp.OutStream     =["Stream1"]
#theApp.OutStreamType ="AthenaOutputStream"
#Stream1 = Algorithm( "Stream1" )
#Stream1.EvtConversionSvc     ="AthenaPoolCnvSvc"
#Stream1.OutputFile = "MultiplePassFile.root"; # ** mandatory parameter ** 
#// The output file name
#Stream1.ItemList=["EventInfo#*", "McEventCollection#*"]


EventSelector = Service( "EventSelector" )
EventSelector.InputCollections += [ "rfio:/castor/cern.ch/user/h/haertel/ESD/InDetESD_01SinglePiPlus_1000GeV1.root" ]
# Number of events to be processed (default is 10)
theApp.EvtMax  = 1
# EventSelector.runs   = [ 1 ]


# write out a summary of the time spent
#
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]

ChronoStatSvc = Service ( "ChronoStatSvc")
# write out summary of the memory usage
#   number of events to be skip to detect memory leak
#   20 is default. May need to be made larger for complete jobs.
#ChronoStatSvc.NumberOfSkippedEventsForMemStat = 20
#theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
#theApp.AuditAlgorithms=True # FIXME necessary even to get CPU time
#MemStatAuditor.OutputLevel = INFO
#theApp.AuditServices=True


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls   += [ "InDetAlignGenTools" ]
theApp.Dlls   += [ "InDetSurveyConstraintTool" ]
#top algorithms to be run
theApp.TopAlg = [ "SurveyConstraintTestAlg" ]
MyAlg = Algorithm( "SurveyConstraintTestAlg" )
MyAlg.NLoop = 1

ToolSvc=Service("ToolSvc")

# Local Misalignment of first Pixel EC module
ToolSvc.SurveyConstraint.TransX=0.0
ToolSvc.SurveyConstraint.TransY=0.0
ToolSvc.SurveyConstraint.TransZ=0.0
ToolSvc.SurveyConstraint.RotX=0.000
ToolSvc.SurveyConstraint.RotX2=0.0
ToolSvc.SurveyConstraint.RotY=0.00
ToolSvc.SurveyConstraint.RotZ=0.000

# Global (!) Misalignment of all but the first Pixel EC module
ToolSvc.SurveyConstraint.TransXG=0.0
ToolSvc.SurveyConstraint.TransYG=0.0
ToolSvc.SurveyConstraint.TransZG=0.0
ToolSvc.SurveyConstraint.RotXG=0.0
ToolSvc.SurveyConstraint.RotYG=0.0
ToolSvc.SurveyConstraint.RotZG=0.0

# Local Random Misalignment of Pixel EC modules according to Gaus distribution
ToolSvc.SurveyConstraint.TransXRand=0.01
ToolSvc.SurveyConstraint.TransYRand=0.01
ToolSvc.SurveyConstraint.TransZRand=0.025
ToolSvc.SurveyConstraint.RotXRand=0.0006
ToolSvc.SurveyConstraint.RotYRand=0.0014
ToolSvc.SurveyConstraint.RotZRand=0.00024
ToolSvc.SurveyConstraint.TransXRandPixB=0.0
ToolSvc.SurveyConstraint.TransYRandPixB=0.0
ToolSvc.SurveyConstraint.TransZRandPixB=0.0
ToolSvc.SurveyConstraint.RotXRandPixB=0.0
ToolSvc.SurveyConstraint.RotYRandPixB=0.0
ToolSvc.SurveyConstraint.RotZRandPixB=0.0
ToolSvc.SurveyConstraint.TransXRandSCTEC=0.0
ToolSvc.SurveyConstraint.TransYRandSCTEC=0.0
ToolSvc.SurveyConstraint.TransZRandSCTEC=0.0
ToolSvc.SurveyConstraint.RotXRandSCTEC=0.0
ToolSvc.SurveyConstraint.RotYRandSCTEC=0.0
ToolSvc.SurveyConstraint.RotZRandSCTEC=0.0
ToolSvc.SurveyConstraint.TransXRandSCTB=0.0
ToolSvc.SurveyConstraint.TransYRandSCTB=0.0
ToolSvc.SurveyConstraint.TransZRandSCTB=0.0
ToolSvc.SurveyConstraint.RotXRandSCTB=0.0
ToolSvc.SurveyConstraint.RotYRandSCTB=0.0
ToolSvc.SurveyConstraint.RotZRandSCTB=0.0
ToolSvc.SurveyConstraint.Gaus=1

# Local Random Misalignment of Pixel EC SECTORS according to Gaus distribution
ToolSvc.SurveyConstraint.TransXRandSect=0.01
ToolSvc.SurveyConstraint.TransYRandSect=0.01
ToolSvc.SurveyConstraint.TransZRandSect=0.030
ToolSvc.SurveyConstraint.RotXRandSect=0.0006
ToolSvc.SurveyConstraint.RotYRandSect=0.0006
ToolSvc.SurveyConstraint.RotZRandSect=0.00032
ToolSvc.SurveyConstraint.GausSect=0

# default =-1: no misalignment with 'IDAlignDBTool->dispGroup'
# if TransLayerRand<0: then 'IDAlignDBTool->setTrans' is called
ToolSvc.SurveyConstraint.TransLayerRand=-1
ToolSvc.SurveyConstraint.Misaligncase=-1

#ToolSvc.SurveyConstraint.SurveyWFile="SurveyTextOut.txt"
ToolSvc.SurveyConstraint.SurveyRFile="SurveyNominal.txt"
#ToolSvc.SurveyConstraint.SurveyRFile="SurveyRand.txt"
ToolSvc.InDetSurveyDBTool.DBRoot="/Indet/SiSurvey"
#ToolSvc.CondStream1.OutputFile="MisalignPixECModuleRand.root"
  
# Select the appropriate shared library
theApp.Dlls += [ "RootHistCnv" ]
# Select HBOOK or ROOT persistency 
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" ) 
# Specify the output file name
HistogramPersistencySvc.OutputFile = "histo.root" 


#NTupleSvc        = Service( "NTupleSvc" )
#NTupleSvc.Output = [ "FILE1 DATAFILE='SurveyAlignment.root' TYP='ROOT' OPT='NEW'" ]
#NTupleSvc.Output = [ "FILE1 DATAFILE='SurveyAlignment.root' TYP='ROOT' OPT='READ'" ]
