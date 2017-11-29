# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   athenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   athenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   athenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   athenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 25
athenaCommonFlags.PoolHitsInput=["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doInDetNoise=False
digitizationFlags.doCaloNoise=False
digitizationFlags.doMuonNoise=False
digitizationFlags.IOVDbGlobalTag='OFLCOND-SDR-BS7T-05-14'
# digitizationFlags.doLowPtMinBias=True
# digitizationFlags.numberOfLowPtMinBias=2.3
# digitizationFlags.LowPtMinBiasInputCols=["", "", "" ]
# digitizationFlags.doCavern=True
# digitizationFlags.numberOfCavern=2
# digitizationFlags.cavernInputCols=["", ""]
# digitizationFlags.doBeamGas=True
# digitizationFlags.numberOfBeamGas=0.5
# digitizationFlags.beamGasInputCols=["", ""]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-16-00-00'

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
