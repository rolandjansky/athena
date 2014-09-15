
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
  
align.OutputLevel             = INFO 
align.derivativeType          = 'Analytical' # be careful... I think this is overwritten later
#align.doVP1=True

align.particleNumber          = 2 # muon

align.fitterType              = "MCTB"

align.writeNtuple = True 
align.dataType = 'data' 

mualign.trackCollectionProvider = "TrackCollectionProvider"
mualign.MuonContainerName       = "MooreTracks"
mualign.inputIDTrackCol         = ""

mualign.jobType               = ''
mualign.endcapsToAlign        = 'BOTH'
mualign.alignCSC              = True

mualign.useAlternateASZT = False
if align.iteration==0:
  mualign.doMultiLevelAlignment = False
else:
  mualign.doMultiLevelAlignment = True

#from RecExConfig.RecFlags import rec
#readESD=rec.readESD
#print "readESD=",readESD
#mualign.setDefaultRecoFlags(readESD)
mualign.setDefaultRecoFlags()
#mualign.setDefaultShiftingDerivativeFlags()
mualign.setDefaultL3Flags()
if align.derivativeType=='Analytical':
  mualign.setDefaultAnalyticalDerivativeFlags()
mualign.setDefaultMatrixSolvingFlags()

if 'doBarrelOnly' in dir() and doBarrelOnly:
  mualign.chamberSelectionString = "[BIL]"
else:
  #mualign.chamberSelectionString = "[EML,2,3]"
  #mualign.chamberSelectionString = "[E?L,2,1-99]"
  #mualign.chamberSelectionString = "[E??]"
  mualign.redoErrorScaling = False
  #mualign.removeBEEHits = False
  #mualign.removeEEHits = False
  mualign.removeCSCHits = False
  mualign.removeBarrelHits = False
  mualign.removeEndcapHits = False
  mualign.pTCorrectTrack = False
  #mualign.chamberSelectionString = "[EIS,8,-2]"
  mualign.chamberSelectionString = "[BEE] | [EEL]"

#mualign.moduleSelectionString = "[EI?,5]"
#mualign.moduleSelectionString = "[BI?]"

mualign.doModuleSelection = True

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
    align.eventList = "goodEvts_iter0.txt" 
#    align.writeEventList = False
