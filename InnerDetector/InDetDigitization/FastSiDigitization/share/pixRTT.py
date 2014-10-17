
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
athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/mc12_8TeV.107233.ParticleGenerator_mu_Pt100.simul.HITS.e1717_s1581_tid01239219_00/HITS.01239219._000001.pool.root.1","/afs/cern.ch/user/t/tsfiligo/mc12_8TeV.107233.ParticleGenerator_mu_Pt100.simul.HITS.e1717_s1581_tid01239219_00/HITS.01239219._000002.pool.root.1","/afs/cern.ch/user/t/tsfiligo/mc12_8TeV.107233.ParticleGenerator_mu_Pt100.simul.HITS.e1717_s1581_tid01239219_00/HITS.01239219._000002.pool.root.1","/afs/cern.ch/user/t/tsfiligo/mc12_8TeV.107233.ParticleGenerator_mu_Pt100.simul.HITS.e1717_s1581_tid01239219_00/HITS.01239219._000004.pool.root.1","/afs/cern.ch/user/t/tsfiligo/mc12_8TeV.107233.ParticleGenerator_mu_Pt100.simul.HITS.e1717_s1581_tid01239219_00/HITS.01239219._000005.pool.root.1"]
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
#DetFlags.Truth_setOn()

#DetFlags.TRT_setOn()

#DetFlags.SCT_setOn()

include("FastSiDigitization/PixelDigiTool_jobOptions.py")
DetFlags.pixel_setOff()
DetFlags.detdescr.pixel_setOn()

include("Digitization/ForceUseOfPileUpTools.py")
include("SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py") 

include("Digitization/Digitization.py")

from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
                                                PixelCalibSvc        = None,
                                                PixelOfflineCalibSvc = None,
                                                UsePixelCalibCondDB  = FALSE)
ToolSvc += InDetClusterMakerTool

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool(name = "PixelRecoDbTool")
ToolSvc.PixelRecoDbTool.InputSource = 1

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.enableSuppression = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.StreamRDO.ItemList += ["InDet::PixelClusterContainer#PixelClusters"]

MessageSvc.OutputLevel = INFO #INFO
