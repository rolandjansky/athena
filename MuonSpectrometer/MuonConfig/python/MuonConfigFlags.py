# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetDetDescrInfo
import re

# Some comments from Ed about existing flags
# MuonCnvExample
# - MuonCnvFlags - not sure we need this - it really only configures single properties of the various cablings - we can just do this directly.
# - MuonCalibFlags - looks like we need this

# MuonByteStream
# - MuonByteStreamFlags.py - AFAICS this is only used here MuonCablings/MuonCablingServers/python/MuonCablingServersConfig.py, MuonCnv/MuonCnvExample/python/MuonCablingConfig.py & duplicates global flag functionality.

# MuonRecExample
# - MuonAlignFlags.py - looks necessary
# - MuonStandaloneFlags.py - necessary, but should be cleaned up (and probably merged with MuonRecFlags i.e. here)
# https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandaloneFlags.py
# - MuonRecFlags.py - necessary, but needs cleaning up.
# https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecFlags.py

def _muonAlignMode(flags):
    # Small function that determines if the alignment flags should be true or false
    # follows the logic in https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonAlignFlags.py
    problematic_tags = ['COMCOND-HLT[A-C]-00[01]-00', 'COMCOND-ES1C-00[01]-00', 'COMCOND-REPC-001', 'COMCOND-SIM-01-00']
    if any(re.match(tag,flags.IOVDb.DatabaseInstance) for tag in problematic_tags):
        return False
    elif flags.Input.isMC:
        return False
    else:
        return True

def createMuonConfigFlags(): 
    mcf=AthConfigFlags()
 
    # Geometry flags
    mcf.addFlag("Muon.doMDTs",True)
    mcf.addFlag("Muon.doTGCs",True)
    mcf.addFlag("Muon.doRPCs",True)
    mcf.addFlag("Muon.doCSCs",lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('HasCSC',"True"))
    mcf.addFlag("Muon.doMicromegas",lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('HasMM',"True"))
    mcf.addFlag("Muon.dosTGCs",lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('HasSTGC',"True"))
    
    # stages of processing
    # 1. Digitization
    mcf.addFlag("Muon.doDigitization",True)
    mcf.addFlag("Muon.doFastDigitization",False)
    mcf.addFlag("Muon.doPseudoTracking",False)
    
    # 2. Reco MuonRecFlags
    
    mcf.addFlag("Muon.doTGCClusterSegmentFinding", False) # Run cluster segment finding
    mcf.addFlag("Muon.doRPCClusterSegmentFinding", False) # Run cluster segment finding
    mcf.addFlag("Muon.prdToxAOD", False) # Run clusterization
    mcf.addFlag("Muon.rpcRawToxAOD", False) # Add RPC RDO to xAOD
    mcf.addFlag("Muon.doMSVertex", False) # Run MS vertex (arXiv:1311.7070)
    # mcf.addFlag("Muon.doDigitization", False) # TODO rename? Re-run muon digitization on-the-fly just before reconstruction. Needs DIGITS as input file.
    # mcf.addFlag("Muon.doSegmentsOnly", False) # Stop reconstruction after segment making. Typically used when making Calibration Ntuple. TODO surely redundant?
    
    mcf.addFlag("Muon.doSegmentT0Fit",False) # Fit MDT segments using a variable t0. Used for cosmics and single beam to compensate for large errors on the trigger time.
    mcf.addFlag("Muon.enableErrorTuning",True) # turn on error tuning to account for misalignments
    mcf.addFlag("Muon.useLooseErrorTuning",False) 
    mcf.addFlag("Muon.useTGCPriorNextBC",False) # Use TGC measurements from Prior and Next Bunch Crossings. These measurements are available in the real data since somewhere in 2008.
    mcf.addFlag("Muon.useAlignmentCorrections",True) # Apply alignment corrections to MuonGeoModel. The corrections are read from a COOL database
    mcf.addFlag("Muon.useWireSagCorrections",False) # tApply wire sag corrections.
    
    # makePRDs - surely this is top level and redundant with makeRIO?
    
    # MuonStandaloneFlags.py 
    mcf.addFlag("Muon.printSummary", False) # Print out a summary for each event at each reco stage
    mcf.addFlag("Muon.segmentOrigin", "Muon") # Can be 'Muon','TruthTracking'
    # reconstructionMode - I think this is a complete duplication of global.BeamType. Dropping.
    mcf.addFlag("Muon.strategy", []) # CutSeedsOnTracks, CombineSegInStation, DynamicSeeding, PreferOutsideIn, AllowOneSharedHit, DoRefinement, DoAmbiSolving
    mcf.addFlag("Muon.trackBuilder", "Moore") # Allowed: 'Moore','TruthTracking','None'
    mcf.addFlag("Muon.refinementTool", "Moore") # Allowed: Moore TODO surely we can drop this if there is only one option?
    mcf.addFlag("Muon.patternsOnly", False) # TODO probably can be dropped? Just disable later steps.
    mcf.addFlag("Muon.createTrackParticles", True ) # TODO do we ever turn this off?
    mcf.addFlag("Muon.straightLineFitMomentum", 2000.0 ) 
    #mcf.addFlag("Muon.doSegmentsOnly", True) # Also in MuonRecFlags ... redundant in both?
    mcf.addFlag("Muon.Chi2NDofCut", 20.0 )  # chi-squared per degree of freedom cut in fitter.
    mcf.addFlag("Muon.enableCurvedSegmentFinding", False ) # TODO I think this one could possibly be removed, since it really is a Tool level configuration.
    mcf.addFlag("Muon.updateSegmentSecondCoordinate", lambda prevFlags : prevFlags.Beam.Type=="collisions") # Do not use for cosmics or singlebeam 
    
    mcf.addFlag("Muon.useSegmentMatching", lambda prevFlags : prevFlags.Beam.Type=="collisions") # Do not use for cosmics or singlebeam 
    mcf.addFlag("Muon.useTrackSegmentMatching", True ) 
    
    # CalibFlags
    mcf.addFlag("Muon.Calib.readMDTCalibFromBlob", True)  # Read mdt tube calibration from blob-folders
    mcf.addFlag("Muon.Calib.correctMdtRtForBField", lambda prevFlags : (prevFlags.Input.isMC is False and prevFlags.Beam.Type=="collisions")) # Apply B-field correction to drift times only for collision data (as done in https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibFlags.py#0028)
    mcf.addFlag("Muon.Calib.correctMdtRtForTimeSlewing", lambda prevFlags : prevFlags.Input.isMC is False) # Apply time slewing correction to drift time only for data (as done in https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibFlags.py#0028)
    mcf.addFlag("Muon.Calib.useMLRt", True) # use ML-RT functions from COOL
    mcf.addFlag("Muon.Calib.applyRtScaling", False) # TODO - apparently not needed, but currently used in MuonCalibConfig. Set false to match https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibFlags.py#0072
    mcf.addFlag("Muon.Calib.correctMdtRtWireSag", False) # Apply RT wiresag correction
    mcf.addFlag("Muon.Calib.mdtCalibrationSource", "MDT") # Source for MDT t0s and rts
    mcf.addFlag("Muon.Calib.mdtPropagationSpeedBeta", lambda prevFlags : 1.0 if prevFlags.Input.isMC else 0.85) #
     
    mcf.addFlag("Muon.Calib.CscPedFromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscNoiseFromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscPSlopeFromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscStatusFromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscRmsFromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscF001FromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscT0BaseFromLocalFile", False)  
    mcf.addFlag("Muon.Calib.CscT0PhaseFromLocalFile", False)  
    
    mcf.addFlag("Muon.Calib.EventTag", "MoMu") 
    
    #  Choose the (MDT) calibration mode.
    # 'ntuple'       : write standard calibration ntuple (up to segments)
    # 'trackNtuple'  : write standard ntuple + tracks
    # 'regionNtuple' : write one ntuple per calibration region
    # 't0Classic'    : do classic t0 calibration
    # 't0MT'         : do MT t0 calibration
    # 'rtClassic'    : do classic rt calibration
    # 'rtAnalytic'   : do analytic rt calibration
    mcf.addFlag("Muon.Calib.mdtMode", "ntuple")  
    
    # Muon Align flags

    mcf.addFlag("Muon.Align.UseALines", lambda prevFlags: (_muonAlignMode(prevFlags)))
    mcf.addFlag("Muon.Align.UseBLines", lambda prevFlags: 'all' if _muonAlignMode(prevFlags) else 'none') # Can be ['none','all','barrel','endcaps']
    mcf.addFlag("Muon.Align.UseILines", lambda prevFlags: (_muonAlignMode(prevFlags)))
    mcf.addFlag("Muon.Align.UseAsBuilt", lambda prevFlags: (_muonAlignMode(prevFlags)))

    # Muon Trigger Flags
    mcf.addFlag("Muon.MuonTrigger", False) 
    mcf.addFlag("Muon.SAMuonTrigger", False) 

    # TODO - add configuration for above    
        
    return mcf

