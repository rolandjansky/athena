# LAr EM and HEC sampling fractions

## Input evgen

### LAr EM
The sampling fractions input electrons are generated in release 21.6 as single electrons with a momentum of 50 GeV, injected at a radius of r=1.5m for the barrel and distance of z=3.7405m for the endcap
```
setupATLAS
asetup 21.6.31,AthGeneration
Gen_tf.py  --ecmEnergy=13000 --firstEvent=1 --maxEvents=10 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid11_Mom50000_Radius1500000_eta_0_140 --outputEVNTFile=mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.EVNT.pool.root
Gen_tf.py  --ecmEnergy=13000 --firstEvent=1 --maxEvents=10 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid11_Mom50000_Z3740500_bec_eta_135_350 --outputEVNTFile=mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.EVNT.pool.root
```
Input files with 100k events can be found in:
```
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/LArEM/mc.PG_pid11_Mom50000_Radius1500000_eta_0_140.EVNT.pool.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/LArEM/mc.PG_pid11_Mom50000_Z3740500_bec_eta_135_350.EVNT.pool.root
```

### HEC
The sampling fractions input particles are generated in release 21.6 as single electrons, positrons and photons with a momentum of 100 GeV, injected at distances of z=4.3195m and z=5.1750m which correspond to the front faces of the two HEC wheels
```
setupATLAS
asetup 21.6.31,AthGeneration
Gen_tf.py  --ecmEnergy=13000 --maxEvents=10000 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid11_Mom100000_Z4319500_bec_eta_150_330 --outputEVNTFile=mc.PG_pid11_Mom100000_Z4319500_bec_eta_150_330.EVNT.pool.root
Gen_tf.py  --ecmEnergy=13000 --maxEvents=10000 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid-11_Mom100000_Z4319500_bec_eta_150_330 --outputEVNTFile=mc.PG_pid-11_Mom100000_Z4319500_bec_eta_150_330.EVNT.pool.root
Gen_tf.py  --ecmEnergy=13000 --maxEvents=10000 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid22_Mom100000_Z4319500_bec_eta_150_330 --outputEVNTFile=mc.PG_pid22_Mom100000_Z4319500_bec_eta_150_330.EVNT.pool.root

Gen_tf.py  --ecmEnergy=13000 --maxEvents=10000 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid11_Mom100000_Z5175000_bec_eta_160_330 --outputEVNTFile=mc.PG_pid11_Mom100000_Z5175000_bec_eta_160_330.EVNT.pool.root
Gen_tf.py  --ecmEnergy=13000 --maxEvents=10000 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid-11_Mom100000_Z5175000_bec_eta_160_330 --outputEVNTFile=mc.PG_pid-11_Mom100000_Z5175000_bec_eta_160_330.EVNT.pool.root
Gen_tf.py  --ecmEnergy=13000 --maxEvents=10000 --randomSeed=1234 --jobConfig=athena/Simulation/Tools/CaloSamplingFractionAnalysis/share/PG_pid22_Mom100000_Z5175000_bec_eta_160_330 --outputEVNTFile=mc.PG_pid22_Mom100000_Z5175000_bec_eta_160_330.EVNT.pool.root
```

Input files with 10k events each can be found in:
```
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC/mc.PG_pid-11_Mom100000_Z4319500_bec_eta_150_330.EVNT.pool.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC/mc.PG_pid-11_Mom100000_Z5175000_bec_eta_160_330.EVNT.pool.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC/mc.PG_pid11_Mom100000_Z4319500_bec_eta_150_330.EVNT.pool.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC/mc.PG_pid11_Mom100000_Z5175000_bec_eta_160_330.EVNT.pool.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC/mc.PG_pid22_Mom100000_Z4319500_bec_eta_150_330.EVNT.pool.root
/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC/mc.PG_pid22_Mom100000_Z5175000_bec_eta_160_330.EVNT.pool.root
```

## G4 Simulation
Simulation is run with calibration hits in batches of 5000 events per job with the following simulation command
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
--inputEVNTFile INPUTEVNTfile \
--outputHITSFile OUTPUTEVNTfile \
--maxEvents 5000 \
--skipEvent 0 \
--imf False
```

For the LAr EM and HEC input files this can be done interactively in a shell with the commands below. When running this, maxjobs will say how many jobs should be launched simulateously from the shell
```
maxjobs=4
eosdir=/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions
G4version=10.1
PhysList=FTFP_BERT_ATL
resultdir=$PWD/"$G4version"-$PhysList
mkdir -p $resultdir

for file in $eosdir/LArEM/mc.PG_pid11_Mom50000_*.EVNT.pool.root; do outfile=$resultdir/$(basename $file); source run_LAr_SamplingFraction_simulation.sh $file ${outfile/EVNT.pool.root/HITS.pool.root} $PhysList 8 $maxjobs;done
for file in $eosdir/HEC/mc.PG_pid*_Mom100000_*EVNT.pool.root; do outfile=$resultdir/$(basename $file); source run_LAr_SamplingFraction_simulation.sh $file ${outfile/EVNT.pool.root/HITS.pool.root} $PhysList 1 $maxjobs;done
```

## LAr EM NTuple creation and analysis
For a sufficient precision, ~40k electrons in the barrel and ~40k electrons in the endcap are needed for LAr EM.

```
G4version=10.1
PhysList=FTFP_BERT_ATL
resultdir=$PWD/"$G4version"-$PhysList
get_files LarEMSamplingFraction_topOptions.py
athena.py -c 'import glob;inFileName=glob.glob("'$resultdir'/mc.PG_pid11_Mom50000_Radius1500000*.HITS.*.pool.root")' LarEMSamplingFraction_topOptions.py
mv LArEM_SF.root $resultdir/LArEM_SF_barrel.root

athena.py -c 'import glob;inFileName=glob.glob("'$resultdir'/mc.PG_pid11_Mom50000_Z3740500*.HITS.*.pool.root")' LarEMSamplingFraction_topOptions.py
mv LArEM_SF.root $resultdir/LArEM_SF_endcap.root

get_files LarEMSamplingFraction_analysis.C
root -b -q 'LarEMSamplingFraction_analysis.C("'$resultdir/LArEM_SF_barrel.root'","'$resultdir/LArEM_SF_endcap.root'")'
mv SF_LAr_barrel.pdf SF_LAr_endcap.pdf $resultdir/
```

## HEC NTuple creation and analysis
For a sufficient precision, 5k events per pdgid and Z position are needed, so in total 30k events

```
G4version=10.1
PhysList=FTFP_BERT_ATL
resultdir=$PWD/"$G4version"-$PhysList
get_files LarEMSamplingFraction_topOptions.py
for file in $resultdir/mc.PG_pid*Mom100000_Z[45]*HITS.*.pool.root;do echo $file;athena.py -c 'inFileName=["'$file'"]' LarEMSamplingFraction_topOptions.py;a=$file;b=${a/Z4319500_bec_eta_150_330.HITS/HECfwh.NTUP};c=${b/Z5175000_bec_eta_160_330.HITS/HECrwh.NTUP};mv LArEM_SF.root $c;done

get_files HEC_SF_analysis
root -b -q HEC_SF_analysis/init.C 'HEC_SF_analysis/store_eta.C("'$G4version'","'$PhysList'","'$PWD'")' 'HEC_SF_analysis/get_SF.C("'$G4version'","'$PhysList'")'
```


# FCal sampling fractions

# Tile sampline fractions
