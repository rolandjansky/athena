MAXPATTERNS	100000000
NUMBER_OF_SECTORS 0
ntracks		1500000000

# variables for efficiency calculation (doesn't affect actual pattern production)
eff_FLAG        0
etaMINlim       0
etaMAXlim       10

BARREL_ONLY     0
RAW_MODE	1
SECTOR_MODE	2
IBL_MODE        0
MERGE           0
TESTGCON        0
NUMBER_OF_PLANES	${L}
# maximum number of missing planes to accept patterns
MAX_MISSING_PLANES 0
# Allow this many missing SCT pairs (stereo + axial) to be counted as
# SINGLE missing planes. 
MAX_MISSING_SCT_PAIRS 0
# In the case of missing SCT pairs, make sure the outer 2 layers are empty
RESTRICT_SCT_PAIR_LAYER 0
# In the case of missing SCT pairs, if set, implies 1 for the previous
# option also. Checks that the outer 2 missing layers are in the
# barrel-disk transition region
RESTRICT_SCT_PAIR_MODULE 0

PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
RMAP_FILE       ${mapdir}/raw_${L}L.rmap
MODULE_LUT      ${mapdir}/raw_${L}L${Lv}.moduleidmap

SS_FILE         ${ssdir}/raw_${sslabel}.ss
# fraction by which ss are shifted. 0.5 means 1/2-ss shifted bank
SS_OFFSET       0.0

#FTKNOCHECK
# special flags for SCTtrk mode:
CURREG          ${i}
# set to 1 to disable region determination and always force it to be i (for all events)
FORCE_REGION    0
# set to 1 to perform 4L training using truth info to fill pseudolayer hits, rather than 8L fit results
TRKLOOKUP_FROM_TRUTH 0
USE_REGLOOKUP   0
USE_TRKLOOKUP   0
MAKE_REGLOOKUP  0
MAKE_SECLOOKUP  0
REGLOOKUP       ${lookupdir}/region_${dataset}.root
TRKLOOKUP       ${lookupdir}/trklookup_${dataset}_8L_reg${i}.root
SECLOOKUP       ${lookupdir}/sector_${dataset}_${L}L_reg${i}.root
#FTKCHECK

# Alberto's clustering
SECOND_CLUSTERING 	1
DIAG_CLUSTERING         1
SCT_CLUSTERING          0
DUPLICATE_GANGED        1
SPLIT_BLAYER_MODULES    0

# PDG code of training tracks
TRAIN_PDG       13
# Minimum pT cut (in GeV) for training truth tracks
PT_THRESHOLD    0.5
# Ignore hits in the overlap region
KILL_OVERLAP_HITS    0
# Ignore hits that don't come from training muons
IGNORE_NONTRAINING    0

NAMBANKS	${N}
NSUBS           ${M}
CURSUB          ${j}
CURREG          ${i}
WEDGE_MIN	${i}
WEDGE_MAX	${i}
HASHDIM		24
REALDATA	1

HWMODEID 0

DUMP_MODE       0
COMPRESS_DATA   0
DATAFILE_LIST	${inputdir}/inputprod_raw_${dataset}.list

STAT_STEP       10000
STAT_FILE1	${garbagedir}/statfile1_${run_name}.stat
STAT_FILE2	${garbagedir}/statfile2_${run_name}.stat
ROOT_OUT	${garbagedir}/statProva_${run_name}.root

# Paths to sectors*.patt files is filled in ftk.py:
