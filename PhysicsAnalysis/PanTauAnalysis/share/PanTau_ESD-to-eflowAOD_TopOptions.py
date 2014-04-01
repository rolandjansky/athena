from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput=["/atlas1/atlas/mc11_7TeV.107054.PythiaWtaunu_incl.recon.ESD.e825_s1349_s1300_r3604_tid792515_00/ESD.792515._000093.pool.root.2"]
#athenaCommonFlags.FilesInput=["/atlas1/atlas/mc11_7TeV.107054.PythiaWtaunu_incl.recon.ESD.e825_s1349_s1300_r3604_tid792515_00/ESD.792515._000041.pool.root.1"]
#athenaCommonFlags.FilesInput=["/tmp/limbach/data11_7TeV.00187501.physics_Muons.recon.DESD_ZMUMU.f396_m716_r2764_tid530016_00/DESD_ZMUMU.530016._000004.pool.root.1"]
athenaCommonFlags.FilesInput=["/tmp/limbach/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3578_tid783783_00/ESD.783783._000006.pool.root.1"]



from RecExConfig.RecFlags import rec#,Rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
#rec.doESD.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(True)

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)
#recAlgs.doTrigger.set_Value_and_Lock(True)

##change some calo flags
from CaloRec.CaloRecFlags import jobproperties as CaloRecJP
CaloRecJP.CaloRecFlags.Enabled.set_Value_and_Lock(True)
CaloRecJP.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
CaloRecJP.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

from eflowRec.eflowRecFlags import jobproperties as eflowRecJP
eflowRecJP.eflowRecFlags.runTauMode.set_Value_and_Lock(True)


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=20
include("RecExCommon/RecExCommon_topOptions.py")

import AthenaCommon.PropertiesManip as manip

manip.appendItemsToList(StreamAOD,'ItemList','eflowObjectContainer#eflowObjects_tauMode')
manip.appendItemsToList(StreamAOD,'ItemList','CaloClusterContainer#eflowClusters_tauMode')

#adjusting outputlevels to get rid of over abundant warnings from MuonMET something
from JetMissingEtTagTools.JetMissingEtTagToolsConf import JetMissingEtTagTool as ConfiguredJetMissingEtTagTool
ConfiguredJetMissingEtTagTool.OutputLevel = ERROR
StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.OutputLevel = ERROR

