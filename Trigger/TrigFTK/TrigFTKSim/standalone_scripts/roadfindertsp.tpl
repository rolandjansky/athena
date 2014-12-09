PE2S_FILE	${patdir}/sectors_raw_${L}L_${M}M_reg${i}_sub${j}.${_PATT}
PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
PMAP_FILE_UNUSED       ${mapdir}/raw_${L}L${Lv}_unused.pmap
RMAP_FILE       ${mapdir}/raw_${L}L.rmap3
MODULE_LUT ${mapdir}/raw_${L}L${Lv}.moduleidmap
BADMOD_FILE	${confdir}/bad_modules/${BMAP}.bmap
BADMOD_FILE_FOR_HIT	${confdir}/bad_modules/${BMAP_FOR_HIT}.bmap

REGIONAL_WRAPPER 0
RAW_MODE 	2
BARREL_ONLY     0
ENABLE_FTKSIM   1
TIMING_OUT	1
SCTTRK_MODE     0

# USETSP_BANK is 0 if ASCII bank format is used, 1 id SQLite3 Db bank are used
USETSP_BANK 1
# Set the level of simulation, used only if USETSP_BANK=1:
# 0: standard AM simulation, but patterns are read from db file
# 1: standard AM simulation, the AM patterns are selected applying a cut on TSP level
# 2: applying the TSP selection the AM is generated and the DC bit are used
# 3: AM and TSP simulations are performed
USETSP_SIM 2
# if USETSP_SIM=0 it is possibile to decide to load TSP patterns (1), or an AM level (2).
USETSP_DBLEVEL 1
# DC Bit Resplit
# 0: No DC Split is performed, maximum grouping of thin patterns into AM patterns is done
# 1: Use at most 1 DC bit for AM patterns after split
# 2: Use at most 2 DC bit for AM patterns after split
# ...
# 99: Use at most 99 DC bit for AM patterns after split
# 100: Complete-Linkage Clustering algorithm
DCBITS_SPLIT 0
# If the minimum coverage is >0 the AM bank is created using all
# the TSP pattern with at least the asked coverage. Then the AM bank
# is cutted as set by the loading parameter requirement
USETSP_MINCOVERAGE 0
# If USETSP_SETAMSIZE is 0 the user sets the number of TSP patterns
# to load, the AM size can change in each tower
# Using 1 or 2 the AM size is instead fixed and the TSP bank is read until
# this size is obtained (1) or before the required size is exceeded (2)
USETSP_SETAMSIZE 0

#FTKNOCHECK
# SCTtrk mode must be enabled in the 4-layer configuration (pix+SCTtrk)
SCTTRK_MODE     0
CUR_REGION    ${i}
CUR_SUBREGION ${j}
# IBL mode?
IBL_MODE     0
# number of subregions
NSUBREGIONS  ${M}
# list of 8L tracks for this region
SCTTRK_TRACKLIST	${otherdir}/${dataset}_tracks_reg${i}_sub${j8L}.list
# list of 8L merged roads for this region
SCTTRK_ROADLIST		${otherdir}/${dataset}_roads_reg${i}_sub${j8L}.list
# a three-column map between [sec4L,sec8L] -> sec11L
SCTTRK_SECTORMAP   ${lookupdir}/sectors_4_8_11_reg${i}.${_DAT}
# number of layers in the final road (after putting pixels and sct back together)
SCTTRK_NLAYERS  11
# number of subregions in the 8L stage [HARDCODED - change if needed]
SCTTRK_NSUBS  16
# index of the SCTtrk layer in 4L configuration
SCTTRK_LASTLAYER 3
# The following is the maximum number of combined missing SCT+PIX hits
# allowed. In the case of split architecture, this is applied after
# pixel roads have already been found. MAX_MISSING_PLANES determines
# the number of missing pixel hits allowed in that step.
SCTTRK_MAX_MISSING_PLANES 1
#FTKCHECK

# maximum number of missing planes to match patterns
# This variable is applied separately at each stage of road finding (in the 
# case of split architecture).
MAX_MISSING_PLANES    1
# It possible to discard in hits in particular logical layers, set accordin
# the following binary mask
MASKLAYERS 0
# Allow this many missing SCT pairs (stereo + axial) to be counted as
# SINGLE missing planes. Must be <= MAX_MISSING_PLANES in the SCT
# stage of split fitting, and must be <= SCTTRK_MAX_MISSING_PLANES in
# the second stage (if applicable).
MAX_MISSING_SCT_PAIRS 0
# In the case of missing SCT pairs, make sure the outer 2 layers are empty
RESTRICT_SCT_PAIR_LAYER 0
# In the case of missing SCT pairs, if set, implies 1 for the previous
# option also. Checks that the outer 2 missing layers are in the
# barrel-disk transition region
RESTRICT_SCT_PAIR_MODULE 0

# road warrior level: 0=none, 1=in-sector, 2=cross-sector, 3=overlap
ROADWARRIOR 	0
# make roadwarrior more aggressive by pairing adjacent SCT planes
SCT_PAIR        0
# Require a hit in the first layer (pixel B-layer). This can also be applied in track fitting.
REQUIRE_FIRST 0
# Use STL hashmap internally in AM - slower, but supports arbitrarily narrow superstrips
FORCE_AM_HASHMAP 1
# if 0, RW removed roads are deleted from the bank
KEEP_REMOVED 0
# encode subregion information into the road (100*subid+regid).
# 0 means disabled, otherwise it gives the value for subid
ENCODE_SUBREGION  0
# If assume to use sectors in place of patterns, useful for sectors' debugging
SECTORSASPATTERNS 0

VERBOSITY       0

# if 1 the offline clusters are used instead of the raw channels
READ_CLUSTERS 0
# enables the FTK clustering algorithm
CLUSTERING 	1
SAVE_CLUSTER_CONTENT 0
DIAG_CLUSTERING         1
SCT_CLUSTERING          0			
DUPLICATE_GANGED        1
GANGED_PATTERN_RECOGNITION      1
SPLIT_BLAYER_MODULES      0
PIXEL_CLUSTERING_MODE   0
# 0 is default used until mid 2013
# 1 includes correction for 400/600um pixels and ToT imbalance used for better centroid

SS_FILE         ${ssdir}/raw_${amlabel}.ss
SS_FILE_TSP         ${ssdir}/raw_${sslabel}.ss
SS_FILE_UNUSED         ${ssdir}/raw_${sslabel}unused.ss
# fraction by which ss are shifted. 0.5 means 1/2-ss shifted bank
SS_OFFSET       0.0

FTKDAT_LIST     ${inputdir}/inputprod_raw_${dataset}.list
events#		${nevents}

OUT_DIR         ${roaddir}/${run_name}
MULT_OUT 	1

# if 1 the job dumps the AM bank used
MAKECACHE 0
# the cache patt is used as template, the Id of the bank
# is added before the extension
CACHEPATH	${patdir}/patterns_raw_${L}L_${sslabel}_${M}M_reg${i}_sub${j}.pcache.root

# Don't print stacktrace on crash (recommended on the grid!)
IGNORE_SIGSEGV  0

# if enabled a statistic for the patterns is collected and dumped at the end
BANK_STAT 0
# store all the SS and not only the SS attached to roads
DUMPALLSS 0

# the following value is 1 if the BANK has to be read as a cached version of an existing bank
CACHEDBANK 0
# Pattern banks [ FILLED IN ftk.py SCRIPT ]
