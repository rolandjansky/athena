
      HOW TO GET SAMPLING FRACTIONS FOR HEC,
      USING GEANT4-BASED SIMULATIONS
  
      Andrei.Kiryunin@cern.ch
  
      March 19-th, 2020
  
  

Procedure to obtain sampling fractions of two HEC wheels (front wheel with 25 mm copper plates and rear wheel with 50 mm copper plates) consists of a few steps:

  - Generation of sets of primary particles to scan over HEC wheels

  - Simulation of detector response (keeping calibration hits in calorimeter cells) with Geant4 version under study and selected physics lists

  - Dumping calibration hits and primary particle information to ROOT-tree

  - Analysis of relevant variables in ROOT-tree to calculate sampling fractions

All necessary files to run generation, simulation and analysis can be found in corresponding sub-directories under the main directory:

  MainDir=$PWD/HEC_SamplingFractions
   

      GENERATION OF PRIMARY PARTICLES

  - Single-particle events
   
  - Three types of primary particles: electrons, positrons, and gammas

    + flat-distributed in pseudorapidity (eta) and azimuthal angle
    + fixed Z-position of vertices (Zvtx) at front face of wheels
    + back pointed to interaction point
    + particle energy = 100 GeV

  - Two scans over HEC wheels

    1. Front wheel scan: 1.5 < eta < 3.3, Zvtx = 431.95 cm 
    2. Rear wheel scan: 1.6 < eta < 3.3, Zvtx = 571.5 cm

  - In total, six generation jobs with names:

    JOB = PPP_eEEEE_scN, where

    + PPP - particle name ("ele" - electrons, "pos" - positrons, "gam" - gammas)
    + EEEE - particle energy (always 0100)
    + N - scan number (1 - front wheel, 2 - rear wheel)

  - 5,000 generated events per job

All relevant files can be found in directory '$MainDir/Generation'. Generation was done with Athena release 17.6.0. 

Prepare environment:

  export AtlasSetup=/afs/cern.ch/atlas/software/releases/17.6.0/AtlasSetup
  source $AtlasSetup/scripts/asetup.sh --tags=AtlasProduction,slc5,17.6.0.3,setup,opt --testarea $PWD

and check out 'ParticleGun' package:

  cmt co -r ParticleGun-01-01-00 Generators/ParticleGun
  cd Generators/ParticleGun/cmt
  gmake

Six sub-directories '$MainDir/Generation/$JOB' contain files 'jobOption.ParticleGun_$JOB.py'.

To run generation, issue in corresponding sub-directory:

  . $MainDir/Generation/env.sh
   
and then run athena:   

  athena.py  jobOption.ParticleGun_$JOB.py  

Output files: 'evgen.pool.root' - can be found (temporarily) under:

  /eos/user/k/kiryunin/HEC_SF/Generation/$JOB
   
   
      SIMULATION AND CALIBRATION-HITS DUMPING

All relevant files can be found in directory '$MainDir/Simulation'. Simulations were done with Athena release 20.3.7, using Geant4 version 10.1 with patch03 (internal ATLAS build geant4.10.1.patch03.atlas01). 

Prepare environment:

  export AtlasSetup=/afs/cern.ch/atlas/software/releases/20.3.7/AtlasSetup
  source $AtlasSetup/scripts/asetup.sh --tags=AtlasProduction,20.3.7.1,setup,opt --testarea $PWD

and check out necessary packages:

  cmt co -r G4PhysicsLists-01-00-05 Simulation/G4Utilities/G4PhysicsLists
  cd Simulation/G4Utilities/G4PhysicsLists/cmt
  gmake
  cd $MainDir/Simulation

  cmt co -r G4AtlasApps-00-11-00-18 Simulation/G4Atlas/G4AtlasApps
  cd Simulation/G4Atlas/G4AtlasApps/cmt
  gmake
  cd $MainDir/Simulation

  cmt co -r CaloD3PDMaker-00-03-09-01 PhysicsAnalysis/D3PDMaker/CaloD3PDMaker
  cd PhysicsAnalysis/D3PDMaker/CaloD3PDMaker/cmt
  gmake
  cd $MainDir/Simulation

Each job is splitted to 25 sub-jobs to simulate 200 events. To prepare all necessary working directories and files, run in the '$MainDir/Simulation':

  ./mkjob.sh PhLis
  
where PhLis - name of the physics list. Two physics lists: FTFP_BERT and FTFP_BERT_ATL - are foreseen for current work. As the result, 150 working directories: 

  $MainDir/Simulation/10.1_$PhLis/$JOB/$SubJob

are created. '$SubJob' runs from '01' to '25'. Each working directory contains:

  - link to necessary file 'evgen.pool.root' with generated events

  - 'simJob.sh' - script to run simulations, where
    + physics list
    + random seed number
    + number of generated events to be skipped
    are defined
  - 'preInclude.VertexPosOff.py' - job config to turn off vertex positioners

  - 'dumpJob.sh' - script to dump calibration hits and primary particle information from simulated files 'myHITS.pool.root' (where name of output ROOT-file is defined)
  - 'addMCpart.py' - job config to add primary particle information  

To run simulations, issue in a working directory:

  . $MainDir/Simulation/env.sh
   
and then run:

  ./simJob.sh
   
[Current simulations were done in the LSF batch system.]      

To dump hits (after successful simulations), run in the same working directory:

  ./dumpJob.sh
   
Output ROOT-tree files: $SubJob.root - can be found (temporarily) under:

  /eos/user/k/kiryunin/HEC_SF/10.1_$PhLis/DumpHits/$JOB


      ANALYSIS OF CALIBRATION HITS TO GET SAMPLING FRACTIONS (SF)

Sampling fraction of a HEC wheel is determined as 

  SF = < E_ACT > / < E_ACT + E_PAS >, where

  - E_ACT - sum of visible energies in active parts of a HEC wheel
  - E_PAS - sum of visible energies in passive parts of a HEC wheel

To calculate final Sampling Fraction (SF0) for each HEC wheel

  - loop over 75 ROOT-tree files from the appropriate scan (three types of primary particles, 25 sub-jobs -> 15,000 events)
  - get pseudorapidity dependence of SF
  - fit this dependence to the constant, excluding edges and regions with tie-rods from the fit
  - take parameter of this fit as the final Sampling Fraction SF0
   
ROOT-based analysis is done in the directory '$MainDir/Analysis', where all necessary C-routines can be found. The workflow of the analysis is the following:

  . $MainDir/Analysis/env.sh  #  use current ATLAS setup

  root -l  #  check calibration hits and get ASCII-files with dependence SF(eta)
  .L store_eta.C
  store_eta ("10.1", "FTFP_BERT_ATL", "/eos/user/k/kiryunin/HEC_SF")
  store_eta ("10.1", "FTFP_BERT",     "/eos/user/k/kiryunin/HEC_SF")
  .q

  root -l -b  #  get final Sampling Fractions for both HEC wheels
  .L get_SF.C 
  get_SF ("10.1", "FTFP_BERT_ATL")
  get_SF ("10.1", "FTFP_BERT")
  .q

Output of the analysis is in the directories '$MainDir/Analysis/10.1_$PhLis'. They contain following files:

  - 'samf_eta_fwh-EM_e0100_sc1.dat' and 'samf_eta_rwh-EM_e0100_sc2.dat' with pseudorapidity dependence of SF for HEC front and rear wheels
    
  - 'SF_EM_e0100.ps' with plots (for both wheels) of dependence SF(eta) with results of the fit:
    + fit to constant and linear fit
    + whole eta-range, cut eta-range, and cut eta-range without tie-rod regions
    
  - 'HEC_SF.txt": fit parameter with error and its rounding value as final sampling fraction (for both wheels)

