# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   jobproperties.AthenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   jobproperties.AthenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   jobproperties.AthenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   jobproperties.AthenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.

jobproperties.AthenaCommonFlags.EvtMax = 5
jobproperties.AthenaCommonFlags.PoolHitsInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.105200.T1_McAtNlo_Jimmy.merge.HITS.e835_s1310_s1300/HITS.508808._000857.pool.root.1"]
jobproperties.AthenaCommonFlags.PoolRDOOutput = "DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag = 'OFLCOND-SDR-BS7T-05-14'
digitizationFlags.overrideMetadata = ['SimLayout', 'PhysicsList']
# digitizationFlags.doInDetNoise = True
# digitizationFlags.doCaloNoise = True
# digitizationFlags.doMuonNoise = True
# digitizationFlags.doLowPtMinBias = True
# digitizationFlags.numberOfLowPtMinBias = 2.3
# digitizationFlags.LowPtMinBiasInputCols = ["", "", "" ]
# digitizationFlags.doCavern = True
# digitizationFlags.numberOfCavern = 2
# digitizationFlags.cavernInputCols = ["", ""]
# digitizationFlags.doBeamGas = True
# digitizationFlags.numberOfBeamGas = 0.5
# digitizationFlags.beamGasInputCols = ["", ""]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-18-01-01'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOn()

include("Digitization/Digitization.py")
