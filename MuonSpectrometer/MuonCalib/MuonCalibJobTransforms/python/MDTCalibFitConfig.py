# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

# define configuration properties
class MDTCalibFitConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # list of attributes
    postOptions            = String("jobOptions fragment to be appended at the end of the skeleton", None)
    CalibrationSite        = String("Calibration Site", "NONE")
    PostProcessCalibration = Boolean("Set to true if you want to insert the calibration constants to the database", True)
    NoRecalculation        = Boolean("If set to true no drift ties and drift radii will be recalculated. Use this for validation", False)
    SelectStation          = String("Select Station", "BIL")
    SelectEta              = Integer("Select Eta", 3)
    SelectPhi              = Integer("Select Phi", 3)
    CalibInputLocation     = String("Input Location", None)
    outputLocation         = String("Output Location", None)

    def __init__(self,name='mdtCalibFitConfig',metaData=None):
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'ABORT'


class CalibrationAlgorithmArg(trfarg.StringChoicesArg):
    """choose here the calibration algorithm to use"""
    def __init__(self, choices, caseSensitive=False, help='calibration algorithm to be used', name='calibrationAlgorithm'):
        trfarg.StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True

class SelectChamberArg(trfarg.StringArg):
    """chamber selection"""
    def __init__(self, help='select the chamber to be used', name='selectChamber'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class CalibNtupleFileListArg(trfarg.StringArg):
    """calibration ntuple file list name"""
    def __init__(self, help='calibration ntuple file list to be used', name='calibNtupleFileList'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class CalibSiteArg(trfarg.StringArg):
    """calibration site name"""
    def __init__(self, help='select the calibration site name', name='calibSite'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class WriteToDBArg(trfarg.BoolArg):
    """If set to false, write only to calibration files"""
    def __init__(self, help='If set to false, write only to calibration files', name='writeToDB'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class T0FitByArg(trfarg.StringChoicesArg):
   """set the tube grouping for the t0 fit"""
   def __init__(self, help="set the tube grouping for the t0 fit", name='t0FitBy'):
      trfarg.StringChoicesArg.__init__(self, ['AUTO','TUBE', 'LAYER', 'MULTILAYER', 'MEZZ_CARD', 'TUBE'], help, name, True)
   def isFullArgument(self):
        return True

class MuonRecoArg(trfarg.IntegerArg):
   """select muon reconstruction alg 3=Muonboy 4=Moore"""
   def __init__(self, help="select muon reconstruction alg 3=Muonboy 4=Moore", name='MuonReco'):
      trfarg.IntegerArg.__init__(self, help, name)
   def isFullArgument(self):
        return True

class ADCFitByArg(trfarg.StringChoicesArg):
   """set the tube grouping for the adc fit"""
   def __init__(self, help="set the tube grouping for the adc fit", name='adcFitBy'):
      trfarg.StringChoicesArg.__init__(self, ['TUBE', 'LAYER', 'MULTILAYER', 'MEZZ_CARD', 'TUBE'], help, name, True)
   def isFullArgument(self):
        return True

class RefineT0Arg(trfarg.BoolArg):
    """  """
    def __init__(self, help=' ', name='refineT0'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class SegmentRefitArg(trfarg.BoolArg):
    """perform a segment refit - not for algorithms like MTT0, Integration, RunScan.. """
    def __init__(self, help='perform a segment refit - not for algorithms like MTT0, Integration, RunScan..', name='segmentRefit'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True    

class InputFromDBArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='inputFromDB'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class InputFromFileArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='inputFromFile'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class OutputToDBArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='outputToDB'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class HeadIdArg(trfarg.IntegerArg):
    """ calibration DB header used (if -1 take the last one) """
    def __init__(self, help='', name='headId'):
    	trfarg.IntegerArg.__init__(self, help, name)
    def isFullArgument(self):
        return True    

class OutputToFileArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='outputToFile'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class MdtDqaFileArg(trfarg.StringArg):
    """ """
    def __init__(self, help='', name='mdtDqaFile'):
    	trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True    

class FillHistosArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='fillHistos'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class DoFinalizeArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='doFinalize'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class DoEfficiencyArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='doEfficiency'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class DoGlobalTimeFitArg(trfarg.BoolArg):
    """ """
    def __init__(self, help='', name='doGlobalTimeFit'):
    	trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class SuppressRecalibrationArg(trfarg.BoolArg):
    """ suppress any refit of the segments """
    def __init__(self, help='', name='suppressRecalibration'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class SegsFromRawArg(trfarg.BoolArg):
    """ Compute segments from raw hits """
    def __init__(self, help='', name='segsFromRaw'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class CurvedSegsArg(trfarg.BoolArg):
    """ Use curved traks to fit hits """
    def __init__(self, help='', name='curvedSegs'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True    

class BfieldCorrArg(trfarg.BoolArg):
    """ Apply Bfield corrections """
    def __init__(self, help='', name='bfieldCorr'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class HitResidArg(trfarg.BoolArg):
    """ MdtDqa: residuals vs radius """
    def __init__(self, help='', name='hitResid'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class DeadElAlgArg(trfarg.IntegerArg):
    """ MdtDqa: Dead Element calculation algorithm """
    def __init__(self, help='', name='DeadElAlg'):
        trfarg.IntegerArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class ADCcutArg(trfarg.FloatArg):
    """ MdtDqa: ADC cut value """
    def __init__(self, help='', name='ADCcut'):
        trfarg.FloatArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class EffNsigArg(trfarg.FloatArg):
    """ MdtDqa: N sigma cut for efficiency calculation """
    def __init__(self, help='', name='EffNsig'):
        trfarg.FloatArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class EffChiCutArg(trfarg.FloatArg):
    """ MdtDqa: chi2 cut in efficiency calculation """
    def __init__(self, help='', name='EffChiCut'):
        trfarg.FloatArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class EffUseDefResArg(trfarg.BoolArg):
    """ MdtDqa: choose default resolution """
    def __init__(self, help='', name='EffUseDefRes'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True


class EffiGtfArg(trfarg.BoolArg):
    """ MdtDqa: use GTF in efficiency calculation """
    def __init__(self, help='', name='effiGtf'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True    

class EffiReCalArg(trfarg.BoolArg):
    """ MdtDqa: recalibrate hits in efficiency calculation """
    def __init__(self, help='', name='effiReCal'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class EffiTimeCorrArg(trfarg.BoolArg):
    """ MdtDqa: use timing corrections in efficiency calculation """
    def __init__(self, help='', name='effiTimeCorr'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
 
class TrackPCut(trfarg.FloatArg):
    """ TrackPCut <=-2: No segmen t on track selection
        -2<TrackPCut<=-1: Select segments on any track
	-1<TrackPCut<=0: Select segments on a track with a valid momentum measurement
	TrackPCut>0: Select segments on a track with the given p-cut. """
    def __init__(self, help="""TrackPCut <=-2: No segmen t on track selection
        -2<TrackPCut<=-1: Select segments on any track
        -1<TrackPCut<=0: Select segments on a track with a valid momentum measurement
        TrackPCut>0: Select segments on a track with the given p-cut. """, name='trackPCut'):
        trfarg.FloatArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class FastTrackMatch(trfarg.BoolArg):
   """ Use faster tsegment to track matching """
   def __init__(self, help="Use faster segment to track matching", name="fastTrackMatch"):
      trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self):
      return True

class SuperFastTrackMatch(trfarg.BoolArg):
   """ Use segment on track info in ntuple """
   def __init__(self, help="Use faster segment to track matching", name="superFastTrackMatch"):
      trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self):
      return True

class ApplyTimeSlewingCorrection(trfarg.BoolArg):
   """ Apply Time Slewing Corrections """
   def __init__(self, help=" Apply Time Slewing Corrections", name="applyTimeSlewingCorrection"):
      trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self):
      return True

class ApplyRtScaling(trfarg.BoolArg):
   """Apply multilayerwise rt scaling"""
   def __init__(self, help= "Apply multilayerwise rt scaling", name="applyRtScaling"):
      trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self) :
      return True
 
class SplitterStoreRaw(trfarg.BoolArg):
   """Store raw branches in splitter"""
   def __init__(self, help= "Store raw branches in splitter", name="splitterStoreRaw"):
      trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self) :
      return True
 
class SplitterStoreTracks(trfarg.BoolArg):
   """Store track branches in splitter"""
   def __init__(self, help= "Store track branches in splitter", name="splitterStoreTracks"):
      trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self) :
      return True
     
class SplitterSplitLevel(trfarg.StringChoicesArg):
   """ntuple split level"""
   def __init__(self, help="ntuple slpit level", name='splitLevel'):
      trfarg.StringChoicesArg.__init__(self, ['NONE', "NAME", "NAME_PHI", "NAME_PHI_ETA"], help, name, True)
   def isFullArgument(self):
      return True

class SplitterADCCut(trfarg.IntegerArg):
    """ADC cut on rawdata"""
    def __init__(self, help='ADC cut on rawdata', name='splitterADCCut'):
    	trfarg.IntegerArg.__init__(self, help, name)
    def isFullArgument(self):
        return True    

class SplitterMergeOutputFiles(trfarg.StringArg):
    """List of files into which output files are merged"""
    def __init__(self, help='List of files into which output files are merged', name='splitterMergeOutputFiles'):
    	trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def setValue(self, value):
        if value and self.value():
           value=self.value() + "," + str(value)
        trfarg.StringArg.setValue(self, value)
	   

# make configuration object
mdtCalibFitConfig = MDTCalibFitConfig()
