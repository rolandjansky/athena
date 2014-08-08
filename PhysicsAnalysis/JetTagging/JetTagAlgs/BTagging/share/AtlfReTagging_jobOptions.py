if not 'topSequence' in dir():
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

from JetTagTools.JetTagToolsConf import Analysis__EmulTag
Emul2D = Analysis__EmulTag(name = "Emul2D", Tagger = ["IP2D","2d"])
Emul3D = Analysis__EmulTag(name = "Emul3D", Tagger = ["IP3D","3d"])
EmulSV = Analysis__EmulTag(name = "EmulSV", Tagger = ["SV1IP3D","6d"])
EmulOF = Analysis__EmulTag(name = "EmulOF", Mode = 1)
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += Emul2D
ToolSvc += Emul3D
ToolSvc += EmulSV
ToolSvc += EmulOF

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
import os
dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
dataPathList.insert(0, os.curdir)
from AthenaCommon.Utils.unixtools import FindFile
weightFile = FindFile("Weight.root" , dataPathList, os.R_OK )
paramFile  = FindFile("Param.root" , dataPathList, os.R_OK )
svcMgr.THistSvc.Input  += ["WeightFile DATAFILE='"+weightFile+"' OPT='OLD'"]
svcMgr.THistSvc.Input  += ["ParamFile  DATAFILE='"+paramFile+"'  OPT='OLD'"]

from BTagging.BTaggingConf import Analysis__BJetBuilderFast
AtlfReTagging = Analysis__BJetBuilderFast(
                                      name        = "AtlfReTagging",
                                      TagToolList = [Emul2D,Emul3D,EmulSV,EmulOF]
                                     )
topSequence += AtlfReTagging
print AtlfReTagging
