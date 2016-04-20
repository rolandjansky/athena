# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file:   MuonAlignmentFlags.py
## @brief:  Flags specific to muon alignment, inherit from 
## 	    JobProperty
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009
##  Modified by Prolay Mal March-28-2010
##           Incoporated the Csc specific stuffs
############################################################


__author__ = 'Robert Harrington'
__version__ = ""
__doc__ = "Muon Alignment specific flags"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import *

from MuonRecExample.MuonRecFlags import muonRecFlags
from RecExConfig.RecFlags import rec as recFlags
from AthenaMonitoring.DQMonFlags import DQMonFlags
from PerfMonComps.PerfMonFlags import jobproperties as perfMonJobProperties
perfMonFlags = perfMonJobProperties.PerfMonFlags

from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

mlog = logging.getLogger( 'MuonAlignmentFlags.py' )

_flags = []

# define default flags
class alignMDT(JobProperty) :
    """ flag to set alignment of MDT chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(alignMDT)

class alignTGC(JobProperty) :
    """ flag to set alignment of TGC chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(alignTGC)

class alignRPC(JobProperty) :
    """ flag to set alignment of RPC chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(alignRPC)

class alignCSC(JobProperty) :
    """ flag to set alignment of CSC chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(alignCSC)

class mdtAlignTraX(JobProperty) :
    """ sets alignment of x-translation for MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignTraX)

class mdtAlignTraY(JobProperty) :
    """ sets alignment of y-translation for MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignTraY)

class mdtAlignTraZ(JobProperty) :
    """ sets alignment of z-translation for MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignTraZ)
    
class mdtAlignRotX(JobProperty) :
    """ sets alignment of x-rotation for MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignRotX)

class mdtAlignRotY(JobProperty) :
    """ sets alignment of y-rotation for MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignRotY)

class mdtAlignRotZ(JobProperty) :
    """ sets alignment of z-rotation for MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignRotZ)
    
class xyRotZPosition(JobProperty) :
    """ position along z-axis of xy-rotations for L1 alignment """
    statusOn = True
    allowedType = ['float']
    StoredValue = 0.
_flags.append(xyRotZPosition)

class modulesToAlign(JobProperty) :
    """ Modules to align (for alignmentLevel<L3)"""
    statusOn = True
    allowedType = ['list']
    StoredValue = [] 
_flags.append(modulesToAlign)

class alignEE(JobProperty) :
    """ Align EEL/BEE chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(alignEE)

class alignEndcap(JobProperty) :
    """ Align endcap chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(alignEndcap)

class alignBarrel(JobProperty) :
    """ Align barrel chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(alignBarrel)
    
class applyStandardSelectionCuts(JobProperty) :
    """ flag to apply standard selection cuts """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(applyStandardSelectionCuts)

class applySelectionCuts(JobProperty) :
    """ flag to apply selection cuts """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(applySelectionCuts)

class cutOnTrackChisquare(JobProperty) :
    """ flag to apply chisquare cuts on track selection """
    statusOn = True
    allowedType = ['double']
    StoredValue = 5.0
_flags.append(cutOnTrackChisquare)

class cutOnNumHolesOnTrack(JobProperty) :
    """ flag to remove holes on the tracks  """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0
_flags.append(cutOnNumHolesOnTrack)

class cutOnNumOutlierOnTrack(JobProperty) :
    """ flag to apply allowed number of outliers on the tracks  """
    statusOn = True
    allowedType = ['int']
    StoredValue = 1
_flags.append(cutOnNumOutlierOnTrack)

class cutOnMaxDriftSignSum(JobProperty) :
    """ flag to apply allowed number of outliers on the tracks  """
    statusOn = True
    allowedType = ['double']
    StoredValue = 2.0
_flags.append(cutOnMaxDriftSignSum)

class cutOnBarrelTracks(JobProperty) :
    """ flag to remove tracks that pass through the barrel """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(cutOnBarrelTracks)

class requireOuterLayerECTracks(JobProperty) :
    """ flag to remove tracks that don't pass through the outer layer endcap MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireOuterLayerECTracks)

class requireInnerLayerECTracks(JobProperty) :
    """ flag to remove tracks that don't pass through the inner layer endcap MDT """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireInnerLayerECTracks)

class requireOneHitPerTubeLayerEC(JobProperty) :
    """ flag to remove tracks that don't have at least 1 hit per tube layer"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireOneHitPerTubeLayerEC)

class requireBarrelECOverlap(JobProperty) :
    """ flag to remove tracks that don't have at least 1 hit in both barrel and endcap"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireBarrelECOverlap)

class requireSmallLargeOverlap(JobProperty) :
    """ flag to remove tracks that don't have at least 1 hit in a small and a large MDT chamber"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireSmallLargeOverlap)


class requireRPCPhiHit(JobProperty) :
    """ flag to remove tracks that don't have at least 1 RPC phi hit"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(requireRPCPhiHit)

class removeBarrelHits(JobProperty) :
    """ flag to remove barrel muon hits from tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(removeBarrelHits)

class removeEndcapHits(JobProperty) :
    """ flag to remove endcap muon hits from tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(removeEndcapHits)

class redoErrorScaling(JobProperty) :
    """ flag to redo error scaling for ROTs on track"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(redoErrorScaling)

class pTCorrectTrack(JobProperty) :
    """ flag to reset the qOverPt for the first non-perigee TSOS on a track """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(pTCorrectTrack)

class removeTRTHits(JobProperty) :
    """ flag to remove TRT hits from tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(removeTRTHits)

class removeCSCHits(JobProperty) :
    """ flag to remove CSC hits from tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(removeCSCHits)

class useFullReco(JobProperty) :
    """ flag to use full reconstruction (uses standalone muon if False) """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(useFullReco)

class jobType(JobProperty) :
    """ string to label type of alignment job """
    statusOn = True
    allowedType = ['string']
    StoredValue = ""
_flags.append(jobType)

class doBarrel(JobProperty) :
    """ flag to align barrel chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(doBarrel)

class doEndcap(JobProperty) :
    """ flag to align endcap chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(doEndcap)

class doModuleSelection(JobProperty) :
    """ flag to select level 1 modules in MuonGeometryManagerTool """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doModuleSelection)

class chamberSelectionString(JobProperty) :
    """ string used by RegionSelectionSvc to select chambers """
    statusOn = True
    allowedType = ['string']
    StoredValue = ""
_flags.append(chamberSelectionString)

class doMultiLevelAlignment(JobProperty) :
    """ flag to perform mutlilevel alignment usign MultiLevelAlignmentDbTool """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doMultiLevelAlignment)

class doShiftDerivTree(JobProperty) :
    """ flag to turn on output tree in ShiftingDerivCalcTool """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(doShiftDerivTree)

class doChi2VChamberShiftsMeasType(JobProperty) :
    """ flag to do chi2 vs. chamber shift and discontinuity cut in shifting derivatives for each type of measurement separately """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doChi2VChamberShiftsMeasType)

class useAlternateASZT(JobProperty) :
    """ flag to turn on use of alternate ASZT file in MuonAlignDBTool (also used by MuonAlignment_jobOptions.py) """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(useAlternateASZT)

class initialAsztFilePath(JobProperty) :
    """ path from which the initial aszt file is read """
    statusOn = True
    allowedType = ['string']
    StoredValue = '../asztfiles/'
_flags.append(initialAsztFilePath)

class asztFilePath(JobProperty) :
    """ path the aszt files are read from / written to"""
    statusOn = True
    allowedType = ['string']
    StoredValue = ''
_flags.append(asztFilePath)

class endcapsToAlign(JobProperty) :
    """ flag to align only one endcap """
    statusOn = True
    allowedType = ['string']
    StoredValue = 'BOTH'
    allowedValues = [ 'BOTH', 'A', 'C' ] 
_flags.append(endcapsToAlign)

class numberOfShifts(JobProperty) :
    """ number of shifts for shifting derivatives """
    statusOn = True
    allowedType = ['int']
    StoredValue = 5
_flags.append(numberOfShifts)

class shiftSizeRotations(JobProperty) :
    """ size of shifts for rotations in shifting derivatives """
    statusOn = True
    allowedType = ['float']
    StoredValue = 0.1
_flags.append(shiftSizeRotations)

class shiftSizeTranslations(JobProperty) :
    """ size of shifts for translations in shifting derivatives """
    statusOn = True
    allowedType = ['float']
    StoredValue = 0.1
_flags.append(shiftSizeTranslations)

class useOpticalAlignment(JobProperty) :
    """ use A-lines from optical alignment system """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(useOpticalAlignment)

class MuonContainerName(JobProperty) :
    """ container name used by MuonTrackPreProcessor and MuonFillNtuple """
    statusOn = True
    allowedType = ['string']
    StoredValue = ""
_flags.append(MuonContainerName)

class inputIDTrackCol(JobProperty) :
    """ container name used by MuonTrackCollectionProvider to get input ID tracks"""
    statusOn = True
    allowedType = ['string']
    StoredValue = "Tracks"
_flags.append(inputIDTrackCol)

class momentumCut(JobProperty) :
    """ momentum cut """
    statusOn = True
    allowedType = ['float']
    StoredValue = 4.0
_flags.append(momentumCut)

class inputTrackCollectionType(JobProperty) :
    """ input track collection type """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0
    allowedValues = [0,1,2] # see MuonTrackCollectionProvider for TrackType enum
_flags.append(inputTrackCollectionType)

class selectCombinedMuons(JobProperty) :
    """ select combined muons in MuonTrackPreProcessor """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(selectCombinedMuons)

class msidPtDiffCut(JobProperty):
    """ apply cut to relative difference in momentum between MS and ID"""
    statusOn = True
    allowedType = ['float']
    StoredValue = 0.15
_flags.append(msidPtDiffCut)

class doBigNtuple(JobProperty):
    """ fill ntuple with all possible branches for debugging and validation """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doBigNtuple)

class doLocalErrorScaling(JobProperty):
    """ do error scaling for muon chambers """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doLocalErrorScaling)

class useStandaloneTracks(JobProperty):
    """ use standalone tracks """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(useStandaloneTracks)

class trackCollectionProvider(JobProperty):
    """ type of trackCollectionProvider to use in AlignAlg"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = 'TrackCollectionProvider'
    allowedValues = [ 'TrackCollectionProvider', 'MuonTrackCollectionProvider' ]
_flags.append(trackCollectionProvider)

class resetScatteringAngles(JobProperty):
    """ reset ScatteringAngles to zero before track refit"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(resetScatteringAngles)

class removePerigeeBeforeRefit(JobProperty):
    """ remove perigee before refit """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(removePerigeeBeforeRefit)

class mdtAlignTraX(JobProperty):
    """ align parameter x-translation"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignTraX)

class mdtAlignTraY(JobProperty):
    """ align parameter y-translation"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignTraY)

class mdtAlignTraZ(JobProperty):
    """ align parameter z-translation"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignTraZ)

class mdtAlignRotX(JobProperty):
    """ align parameter x-rotation"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignRotX)

class mdtAlignRotY(JobProperty):
    """ align parameter y-rotation"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignRotY)

class mdtAlignRotZ(JobProperty):
    """ align parameter z-rotation"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_flags.append(mdtAlignRotZ)

# add to jobproperties
class MuonAlignmentFlags(JobPropertyContainer):
    """ The alignment flag/job property container """
    
    # some derived flags
    def alignModuleListType(self) :
        if align.alignmentLevel()==0 :
            return 0
        elif align.alignmentLevel()==1 :
            return 1
        elif align.alignmentLevel()==23 :
            return 2
        elif align.alignmentLevel()==3 :
            return 3
        else :
            print "bad option"
            return -1

    ## returns instance of MuonAlgnmentIIOFiles
    def muonAlignmentIOFiles(self) :
        from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
        return MuonAlignmentIOFiles()    

    # methods for setting sets of flags

    ## sets default flags for muon reconstruction (see ConfigureMuonAlignment for more flag setting)
    def setDefaultRecoFlags(self,readESD=False) :
        print "setDefaultRecoFlags"
        muonRecFlags.doMuonboy.set_Value_and_Lock(False)
        muonRecFlags.doCalib.set_Value_and_Lock(False)
        muonRecFlags.doCalibNtuple.set_Value_and_Lock(False)

        #recFlags.readESD    = readESD
        #print "readESD set to ",recFlags.readESD

        recFlags.oldFlagTopSteering.set_Off()

        #recFlags.doESD      = True
        recFlags.doWriteESD = False
        recFlags.doWriteAOD = False
        recFlags.doWriteTAG = False
        recFlags.doCBNT     = False
        recFlags.doTrigger  = False
        recFlags.doTruth.set_Value_and_Lock(False)
        recFlags.doAOD.set_Value_and_Lock(False)
        recFlags.doDetailedPerfMon.set_Value_and_Lock(False)
        recFlags.doPerfMon.set_Value_and_Lock(False)

        DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)
        DQMonFlags.doMuonRawMon.set_Value_and_Lock(False)
        DQMonFlags.doMuonSegmentMon.set_Value_and_Lock(False)
        DQMonFlags.doMuonTrackMon.set_Value_and_Lock(False)

        perfMonFlags.doDsoMonitoring.set_Value_and_Lock(False)
        perfMonFlags.doDetailedMonitoring.set_Value_and_Lock(False)
        perfMonFlags.doFullMon.set_Value_and_Lock(False)
        perfMonFlags.doMonitoring.set_Value_and_Lock(False)
        perfMonFlags.doPersistencyMonitoring.set_Value_and_Lock(False)
        
        from AthenaCommon.Include import include
        include.block("PerfMonComps/DomainsRegistry.py")

    ## sets default flags for shifting(numerical) derivatives 
    #  (see ConfigureMuonAlignment for more flag setting)
    def setDefaultShiftingDerivativeFlags(self) :
        self.doShiftDerivTree = True
        self.doChi2VChamberShiftsMeasType = True
        self.numberOfShifts = 11

    ## sets default flags for analytical derivatives 
    #  (see ConfigureMuonAlignment for more flag setting)
    def setDefaultAnalyticalDerivativeFlags(self) :
        self.doShiftDerivTree = False
        self.doChi2VChamberShiftsMeasType = False
        align.includeScatteringResiduals = False
        #align.includeScatterers = True
        align.removeATSOSNotInAlignModule=True
        #self.declusterTGCHits = True
    
    ## sets default flags for L0 alignment (see ConfigureMuonAlignment
    #  for more flag setting)
    def setDefaultL0Flags(self) :

        from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
        muonCombinedRecFlags.doMuidMuonCollection=True

        align.alignmentLevel              = 0
        align.includeScatterers           = True 
        align.removeATSOSNotInAlignModule = False
        align.includeScatteringResiduals  = True
        align.trackAlignParamCut          = 100.
        align.setMinIterations            = True 

        self.chamberSelectionString       = ''
        self.doModuleSelection            = True
        self.shiftSizeRotations           = 1.5
        self.shiftSizeTranslations        = 1.5
        self.selectCombinedMuons          = True
        self.msidPtDiffCut                = 999. # 0.15
        self.alignTGC                     = False
        self.alignCSC                     = True
        self.alignRPC                     = False
        self.alignMDT                     = True
        self.requireOuterLayerECTracks    = False 
        self.requireInnerLayerECTracks    = False
        self.requireOneHitPerTubeLayerEC  = False
        self.removeTGCHits                = False
        self.removeCSCHits                = False
        self.removeTRTHits                = False
        self.removeBarrelHits             = False
        self.cutOnBarrelTracks            = False
        align.refitTracks                 = True

        self.doBigNtuple = True

    ## sets default flags for L1 alignment (see ConfigureMuonAlignment
    #  for more flag setting)
    def setDefaultL1Flags(self) :

        from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
        muonCombinedRecFlags.doMuidMuonCollection=True

        align.alignmentLevel              = 1
        #if align.fitterType == 'iPat' :
        #align.inputTrkCol               = 'ConvertedMuIdCBTracks'
        #else :
        #  align.inputTrkCol               = 'Combined_Tracks'

        align.includeScatterers           = False #rdh 
        align.removeATSOSNotInAlignModule = False
        align.includeScatteringResiduals  = True 
        align.trackAlignParamCut          = 100.
        #align.trackAlignParamCut          = 1.e-5
        #align.trackAlignParamCut          = 1.e-4
        align.setMinIterations            = True 

        self.chamberSelectionString       = ''
        self.doModuleSelection            = True
        #self.shiftSizeRotations           = 1.0
        #self.shiftSizeTranslations        = 1.0
        self.shiftSizeRotations           = 1.5
        self.shiftSizeTranslations        = 1.5
        self.selectCombinedMuons          = True
        self.msidPtDiffCut                = 999. # 0.15
        #self.MuonContainerName            = "MuidMuonCollection"
        self.alignTGC                     = True
        self.alignCSC                     = True
        self.alignRPC                     = True
        self.alignMDT                     = True
        self.requireOuterLayerECTracks    = False
        self.requireInnerLayerECTracks    = False
        self.requireOneHitPerTubeLayerEC  = False
        self.removeCSCHits                = False
        self.removeTRTHits                = False
        self.removeBarrelHits             = False
        self.cutOnBarrelTracks            = False
        align.refitTracks                 = True
        
        self.doBigNtuple = True

        #if not self.alignTGC:
        #    muonRecFlags.doTGCs.set_Value_and_Lock(False)
        #if not self.alignCSC: 
        #    muonRecFlags.doCSCs.set_Value_and_Lock(False)
        #if not self.alignMDT: 
        #    muonRecFlags.doMDTs.set_Value_and_Lock(False)

    ## sets default flags for L2.3 alignment (see ConfigureMuonAlignment
    #  for more flag setting)
    def setDefaultL23Flags(self) :
        align.alignmentLevel              = 23
        align.inputTrkCol                 = 'MooreTracks'
        align.includeScatteringResiduals  = True
        align.includeScatterers           = True
        align.removeATSOSNotInAlignModule = False
        align.refitTracks                 = True
        align.minNumHitsPerModule         = 0
        align.minNumTrksPerModule         = 1
        self.chamberSelectionString       = ''
        self.doModuleSelection            = True
        self.shiftSizeRotations           = 1.0
        self.shiftSizeTranslations        = 1.0
        self.mdtAlignTraX                 = False
        self.useAlternateASZT             = False
        self.selectCombinedMuons          = False
        self.MuonContainerName            = ""
        self.alignMDT                     = True
        self.applySelectionCuts           = False
        self.applyStandardSelectionCuts   = True

    ## sets default flags for L3 alignment (see ConfigureMuonAlignment
    #  for more flag setting)
    def setDefaultL3Flags(self) :
        align.alignmentLevel              = 3
        align.includeScatteringResiduals  = True
        align.includeScatterers           = True
        align.removeATSOSNotInAlignModule = False
        align.refitTracks                 = True
        align.minNumHitsPerModule         = 0
        align.minNumTrksPerModule         = 400
        #align.minNumTrksPerModule         = 1
        self.chamberSelectionString       = ''
        self.doModuleSelection            = True
        self.shiftSizeRotations           = 1.0
        self.shiftSizeTranslations        = 1.0
        self.mdtAlignTraX                 = False
        #self.alignParams                  = [1,2,3]
        self.useAlternateASZT             = False
        self.selectCombinedMuons          = False
        #self.MuonContainerName            = ""
        self.alignMDT                     = True
        self.applySelectionCuts           = False
        self.applyStandardSelectionCuts   = True
        
    ## sets default flags for L3 alignment (see ConfigureMuonAlignment 
    #  for more flag setting) 
    def setDefaultCscL3Flags(self) : 
        align.alignmentLevel              = 3 
        align.inputTrkCol                 = 'MooreTracks' 
        align.includeScatteringResiduals  = True 
        align.includeScatterers           = True 
        align.removeATSOSNotInAlignModule = False
        align.refitTracks                 = True 
        align.minNumHitsPerModule         = 0
        align.minNumHitsPerModule         = 1
        self.requireInnerLayerECTracks    = False 
        self.requireOneHitPerTubeLayerEC  = False 
        self.requireOuterLayerECTracks    = False
        self.removeBarrelHits             = False
        self.removeCSCHits                = False
        self.removeTRTHits                = False
        self.cutOnTrackChisquare          = 10.
        self.cutOnNumHolesOnTrack         = 100
        self.cutOnNumOutlierOnTrack       = 100
        self.cutOnMaxDriftSignSum         = 100         
        self.chamberSelectionString       = '' 
        self.doModuleSelection            = False 
        self.shiftSizeRotations           = 1.0 
        self.shiftSizeTranslations        = 1.0 
        self.mdtAlignTraZ                 = False
        self.mdtAlignRotX                 = False
        self.mdtAlignRotY                 = False
        self.useAlternateASZT             = True 
        self.selectCombinedMuons          = False 
        self.MuonContainerName            = "" 
        self.alignCSC                     = True 
        self.alignMDT                     = False 
        self.alignRPC                     = False 
        self.alignTGC                     = False 
        self.applySelectionCuts           = False
        self.applyStandardSelectionCuts   = True

    ## sets default flags for matrix solving (see ConfigureMuonAlignment
    #  for more flag setting)
    def setDefaultMatrixSolvingFlags(self) :
        align.weakModeToRemove = 1
        align.matrixToolSolveOption = 6 # SOLVE_ROOT
        align.useSparseMatrix  = True
        align.writeEigenMatrices = False
        align.writeModuleNames = True

jobproperties.add_Container(MuonAlignmentFlags)

for flag in _flags:
    jobproperties.MuonAlignmentFlags.add_JobProperty(flag)
del _flags

MuonAlignmentFlags = jobproperties.MuonAlignmentFlags
