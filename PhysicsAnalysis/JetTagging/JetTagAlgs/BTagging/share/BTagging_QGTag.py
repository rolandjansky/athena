# Get THistSVC so I can grab likelihood LUT

#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#if not hasattr(svcMgr, 'THistSvc'):
#  from GaudiSvc.GaudiSvcConf import THistSvc
#  svcMgr += THistSvc()
#
#import os
#dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
#dataPathList.insert(0, os.curdir)
#from AthenaCommon.Utils.unixtools import FindFile
#
#QGLikelihoodMCFileName   = "QGLikelihood_MC_v0001.root"
#QGLikelihoodDATAFileName = "QGLikelihood_DATA_v0001.root"
#
#QGLikelihoodMCFile    = FindFile(QGLikelihoodMCFileNameFileName , dataPathList, os.R_OK )
#QGLikelihoodDATAFile  = FindFile(QGLikelihoodDATAFileNameFileName , dataPathList, os.R_OK )
#
#svcMgr.THistSvc.Input += ["QGLikelihoodMCFile   DATAFILE='"+QGLikelihoodMCFile+"' OPT='OLD'"]
#svcMgr.THistSvc.Input += ["QGLikelihoodDATAFile DATAFILE='"+QGLikelihoodDATAFile+"' OPT='OLD'"]
  
# Track selector tool
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
QGTagTrackSelector = Analysis__TrackSelector( 
    name = "QGTagTrackSelector",
    trackToVertexTool = BTagTrackToVertexTool,
    OutputLevel = BTaggingFlags.OutputLevel,
    pTMin = 1000.,
    d0Max = 1.0,
    z0Max = 1.0,
    nHitPix = 1,
    nHitSct = 6,
    fitChi2OnNdfMax = 3.0,
    )
ToolSvc += QGTagTrackSelector
if BTaggingFlags.OutputLevel < 3:
  print QGTagTrackSelector

#
from JetTagTools.JetTagToolsConf import Analysis__HadFlavorTag
QGTag = Analysis__HadFlavorTag( name = "QGTag",
                                OutputLevel = BTaggingFlags.OutputLevel,
                                tagMode = "QG",
                                Runmodus = BTaggingFlags.Runmodus,
                                trackSelectorTool = QGTagTrackSelector,
                                QGLikelihoodMCFileName = "QGLikelihood_MC_v0001.root",
                                QGLikelihoodDATAFileName = "QGLikelihood_DATA_v0001.root",
                                QGPtBounds = [40,60,80,120,160,210,260,310,400],
                                QGEtaBounds = [0,0.8,1.2,2.5],
                                )

ToolSvc += QGTag

if BTaggingFlags.OutputLevel < 3:
  print QGTag


