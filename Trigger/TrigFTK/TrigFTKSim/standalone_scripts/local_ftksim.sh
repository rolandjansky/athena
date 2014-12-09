#!/bin/bash

##############################################################################
#
#
# Run FTK
#
#
##############################################################################

source ./local_ftksim_tool.sh

# form output
OUTPUTDIR=${OUTPUTPATH}/${OUTPUTNAME}

if [ ${BATCHMODE} ]; then
  setup
fi

summary

if [ 1 ]; then

  # Create output directory
  mkdir -p ${OUTPUTDIR}

  # General options
  QUEUE=normal
  SIMDIR=$DIR/../
  _PATT='patt.root'
  SAMPLE=${SAMPLE}
  BANKDIR=${BANKPOSITION}
  SS=15x16x36Ibl
  AMLABEL=30x32x72Ibl
  PMAP=8LcIbl123
  COMPLETEPMAP=12Libl

  # Extra options
  # TDAQTDRv2
  EXTRAOPTION=IGNORE_SIGSEGV
  EXTRAOPTION=${EXTRAOPTION},strict_error_check
  EXTRAOPTION=${EXTRAOPTION},patt_type=patterns
  EXTRAOPTION=${EXTRAOPTION},_PATT=${_PATT}
  EXTRAOPTION=${EXTRAOPTION},_GCON=gcon.bz2
  EXTRAOPTION=${EXTRAOPTION},MERGED_ROADS=0
  EXTRAOPTION=${EXTRAOPTION},N=64
  EXTRAOPTION=${EXTRAOPTION},SAVE_INCOMPLETE=1
  EXTRAOPTION=${EXTRAOPTION},MAX_MISSING_EXTRAPLANES=1
  EXTRAOPTION=${EXTRAOPTION},SS_FILE_COMPLETE=${SIMDIR}/scripts/${SSFILE}
  EXTRAOPTION=${EXTRAOPTION},MAKE_DIAGNOSTIC_PLOT=1
  EXTRAOPTION=${EXTRAOPTION},amlabel=${AMLABEL}
  EXTRAOPTION=${EXTRAOPTION},sslabel=${SS}

  EXTRAOPTION=${EXTRAOPTION},RESTRICT_SCT_PAIR_LAYER=1                                             # RestrictSctPairLayer
  EXTRAOPTION=${EXTRAOPTION},RESTRICT_SCT_PAIR_MODULE=1                                            # RestrictSctPairModule
  EXTRAOPTION=${EXTRAOPTION},MAX_MISSING_SCT_PAIRS=1                                               # MaxMissingSCTPairs
  EXTRAOPTION=${EXTRAOPTION},USETSP_MINCOVERAGE=0                                                  # TSPMinCoverage
  EXTRAOPTION=${EXTRAOPTION},RMAP_FILE=${SIMDIR}/config/map_file/raw_12Libl.tmap                   # rmap_path
  EXTRAOPTION=${EXTRAOPTION},RMAP_FILE=${SIMDIR}/config/map_file/raw_12Libl.tmap                   # rmap_path
  EXTRAOPTION=${EXTRAOPTION},PMAP_FILE=${SIMDIR}/scripts/${PMAPFILE}                               # pmap_path
  EXTRAOPTION=${EXTRAOPTION},SS_FILE=${SIMDIR}/config/ss_file/raw_${AMLABEL}.ss                    # ssmap_path
  EXTRAOPTION=${EXTRAOPTION},SS_FILE_INCOMPLETE=${SIMDIR}/config/ss_file/raw_${AMLABEL}.ss         # ssmap_path
  EXTRAOPTION=${EXTRAOPTION},SS_FILE_COMPLETE=${SIMDIR}/scripts/${SSFILE}                          # ssmapunused_path
  EXTRAOPTION=${EXTRAOPTION},SS_FILE_UNUSED=${SIMDIR}/scripts/${SSFILE}                            # ssmapunused_path
  EXTRAOPTION=${EXTRAOPTION},PMAP_FILE_UNUSED=${SIMDIR}/config/map_file/raw_${PMAP}_unused.pmap    # pmapunused_path
  EXTRAOPTION=${EXTRAOPTION},PMAP_FILE_COMPLETE=${SIMDIR}/config/map_file/raw_${COMPLETEPMAP}.pmap # pmapcomplete_path

  EXTRAOPTION=${EXTRAOPTION},MAXPATTERNS=${MAXPATTERNS}                                            # bankpatterns
  EXTRAOPTION=${EXTRAOPTION},USETSP_SIM=2                                                          # TSPSimulationLevel
  EXTRAOPTION=${EXTRAOPTION},USETSP_DBLEVEL=1                                                      # DBBankLevel
  EXTRAOPTION=${EXTRAOPTION},USETSP_BANK=1                                                         # UseTSPBank
  EXTRAOPTION=${EXTRAOPTION},SS_FILE_TSP=${SIMDIR}/config/ss_file/raw_${SS}.ss                     # ssmaptsp_path
  EXTRAOPTION=${EXTRAOPTION},NCONN=${NCONN}                                                        # SSFNConnections
  EXTRAOPTION=${EXTRAOPTION},MULTI_CONN_MODE=1                                                     # SSFMultiConnection
  EXTRAOPTION=${EXTRAOPTION},TRACKFITTER_MODE=3                                                    # TRACKFITTER_MODE
  EXTRAOPTION=${EXTRAOPTION},USETSP_SETAMSIZE=2                                                    # SetAMSize
  EXTRAOPTION=${EXTRAOPTION},HWSDEV_FILE=${SIMDIR}/scripts/${HWSDEV}                               # loadHWConf_path
  EXTRAOPTION=${EXTRAOPTION},MAX_TRANSITION_ETA=1.4                                                # SSFTRMaxEta
  EXTRAOPTION=${EXTRAOPTION},MIN_TRANSITION_ETA=1.0                                                # SSFTRMinEta
  EXTRAOPTION=${EXTRAOPTION},CHECK_TRANSITION_BY=1                                                 # SSFTRDefn
  EXTRAOPTION=${EXTRAOPTION},ALLOW_EXTRA_MISS_IN_SUPEREXP=1                                        # SSFAllowExtraMiss=1
  EXTRAOPTION=${EXTRAOPTION},IBL_MODE=1                                                            # IBLMode=1
  EXTRAOPTION=${EXTRAOPTION},PIXEL_CLUSTERING_MODE=1                                               # PixelClusteringMode=1
  EXTRAOPTION=${EXTRAOPTION},SCT_CLUSTERING=0                                                      # SctClustering=0
  EXTRAOPTION=${EXTRAOPTION},NCOORDS=${NCOORD}                                                     # SctClustering=0
  #EXTRAOPTION=${EXTRAOPTION},VERBOSITY=-10                                                         # Verbosity
  
  echo "Full list of extra options --"
  echo ${EXTRAOPTION}
  
  echo "Starting simulation"
  action=${ACTION}
  ./ftk.py \
  -n ${MAXEVENTS} \
  -m4 \
  -p \
  -r ${OUTPUTDIR} \
  -t ${BANKDIR} \
  -d raw_${PMAP}_${SS}_${SAMPLE} \
  -q ${QUEUE} \
  -a ${ACTION} \
  -o ${EXTRAOPTION}

fi

#eof
