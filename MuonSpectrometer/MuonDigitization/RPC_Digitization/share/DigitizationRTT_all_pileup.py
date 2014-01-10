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
jobproperties.AthenaCommonFlags.EvtMax=-1
jobproperties.AthenaCommonFlags.PoolHitsInput=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._02899.pool.root.1"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"


#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties

jobproperties.Digitization.doXingByXingPileUp=True

jobproperties.Digitization.doMinimumBias=True
jobproperties.Digitization.doCavern=True
jobproperties.Digitization.doBeamGas  = True
jobproperties.Digitization.doBeamHalo = True

jobproperties.Digitization.bunchSpacing=50
jobproperties.Digitization.numberOfCavern=2
jobproperties.Digitization.numberOfCollisions=3.7
jobproperties.Digitization.initialBunchCrossing=-32 #default
jobproperties.Digitization.finalBunchCrossing=32 #default
jobproperties.Digitization.numberOfBeamGas = 0.0003
jobproperties.Digitization.numberOfBeamHalo = 0.05

jobproperties.Digitization.minBiasInputCols=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                                             "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root"]

jobproperties.Digitization.cavernInputCols=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00040.pool.root.1",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00063.pool.root.1",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00120.pool.root.1",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00146.pool.root.1",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00171.pool.root.1",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00201.pool.root.2",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00206.pool.root.2",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00235.pool.root.1",
                                            "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00285.pool.root.1"]

jobproperties.Digitization.beamGasInputCols = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00005.pool.root.1",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00009.pool.root.1",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00010.pool.root.1",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00023.pool.root.2",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00024.pool.root.2",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00028.pool.root.3",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00029.pool.root.2",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00032.pool.root.2",
                                               "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00033.pool.root.3"]

jobproperties.Digitization.beamHaloInputCols = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00007.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00008.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00009.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00013.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00015.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00026.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00027.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00028.pool.root",
                                                "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00041.pool.root"]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-02-01-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()

include( "Digitization/Digitization.py" )
from RecExConfig.RecFlags import rec
rec.doCBNT = True
#
