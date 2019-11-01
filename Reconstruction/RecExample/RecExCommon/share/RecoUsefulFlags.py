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

from AthenaCommon.Logging import logging
log_UsefulFlags = logging.getLogger( 'RecExCommon_UsefulFlags' )

try:
    from AthenaMonitoring.DQMonFlags import DQMonFlags
except Exception:
    log_UsefulFlags.info("Could not load DQMonFlags")
    
try:
    from TriggerJobOpts.TriggerFlags import TriggerFlags
except Exception:
    log_UsefulFlags.info("Could not load TriggerFlags")

#      from InDetRecExample.InDetJobProperties import InDetFlags
try:
    from LArConditionsCommon.LArCondFlags import larCondFlags
except Exception:
    log_UsefulFlags.info("Could not load LArCondFlags")

try:
    from LArROD.LArRODFlags import larRODFlags
except Exception:
    log_UsefulFlags.info("Could not load LArRODFlags")

try:
    from CaloRec.CaloRecFlags import jobproperties
except Exception:
    log_UsefulFlags.info("Could not load CaloRecFlags")

try:
    from CaloRec.CaloCellFlags import jobproperties
except Exception:
    log_UsefulFlags.info("Could not load CaloCellFlags")

try:
    from CaloRec.CaloTopoClusterFlags import jobproperties
except Exception:
    log_UsefulFlags.info("Could not load CaloTopoClusterFlags")


try:
    from TileRecUtils.TileRecFlags import jobproperties
except Exception:
    log_UsefulFlags.info("Could not load TileRecFlags")


try:
    from MuonRecExample.MuonRecFlags import muonRecFlags
except Exception:
    log_UsefulFlags.info("Could not load muonRecFlags")

try:
    from MuonCnvExample.MuonCnvFlags import muonCnvFlags
except Exception:
    log_UsefulFlags.info("Could not load muonCnvFlags")

try:
    from MuonCnvExample.MuonCalibFlags import muonCalibFlags
except Exception:
    log_UsefulFlags.info("Could not load muonCalibFlags")

try:
    from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
except Exception:
    log_UsefulFlags.info("Could not load muonCombinedRecFlags")



try:
    from tauRec.tauRecFlags import jobproperties
except Exception:
    log_UsefulFlags.info("Could not load tauRecFlags")

try:
    from egammaRec.egammaRecFlags import jobproperties
except Exception:
    log_UsefulFlags.info("Could not load egammaRecFlags")


try:
    from JetRec.JetRecFlags import jetFlags
except Exception:
    log_UsefulFlags.info("Could not load jetFlags")


try:
    from BTagging.BTaggingFlags import BTaggingFlags
except Exception:
    log_UsefulFlags.info("Could not load BTaggingFlags")
    

try:
    from ParticleBuilderOptions.AODFlags import AODFlags
except Exception:
    log_UsefulFlags.info("Could not load AODFlags")

try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
except Exception:
    log_UsefulFlags.info("Could not load primDPD")

try:
    from D2PDMaker.D2PDFlags import D2PDFlags
except Exception:
    log_UsefulFlags.info("Could not load D2PDFlags")
