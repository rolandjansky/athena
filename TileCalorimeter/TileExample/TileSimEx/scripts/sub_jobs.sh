#!/bin/bash

# This script runs a series of single particle simulation and
#  digitization jobs in a loop.  The input is only the configuration
#  in this script and the myjob_sim.py and myjob_dig.py job option
#  skeletons.   The output of simulation job (myjob_sim.py) is one hits pool file.
#  The output of digitization job(myjob_dig.py) is three ntuple files: one called d3pd,
#  one called aant, and one called ntup
#  The drawing scripts associated with this package assume that the
#  outputs are already structured as they are by this script.

# Flags to run digitization and simulation if need be
#  Note that sim / digi will be skipped if output HIT/RDO files
#  already exist with the same naming convention of this script
#  You will need to delete them if you wish to run again.
doSim=false
doDig=false

# By default, all jobs will run in the current working directory
# and all output files will be placed in the current working directory.
#  If you wish, you can move the output to some other directory
#  This is useful if the jobs have run on lxbatch in a work dir
#  and need to be copied back to your permanent afs / eos space
doMove=true

# Main directory for the output if we are moving it
OutDir=/rdata2/dengfeng/QT/20.3.7.4/OutputFlagZ2795/
# Test if the main output dir exists, if not, make it.
if ( test -d $OutDir )
then
  echo " $OutDir does already exist ..skip"
else
  echo making directory $OutDir
  mkdir $OutDir
fi

# Events per job
NEvents=10000

# Particles to use -- pions (211) and protons (2212) for the test beam
#  Note that test beam was only pi+, not pi-, so we only run 211 here
ParticleIDs=(211 2212)
#ParticleIDs=(2212)

# List of energies to run [MeV].  Can be used for a more detailed curve or
#  just to compare to data.  Testbeam data are at 20, 50, 100, and 180 GeV
#  No proton data at 20 GeV
#Energies=(20000 50000 100000 180000)
#Energies=(10000 30000 40000 60000 700000 80000 90000 110000 120000 130000 140000 150000 160000 170000 190000 200000)
Energies=(20000 50000 100000 180000)

# List of G4 physics lists to use in the job
PhysicsLists=(FTFP_BERT FTFP_BERT_ATL_VALIDATION QGSP_BERT QGSP_BIC)

# Loop over particles
for ParticleID in ${ParticleIDs[*]}
do
  # Assign a particle name for folder/output naming
  if ( test $ParticleID -eq 11 ) 
  then
    ParticleName='e'
  elif ( test $ParticleID -eq 211 )
  then 
    ParticleName='pi'
  elif ( test $ParticleID -eq 2212 )
  then 
    ParticleName='pr'
  else
    echo No Particle
    return
  fi
  # Loop over beam energies
  for Energy in ${Energies[*]}
  do
    # Loop over G4 physics lists
    for PhysicsList in ${PhysicsLists[*]}
    do
      # Create a job options directory to hold the JO 
      # This will be unique for each job based on their settings
      echo $ParticleName  $Energy  $PhysicsList
      jobOptionDir=`pwd`/TileTB90-E$Energy-$ParticleName-$PhysicsList/
      # If the user has requested it, run the simulation job
      if $doSim
      then
        # Check if we have already run simulation.  If so, skip it.
        if [ ! -e tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.HITS.pool.root ]
        then
          # Make the directory for holding JO and output, if it doesn't exist.
          if ( test -d $jobOptionDir )
          then
            echo " $jobOptionDir does already exist ..skip"
          else
            echo making directory $jobOptionDir
            mkdir $jobOptionDir
          fi
          # Settings in the simulation JO - the current ones are a skeleton
          #  that need some strings replaced to run generic jobs
          sed -e s/NEVENTS/$NEvents/ < myjob_sim.py > myjob_sim_tmp1.py
          sed -e s/PARTICLENAME/$ParticleName/ < myjob_sim_tmp1.py > myjob_sim_tmp2.py
          sed -e s/PARTICLEID/$ParticleID/ < myjob_sim_tmp2.py > myjob_sim_tmp3.py
          sed -e s/BEAMENERGY/$Energy/ < myjob_sim_tmp3.py > myjob_sim_tmp4.py
          sed -e s/PHYSICSLIST/$PhysicsList/ < myjob_sim_tmp4.py > myjob_sim_tmp5.py
          mv myjob_sim_tmp5.py $jobOptionDir/jobOptions_TileTB_Sim.py
          rm myjob_sim_tmp*.py
          # Now run the actual athena job
          #athena.py $jobOptionDir/jobOptions_TileTB_Sim.py 
        fi
      fi
      # If the user has requested it, run the digitization
      if $doDig
      then
        # Check that the ntuple does not already exist and that the input
        #  hit file does exist -- if they don't, then skip the job
        if [ -e tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.HITS.pool.root ] && [ ! -e tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.ntup.root ]
        then
          # Settings in the digitization JO - the current ones are a skeleton
          #  that need some strings replaced to run these jobs
          sed -e s/PARTICLENAME/$ParticleName/ < myjob_dig.py > myjob_dig_tmp1.py
          sed -e s/BEAMENERGY/$Energy/ < myjob_dig_tmp1.py > myjob_dig_tmp2.py
          sed -e s/PHYSICSLIST/$PhysicsList/ < myjob_dig_tmp2.py > myjob_dig_tmp3.py
          mv myjob_dig_tmp3.py $jobOptionDir/jobOptions_TileTB_Dig.py
          rm myjob_dig_tmp*.py
          # Now run the actual athena job
          #athena.py $jobOptionDir/jobOptions_TileTB_Dig.py 
        fi
      fi
      # If the user has requested it, relocate the output files
      if $doMove
      then
        # Check if the directory that we need already exists; otherwise make it
        if ( test -d $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList/ )
        then
          echo " $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList/ does already exist ..skip"
        else
          echo making directory $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList/
          mkdir $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList/
        fi
        # Move the three types of ntuples and hits pool file to the output directory
        mv  tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.d3pd.root $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList
        mv  tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.ntup.root $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList
        mv  tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.aant.root $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList
        mv  tiletb90-E${Energy}-${ParticleName}_${PhysicsList}.HITS.pool.root $OutDir/TileTB90-E$Energy-$ParticleName-$PhysicsList
      fi
    done
    # Done looping over physics lists
  done
  # Done looping over beam energies
done
# Done looping over particles
