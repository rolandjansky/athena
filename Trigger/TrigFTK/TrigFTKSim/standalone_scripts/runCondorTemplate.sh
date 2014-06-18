#!/bin/bash                                                                                                                                    
echo "Starting my condor job"

#
#  Configurables for the job
#

rundir=/home/johnda/FTK/BankProduction/UpdateToCondor/baseG18/base8Lc/run/raw_8Lc_50x64x144_constG18muons/pattgen/63
app=/share/t3data/ftkdata/ftkdata/johnda/BankProduction/UpdateToCondor/Trigger/TrigFTK/TrigFTKSim/scripts/ftk.py
fin=FIN
EXTRA_ARGS=
FTK_ENV_SETUP=FTKSETUP


#
#  Set the enviroment
#

echo "Setting environment variables..."
echo "Exporting environment:"
cat ${FTK_ENV_SETUP}
source ${FTK_ENV_SETUP}

#echo "Submitting on ${PBS_O_HOST}"
echo "Running on ${HOSTNAME} in ${rundir}"
#echo "Arguments: $@"
echo "ROOTSYS = ${ROOTSYS}"
echo "App = ${app}"
echo "Fin = ${fin}"
echo "EXTRA_ARGS = ${EXTRA_ARGS}"
echo "PWD = ${PWD}"
echo "PATH = ${PATH}"
echo "LD_LIBRARY_PATH = ${LD_LIBRARY_PATH}"
echo "PYTHONPATH = ${PYTHONPATH}"



cd ${rundir}
delay=1
if [ "${delay}" ]; then
    echo "Sleeping for ${delay} seconds..."
    date
    sleep ${delay}
fi;
echo 'Running:' ${app} ${fin}  ${EXTRA_ARGS}
time ${app} ${fin} ${EXTRA_ARGS}
cd - 
echo "DONE!"



