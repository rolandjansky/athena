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
#numberOfHighPtMinBias=0.045336 numberOfLowPtMinBias=39.954664

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 5
athenaCommonFlags.PoolHitsInput=["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.simul.HITS.e1193_s1469/HITS.782686._001790.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
include("Digitization/ForceUseOfPileUpTools.py")
include("SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py")

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-MC12-SDR-06'
digitizationFlags.doLowPtMinBias=True
digitizationFlags.doHighPtMinBias=True
digitizationFlags.doCavern=False
digitizationFlags.doBeamGas=False
digitizationFlags.doBeamHalo=False

digitizationFlags.bunchSpacing=25
digitizationFlags.numberOfLowPtMinBias=7.9909328
digitizationFlags.numberOfHighPtMinBias=0.0090672
digitizationFlags.initialBunchCrossing=-32 #default
digitizationFlags.finalBunchCrossing=32 #default

digitizationFlags.LowPtMinBiasInputCols=["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000029.pool.root.1",
                                         "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000200.pool.root.1",
                                         "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000614.pool.root.5",
                                         "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000817.pool.root.5"]

digitizationFlags.HighPtMinBiasInputCols=["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._014186.pool.root.1",
                                          "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._017085.pool.root.1",
                                          "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._017301.pool.root.1",
                                          "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._019675.pool.root.1",]

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

include( "Digitization/Digitization.py" )
