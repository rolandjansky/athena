# pmap file (needed with SCT_PAIR setting)
PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
# if 0 HF removed roads are deleted from the bank
KEEP_REMOVED 0
# maximum # of roads in shifted bank that are allowed to match a given road in default bank
HF_MAX_MATCHES 50
# apply additional topological road matching criteria (broken, do NOT use)
HF_TOPO_MATCHES 0
# output directory
OUTDIR  ${filterdir}/${run_name}
# number of the banks will be merged
NBANKS ${N}
# number of sub-regions in each bank
NGROUPS 2
# list of regions to skip during the merging procedure
SKIPREGIONS ${!i}
# maximum number of events
NEVENTS ${nevents}

# the next line declare the region and group IDs 
# and the number of files. The files list is opened
# by the pattern bank path [FILLED BY ftk.py SCRIPT]

