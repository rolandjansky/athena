# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file:   AlignmentFlags.py
## @brief:  Flags specific to alignment, inherit from 
## 	    JobProperty
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

__author__ = 'Robert Harrington'
__version__ = ""
__doc__ = "Alignment specific flags"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import ALL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL

mlog = logging.getLogger( 'AlignmentFlags.py' )

_flags = []

# Define default flags
class derivativeType(JobProperty):
    """ a flag to determine type of derivatives """
    statusOn = True
    allowedType=['string']
    StoredValue='Analytical'
    allowedValues = [ 'Analytical', 'Numerical' ]
_flags.append(derivativeType)

class alignMS(JobProperty):
    """ flag to determine whether to align MS or not """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True 
_flags.append(alignMS)

class alignID(JobProperty):
    """ flag to determine whether to align ID  or not """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True 
_flags.append(alignID)

class alignmentType(JobProperty):
    """ flag to determine type of alignment """
    statusOn = True
    allowedType = ['string']
    StoredValue = 'GlobalChi2'
    allowedValues = [ 'GlobalChi2' , 'LocalChi2' ]
_flags.append(alignmentType)

class alignmentLevel(JobProperty):
    """ a flag to determine alignment level """
    statusOn=True
    allowedType=['int']
    StoredValue = 3
    allowedValues = [ 0, 1, 2, 3, 23 ]
_flags.append(alignmentLevel)

class dataType(JobProperty):
    """ type of data (if changed, don't forget to change isMC() below"""
    statusOn = True
    allowedType = ['string']
    StoredValue = 'MC'
    allowedValues = [ 'MC', 'data', 'cosmic', 'simCosmic' ]

_flags.append(dataType)
    
class inputTrkCol(JobProperty):
    """ input track collection name """
    statusOn = True
    allowedType = ['string']
    StoredValue = 'MooreTracks'
_flags.append(inputTrkCol)

class iteration(JobProperty):
    """ flag to indicate the iteration number """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0
_flags.append(iteration)

class jobIndex(JobProperty):
    """ flag to indicate the index of the job (e.g. when running in batch) """
    statusOn = True
    allowedType = ['int']
    StoredValue = -1
_flags.append(jobIndex)

class useSLFitter(JobProperty):
    """ flag to determine whether to use SL fitter to fit straight tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(useSLFitter)

class includeScatterers(JobProperty):
    """ flag to determine whether to include scatterers as AlignTSOS on AlignTrack """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(includeScatterers)

class includeScatteringResiduals(JobProperty):
    """ flag to determine whether to include contributions to chi2 from 
    scattering centers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(includeScatteringResiduals)

class runOutlierRemoval(JobProperty):
    """ flag to run outlier removal in track refitting """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(runOutlierRemoval)

class refitTracks(JobProperty):
    """ flag to refit tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(refitTracks)

class particleNumber(JobProperty):
    """ flag to indicate particle number (see TrkEventPrimitives::ParticleHypothesis) """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0
    allowedValues = [ 0, # non-interacting or geantino
                      2, # muon
                      99 # undefined
                      ]
_flags.append(particleNumber)

class OutputLevel(JobProperty):
    """ General msg output level VERBOSE DEBUG INFO """ 
    statusOn=True
    allowedTypes=['int']
    allowedValues=[ ALL,VERBOSE,DEBUG,INFO,WARNING,ERROR,FATAL ]
    StoredValue=INFO
_flags.append(OutputLevel)
    
class fitterType(JobProperty) :
    """ type of track fitter used to refit tracks for alignment """
    statusOn = True
    allowedType=['string']
    allowedValues = [ 'GlobalChi2', 'MCTB', 'iPat', 'CombMuonTrackFitter' ]
    StoredValue = 'GlobalChi2'
_flags.append(fitterType)

class fileRootName(JobProperty) :
    """ root name used for file names """
    statusOn = True
    allowedType = ['string']
    StoredValue = ""
_flags.append(fileRootName)

class outputPath(JobProperty) :
    """ output path """
    statusOn = True
    allowedType = ['string']
    StoredValue = ""
_flags.append(outputPath)

class readEventList(JobProperty) :
    """ flag to read run and event numbers from ASCII file for event selection """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(readEventList)

class writeEventList(JobProperty) :
    """ flag to write run and event numbers to ASCII file after event selection """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(writeEventList)

class eventList(JobProperty) :
    """ name of file containing run/event numbers of good events """
    statusOn = True
    allowedType = ['string']
    StoredValue = ""
_flags.append(eventList)

class writeNtuple(JobProperty) :
    """ flag to require writing ntuple"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(writeNtuple)

class ntupleFileName(JobProperty) :
    """ name of ntuple file """
    statusOn = True
    allowedType = ['string']
    StoredValue = "Align.root"
_flags.append(ntupleFileName)

class ntupleFilePath(JobProperty) :
    """ name of ntuple file path """
    statusOn = True
    allowedType = ['string']
    StoredValue = "./"
_flags.append(ntupleFilePath)

class requireOverlap(JobProperty) :
    """ flag to require tracks passing through more than one AlignModule (used in AlignTrackCreator) """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireOverlap)

class removeATSOSNotInAlignModule(JobProperty) :
    """ flag to remove ATSOS not in AlignModule in AlignTrackCreator """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(removeATSOSNotInAlignModule)

# flags for MatrixTool
class useSparseMatrix(JobProperty) :
    """ flag to use sparse matrix in MatrixTool """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(useSparseMatrix)

class matrixToolSolveOption(JobProperty) :
    """ flag to determine solving option in MatrixTool """
    statusOn = True
    allowedType = ['int']
    StoredValue = 3
    allowedValues = [ 0, # NONE (for batch jobs)
                      3, # DIRECT_SOLVE
                      4, # DIRECT_SOLVE_FAST
                      6] # SOLVE_ROOT
_flags.append(matrixToolSolveOption) 

class writeMatrix(JobProperty) :
    """ flag to write matrix and vector to binary and text files """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(writeMatrix)

class writeEigenMatrices(JobProperty) :
    """ flag eigenvalue matrices to files """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(writeEigenMatrices)

class writeModuleNames(JobProperty) :
    """ write module names to vector files """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(writeModuleNames)

class weakModeToRemove(JobProperty) :
    """used for weak mode removal in matrix solving """
    statusOn = True
    allowedType = ['int']
    StoredValue = -1 # default does nothing
_flags.append(weakModeToRemove)

class softEigenmodeCut(JobProperty) :
    """ flag used to cut on soft eigenmodes """
    statusOn = True
    allowedType = ['Double']
    StoredValue = 0.0
_flags.append(softEigenmodeCut)

class secondDerivativeCut(JobProperty) :
    """ cut on second derivative (used in GlobalChi2AlignTool) """
    statusOn = True
    allowedType = ['Double']
    StoredValue = 0.0
_flags.append(secondDerivativeCut)

class trackAlignParamCut(JobProperty) :
    """ cut used on track alignment parameter quality variable (algorithm-specific) """
    statusOn = True
    allowedType = ['Double']
    StoredValue = 1e6
_flags.append(trackAlignParamCut)

class setMinIterations(JobProperty) :
    """ used for numerical derivatives.  Track fitter required to perform a minimum number of iterations."""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(setMinIterations)

class runLocalMethod(JobProperty) :
    """ flag to run local solving before global """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(runLocalMethod)

class runCLHEPDiagonalize(JobProperty) :
    """ flag to run diagonalization when using CLHEP solving """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(runCLHEPDiagonalize)

class minNumHitsPerModule(JobProperty) :
    """ flag to set minimum number of hits per align module """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0
_flags.append(minNumHitsPerModule)

class minNumTrksPerModule(JobProperty) :
    """ flag to set minimum number of tracks per align module """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0
_flags.append(minNumTrksPerModule)

class writeHitmap(JobProperty) :
    """ flag to write a binary file containing number of hits in each module """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(writeHitmap)

class writeHitmapTxt(JobProperty) :
    """ flag to write a text file containing number of hits in each module """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(writeHitmapTxt)

class readHitmaps(JobProperty) :
    """ flag to read binary file containing number of hits in each module """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(readHitmaps)

class inputHitmapFiles(JobProperty) :
    """ list of input hitmap files """
    statusOn = True
    allowedType = ['list']
    StoredValue = []
_flags.append(inputHitmapFiles)

class inputMatrixFiles(JobProperty) :
    """ list of input matrix files """
    statusOn = True
    allowedType = ['list']
    StoredValue = []
_flags.append(inputMatrixFiles)

class inputVectorFiles(JobProperty) :
    """ list of input vector files """
    statusOn = True
    allowedType = ['list']
    StoredValue = []
_flags.append(inputVectorFiles)

class solveOnly(JobProperty) :
    """ flag to do solving only """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(solveOnly)

class scaleMatrix(JobProperty) :
    """ flag to scale matrix and vector before solving """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(scaleMatrix)

class doVP1(JobProperty) :
    """ flag to run VP1 algorithm """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doVP1)

class Align(JobPropertyContainer):
    """ The alignment flag/job property container """

    def isMC(self):
        val=self.dataType()
        return val=='MC' or val=='simCosmic'

jobproperties.add_Container(Align)

for flag in _flags:
    jobproperties.Align.add_JobProperty(flag)
del _flags

AlignmentFlags = jobproperties.Align
