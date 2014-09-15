
# setup flags for L3 MDT endcap alignment
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

# set up alignment flags
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

if 'solveOnly' in dir():
    align.solveOnly = solveOnly    

align.OutputLevel             = DEBUG

align.derivativeType          = 'Analytical' # be careful... I think this is overwritten later

align.particleNumber          = 2 # muon

align.fitterType              = "MCTB"

mualign.trackCollectionProvider = "MuonTrackCollectionProvider"
mualign.MuonContainerName       = "MuidMuonCollection"
mualign.useStandaloneTracks     = False

mualign.jobType               = 'EC'
mualign.endcapsToAlign        = 'A' # A-side only
mualign.doModuleSelection     = True
mualign.doMultiLevelAlignment = False

#mualign.setDefaultRecoFlags()
mualign.setDefaultAnalyticalDerivativeFlags()
mualign.setDefaultCscL3Flags()
mualign.setDefaultMatrixSolvingFlags()

#mualign.chamberSelectionString = "[EIL]"
#mualign.chamberSelectionString = "[EI?,5]"
#mualign.chamberSelectionString = "[BI?]"
mualign.useFullReco = False

if 'rootName' in dir():
    align.fileRootName = rootName
    align.fileRootName.lock()

if 'isBatch' in dir():
    align.outputPath = ""
    align.outputPath.lock()
    mualign.asztFilePath = ""
    mualign.asztFilePath.lock()
    mualign.initialAsztFilePath = ""

if align.iteration>0:
    align.readEventList = True
    align.writeEventList = False
