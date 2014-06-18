BARREL_ONLY     0
RAW_MODE	1
IBL_MODE        0

PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
RMAP_FILE       ${mapdir}/raw_${L}L.rmap
PE2S_FILE       ${patdir}/sectors_raw_${L}L_reg${i}.patt
MAPDIM          3

COMPRESS_DATA   0
DATAFILE_LIST	${inputdir}/inputprod_raw_${dataset}.list

#FTKNOCHECK
# special flags for SCTtrk mode:
CURREG          ${i}
# set to 1 to disable region determination and always force it to be i (for all events)
FORCE_REGION    0
# set to 1 to perform 4L training using truth info to fill pseudolayer hits, rather than 8L fit results
TRKLOOKUP_FROM_TRUTH 0
USE_REGLOOKUP   0
USE_TRKLOOKUP   0
REGLOOKUP       ${lookupdir}/region_${dataset}.root
TRKLOOKUP       ${lookupdir}/trklookup_${dataset}_8L_reg${i}.root
#FTKCHECK

#FTKNOCHECK
GCON_FILE        ${gcondir}/corrgen_raw_${L}L_reg${i}.gcon
SLICE_FILE       ${gcondir}/slices_${L}L_reg${i}.root
#FTKCHECK

# sector slice binning in the slcies.root file - DO NOT CHANGE
PAR_C_SLICES	250
PAR_C_MAX	7E-4
PAR_C_MIN	-7E-4
PAR_D_SLICES	250
PAR_D_MAX	2.2
PAR_D_MIN	-2.2
# phi is in units of pi, NOT in radians!
PAR_PHI_SLICES	100
PAR_PHI_MAX	1
PAR_PHI_MIN	-1
PAR_Z0_SLICES	100
PAR_Z0_MAX	120
PAR_Z0_MIN	-120
PAR_ETA_SLICES	100
PAR_ETA_MAX	3.0
PAR_ETA_MIN	-3.0

CHANGE_ORIGIN    1

# Alberto's clustering
SECOND_CLUSTERING 	1
DIAG_CLUSTERING         1
SCT_CLUSTERING          0
DUPLICATE_GANGED        1
SPLIT_BLAYER_MODULES    0
PIXEL_CLUSTERING_MODE   0
# 0 is default used until mid 2013
# 1 includes correction for 400/600um pixels and ToT imbalance used for better centroid

# PDG code of training tracks
TRAIN_PDG       13
# Minimum pT cut (in GeV) for training truth tracks
PT_THRESHOLD    0.5
# Ignore hits in the overlap region
KILL_OVERLAP_HITS    0
# Minimum number of tracks in a good sector (usually not needed if you filter sectors)
MIN_TRACK_SEC   5

DIM 2

# dump, for each recostructed track an ntuple in ascii, !!!
# VERY VERBOSE, use only with scritp that filter output
ASCIINTP 0
ROOTNTP 1

# read in existing constants
CHECK_MODE 0
# number of events in the check loop (works with both values of check mode!)
CHECK_LOOP_NEVENTS 2000000
