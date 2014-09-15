
# setup flags for L3 MDT endcap alignment
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

# set up alignment flags
if 'iteration' in dir():
    align.iteration = int(iteration)
else:
    align.iteration = 0

#if 'readEventList' in dir():
#    align.readEventList=readEventList

if 'inputMatrixFiles' in dir():
    align.inputMatrixFiles=inputMatrixFiles

if 'inputVectorFiles' in dir():
    align.inputVectorFiles=inputVectorFiles

if 'inputHitmapFiles' in dir():
    align.inputHitmapFiles=inputHitmapFiles

if 'solveOnly' in dir():
    align.solveOnly = solveOnly    
  
#align.OutputLevel             = VERBOSE
align.OutputLevel             = DEBUG
#align.OutputLevel             = INFO
align.derivativeType          = 'Numerical' # be careful... I think this is overwritten later

align.particleNumber          = 2 # muon

align.fitterType              = "MCTB"

mualign.trackCollectionProvider = "TrackCollectionProvider"
mualign.MuonContainerName       = "MooreTracks"

mualign.jobType               = 'Barrel'

mualign.setDefaultRecoFlags()
mualign.setDefaultL23Flags()
mualign.setDefaultMatrixSolvingFlags()

if align.derivativeType=='Analytical':
    mualign.setDefaultAnalyticalDerivativeFlags()
else:
    mualign.setDefaultShiftingDerivativeFlags()  

if 'doBarrelOnly' in dir() and doBarrelOnly:
    mualign.modulesToAlign = ["SectorA4_I","SectorA4_M","SectorA4_O"]
    #mualign.modulesToAlign = ["SectorA5"]
    #mualign.chamberSelectionString = "[B?S, 2, 2]"
else:
    print "L23 not implemented for Endcap."
    sys.exit();

mualign.doMultiLevelAlignment = True
mualign.doModuleSelection = True
mualign.useFullReco = False
#mualign.useAlternateASZT  = True
#mualign.alignParams = [1,2]

#SelectionCuts:
mualign.applySelectionCuts = True
mualign.requireRPCPhiHit = True
mualign.removeRPCEtaHits = True
mualign.cutOnNumHolesOnTrack=-1
mualign.cutOnNumOutlierOnTrack=-1
mualign.requireSmallLargeOverlap = True

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

def doSetup():
    print "in setup method"
    dresser = topSequence.AlignAlg.AlignTrackDresser
    creator = topSequence.AlignAlg.AlignTrackCreator
    dresser.OutputLevel = INFO
    creator.OutputLevel = INFO
    preprocessor = topSequence.AlignAlg.AlignTrackPreProcessor
    #preprocessor.RemoveRPCEtaHits = True
    #preprocessor.RequireRPCPhiHit = True
    #preprocessor.RequireSmallLargeOverlap=True
    print preprocessor
    
