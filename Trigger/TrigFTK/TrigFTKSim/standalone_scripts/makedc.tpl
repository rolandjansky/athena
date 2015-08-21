PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
RMAP_FILE       ${mapdir}/raw_${L}L.rmap

#CUR_REGION 0
#CUR_SUBREGION 0
## number of subregions
#NSUBREGIONS 16

CUR_REGION ${i}
NSUBREGIONS ${M}
CUR_SUBREGION ${j}


# encode subregion information into the road (100*subid+regid).
# -1 means disabled, otherwise it gives the value for subid
ENCODE_SUBREGION 0

VERBOSITY 0

# 
#HWMODEID 2

SS_FILE_AM         ${ssdir}/raw_${amlabel}.ss
SS_FILE_TSP         ${ssdir}/raw_${sslabel}.ss

# if ROOTBANK is set to 1 the ROOT format bank is produced instead of the 
ROOTBANK 1
# the AM conversion can ignore TSP pattern with small coverage
MINCOVERAGE 2

# It is possible to remove the original TSP bank from the output file, value >0
REMOVETSP 0

# set the SS calculation method, 1 is used for the Vertical Slice SS format, 2 for the final HW format
HWMODEID 0

# Pattern banks to convert
# The following lines will be added by ftk.py
