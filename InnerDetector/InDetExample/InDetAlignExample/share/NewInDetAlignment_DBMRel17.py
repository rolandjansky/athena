#
# This jobOption file allows to run a single standalone alignment job.
# It contains mostly used  of the steering jobOptions for setting up
# the alignment geometry, solving etc. Further jobOptions can be found
# by reading through
#   NewInDetAlignAlgSetup.py and
#   NewInDetAlignGeometrySetup.py.
#
# This is a good example for beginners to start with. It loads file
# loadInDetRec*.py for setting up the reconstruction options and file
# NewInDetAlignAlgSetup.py for the actual alignment processing.
#
# author: Daniel Kollar <daniel.kollar@cern.ch>
#
## ===================================================================
## ===================================================================
## ==                                                               ==
## ==                  Settings for the alignment                   ==
## ==                                                               ==
## message level for the alignment
## INFO, DEBUG, ...
outputLevel=DEBUG
#
## track collection to process
trackCollection = "Tracks"
#
#
##
## if readConstantsFromPool is set to true the initial alignment constants
## are read in from the pool file, otherwise they are taken from the DB
## according to the global tag
##
readConstantsFromPool = False
readSilicon = True
readTRT = False
#inputPoolFiles = [ "NewRun2Si_pool.root" ]
ErrorScaling = False
#
##
## write final alignment constants to pool file
writeConstantsToPool = True
writeSilicon = True
writeTRT = False
outputPoolFile = "alignment_output.pool.root"
tagSi = "IndetAlign_test"
tagTRT = "TRTAlign_test"
writeTextFiles = True
siliconTextFile = "OutputSiAlignment.txt"
trtTextFile = "OutputTRTAlignment.txt"
#
## write initial alignment constants into text file
writeOldConstants = True
#
##
## What to align?
alignInDet = True
alignSilicon = True
alignPixel = True
alignSCT = False
alignTRT = False

trtAlignmentLevel = 1
trtAlignmentLevelBarrel = -1
trtAlignmentLevelEndcaps = -1
#
## Pixel alignment
##   1 - L1
##   2 - L2
##   3 - L3
pixelAlignmentLevel = 2
pixelAlignDBM = True
# if pixelAlignDBM is configured to "True" the pixelAlignmentLevel is used for DBM alignment
# Note, only levels 1, 2, and 3 ar epossible. If set to "False" DBM is ignored (default)
pixelAlignmentLevelBarrel = -1
pixelAlignmentLevelEndcaps = -1

## Pixel alignment DoFs
pixelAlignEndcaps = True
## alignment parameters Endcap
pixelAlignEndcapX = True
pixelAlignEndcapY = True
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = True
pixelAlignEndcapRotY = True
pixelAlignEndcapRotZ = True


sctAlignmentLevel = 1
sctAlignmentLevelBarrel = -1
sctAlignmentLevelEndcaps = -1

#
## Silicon alignment
## -1 - use standalone Pixel and SCT setup
##  0 - L0
siAlignmentLevel = -1
#
## alignment parameters
siAlignX = True
siAlignY = True
siAlignZ = True
siAlignRotX = True
siAlignRotY = True
siAlignRotZ = True
#
## InDet alignment
## -1 - use standalone Silicon and TRT setup
##  0 - L0
indetAlignmentLevel = -1
#
## alignment parameters
indetAlignX = True
indetAlignY = True
indetAlignZ = True
indetAlignRotX = True
indetAlignRotY = True
indetAlignRotZ = True
#
## do module selection using identifiers
doModuleSelection = False
## list of identifiers for selected modules
moduleSelection = []
#
####################################################################
##
## Accumulate and Solving
##
runAccumulate = False
runSolving = True

#
## if we only run solving we need the list of files from
## which to accumulate
inputMatrixFiles = [ "matrix.bin" ]
inputVectorFiles = [ "vector.bin" ]
#
#####################################
####### Full Local Chi2 method ######
# by default the global method is run
# if you want to include the full local method
# set runLocal to True
runLocal = True
#
# for full local method one can also setup different kinds of residuals
# (will be ignored if runLocal==False)
# 0 - hit-only (biased) residuals
# 1 - unbiased
#residualType = 0
#
# for full local method one can also switch off refitting of tracks
# (will be ignored if runLocal==False)
#refitTracks = False
#
######################################
####### Full Global Chi2 method ######
## solving option
##   0 - No global solving
##   1 - Lapack
##   2 - Eigen
##   6 - ROOT
##   7 - CLHEP
solvingOption = 1
#
####
## run local solving
## this option is automatically set to True if runLocal is selected
solveLocal = False
#
## if diagonalization is set to false the matrix inversion is run
## only applies to CLHEP
runDiagonalization = True
## number of smallest eigenvalues (global movements) to cut
ModCut = 0
#softModeCut = 1
## cut on eigenvalue for diagonalization
eigenvalueCut = 0.
#
############################
###### common options ######
## minimum number of hits per module for solving
## is ignored if runAccumulate=False
minHits = 0
#
## options of the track refit needed to obtain the derivatives
## and the full covariance matrix of the track
##
## run outlier removal when refitting the track
runOutlier     = True
## particle hypothesis when refitting the track
##   0 - non-interacting
particleNumber = 0
#
## write special alignment ntuple
writeAlignNtuple = False
##
## store derivatives of residuals wrt. alignment parameters
## in the ntuple
writeDerivatives = False
##
## ==                                                               ==
## ==            End of settings for the alignment                  ==
## ==                                                               ==
## ===================================================================
## ===================================================================

## ===================================================================
## ===================================================================
## ==                                                               ==
## ==             Settings for the Reconstruction                   ==
## ==                                                               ==

# The following options are the defaults for configuring InnerDetector reconstruction.
# They may be changed by uncommenting the lines and specifying the appropriate input.
# ========================================

# ID Reconstruction  Options
# ==========================

detectorDescription = "ATLAS-IBL3D25-04-00-01"
realData = False
#globalTag = "OFLCOND-MC12-IBL-20-30-25"
globalTag = "OFLCOND-SDR-BS14T-IBL-CORR-06-02"

from IOVDbSvc.CondDB import conddb 
conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01') 

#doTrkNtuple = False
#doMonitoring = True
#siPoolFile = ""
siAlignmentTag = "InDetAlign_R2_Nominal"
#errorScalingTag = ""
#TRTCalibTextFile = ""
numberOfEvents = 10
import glob
inputFiles = glob.glob('/afs/cern.ch/work/m/mrudolph/public/singleMuon_minus_DBM_digits.pool.root')
print inputFiles

# Run collision reconstruction
# ==========================
# (Collision Specific Options)
#doReadBS = False
include("RecJobTransforms/UseFrontier.py")

include("InDetAlignExample/loadInDetRec_DBMRel17.py")

# Run Alignment
# ========================
include("InDetAlignExample/InDetAlignAlgSetup_DBM.py")

