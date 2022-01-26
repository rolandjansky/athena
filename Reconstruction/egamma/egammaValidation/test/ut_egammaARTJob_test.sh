#!/bin/sh


# Reco stage
Reco_tf.py --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/WorkflowReferences/master/q443/v1/myRDO.pool.root  --outputAODFile=Nightly_AOD_electron.pool.root --maxEvents=1 --autoConfiguration="everything" --preExec="from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyRecoFlags; setRunEgammaOnlyRecoFlags()" --postInclude "ESDtoAOD:egammaValidation/egammaArtCaloCalPostInclude.py" "POOLMergeAthenaMPAOD0:egammaValidation/egammaArtCaloCalPostInclude.py" >>/dev/null 2>&1 

stat=$?
if [ $stat -eq 0 ] 
then
	echo "=== RECO TF SUCCESS === "
else
	echo "=== RECO TF FAILURE ==="
	echo " RAW TO ESD step log ===> "
	cat log.RAWtoESD
	echo " ESD TO AOD step log ===> "
	cat log.ESDtoAOD
	exit $stat
fi
# rm files not needed anymore
rm tmp.ESD >> /dev/null 2>&1
rm log.RAWtoESD >> /dev/null 2>&1
rm log.ESDtoAOD >> /dev/null 2>&1


# Merge stage
AODMerge_tf.py --inputAODFile=Nightly_AOD_electron.pool.root --outputAOD_MRGFile=Nightly_AOD_electron.pool.root --preExec "from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyMergeFlags; setRunEgammaOnlyMergeFlags()" --postInclude "all:egammaValidation/egammaArtCaloCalPostInclude.py">>/dev/null 2>&1 

stat=$?
if [ $stat -eq 0 ] 
then
	echo "=== MERGE SUCCESS ==== "
else
	echo "==== MERGE FAILURE"
	echo " AOD MERGE  log ===> "
	cat log.AODMerge
	exit $stat
fi
# rm files not needed anymore
rm log.AODMerge >> /dev/null 2>&1


# Histo stage 
get_files -jo egamma_art_checker_joboptions.py >> /dev/null 2>&1
athena -c "particleType='electron'" egamma_art_checker_joboptions.py >> histo.log 2>&1

stat=$?
if [ $stat -eq 0 ] 
then
	echo "=== HISTO MAKER SUCCESS === "
else
	echo "==== HISTO MAKER FAILURE"
	echo " HISTO MAKER  log ===> "
	cat  histo.log
	exit $stat
fi


# rm files not needed anymore
rm Nightly_AOD_electron.pool.root >> /dev/null 2>&1
rm histo.log  >> /dev/null 2>&1


# Final plot stage 
EgammaARTmonitoring_plotsMaker.py Nightly-monitoring_electron.hist.root Nightly-monitoring_electron.hist.root electron >> plot.log 2>&1

stat=$?
if [ $stat -eq 0 ] 
then
	echo "=== PLOT MAKER SUCCESS === "
else
	echo "=== PLOT MAKER SUCCESS === "
	echo " PLOT MAKER  log ===> "
	cat plot.log
	exit $stat
fi

# rm files not needed anymore
rm *.png >> /dev/null 2>&1
rm Nightly-monitoring_electron.hist.root >> /dev/null 2>&1
rm BN_ComparisonPlots_electron.hist.root >>  /dev/null 2>&1
rm plot.log >>  /dev/null 2>&1

