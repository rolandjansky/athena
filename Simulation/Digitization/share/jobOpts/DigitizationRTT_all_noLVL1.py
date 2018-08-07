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
athenaCommonFlags.PoolHitsInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-MC16-SDR-16'
digitizationFlags.dataRunNumber=284500
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
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"

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
