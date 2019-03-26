#!/bin/sh
# art-type: build
# art-ci: master


## READ OPTIONS
OPT_FOLLOW=0
OPT_DEBUG=""
OPT_EVT=10
OPT_THREADS=1
OPT_FILES_INPUT=""/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1""
OPT_JOBO="TrigUpgradeTest/met.menu.py"
OPT_DRY_RUN=0
OPT_LOG_LEVEL=""
OPT_INTERACTIVE=0

while [ ! $# -eq 0 ]
do
	case "$1" in
		# Set '-d' debug flag for athena
		-d )
			OPT_DEBUG="-d"
			;;

		# Set '--debug' flag for athena, follow with one of ('conf', 'init', 'exec', 'fini')
		# See `athena --help`
		--debug) 
			shift
			OPT_DEBUG="--debug ${1}"
			;;

		# Follow the output of the job with `less +F`
		--follow | -f)
			OPT_FOLLOW=1
			;;

		# Maximum number of events to run on
		# Sets the --evtMax option of athena
		--evtMax)
			shift
			OPT_EVT="$1"
			;;

		# Number of threads to run on
		# Set to >1 for multi-threaded
		--threads)
			shift
			OPT_THREADS="$1"
			;;

		# Path to job option
		--jobo | -j)
			shift
			OPT_JOBO="$1"
			;;

		# Print the command to be executed, then exit
		-n)
			OPT_DRY_RUN=1
			;;

		# Set --loglevel option of athena
		-l | --loglevel)
			shift
			OPT_LOG_LEVEL="--loglevel $1"
			;;

		# Interactive athena, equivalent to `athena -i`
		-i )
			OPT_INTERACTIVE=1
			;;

		# Comma separated list of files to run over\
		# Sets --filesInput option of athena
		--filesInput )
			shift
			OPT_FILES_INPUT=$1
			;;

		# Clear current directory before execution
		-o)
			rm ./*
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

STARDATE=`date +%g%m%d.%H%M`
RUN_LOG="${OPT_JOBO##*/}_${STARDATE}.log"

athena $ATH_OPT &> ${RUN_LOG} &
if [ ${OPT_FOLLOW} -eq 0 ]; then
	echo "Running in background. Log output to ${RUN_LOG}. "
	echo "Execute 'tail -f ${RUN_LOG}' to follow. "
else
	less +F "${RUN_LOG}"
fi
