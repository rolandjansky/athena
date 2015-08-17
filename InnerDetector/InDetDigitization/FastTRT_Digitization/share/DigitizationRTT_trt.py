
## These job options are roughly equivalent to the transform command below:

# Digi_trf.py inputHitsFile= maxEvents=25 skipEvents=0 preInclude=SimulationJobOptions/preInclude.BCMOffDigitConfig.py,SimulationJobOptions/preInclude.CaloOffDigitConfig.py,SimulationJobOptions/preInclude.MuonOffDigitConfig.py,SimulationJobOptions/preInclude.ForwardOffDigitConfig.py,SimulationJobOptions/preInclude.PixelOffDigitConfig.py,SimulationJobOptions/preInclude.SCTOffDigitConfig.py,SimulationJobOptions/preInclude.TruthOffDigitConfig.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py conditionsTag=OFLCOND-MC12-SDR-06 outputRDOFile=mc12_ttbar.RDO.pool.root geometryVersion=ATLAS-GEO-20-00-01 numberOfLowPtMinBias=9.988666 numberOfHighPtMinBias=0.011334 'LowPtMinbiasHitsFile=root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000[029,200].pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000[614,817].pool.root.5' digiSeedOffset1=11 digiSeedOffset2=22 'HighPtMinbiasHitsFile=root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._01[4186,7085,7301,9675].pool.root.1' pileupInitialBunch=-4 pileupFinalBunch=4




#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=10
athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005122.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005207.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005259.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005319.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005366.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005435.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005453.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005555.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005595.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005607.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005619.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005656.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005667.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005717.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005745.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005758.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005838.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005902.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005921.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005976.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-MC12-SDR-06'
digitizationFlags.finalBunchCrossing = 4
digitizationFlags.initialBunchCrossing = -4
digitizationFlags.doLowPtMinBias=True
digitizationFlags.numberOfLowPtMinBias=9.988666
digitizationFlags.LowPtMinBiasInputCols=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000029.pool.root.1",
                                        "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000029.pool.root.1",
                                        "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000614.pool.root.5",
                                        "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000817.pool.root.5" ]

digitizationFlags.doHighPtMinBias=True
digitizationFlags.numberOfHighPtMinBias=0.011334
digitizationFlags.HighPtMinBiasInputCols=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._014186.pool.root.1",
                                          "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._017085.pool.root.1",
                                          "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._017301.pool.root.1",
                                          "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471/HITS.743322._019675.pool.root.1"]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-20-00-01'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

DetFlags.TRT_setOn()
#DetFlags.pixel_setOn()
#DetFlags.SCT_setOn()

include("Digitization/ForceUseOfPileUpTools.py")
include("SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py") 

include("Digitization/Digitization.py")


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.StreamRDO.ItemList += ["InDet::TRT_DriftCircleContainer#TRT_DriftCircles"]
