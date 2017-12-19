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
## ==          Parse some inputs to make running simpler            ==
## ==                                                               ==
if 'eventType' in dir():
    if eventType=="cosmics":
        print "Config: Alignment is performed on Cosmics: "


        Cosmics = True
    elif eventType=="collisions":
        print "Config: Alignment is performed on Collisions: "
        Cosmics = False
else:
    Cosmics = False
    eventType = "collisions"
    print "Config: Alignment is performed on Collisions and as EventType is not specified, and Cosmics-flag is set to: ",Cosmics

if 'inputConstantsFile' in dir():
    print "Config: Read alignment constants from Pool-file instead of DB: "
    readConstantsFromPool = True
    readTRT = False   # this is for now important as the pool file does not contain TRT     
    print "Config: inputConstantsFile is specified as: ", inputConstantsFile
else:
    inputConstantsFile = ""
    readConstantsFromPool = False
    readTRT = True
    print "Config: Reading alignment constants from DB. "

if 'nEvents' in dir():
    print "Config: nEvents = ",nEvents
else:
    nEvents = -1
    print "Config: setting nEvents to: ",nEvents

if 'preIBLgeometry' in dir():
    print "Config: You configured to run on pre-IBL geometry. Make sure all conditions are set correctly! "
    print "Config: This is especially important when running on MC"
else:
    print "Config: Running geometry including IBL"
    preIBLgeometry=False


if 'inputFileDir' in dir():
    print  "Config: Data directory for files is configured to: ",inputFileDir
    print  "Config: !!Note!! All root files within this directory will be used as input"
    oldFileDefault=False
else:
    if preIBLgeometry == False:
        oldFileDefault=False
        inputFileDir='/afs/cern.ch/user/m/mdanning/hias/public/OUT'
    else:
        #inputFileDir='/afs/cern.ch/user/m/mdanning/eos/atlas/user/s/shuli/Alignment_DATA/data12_8TeV.00200863.physics_Muons.recon.DESD_ZMUMU.r4644_p1559_r4644_tid01296318_00/DESD_ZMUMU.01296318._000001.pool.root.1'
        oldFileDefault=True
        inputFileDir='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.107233.singlepart_mu100.digit.RDO.e605_s933_s946_d369//RDO.197117._000005.pool.root.1'
    print  "Config: !!Note!! You have not specified an inputFileDir -- Default sample is chosen!"

if 'doMonitoring' in dir():
    print "Config: Monitoring is enabled"
else:
    doMonitoring = False;

if 'isData' in dir():
    print "Config: Running on real data"
    realData = True    
else:
    print "Config: Running on MC"
    realData = False;

## ===================================================================
## ===================================================================
## ==                                                               ==
## ==                  Settings for the alignment                   ==
## ==                                                               ==
## message level for the alignment
## INFO, DEBUG, ...
outputLevel=INFO
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
#readConstantsFromPool = False  ## is set as input to the script
readSilicon = True
#readTRT = False  ## is also set as input to the script for now 
inputPoolFiles = [ inputConstantsFile ]
ErrorScaling = False
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
alignSCT = True
alignTRT = True

#
##
## TRT alignment
##  -1 - unknown, take setup from barrel and endcaps
##   0 - L0
##   1 - L1
##   2 - L2
trtAlignmentLevel = 1
#
## TRT barrel alignment
## overwrite trtAlignmentLevel if present
##  -1 - take setup from trtAlignmentLevel
##   1 - L1
##   2 - L2
##   3 - L3
trtAlignmentLevelBarrel = -1
#
## TRT barrel alignment
## overwrite trtAlignmentLevel if present
##  -1 - take setup from trtAlignmentLevel
##   1 - L1
##   2 - L2
## 200 - L2 old setup
trtAlignmentLevelEndcaps = -1
#
## alignment geometry
trtAlignBarrel  = True
trtAlignEndcaps = True
## alignment parameters Barrel
trtAlignBarrelX = True
trtAlignBarrelY = True
trtAlignBarrelZ = False
trtAlignBarrelRotX = True
trtAlignBarrelRotY = True
trtAlignBarrelRotZ = True
## alignment parameters Endcap
trtAlignEndcapX = True
trtAlignEndcapY = True
trtAlignEndcapZ = False
trtAlignEndcapRotX = True
trtAlignEndcapRotY = True
trtAlignEndcapRotZ = True

#
## Pixel alignment
##  -1 - unknown, take setup from barrel and endcaps
##  11 - L11 IBL subdetector as one structure + all "old" pixel subdetector as one structure  
##   1 - L1
##   2 - L2
##   3 - L3
pixelAlignmentLevel = 2
pixelAlignDBM = False
# if pixelAlignDBM is configured to "True" the pixelAlignmentLevel is used for DBM alignment
# Note, only levels 1, 2, and 3 ar epossible. If set to "False" DBM is ignored (default)
#
## Pixel barrel alignment
## overwrite pixelAlignmentLevel if present
##  -1 - take setup from pixelAlignmentLevel
##  15 - L15: 2 halfs
##   2 - L2
##  22 - L22: 6 half-layers
##   3 - L3
pixelAlignmentLevelBarrel = -1
#
## Pixel barrel alignment
## overwrite pixelAlignmentLevel if present
##  -1 - take setup from pixelAlignmentLevel
##   1 - L1
##   2 - L2
##   3 - L3
pixelAlignmentLevelEndcaps = -1

#
## Pixel alignment DoFs
pixelAlignBarrel  = True
pixelAlignEndcaps = True
## alignment parameters Barrel
pixelAlignBarrelX = True
pixelAlignBarrelY = True
pixelAlignBarrelZ = True
pixelAlignBarrelRotX = True
pixelAlignBarrelRotY = True
pixelAlignBarrelRotZ = True
## alignment parameters Endcap
pixelAlignEndcapX = True
pixelAlignEndcapY = True
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = False
pixelAlignEndcapRotY = False
pixelAlignEndcapRotZ = True
#
## SCT alignment
##  -1 - unknown, take setup from barrel and endcaps
##   0 - L0
##   1 - L1
##   2 - L2
##   3 - L3
sctAlignmentLevel = 1
#
## SCT barrel alignment
## overwrite sctAlignmentLevel if present
##  -1 - take setup from sctAlignmentLevel
##   2 - L2
##   3 - L3
sctAlignmentLevelBarrel = -1
#
## SCT barrel alignment
## overwrite sctAlignmentLevel if present
##  -1 - take setup from sctAlignmentLevel
##   1 - L1
##   2 - L2
##   3 - L3
sctAlignmentLevelEndcaps = -1
#
## SCT alignment DoFs
sctAlignBarrel  = False
sctAlignEndcaps = True
## alignment parameters Barrel
sctAlignBarrelX = True
sctAlignBarrelY = True
sctAlignBarrelZ = True
sctAlignBarrelRotX = True
sctAlignBarrelRotY = True
sctAlignBarrelRotZ = True
## alignment parameters Endcap
sctAlignEndcapX = True
sctAlignEndcapY = True
sctAlignEndcapZ = True
sctAlignEndcapRotX = True
sctAlignEndcapRotY = True
sctAlignEndcapRotZ = True
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
runAccumulate = True
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
runLocal = False
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
##   2 - ***REMOVED***
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
#writeAlignNtuple = True
##
## store derivatives of residuals wrt. alignment parameters
## in the ntuple
#writeDerivatives = True
##
## ==                                                               ==
## ==            End of settings for the alignment                  ==
## ==                                                               ==
## ===================================================================
## ===================================================================
#
## ===================================================================
## ===================================================================
## ==                                                               ==
## ==             Settings for the Reconstruction                   ==
## ==                                                               ==
# ID Reconstruction  Options
# ==========================
#realData = False

if preIBLgeometry == True:
    detectorDescription = "ATLAS-GEO-21-02-01"
    ## This tag is an example tag for MC
    ## Please check AMI DB for each MC sample when using old preIBLgeometry
    globalTag = "OFLCOND-SDR-BS7T-04-02"
    ## This is the data tag which should be used for Run1
    #globalTag = "COMCOND-BLKPA-RUN1-06"
else:
    detectorDescription = "ATLAS-IBL3D25-04-00-01"
    if eventType == "collisions":
        globalTag = "OFLCOND-MC12-IBL-20-30-25" 
    else: 
        globalTag = "OFLCOND-MC12-IBL-20-30-50" #Cosmics

#doTrkNtuple = False
#doMonitoring = True
#siPoolFile = ""
if not preIBLgeometry == True:
    siAlignmentTag = "InDetAlign_R2_Nominal"

#errorScalingTag = ""
#TRTCalibTextFile = ""
numberOfEvents = nEvents

# Input Files
import glob

if oldFileDefault == True:
    inputFiles = [str(inputFileDir)]
else:
    inputFiles = glob.glob(str(inputFileDir)+'*')

print inputFiles
#x-check the files I read in  

# Run cosmic reconstruction or Collision: This is the same script now. 
# Important flag is Cosmics true/false, which will be picked up by the loadInDetRec script,
# and will set all options accordingly 

#doReadBS = False
include("RecJobTransforms/UseFrontier.py")
include("InDetAlignExample/loadInDetRec_Run2Rel17.py")

# Run Alignment
# ========================
include("InDetAlignExample/NewInDetAlignAlgSetup.py")

