#
# Configuration jobOptions for setting up Muon Combined Reconstruction
#
include.block("MuonCombinedRecExample/MuonCombinedRec_config.py")

from AthenaCommon.Resilience import treatException,protectedInclude
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

muonCombinedRecFlags.setDefaults()

if muonCombinedRecFlags.useNewConfig():
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
    from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedReconstructionCfg

    ConfigFlags.Input.Files = athenaCommonFlags.FilesInput()
    ConfigFlags.Detector.GeometryMDT   = True 
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryCSC   = True     
    ConfigFlags.Detector.GeometryRPC   = True     
    # TODO Keep here for the moment, since we still have debugging to do.
    from AthenaCommon.Logging import logging
    log = logging.getLogger( "conf2toConfigurable".ljust(30) )
    log.setLevel(DEBUG)
    CAtoGlobalWrapper(MuonCombinedReconstructionCfg,ConfigFlags)

else:
    include ("MuonCombinedRecExample/MuonCombinedRec_preprocessing.py")

    include ("MuonCombinedRecExample/MuonCombinedRec_identification.py")

    if not rec.doAODMerging():    
        include ("MuonCombinedRecExample/MuonCombinedRec_postprocessing.py")
