#!/bin/sh
# art-type: build
# art-ci: master

STARDATE=`date +%g%m%d.%H%M`
RUN_LOG="run_${STARDATE}.log"


## READ OPTIONS
OPT_FOLLOW=0
OPT_DEBUG=""
OPT_EVT=10
OPT_THREADS=1
OPT_FILES_INPUT=""/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1""
OPT_JOBO="TrigEFMissingET/testMETCluster.py"
OPT_DRY_RUN=0
OPT_LOG_LEVEL=""
OPT_INTERACTIVE=0

while [ ! $# -eq 0 ]
do
	case "$1" in
		-d )
			OPT_DEBUG="-d"
			;;
		--debug)
			shift
			OPT_DEBUG="--debug ${1}"
			;;
		--follow | -f)
			OPT_FOLLOW=1
			;;
		--evtMax)
			shift
			OPT_EVT="$1"
			;;
		--threads)
			shift
			OPT_THREADS="$1"
			;;
		--jobo | -j)
			shift
			OPT_JOBO="$1"
			;;
		-n)
			OPT_DRY_RUN=1
			;;
		-l | --loglevel)
			shift
			OPT_LOG_LEVEL="--loglevel $1"
			;;
		-i )
			OPT_INTERACTIVE=1
			;;
		--filesInput )
			shift
			OPT_FILES_INPUT=$1
			;;
	esac
	shift
done


## BUILD ATHENA COMMAND
ATH_OPT="--filesInput=${OPT_FILES_INPUT} --threads=${OPT_THREADS} --evtMax=${OPT_EVT}"
ATH_OPT="${OPT_DEBUG} ${ATH_OPT} ${OPT_LOG_LEVEL}"
ATH_OPT="${ATH_OPT} ${OPT_JOBO}"

## EXECUTE AND FOLLOW LOG
echo "Executing..."
echo "athena ${ATH_OPT}"

if [ "${OPT_DEBUG}" != "" ]; then
	athena ${ATH_OPT}
	exit
elif [ $OPT_INTERACTIVE -eq 1 ]; then
    athena -i ${ATH_OPT}
    exit
elif [ $OPT_DRY_RUN -eq 1 ];then
	exit
fi

athena $ATH_OPT &> ${RUN_LOG} &
if [ ${OPT_FOLLOW} -eq 0 ]; then
	echo "Running in background. Log output to ${RUN_LOG}. "
	echo "Execute 'tail -f ${RUN_LOG}' to follow. "
else
	less +F "${RUN_LOG}"
fi
