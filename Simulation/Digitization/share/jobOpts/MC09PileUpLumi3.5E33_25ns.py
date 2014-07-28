#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doLowPtMinBias=True
digitizationFlags.doCavern=True
digitizationFlags.doBeamGas=True
digitizationFlags.doBeamHalo=True

digitizationFlags.bunchSpacing=25
digitizationFlags.numberOfCavern=8
digitizationFlags.numberOfLowPtMinBias=8.0
digitizationFlags.initialBunchCrossing=-32 #default
digitizationFlags.finalBunchCrossing=32 #default
digitizationFlags.numberOfBeamHalo=0.05
digitizationFlags.numberOfBeamGas=0.0003

#include("Digitization/minbiasfiles.py")
digitizationFlags.LowPtMinBiasInputCols = ["/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.105001.pythia_minbias.merge.HITS.e479_s851_s860_tid149154_00/HITS.149154._000075.pool.root.1",
                                               "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.105001.pythia_minbias.merge.HITS.e479_s851_s860_tid149154_00/HITS.149154._000079.pool.root.1",
                                               "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.105001.pythia_minbias.merge.HITS.e479_s851_s860_tid149154_00/HITS.149154._000104.pool.root.1",
                                               "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.105001.pythia_minbias.merge.HITS.e479_s851_s860_tid149154_00/HITS.149154._000106.pool.root.1"]

#include("Digitization/cavernfiles.py")
digitizationFlags.cavernInputCols = ["/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.005008.CavernInput.simul.HITS.e4_e563_s854_tid149152_00/HITS.149152._000028.pool.root.1",
                                              "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.005008.CavernInput.simul.HITS.e4_e563_s854_tid149152_00/HITS.149152._000080.pool.root.1",
                                              "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.005008.CavernInput.simul.HITS.e4_e563_s854_tid149152_00/HITS.149152._000368.pool.root.1",
                                              "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.005008.CavernInput.simul.HITS.e4_e563_s854_tid149152_00/HITS.149152._000372.pool.root.1"]

digitizationFlags.beamGasInputCols = ["/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.108863.Hijing_beamgas.simul.HITS.e4_s853_tid149149_00/HITS.149149._000009.pool.root.1"]

digitizationFlags.beamHaloInputCols = ["/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.108852.BeamHaloInputs.simul.HITS.e4_e567_s852_tid149394_00/HITS.149394._000094.pool.root.1"]
