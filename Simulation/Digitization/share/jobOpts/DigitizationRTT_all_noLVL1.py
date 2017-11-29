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
athenaCommonFlags.EvtMax=25
athenaCommonFlags.PoolHitsInput=["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc15a/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._001041.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-RUN12-SDR-25'
#digitizationFlags.dataRunNumber=222500
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
globalflags.DetDescrVersion = "ATLAS-R2-2015-03-01-00"

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

include("Digitization/Digitization.py")
