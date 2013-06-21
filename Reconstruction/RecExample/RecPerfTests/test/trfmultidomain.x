#!/bin/bash

# Script for running multiple jobs with various domains enabled.
# Authors: David Rousseau & Thomas Kittelmann

###########################
#  Common configuration:  #
###########################

#1) The transform:
TRANSFORM=RAWtoESD_trf.py

#2) Input file: Choose input by uncommenting in next lines to pick either BS or RDO:

#7TeV collisions BS file:
INPUTISBS=1
INPUT=/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00182726.physics_JetTauEtmiss.merge.RAW._lb0215._SFO-5._0001.1_1000evts

#Various older inputs:
#INPUT=/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams/data11_7TeV.00178044.physics_JetTauEtmiss.merge.RAW._lb0092._SFO-4._0001.1
#/afs/cern.ch/atlas/project/rig/data/data10_7TeV.00165632.express_express.merge.RAW._lb0200._SFO-ALL._0001.1
#/afs/cern.ch/atlas/project/rig/data/data10_7TeV.00153565.physics_L1CaloEM.merge.RAW._lb0420._0001.1 (1727 events)
#(early minbias) INPUT=/afs/cern.ch/atlas/project/rig/data/data10_7TeV.00152166.physics_MinBias.merge.RAW._lb0206._0001.1
#Small cosmic (10 events)
#INPUTISBS=1
#INPUT="/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data"
#Big cosmic file (406 evts):
#INPUTISBS=1
#INPUT="/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/data09_cos.00122096.physics_IDCosmic.daq.RAW._lb0000._SFO-3._0497.data"
#Big RDO (250 evts)
#INPUTISBS=0
#INPUT="rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/RTTdata/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414/RDO.039414._00001.pool.root.1"

#3) Overall flags (excluding the input):
COMMONFLAGS="maxEvents=5 autoConfiguration=everything --athenaopts=--stdcmalloc"

#Special flag used to update the RecPerfTests xml:
PRODUCETESTXML=0

#Overall env var:
export ATHENA_PYCINTEX_MINVMEM=150

######################
#  Helper function:  #
######################

if [ $INPUTISBS == 1 ]; then
    TYPELOW="bs"
    TYPEUP="BS"
else
    TYPELOW="rdo"
    TYPEUP="RDO"
fi
COMMONFLAGS="input${TYPEUP}File=$INPUT $COMMONFLAGS"

RUNDIR=$PWD
LOGDIR=$RUNDIR/logs
function runjob {
    NAME=$1
    DOC=$2
    shift 2
    EXTRAARGS=$*
    if [ "x$DOC" == "x" ]; then
	DOC=$NAME
    fi
    DOC="RAWtoESD_trf.py on ${TYPEUP} running $DOC."

    if [ "x$PRODUCETESTXML" != "x1" ]; then
	JOBDIR=$RUNDIR/job_$NAME
	LOGFILE=$LOGDIR/rawtoesdtrf_$NAME.log
	mkdir -p $LOGDIR
	mkdir $JOBDIR && cd $JOBDIR
	echo "-------------------------------------------------------------------------"
	echo "Running job: '$NAME'"
	echo "   Command:    "$TRANSFORM $COMMONFLAGS $EXTRAARGS
	echo "   Description: $DOC"
	echo "   Running in: $PWD"
	echo "   Output log: $LOGFILE"
        time $TRANSFORM $COMMONFLAGS $EXTRAARGS 2>&1 > $LOGFILE
	EC=$?
	echo "   Exit Code: ${EC}"
	echo "-------------------------------------------------------------------------"
	echo
	cd $RUNDIR
    else
	RTTGROUP="TrfRecPerf${TYPEUP}"
	TRENDID="${NAME}_${TYPELOW}"
	echo "      <jobTransform trendId=\"$TRENDID\">"
	echo "        <doc>$DOC</doc>"
	echo "        <jobTransformJobName>$TRENDID</jobTransformJobName>"
	echo "        <jobTransformCmd>$TRANSFORM $COMMONFLAGS $EXTRAARGS</jobTransformCmd>"
	echo "        <group>TrfRecPerf${TYPEUP}</group>"
	echo "        <dataset_info>"
	echo "          <jobTransformData />"
	echo "          <datasetName>$INPUT</datasetName>"
	echo "          </dataset_info>"
	echo "        <queue>medium</queue>"
	echo "      </jobTransform>"
	echo
    fi
}

###############
#  The jobs:  #
###############

MONFLAGS="outputDQMonitorFile=myMon.root"
#NB: MUONCALIBFile and ZMUMUFile only on Muons stream and ZEEFile only on Egamma stream.
OUTPUTFLAGS="outputESDFile=myESD.pool.root outputNTUP_MUONCALIBFile=myNTUP_MUONCALIB.root outputDRAW_ZMUMUFile=myDRAW_ZMUMU.data outputDRAW_ZEEFile=myDRAW_ZEE.data"
PREEXEC_ALL="from@PerfMonComps.PerfMonFlags@import@jobproperties@as@pmjp,,pmjp.PerfMonFlags.doSemiDetailedMonitoringFullPrint=True"

#First 6 jobs to disentangle core,id,muon,calo:
runjob core nothing preExec=$PREEXEC_ALL,,rec.doCalo=False,,rec.doForwardDet=False,,rec.doInDet=False,,rec.doMuon=False,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doMuonCombined=False,,rec.doTau=False,,rec.doTrigger=False
runjob id '' preExec=$PREEXEC_ALL,,rec.doCalo=False,,rec.doForwardDet=False,,rec.doMuon=False,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doMuonCombined=False,,rec.doTau=False,,rec.doTrigger=False
runjob calo '' preExec=$PREEXEC_ALL,,rec.doInDet=False,,rec.doMuon=False,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doMuonCombined=False,,rec.doTau=False,,rec.doTrigger=False
runjob muon '' preExec=$PREEXEC_ALL,,rec.doCalo=False,,rec.doForwardDet=False,,rec.doInDet=False,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doMuonCombined=False,,rec.doTau=False,,rec.doTrigger=False
runjob idmuon '' preExec=$PREEXEC_ALL,,rec.doCalo=False,,rec.doForwardDet=False,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doMuonCombined=False,,rec.doTau=False,,rec.doTrigger=False
runjob idmuoncalo '' preExec=$PREEXEC_ALL,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doMuonCombined=False,,rec.doTau=False,,rec.doTrigger=False

#combined reco and egamma:
runjob idmuoncalomucomb '' preExec=$PREEXEC_ALL,,rec.doJetMissingETTag=False,,rec.doEgamma=False,,rec.doTau=False,,rec.doTrigger=False
runjob nojet idmuoncalomucombegamma preExec=$PREEXEC_ALL,,rec.doJetMissingETTag=False,,rec.doTau=False,,rec.doTrigger=False

#Jets/tau/etmiss/btagging. Not that it only makes sense to enable the next domains in the order: jet->btag->tau->etmiss:
NOETMISS='"recAlgs.doMissingET.set_Value_and_Lock(False)"'
runjob jet idmuoncalomucombegammajet preExec=$PREEXEC_ALL,,rec.doJetMissingETTag=True,,rec.doTau=False,,rec.doTrigger=False,,jobproperties.JetRecFlags.doBTagging=False,,$NOETMISS
runjob jetbtag idmuoncalomucombegammajetbtag preExec=$PREEXEC_ALL,,rec.doJetMissingETTag=True,,rec.doTau=False,,rec.doTrigger=False,,$NOETMISS
runjob jetbtagtau idmuoncalomucombegammajetbtagtau preExec=$PREEXEC_ALL,,rec.doJetMissingETTag=True,,rec.doTau=True,,rec.doTrigger=False,,$NOETMISS
runjob notrig idmuoncalomucombegammajetbtagtauetmiss preExec=$PREEXEC_ALL,,rec.doTrigger=False

#Special jobs with output/mon enabled before the trigger - to split up those domains (and to provide output/mon info when trigger fails to run):
runjob notrigmon idmuoncalomucombegammajetbtagtauetmissmon preExec=$PREEXEC_ALL,,rec.doTrigger=False $MONFLAGS
runjob notrigmonout idmuoncalomucombegammajetbtagtauetmissmonoutput preExec=$PREEXEC_ALL,,rec.doTrigger=False $MONFLAGS $OUTPUTFLAGS

#Enable the trigger in three steps: base/lvl2/event-filter:
runjob trigbase idmuoncalomucombegammajetbtagtauetmisstrigbase preExec=$PREEXEC_ALL,,rec.doTrigger=True,,TriggerFlags.doLVL1=True,,TriggerFlags.doLVL2=False,,TriggerFlags.doEF=False
runjob trigbaselvl2 idmuoncalomucombegammajetbtagtauetmisstrigbaselvl2 preExec=$PREEXEC_ALL,,rec.doTrigger=True,,TriggerFlags.doLVL1=True,,TriggerFlags.doEF=False
runjob trig idmuoncalomucombegammajetbtagtauetmisstrigbaselvl2ef preExec=$PREEXEC_ALL

#Finally enable monitoring and output writing in that order:
runjob trigmon idmuoncalomucombegammajetbtagtauetmisstrigbaselvl2efmon preExec=$PREEXEC_ALL $MONFLAGS
runjob all idmuoncalomucombegammajetbtagtauetmisstrigbaselvl2efmonout preExec=$PREEXEC_ALL $MONFLAGS $OUTPUTFLAGS

