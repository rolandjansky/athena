#!/bin/bash

#
# (1) CHECK THIS:
#
# this directory which will be used to store the reference files.
# make sure you set this to a directory for which you have write access
# (~1.5 GB free space needed)
OUTPUTDIR=/afs/cern.ch/atlas/groups/Simulation/ISF/RTT-MCProd-Reference-Files-Output



MINBIASEVGEN="/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/minbias_Inelastic-pythia8-7000.evgen.pool.root"
MINBIASEVGEN_8TEV="/afs/cern.ch/atlas/groups/Simulation/ISF/EvGen/mc12_8TeV.minbias.evgen.pool.root"
TTBAREVGEN="/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root"

SIMSCRIPT=run_script_sim.sh
DIGIRECOSCRIPT=run_script_digireco.sh

COPYSCRIPT=${OUTPUTDIR}/copy_to_RTT_directory.sh

SUBMITTEDJOBS=()

# function will setup a new output directory for reference files
#  -> function is called from below
function setupdir {
  SUBMITTEDJOBS+=("${1}")
  CURFOLDER=${OUTPUTDIR}/${1}
  echo setting up and submitting job: ${CURFOLDER}
  mkdir -p ${CURFOLDER}
  cd ${CURFOLDER}
}

# function will generate SIMSCRIPT shell script (used for lxbatch submission)
#  -> function is called from below
function setupsim {
  ASETUP=${1}
  JOBTYPE=${2}
  INPUT=${3}
  NUMEVTS=${4}
  touch ${SIMSCRIPT}
  chmod a+x ${SIMSCRIPT}

  echo "export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup" > ${SIMSCRIPT}
  echo "source \$AtlasSetup/scripts/asetup.sh ${ASETUP}" >> ${SIMSCRIPT}
  echo "export TRF_ECHO=True" >> ${SIMSCRIPT}
  echo "Sim_tf.py --simulator ${JOBTYPE} --useISF True --randomSeed 123 --enableLooperKiller True --maxEvents ${NUMEVTS} --geometryVersion ATLAS-GEO-20-00-01 --conditionsTag OFLCOND-MC12-SIM-00 --outputHitsFile HITS.pool.root --inputEvgenFile ${INPUT} --postInclude=G4AtlasTests/postInclude.DCubeTest.py" >> ${SIMSCRIPT}
  echo "source \$AtlasSetup/scripts/asetup.sh 17.6.51.3,slc5,opt,runtime,64" >> ${SIMSCRIPT}
  echo "HITSMerge_tf.py --DBRelease=current --inputHITSFile=HITS.pool.root --outputHITS_MRGFile=HITS.merged.pool.root" >> ${SIMSCRIPT}
  echo "cp HITS.merged.pool.root truth.root `pwd`" >> ${SIMSCRIPT}
}

# function will generate DIGIRECOSCRIPT shell script (used for lxbatch submission)
#  -> function is called from below
function setupdigireco {
  ASETUP=${1}
  DETCOND=${2}
  touch ${DIGIRECOSCRIPT}
  chmod a+x ${DIGIRECOSCRIPT}

  echo "export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup" > ${DIGIRECOSCRIPT}
  echo "source \$AtlasSetup/scripts/asetup.sh ${ASETUP} --grouparea=/afs/cern.ch/atlas/groups/Simulation/ISF/GroupAreas/17.3.12.5_updated_for_RTT" >> ${DIGIRECOSCRIPT}
  echo "export TRF_ECHO=True" >> ${SIMSCRIPT}
  echo "DigiMReco_trf.py digiRndmSvc='AtDSFMTGenSvc' preExec_e2a='rec.Commissioning.set_Value_and_Lock(True)' jobNumber='1' conditionsTag=${DETCOND} preInclude_h2r='SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py,RunDependentSimData/configLumi_user.py' geometryVersion='ATLAS-GEO-20-00-01' preExec_h2r='userRunLumiOverride={\"run\":195847, \"lb\":1, \"starttstamp\":1328040250, \"mu\":0.000}' autoConfiguration='everything' cavernHitsFile='root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471/HITS.743321._000029.pool.root.1' DataRunNumber='-1' preExec_r2e='rec.Commissioning.set_Value_and_Lock(True);from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False;InDetFlags.doStandardPlots.set_Value_and_Lock(True)' numberOfCavernBkg='0' inputHitsFile=`pwd`/HITS.merged.pool.root outputESDFile=ESD.pool.root outputAODFile=AOD.pool.root maxEvents=99999 outputRDOFile=RDO.pool.root outputHIST_PHYSVALMONFile=PhysValMon.root postInclude_h2r=DigitizationTests/postInclude.RDO_Plots.py" >> ${DIGIRECOSCRIPT}
  echo "cp InDetStandardPlots.root PhysValMon.root RDO_truth.root `pwd`" >> ${DIGIRECOSCRIPT}
}

# function will populate current directory with SIMSCRIPT and DIGIRECOSCRIPT
#  -> function is called from below
function setupsimchain {
  ASETUP=${1}
  JOBTYPE=${2}
  INPUT=${3}
  NUMEVTS=${4}
  DIGIRECODETCOND=${5}
  
  setupsim ${ASETUP} ${JOBTYPE} ${INPUT} ${NUMEVTS}
  setupdigireco 17.3.12.5,runtime,opt,32,slc5 ${DIGIRECODETCOND}
  cd `pwd` >> ${SIMSCRIPT}
  echo "bsub -q 2nw `pwd`/${DIGIRECOSCRIPT}" >> ${SIMSCRIPT}
}



###########################################
#                                         #
#   (2) MAIN PART: JOB SUBMISSION         #
#                                         #
###########################################

# setup and submit the individual jobs to lxbatch:
#
setupdir ttbar_G4_4evt
setupsimchain 17.7.3.9,opt,runtime,64,slc6,gcc46 MC12G4 ${TTBAREVGEN} 4 OFLCOND-MC12-SDR-06
bsub -q 1nd `pwd`/${SIMSCRIPT}

setupdir ttbar_G4_500evt
setupsimchain 17.7.3.9,opt,runtime,64,slc6,gcc46 MC12G4 ${TTBAREVGEN} 500 OFLCOND-MC12-SDR-06
bsub -q 2nw `pwd`/${SIMSCRIPT}

setupdir ttbar_ATLFASTII_500evt
setupsimchain 17.7.3.10,opt,runtime,64,slc6,gcc46 ATLFASTII ${TTBAREVGEN} 500 OFLCOND-MC12-AFII-07
bsub -q 1nw `pwd`/${SIMSCRIPT}

setupdir ttbar_ATLFASTIIF_500evt
setupsimchain 17.7.3.10,opt,runtime,64,slc6,gcc46 ATLFASTIIF ${TTBAREVGEN} 500 OFLCOND-MC12-AFII-07
bsub -q 1nw `pwd`/${SIMSCRIPT}


setupdir minbias_G4_50evt
setupsimchain 17.7.3.9,opt,runtime,64,slc6,gcc46 MC12G4 ${MINBIASEVGEN} 50  OFLCOND-MC12-SDR-06
bsub -q 2nd `pwd`/${SIMSCRIPT}

setupdir minbias_G4_2000evt
setupsimchain 17.7.3.9,opt,runtime,64,slc6,gcc46 MC12G4 ${MINBIASEVGEN_8TEV} 2000 OFLCOND-MC12-SDR-06
bsub -q 2nw `pwd`/${SIMSCRIPT}

setupdir minbias_ATLFASTII_2000evt
setupsimchain 17.7.3.10,opt,runtime,64,slc6,gcc46 ATLFASTII ${MINBIASEVGEN_8TEV} 2000 OFLCOND-MC12-AFII-07
bsub -q 2nw `pwd`/${SIMSCRIPT}

setupdir minbias_ATLFASTIIF_2000evt
setupsimchain 17.7.3.10,opt,runtime,64,slc6,gcc46 ATLFASTIIF ${MINBIASEVGEN_8TEV} 2000 OFLCOND-MC12-AFII-07
bsub -q 2nw `pwd`/${SIMSCRIPT}




###########################################
#                                         #
#   (3) generate COPYSCRIPT               #
#                                         #
###########################################
echo '#!/bin/bash' > ${COPYSCRIPT}
echo 'RTTREFERENCES=/afs/cern.ch/atlas/project/RTT/DCubeReference/offline/Simulation/ISF/ISF_Validation/' >> ${COPYSCRIPT}
echo "JOBS=( ${SUBMITTEDJOBS[@]} ) " >> ${COPYSCRIPT}
echo 'echo ""' >> ${COPYSCRIPT}
echo 'echo "INFO: if you intent to use the new reference files, DO OVERWRITE the existing ones ;)"' >> ${COPYSCRIPT}
echo 'echo ""' >> ${COPYSCRIPT}
echo 'for CUR_JOB in "${JOBS[@]}"' >> ${COPYSCRIPT}
echo 'do' >> ${COPYSCRIPT}
echo '  echo copying files for job:  ${CUR_JOB}' >> ${COPYSCRIPT}
echo "  cp -i ${OUTPUTDIR}/"'${CUR_JOB}/truth.root $RTTREFERENCES/MCProd/${CUR_JOB}.truth.root' >> ${COPYSCRIPT}
echo "  cp -i ${OUTPUTDIR}/"'${CUR_JOB}/InDetStandardPlots.root $RTTREFERENCES/MCProd/${CUR_JOB}.InDetStandardPlots.root' >> ${COPYSCRIPT}
echo "  cp -i ${OUTPUTDIR}/"'${CUR_JOB}/PhysValMon.root $RTTREFERENCES/MCProd/${CUR_JOB}.PhysValMon.root' >> ${COPYSCRIPT}
echo "  cp -i ${OUTPUTDIR}/"'${CUR_JOB}/RDO_truth.root $RTTREFERENCES/MCProd/${CUR_JOB}.RDO_truth.root' >> ${COPYSCRIPT}
echo 'done' >> ${COPYSCRIPT}


echo ""
echo "!!!!!!!!!!   READ THIS   !!!!!!!!!!!"
echo "!!  0) a total of ${#SUBMITTEDJOBS[@]} batch jobs were submitted to generate the new reference files"
echo "!!  1) check the contents of the output directory once the batch jobs have finished:"
echo "!!      -> ${OUTPUTDIR}"
echo "!!  2) run the following script to copy the new reference files from the output directory to the RTT reference directory:"
echo "!!      -> ${COPYSCRIPT}"
echo "!!!!!!!!!!   READ THIS   !!!!!!!!!!!"
echo ""
chmod a+x ${COPYSCRIPT}
