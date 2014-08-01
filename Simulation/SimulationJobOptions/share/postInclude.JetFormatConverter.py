####################################
## postInclude.JetFormatConverter.py
####################################

from xAODJetCnv.xAODJetCnvConf import xAODMaker__JetCnvAlg,xAODMaker__JetCnvTool
jetcnvtool = xAODMaker__JetCnvTool()
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.ToolSvc += jetcnvtool
jcnv = xAODMaker__JetCnvAlg(OutputLevel=ERROR,
                            InputJetCollection="AntiKt4TruthJets",
                            ConstituentContainer="none",
                            JetCnvTool= jetcnvtool,
                            )
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
theJob += jcnv
Stream1.ExcludeList = ['JetCollection#*']
Stream1.ItemList+=["xAOD::JetContainer_v1#AntiKt4TruthJets"]
