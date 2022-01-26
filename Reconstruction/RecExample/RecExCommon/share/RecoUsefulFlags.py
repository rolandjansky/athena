# import usual flags container (except indet for now, because of ordering problem)
# FIXME do not load muon, and muon combined flags (ordering problem)
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags  import recAlgs

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BFieldFlags import jobproperties 

from InDetRecExample.InDetJobProperties import InDetFlags

from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/BTagging/BTaggingFlags.py")
excludeTracePattern.append("*/RecExConfig/RecoFunctions.py")

try:
    from AthenaMonitoring.DQMonFlags import DQMonFlags
except Exception:
    printfunc ("WARNING could not load DQMonFlags")
    
#      from InDetRecExample.InDetJobProperties import InDetFlags
try:
    from LArConditionsCommon.LArCondFlags import larCondFlags
except Exception:
    printfunc ("WARNING could not load LArCondFlags")

try:
    from LArROD.LArRODFlags import larRODFlags
except Exception:
    printfunc ("WARNING could not load LArRODFlags")

try:
    from CaloRec.CaloRecFlags import jobproperties
except Exception:
    printfunc ("WARNING could not load CaloRecFlags")

try:
    from CaloRec.CaloCellFlags import jobproperties
except Exception:
    printfunc ("WARNING could not load CaloCellFlags")

try:
    from CaloRec.CaloTopoClusterFlags import jobproperties
except Exception:
    printfunc ("WARNING could not load CaloTopoClusterFlags")


try:
    from TileRecUtils.TileRecFlags import jobproperties
except Exception:
    printfunc ("WARNING could not load TileRecFlags")


try:
    from MuonRecExample.MuonRecFlags import muonRecFlags
except Exception:
    printfunc ("WARNING could not load muonRecFlags")

try:
    from MuonCnvExample.MuonCnvFlags import muonCnvFlags
except Exception:
    printfunc ("WARNING could not load muonCnvFlags")

try:
    from MuonCnvExample.MuonCalibFlags import muonCalibFlags
except Exception:
    printfunc ("WARNING could not load muonCalibFlags")

try:
    from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
except Exception:
    printfunc ("WARNING could not load muonCombinedRecFlags")



try:
    from tauRec.tauRecFlags import jobproperties
except Exception:
    printfunc ("WARNING could not load tauRecFlags")

try:
    from JetRec.JetRecFlags import jetFlags
except Exception:
    printfunc ("WARNING could not load jetFlags")


try:
    from BTagging.BTaggingFlags import BTaggingFlags
except Exception:
    printfunc ("WARNING could not load BTaggingFlags")
    

try:
    from ParticleBuilderOptions.AODFlags import AODFlags
except Exception:
    printfunc ("WARNING could not load AODFlags")

try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
except Exception:
    printfunc ("WARNING could not load primDPD")
