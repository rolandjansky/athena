
# Python job to run b-tagging+analysis job on a pre-existing AOD

#
# run in 'Standard' mode or with all tracking algos?
#
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc.GlobalTag="OFLCOND-SIM-00-00-03"


allTrackingAlgos = False


#theApp.Dlls += [ "GaudiAud" ]
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors += [ "ChronoAuditor" ]
#theApp.AuditAlgorithms = True 



# ------ Geometry:
#include( "AthenaCommon/AthenaCommonFlags.py" )
#include( "RecExCommon/AllDet_detDescr.py")

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags  import globalflags

#DetDescrVersion = "ATLAS-CSC-02-01-00"
DetDescrVersion = "ATLAS-GEO-02-01-00"
#DetDescrVersion = "ATLAS-CSC-05-00-00"
#GeoModelSvc.IgnoreTagDifference = True

include( "RecExCommon/AllDet_detDescr.py")



import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["ZEBRA.P"]

theApp.EvtMax = -1

# ------ POOL:
include( "PartPropSvc/PartPropSvc.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
#theApp.Dlls += [ "AthenaPoolUtilities" ]
theApp.Dlls += ["CaloTools" ]
theApp.Dlls += ["TileRecAlgs"]
theApp.Dlls += ["LArClusterRec"]
theApp.Dlls += ["CaloRec"]
theApp.Dlls += ["TileRec"]
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "InDetEventCnvTools/InDetEventCnvTools_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")


####################################################END VERTEXING

#REOBTAIN some of the Jet algorithms...
from JetRec.JetGetters import *
from JetRec.JetRecFlags import jetFlags

jetFlags.inputFileType.set_Value('AOD')


Kt6alg = make_StandardJetGetter('Kt', 0.6, 'LCTopo').jetAlgorithmHandle()
Cone7alg = make_StandardJetGetter('Cone', 0.7, 'LCTopo').jetAlgorithmHandle()
Kt4alg = make_StandardJetGetter('Kt', 0.4, 'LCTopo').jetAlgorithmHandle()
#Cone4alg = make_StandardJetGetter('Cone', 0.4, 'LCTopo').jetAlgorithmHandle()

Kt6Truthalg = make_StandardJetGetter('Kt', 0.6, 'Truth').jetAlgorithmHandle()
Cone7Truthalg = make_StandardJetGetter('Cone', 0.7, 'Truth').jetAlgorithmHandle()
Kt4Truthalg = make_StandardJetGetter('Kt', 0.4, 'Truth').jetAlgorithmHandle()
#Cone4Truthalg = make_StandardJetGetter('Cone', 0.4, 'Truth').jetAlgorithmHandle()

#Kt7alg.OutputLevel = 3
#Kt5alg.OutputLevel = 2

# ------ BTagging configuration:
if not 'BTaggingFlags' in dir():
          from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.Print()
BTaggingFlags.Runmodus      = "analysis" 
BTaggingFlags.PoolInputType = "AOD"
#BTaggingFlags.SoftEl = False
#BTaggingFlags.SoftMu = False
#BTaggingFlags.JetFitterTag = True
BTaggingFlags.OutputLevel = INFO
# rerun only cone 0.4 Jets
BTaggingFlags.Jets=['Cone4H1Topo',
                    'Cone4H1Tower',
                    'Cone4Truth',
                    'Cone7H1Tower',
                    'Cone7LCTopo',
                    'Kt4LCTopo',
                    'Kt6LCTopo',
                    'Kt6Truth',
                    'Kt4Truth',
                    'Cone7Truth'] 
BTaggingFlags.CalibrationChannelAliases=['Cone4H1Tower->Cone4H1Topo',
                                         'Cone4Truth->Cone4H1Topo',
                                         'Cone7H1Tower->Cone4H1Topo',
                                         'Cone7LCTopo->Cone4H1Topo',
                                         'Kt4LCTopo->Cone4H1Topo',
                                         'Kt6LCTopo->Cone4H1Topo',
                                         'Kt6Truth->Cone4H1Topo',
                                         'Kt4Truth->Cone4H1Topo',
                                         'Cone7Truth->Cone4H1Topo']
#
# Steering (BEFORE including the other job options files)
#

if allTrackingAlgos:
  include( "BTagging/MyBTagging_jobOptions.py" )
else:	
  BTaggingFlags.PrimaryVertexCollectionName = "VxPrimaryCandidate"
  BTaggingFlags.TrackParticleCollectionName = "TrackParticleCandidate"
  BTaggingFlags.CalibrationTag = "BTagCalib-02-00"
  include( "BTagging/BTagging_jobOptions.py" )
#  redo only on cone0.4 jets, disable b-tagging on other jet collections:
#CW  import AthenaCommon.PropertiesManip as manip
#  manip.removeFromList(theApp,'TopAlg',"Analysis::BJetBuilder/BJetBuilderCone7")
#  manip.removeFromList(theApp,'TopAlg',"Analysis::BJetBuilder/BJetBuilderKt")






# ------ BTagAna part:

theApp.Dlls += [ "JetTagRTT" ]
theApp.TopAlg += [ "BTagAna" ]
Analysis = Algorithm( "BTagAna" )

Analysis.JetCollection =  "Cone4H1TowerJets"
Analysis.HistogramStreamName = "File1"
Analysis.OutputLevel = INFO

Analysis.SaveTrees = True

Analysis.FlavourTrees = False

# note that the new collection has a different name:
# NOW: Has the tracking appendix added!!
#      also run over the existing one without re-reconstructing
Analysis.JetCollectionsForTree = [
    'Cone4H1TopoJetsAOD',
    'Cone4H1TowerJetsAOD',
    'Cone4TruthJetsAOD',
    'Cone7H1TowerJetsAOD',
    'Cone7LCTopoJetsAOD',
    'Kt4LCTopoJetsAOD',
    'Kt6LCTopoJetsAOD',
    'Kt6TruthJetsAOD',
    'Kt4TruthJetsAOD',
    'Cone7TruthJetsAOD'
    ]

#if allTrackingAlgos:
#Analysis.JetCollectionsForTree += [
#  "Cone4TowerJetsAODIP", 
#  "Cone4TowerJetsAODXK", 
#  "Cone4TowerJetsAODNT",
#  ]

Analysis.BTagAlgorithms = [
    "JetProb",   
    "IP2D"            ,
    "IP3D"		  ,
    "SV1" 		  ,
    "SV2" 		  ,
    "COMB"		  ,
    "JetFitterTag"    ,
    "JetFitterCOMB"   ,
    "JetFitterTagNN",
    "JetFitterCOMBNN",
    "SoftMuon",
    "SoftElectron"]

Analysis.OutputLevel = INFO
#


# histogram file (rm file first otherwise crash):
import commands
stat, out = commands.getstatusoutput('rm BTagAna.root')
THistSvc = Service("THistSvc")
THistSvc.Output += ["File1 DATAFILE='BTagAna.root' OPT='RECREATE'"]



MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 9999999  # all messages
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"


