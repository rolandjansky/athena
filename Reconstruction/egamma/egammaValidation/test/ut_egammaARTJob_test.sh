#!/bin/sh


# Reco stage
Reco_tf.py --steering doRAWtoALL --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/WorkflowReferences/master/q443/v1/myRDO.pool.root  --outputAODFile=Nightly_AOD.pool.root --maxEvents=1 --autoConfiguration="everything" --preExec="from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyRecoFlags; setRunEgammaOnlyRecoFlags()" --postInclude "RAWtoALL:egammaValidation/egammaArtCaloCalPostInclude.py" "POOLMergeAthenaMPAOD0:egammaValidation/egammaArtCaloCalPostInclude.py" >>/dev/null 2>&1

stat=$?
if [ $stat -eq 0 ] 
then
	echo "=== RECO TF SUCCESS === "
else
	echo "=== RECO TF FAILURE ==="
	echo " RAW TO ALL step log ===> "
	cat log.RAWtoALL
	exit $stat
fi
# rm files not needed anymore
rm log.RAWtoALL >> /dev/null 2>&1

# Merge stage
AODMerge_tf.py --inputAODFile=Nightly_AOD.pool.root --outputAOD_MRGFile=Nightly_AOD.pool.root --preExec "from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyMergeFlags; setRunEgammaOnlyMergeFlags()" --postInclude "all:egammaValidation/egammaArtCaloCalPostInclude.py">>/dev/null 2>&1

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
rm -f PoolFileCatalog.xml
ln -fs Nightly_AOD.pool.root Nightly_AOD_electron.pool.root
get_files -jo egamma_art_checker_joboptions.py >> /dev/null 2>&1
athena -c "particleType='electron'" egamma_art_checker_joboptions.py >> histo.log 2>&1
state=$?
rm -f PoolFileCatalog.xml
ln -fs Nightly_AOD.pool.root Nightly_AOD_gamma.pool.root
athena -c "particleType='gamma'" egamma_art_checker_joboptions.py >> histo.log 2>&1
statg=$?

if [ $state -eq 0 -a $statg -eq 0 ]
then
	echo "=== HISTO MAKER SUCCESS === "
else
	echo "==== HISTO MAKER FAILURE" $state $statg
	echo " HISTO MAKER  log ===> "
	cat  histo.log
	exit 1
fi


# rm files not needed anymore
rm -f Nightly_AOD*.pool.root >> /dev/null 2>&1
rm histo.log  >> /dev/null 2>&1


# Final plot stage 
EgammaARTmonitoring_plotsMaker.py Nightly-monitoring_electron.hist.root Nightly-monitoring_electron.hist.root electron >> plot.log 2>&1
state=$?
EgammaARTmonitoring_plotsMaker.py Nightly-monitoring_gamma.hist.root Nightly-monitoring_gamma.hist.root gamma >> plot.log 2>&1
statg=$?

if [ $state -eq 0 -a $statg -eq 0 ]
then
	echo "=== PLOT MAKER SUCCESS === "
else
	echo "=== PLOT MAKER FAILURE === " $state $statg
	echo " PLOT MAKER  log ===> "
	cat plot.log
	exit 1
fi

# rm files not needed anymore
rm *.png >> /dev/null 2>&1
rm Nightly-monitoring_*.hist.root >> /dev/null 2>&1
rm BN_ComparisonPlots_*.hist.root >>  /dev/null 2>&1
rm plot.log >>  /dev/null 2>&1

