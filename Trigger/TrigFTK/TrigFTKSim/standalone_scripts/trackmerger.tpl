# verbosity level
VERBOSITY 0
# number of banks to read
NBANKS	${N}
# set to 1 if merging output of tracks_merge/$reg/*.root, rather than tracks/$reg[/$sub]/*.root
INPUT_TREE_MERGED 0
# number of sub-regions in each bank
NSUBREGIONS ${M}
# Number of coordinates used in the fits
NCOORDS 16 
# plane-map
PMAP_FILE  ${mapdir}/raw_12Libl.pmap
# enable Hit-Warrior algorithm: 1=in-road(useful for CM architecture), 2=global(default)
HITWARRIOR 2
# >0 to keep rejected roads (1 HW rej, 2 bad chi2)
KEEP_REJECTED 0
# decide max number of different points
HW_NDIFF 6
# path of the files with the HW tollerances
HWSDEV_FILE  ${hwdir}/raw_${L}L${Lv}.hw
# number of event to process
NEVENTS  ${nevents}
# output directory (usually overridden by job builder)
OUT_DIR  ${trackdir}/${run_name}

# the stuff below specifies track files from each region [FILLED by ftk.py SCRIPT]
