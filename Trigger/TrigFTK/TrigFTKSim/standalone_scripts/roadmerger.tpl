# pmap file (needed with SCT_PAIR setting)
PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
# enable more aggressive version of roadwarrior that treats SCT planes in pairs
SCT_PAIR   0
# maximum number of missing planes to match a patterns
MAX_MISSING_PLANES    1
# if 0, RW removed roads are deleted from the bank
KEEP_REMOVED 0
# road warrior level: 0=none, 1=in-sector, 2=cross-sector, 3=overlap
ROADWARRIOR  0
# delete road files after they are not needed anymore (careful!)
DELETE_ROADS_AFTER_USE   0
# encode subregion information into the road (100*subid+regid)
# 0 means it's already been done at roadfinder level, so just copy it from there
ENCODE_SUBREGION  1
# output directory
OUTDIR  ${mergedir}/${run_name}
# number of the banks will be merged
NBANKS ${N}
# number of sub-regions in each bank
NSUBREGIONS ${M}
# list of regions to skip during the merging procedure
SKIPREGIONS ${!i}
# maximum number of events
NEVENTS ${nevents}

# the next line declare the region and subregion IDs 
# and the number of files. The files list is opened
# by the pattern bank path [FILLED BY ftk.py SCRIPT]
