# LAr EM sampling fractions

## Input evgen

The sampling fractions input electrons are generated in release 21.6 as single electrons with a momentum of 50 GeV, injected at a radius of r=1.5m for the barrel and distance of z=3.7405m for the endcap
```
setupATLAS
asetup 21.6.31,AthGeneration
Gen_tf.py  --ecmEnergy=13000 --firstEvent=1 --maxEvents=10 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid11_Mom50000_Radius1500000_eta_0_140 --outputEVNTFile=mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.HITS.root
Gen_tf.py  --ecmEnergy=13000 --firstEvent=1 --maxEvents=10 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid11_Mom50000_Z3740500_bec_eta_135_350 --outputEVNTFile=mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.HITS.pool.root
```
Input files with 100k events can be found in:
```
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/LArEM/mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.100k.EVNT.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/LArEM/mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.100k.EVNT.root
```

## G4 Simulation
Simulation is run with calibration hits
```
Sim_tf.py --simulator 'FullG4' \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--postExec 'EVNTtoHITS:topSequence.BeamEffectsAlg.GenEventManipulators = [getPublicTool("GenEventValidityChecker")]' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/LArEM/mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.100k.EVNT.root" \
--outputHITSFile 'mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.0_10k.HITS.root' \
--maxEvents 10000 \
--skipEvent 0 \
--imf False
Sim_tf.py --simulator 'FullG4' \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--postExec 'EVNTtoHITS:topSequence.BeamEffectsAlg.GenEventManipulators = [getPublicTool("GenEventValidityChecker")]' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/afs/cern.ch/user/m/mduehrss/public/mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.100k.EVNT.root" \
--outputHITSFile 'mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.0_5k.HITS.pool.root' \
--maxEvents 5000 \
--skipEvent 0 \
--imf False
```

## NTuple creation and analysis
For a sufficient precision, ~40k electrons in the barrel and ~40k electrons in the endcap are needed

```
get_files LarEMSamplingFraction_topOptions.py
athena.py -c 'inFileName=["mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.0_10k.HITS.root","mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.10k_20k.HITS.root","mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.20k_30k.HITS.root","mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.30k_40k.HITS.root"]' LarEMSamplingFraction_topOptions.py
mv LArEM_SF.root LArEM_SF_barrel.root

athena.py -c 'inFileName=["mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.0_5k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.5k_10k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.10k_15k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.15k_20k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.20k_25k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.25k_30k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.30k_35k.HITS.pool.root","mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.35k_40k.HITS.pool.root"]' LarEMSamplingFraction_topOptions.py
mv LArEM_SF.root LArEM_SF_endcap.root

get_files LarEMSamplingFraction_analysis.C
root -b -q LarEMSamplingFraction_analysis.C 
```

# HEC sampling fractions

# FCal sampling fractions

# Tile sampline fractions
