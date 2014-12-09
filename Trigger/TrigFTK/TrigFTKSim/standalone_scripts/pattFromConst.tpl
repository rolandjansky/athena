MAXPATTERNS	100000000
NUMBER_OF_SECTORS 0
ntracks		1500000000

# variables for efficiency calculation
eff_FLAG        0
etaMINlim       0
etaMAXlim       10

IBL_MODE        1

BARREL_ONLY     0
RAW_MODE	1
SECTOR_MODE	2
#FlagAA: 2010-03-30 use RANDOM_MODE=1 by default
RANDOM_MODE     1 
MERGE           0
TESTGCON        0
NUMBER_OF_PLANES	${L}
# may need to add .ibl4 for ibl
PMAP_FILE       ${mapdir}/raw_${L}L${Lv}.pmap
RMAP_FILE       ${mapdir}/raw_${L}L.rmap
MODULE_LUT      ${mapdir}/raw_${L}L${Lv}.moduleidmap
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

SS_FILE         ${ssdir}/raw_${sslabel}.ss
# fraction by which ss are shifted. 0.5 means 1/2-ss shifted bank
SS_OFFSET       0.0

#FTKNOCHECK
# special flags for SCTtrk mode:
CURREG          ${i}
# set to 1 to disable region determination and always force it to be CURREG (for all events)
FORCE_REGION    0
# set to 1 to perform 4L training using truth info to fill pseudolayer hits, rather than 8L fit results
TRKLOOKUP_FROM_TRUTH 0
USE_REGLOOKUP   0
USE_TRKLOOKUP   0
REGLOOKUP       ${lookupdir}/region_${dataset}.root
TRKLOOKUP       ${lookupdir}/trklookup_${dataset}_8L_reg${i}.root
#FTKCHECK

# Alberto's clustering
SECOND_CLUSTERING 	1
DIAG_CLUSTERING         1
SCT_CLUSTERING          0
DUPLICATE_GANGED        1

# PDG code of training tracks
TRAIN_PDG       13
# Ignore hits in the overlap region
KILL_OVERLAP_HITS    0

NAMBANKS	${N}
NSUBS           1
CURSUB          0
CURREG          ${i}
WEDGE_MIN	${i}
WEDGE_MAX	${i}
HASHDIM		24
REALDATA	1

HWMODEID 0

# patterns-from-constants sector slice file
#FTKNOCHECK
SLICE_FILE       ${gcondir}/slices_${L}L_reg${i}.root
#FTKCHECK

# sector slice binning in the slices.root file - DO NOT CHANGE
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

# patterns-from-constants parameter generation limits
# use this to adjust parameter ranges
PTMIN   800
#ETAMIN  -1.2
#ETAMAX  1.2
#ETAMIN  -2.8
#ETAMAX  2.8
SPOTR   2.2
SPOTZ   120
XVERT   0
YVERT   0
ZVERT   0

# keyword to enable a limitation in the number of sectors to use for pattern generation in a bank
MAXSECTORID -1

# number of trials in patt-from-const generation
MAX_GEN_TRACKS   ${nevents}
# an explicit chi2 cut on patt-from-const tracks. 0 means disabled
PCONST_MAXCHI2   0
# overlap removal in patt-from-const: 0=disabled, 1=barrel-only(old default), 2=everywhere (thx Joe!)
PCONST_OVERLAP   2
# random seed for patt-from-const (0=bootstrap from system time)
PCONST_RANDSEED   0

# patterns-from-constants bootstrapping 
# if PC_MODE=0, run default patt-from-const
# if PC_MODE=1, bootstrap 8L/4L patt-from-const from 11L patt-from-const, using truth 8L pars in 4L generation
# if PC_MODE=2, same as above, but using the actual fitted 8L pars in 4L generation, instead of truth

PC_MODE       0
PC_NPLANES    4
#FTKNOCHECK
# 8L or 4L sector file (nlayers in final patterns)
PC_SECFILE     ${patdir}/sectors_raw_${PC_NPLANES}L_reg${i}.patt
# for PC_MODE=2 and PC_NPLANES=4
PC_SS4L	       20x150I150x24
PC_SSMAP       ${ssdir}/raw_${PC_SS4L}.ss
PC_PMAP        ${mapdir}/raw_4L.pmap
PC_RMAP        ${mapdir}/raw_4L.rmap
PC_NPLANESTRK     8
PC_SECFILETRK     ${patdir}/sectors_raw_8L_reg${i}.patt
PC_GCONFILETRK    ${gcondir}/corrgen_raw_8L_reg${i}.gcon
#FTKCHECK

DUMP_MODE       0
COMPRESS_DATA   0
DATAFILE_LIST	${inputdir}/inputprod_raw_${dataset}.list

STAT_STEP       10000
STAT_FILE1	${garbagedir}/statfile1_${run_name}.stat
STAT_FILE2	${garbagedir}/statfile2_${run_name}.stat
ROOT_OUT	${garbagedir}/statProva_${run_name}.root

# Paths to sectors*.patt files is filled in ftk.py:
