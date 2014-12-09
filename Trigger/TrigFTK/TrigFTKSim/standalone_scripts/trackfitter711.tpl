# verbosity level
VERBOSITY 0
# plane-map HARDCODE, TO CHANGE
PMAP_FILE_INCOMPLETE  ${mapdir}/raw_${L}L${Lv}.pmap
#PMAP_FILE_COMPLETE  ${mapdir}/raw_11L.pmap
PMAP_FILE_COMPLETE  ${mapdir}/raw_12Libl.pmap
RMAP_FILE       	${mapdir}/raw_${L}L.rmap
SS_FILE_INCOMPLETE         ${ssdir}/raw_${amlabel}.ss
SS_FILE_COMPLETE         ${ssdir}/raw_${sslabel}unused.ss

# decide the track fitter work: 0 use synched sectors, 1 uses the connection files, 2 resolution mode
TRACKFITTER_MODE ${TRACKFITTER_MODE}
# multiple connection option
MULTI_CONN_MODE 1
# use up to N connections
NCONN 4
# set the algorithm used to extrapolate the stereo layers,
# 1 use majority logic algorithm (default or not set), 0 geometric extrapolation
USEGUESSING 1
# chi2 cut for complete tracks
CHI2_CUT 17
# chi2 cut for majority tracks
CHI2_CUT5 14
# chi2 above which majority recovery is not attempted. -1 means always try.
CHI2_VETOMAJ -1
# Chi2 per degree of freedom cut
CHI2_DOF 4
# Maximum number of missing layers to accept a track. 
MAX_MISSING_EXTRAPLANES    1
# If 0 the second fitting step is skipped, only the extrapolation
# in the layers ingored in the RF is performed.
EXTRAFITS 1
# enable Hit-Warrior algorithm: 1=in-road(useful for CM architecture), 2=global(default)
HITWARRIOR 2
# if >0 save the track in the intermediate step
SAVE_INCOMPLETE 0
# >0 to keep rejected roads (1 HW rej, 2 bad quality)
KEEP_REJECTED 0
# fit roads removed by 1 - in-sector RW, 2 - cross-sector RW, 3 - overlap RW
FIT_REMOVED 0
# Do majority fits? 0 - Never, 1 - for all failed full fits, 2 - only if all full fits in a road fail
DO_MAJORITY 1
# Output only one track per road?
ONE_PER_ROAD 0
# Require a hit in the first layer (pixel B-layer). Improves d0 resolution (for b-tagging).
REQUIRE_FIRST 0
# decide max number of different points
HW_NDIFF 6
# path of the files with the HW tollerances
#HWSDEV_FILE  ${hwdir}/raw_11L.hw
HWSDEV_FILE  ${hwdir}/raw_12L.hw
# maximum number of fit combinations per road
MAXNCOMB 1000000
# maximum number of tracks per region per event
MAXTRKOUT 20000000
# do not attempt majority recovery in superstrips that have this many hits or more
NORECOVERY_NHITS 1000
# number of event to process
NEVENTS  ${nevents}
# output directory, in single file-name mode use OUT_FILE
OUT_DIR  ${trackdir}/${run_name}
# 1 if there will an output file for each input file
MULT_OUT 1
# IBL?
IBL_MODE 1
# Allow extra miss for TRACKFITTER_MODE=3 (i.e. super extrapolate)
ALLOW_EXTRA_MISS_IN_SUPEREXP 1
# Check if a track is in transition region or not by checking the moduleID of last SCT layer (option = 0) or the eta of 1st stage track (option = 1)
CHECK_TRANSITION_BY 1
# Allow extra miss for TRACKFITTER_MODE=3 (i.e. super extrapolate)
MAX_TRANSITION_ETA 1.4
# Allow extra miss for TRACKFITTER_MODE=3 (i.e. super extrapolate)
MIN_TRANSITION_ETA 1.0
# Make additional diagnostic plots
MAKE_DIAGNOSTIC_PLOT 0

# the stuff below specifies gcon banks and merged roads [FILLED by ftk.py SCRIPT]
