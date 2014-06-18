# verbosity level
VERBOSITY 0
# plane-map
PMAP_FILE  ${mapdir}/raw_${L}L${Lv}.pmap
# chi2 cut for complete tracks
CHI2_CUT 17
# chi2 cut for majority tracks
CHI2_CUT5 14
# chi2 above which majority recovery is not attempted. -1 means always try.
CHI2_VETOMAJ -1
# Chi2 per degree of freedom cut
CHI2_DOF 4
# enable Hit-Warrior algorithm: 1=in-road(useful for CM architecture), 2=global(default)
HITWARRIOR 2
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
HW_NDIFF 3
# path of the files with the HW tollerances
HWSDEV_FILE  ${hwdir}/raw_${L}L${Lv}.hw
# maximum number of fit combinations per road
MAXNCOMB 10000
# maximum number of tracks per region per event
MAXTRKOUT 2000000
# do not attempt majority recovery in superstrips that have this many hits or more
NORECOVERY_NHITS -1
# number of event to process
NEVENTS  ${nevents}
# SCTtrk mode must be enabled in the 4-layer configuration (pix+SCTtrk)
SCTTRK_MODE     0
# output directory (usually overridden by job builder)
OUT_DIR  ${trackdir}/${run_name}
# 1 if there will an output file for each input file
MULT_OUT 1
# IBL mode?
IBL_MODE     0

# the stuff below specifies gcon banks and merged roads [FILLED by ftk.py SCRIPT]
