# Choose one. Careful - it can eat your banks and not say thank you!
SECTOR_MODE    0
PATTGEN_MODE   0
TRACK_MODE     0

# Main map files to be used for pattern / sector generation
PMAP_FILE ${mapdir}/raw_${L}L${Lv}.pmap
RMAP_FILE ${mapdir}/raw_${L}L.rmap
SS_FILE ${ssdir}/raw_${sslabel}.ss

# fraction by which ss are shifted. 0.5 means 1/2-ss shifted bank
SS_OFFSET 0.0

# If REGIONS_11L==1, region is determined based on 11L rmap
REGIONS_11L 0
PMAP_FILE11 ${mapdir}/raw_11L.pmap
RMAP_FILE11 ${mapdir}/raw_11L.rmap

# chi2 cut for single muon tracks that will be dumped (for TRACK_MODE only)
CHI2_CUT 14

CUR_REGION  ${i}
NCOORDS    ${ncoords}
NSUBS      ${M}

RAW_MODE 1
BARREL_ONLY 0
VERBOSITY 0

# Alberto's clustering
SECOND_CLUSTERING 1
DIAG_CLUSTERING 1
SCT_CLUSTERING          0
# These have to be disabled for single muon mode:
DUPLICATE_GANGED 0
GANGED_PATTERN_RECOGNITION 0

events# ${nevents}
FTKDAT_LIST     ${inputdir}/inputprod_raw_${dataset}.list

OUT_FILE     ${lookupdir}/trklookup_${dataset}_${L}L_reg${i}.root

# Paths to bank files is filled in ftk.py:
