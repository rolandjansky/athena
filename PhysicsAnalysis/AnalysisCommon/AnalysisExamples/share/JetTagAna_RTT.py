# Python job to run b-tagging+JetTagAna in RTT framework
# Laurent Vacavant 2005/10/12

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags  import globalflags

DetDescrVersion = "ATLAS-GEO-02-01-00"
include( "RecExCommon/AllDet_detDescr.py")

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]
theApp.EvtMax = -1

include( "PartPropSvc/PartPropSvc.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
theApp.Dlls += ["CaloTools" ]
theApp.Dlls += ["TileRecAlgs"]
theApp.Dlls += ["LArClusterRec"]
theApp.Dlls += ["CaloRec"]
theApp.Dlls += ["TileRec"]
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
# non-existent file: include( "InDetEventCnvTools/InDetEventCnvTools_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")

# ------ BTagging configuration:
redoBtag = True
if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags
if redoBtag:
  BTaggingFlags.Runmodus      = "analysis" 
  BTaggingFlags.PoolInputType = "AOD"
  from ParticleBuilderOptions.AODFlags import AODFlags
  AODFlags.TruthParticleJet = False
  BTaggingFlags.OutputLevel = INFO
  BTaggingFlags.Jets = ['Cone4H1Tower']
  BTaggingFlags.JetsWithInfoPlus = ['Cone4H1Tower']
  include( "BTagging/BTagging_jobOptions.py" )
  from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
  JetFitterTagTool = Analysis__JetFitterTag(name = "JetFitterTag")
  JetFitterTagTool.jetCollectionList = ['Cone4H1Tower'] 

# ------ JetTagAna part:
include("AnalysisExamples/JetTagAna_jobOptions.py")
if redoBtag:
  MyJetTagAna.JetContainer = 'Cone4H1TowerJetsAOD'
else:
  MyJetTagAna.JetContainer = 'Cone4H1TowerJets'
MyJetTagAna.PrimaryVertexContainer = BTaggingFlags.PrimaryVertexCollectionName
MyJetTagAna.TrackParticleContainer = BTaggingFlags.TrackParticleCollectionName
print MyJetTagAna

# ------ MsgSvc:
ServiceMgr.MessageSvc.OutputLevel = ERROR
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

