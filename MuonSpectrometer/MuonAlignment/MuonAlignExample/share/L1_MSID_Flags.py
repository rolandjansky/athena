
# set up flags for L1 MDT endcap alignment
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

try:
    doVP1
    #rec.doVP1=doVP1
except:
    rec.doVP1=False

if 'iteration' in dir():
    align.iteration = int(iteration)
else:
    align.iteration = 0

if 'readEventList' in dir():
    align.readEventList=readEventList

if 'inputMatrixFiles' in dir():
    align.inputMatrixFiles=inputMatrixFiles

if 'inputVectorFiles' in dir():
    align.inputVectorFiles=inputVectorFiles

if 'inputHitmapFiles' in dir():
    align.inputHitmapFiles=inputHitmapFiles

if 'jobIndex' in dir():
    align.jobIndex = int(jobIndex)
else:
    align.jobIndex = -1

if 'solveOnly' in dir():
    align.solveOnly = solveOnly    

align.OutputLevel             = INFO 
align.derivativeType          = 'Analytical' # be careful... I think this is overwritten later

#align.fitterType              = "iPat"
#align.particleNumber          = 0 # non-interacting so iPat doesn't add material
#mualign.resetScatteringAngles = True

align.fitterType              = "CombMuonTrackFitter"
align.particleNumber          = 2 # muon so MCTB will work ok with scattering already on track
mualign.resetScatteringAngles = False

mualign.trackCollectionProvider = "MuonTrackCollectionProvider"
mualign.MuonContainerName       = "MuidMuonCollection"
mualign.useStandaloneTracks     = False

mualign.asztFilePath          = './'

mualign.modulesToAlign  = ['ECA']
mualign.jobType               = 'ECmisal'
mualign.endcapsToAlign        = 'A' # A-side only 
mualign.doMultiLevelAlignment = True
mualign.requireOneHitPerTubeLayerEC = True 
#mualign.declusterTGCHits      = False

readESD=rec.readESD
mualign.setDefaultRecoFlags(readESD)
mualign.setDefaultL1Flags()
if align.derivativeType=='Analytical':
  mualign.setDefaultAnalyticalDerivativeFlags()
else:
  mualign.setDefaultShiftingDerivativeFlags()
mualign.setDefaultMatrixSolvingFlags()

mualign.useFullReco = True
#mualign.mdtAlignParams=[0,1,2,3,4]
#mualign.xyRotZPosition = 5700.
mualign.xyRotZPosition = 0.

mualign.applySelectionCuts = False 

#mualign.doLocalErrorScaling = doLocalErrorScaling
if 'readEventList' in dir():
    align.readEventList=readEventList

if 'rootName' in dir():
    align.fileRootName = rootName
    align.fileRootName.lock()

if 'isBatch' in dir():
    align.outputPath = ""
    align.outputPath.lock()
    mualign.asztFilePath = ""
    mualign.asztFilePath.lock()
    mualign.initialAsztFilePath = ""

#if iteration>0:
#  align.readEventList = True
#  align.writeEventList = False
