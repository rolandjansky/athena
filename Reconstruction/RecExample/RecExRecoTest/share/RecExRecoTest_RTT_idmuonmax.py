include ("RecExRecoTest/RecExRecoTest_RTT_common.py")


jp.AthenaCommonFlags.PoolESDOutput="muon_ESD.pool.root"
rec.doTrigger=False
#obsolete doAODLVL1False
#rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG


from RecExConfig.RecFlags import rec
rec.doForwardDet=False
rec.doInDet=True
rec.doMuon=True
rec.doCalo=False
rec.doEgamma=False

# If Calo is off, this needs to be off too.
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doCaloTrkMuId = False

from IsolationAlgs.IsoGetter import isoGetter
isoGetter (disable=True)

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrackParticleCellAssociation = False


include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from TrkMaterialProvider import TrkMaterialProviderConf
trkMaterialProviderTool=TrkMaterialProviderConf.Trk__TrkMaterialProviderTool('TrkMaterialProviderTool', UseCaloEnergyMeasurement=False)
ToolSvc += trkMaterialProviderTool


include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
