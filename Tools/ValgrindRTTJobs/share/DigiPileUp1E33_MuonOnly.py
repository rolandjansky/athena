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
jobproperties.AthenaCommonFlags.EvtMax=5
jobproperties.AthenaCommonFlags.PoolHitsInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.105200.T1_McAtNlo_Jimmy.merge.HITS.e835_s1310_s1300/HITS.508808._000857.pool.root.1"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doLowPtMinBias = True
digitizationFlags.doHighPtMinBias = True
digitizationFlags.doCavern = True
digitizationFlags.doBeamGas  = False
digitizationFlags.doBeamHalo = False

digitizationFlags.bunchSpacing = 25
digitizationFlags.numberOfCavern = 2
digitizationFlags.numberOfLowPtMinBias = 2.3
digitizationFlags.numberOfHighPtMinBias = 0.0002
digitizationFlags.initialBunchCrossing = -32 #default
digitizationFlags.finalBunchCrossing = 32 #default
digitizationFlags.IOVDbGlobalTag = 'OFLCOND-SDR-BS7T-05-14'
digitizationFlags.overrideMetadata = ['SimLayout', 'PhysicsList']

digitizationFlags.LowPtMinBiasInputCols = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1299_s1303/HITS.500617._000382.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1299_s1303/HITS.500617._001308.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1299_s1303/HITS.500617._001883.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1299_s1303/HITS.500617._001944.pool.root.1"]

digitizationFlags.HighPtMinBiasInputCols = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e848_s1299_s1303/HITS.500616._001496.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e848_s1299_s1303/HITS.500616._002374.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e848_s1299_s1303/HITS.500616._007836.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e848_s1299_s1303/HITS.500616._009288.pool.root.1"]
    
digitizationFlags.cavernInputCols = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1268_s1269_s1259/HITS.475989._001423.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1268_s1269_s1259/HITS.475989._001891.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1268_s1269_s1259/HITS.475989._002585.pool.root.1",
"root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11a/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1268_s1269_s1259/HITS.475989._003578.pool.root.1"]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-18-01-01'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

include( "Digitization/Digitization.py" )
