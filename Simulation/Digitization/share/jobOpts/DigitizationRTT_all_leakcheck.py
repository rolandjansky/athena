# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   athenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   athenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   athenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   athenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------
# Hephaestus configuration
#--------------------------------------------------------------
import  Hephaestus.MemoryTracker as memtrack
memtrack.trace( "Algorithm::sysExecute" )
memtrack.ignore( 'StoreGateSvc::setupProxy' )

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 25
athenaCommonFlags.PoolHitsInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.simul.HITS.e1193_s1469/HITS.782686._001790.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-MC12-SDR-06'
# digitizationFlags.doInDetNoise=True
# digitizationFlags.doCaloNoise=True
# digitizationFlags.doMuonNoise=True
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
globalflags.DetDescrVersion='ATLAS-GEO-20-00-01'

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
