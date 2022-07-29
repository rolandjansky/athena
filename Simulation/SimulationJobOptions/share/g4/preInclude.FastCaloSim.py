######################################################
##
## Use FastCaloSim for fast simulation of the calorimeter
##
######################################################

from G4AtlasApps.SimFlags import simFlags
from ISF_Config.ISF_jobProperties import ISF_Flags


# FastCaloSim requires the Sampling Fractions to be present
from IOVDbSvc.CondDB import conddb
conddb.addOverride("/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-07")

# LArParametrization set to 4 for FastCaloSim
simFlags.LArParameterization = 4
# Deacticate dead material hits for calibration run
simFlags.CalibrationRun.set_Off()
# ISF run with HITS merging to produce a FCS, a G4 and a combined HIT collection
mergeDict = {'ID':False, 'CALO':True, 'MUON':False}
ISF_Flags.HITSMergingRequired.get_Value().update(mergeDict)
# Set simulator name as metadata
ISF_Flags.Simulator = "ATLFAST3" 

