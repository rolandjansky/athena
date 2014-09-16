# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -*- python -*-

from PyJobTransformsCore.full_trfarg import *
from PATJobTransforms.Configuration import *


## G4 Simulation specific arguments
class UseISFArg(BoolArg):
    """Use the ISF"""
    def __init__(self, help = 'Use the ISF', name = 'useISF'):
        BoolArg.__init__(self, help, name)

    def isFullArgument(self):
        return True
def AddUseISFArg(trf, inDic):
    trf.add( UseISFArg() )
    return
AddToConfigDic('useISF', AddUseISFArg)

def AddSimRandomSeedArg(trf, inDic):
    trf.add( RandomSeedArg(name = 'randomSeed', help = 'random seed for G4 simulation') )
    return
AddSimRandomSeedArg.subSteps = ['e2h']
AddToConfigDic('randomSeed', AddSimRandomSeedArg)

def AddFirstEventArg(trf, inDic):
    trf.add( FirstEventArg('number of the first event in the output file') )
    return
AddFirstEventArg.subSteps = ['e2h']
AddToConfigDic('firstEvent', AddFirstEventArg)

class PhysicsListArg(StringChoicesArg): ##TODO change to BasicStringArg and do the allowed value checking in the SimFlag?
    """choose here the physics list to use"""
    def __init__(self, choices, caseSensitive = False, help = 'default', name = 'physicsList'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddPhysicsListArg(trf, inDic):
    #Get allowed values:
    from AthenaCommon.AthenaCommonFlags import AthenaCommonFlags 
    from G4AtlasApps.SimFlags import simFlags
    trf.add( PhysicsListArg(simFlags.PhysicsList.allowedValues) )
    return
AddPhysicsListArg.subSteps = ['e2h']
AddToConfigDic('physicsList', AddPhysicsListArg)

class CosmicFilterVolumeArg(StringChoicesArg):
    """choose here the volume used for cosmic filtering"""
    def __init__(self, choices, caseSensitive = False, help = 'default', name = 'CosmicFilterVolume'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddCosmicFilterVolumeArg(trf, inDic):
    # TODO: Convert to take allowed values from new cosmics SimFlags entries when they exist
    trf.add( CosmicFilterVolumeArg(['Pixel', 'TRT_Barrel', 'TRT_EC', 'SCT_Barrel', 'InnerDetector', 'Calo', 'Muon']) )
    return
AddCosmicFilterVolumeArg.subSteps = ['e2h']
AddToConfigDic('CosmicFilterVolume', AddCosmicFilterVolumeArg)

class CosmicFilterVolume2Arg(StringChoicesArg):
    """choose here the 2nd volume used for cosmic filtering"""
    def __init__(self, choices, caseSensitive = False, help = 'default', name = 'CosmicFilterVolume2'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddCosmicFilterVolume2Arg(trf, inDic):
    # TODO: Convert to take allowed values from new cosmics SimFlags entries when they exist
    trf.add( CosmicFilterVolume2Arg(['Pixel', 'TRT_Barrel', 'TRT_EC', 'SCT_Barrel', 'InnerDetector', 'Calo', 'Muon', 'NONE']) )
    return
AddCosmicFilterVolume2Arg.subSteps = ['e2h']
AddToConfigDic('CosmicFilterVolume2', AddCosmicFilterVolume2Arg)

class CosmicPtSliceArg(StringChoicesArg):
    """choose the pT slice for generating cosmic rays"""
    def __init__(self, choices, caseSensitive = False, help = 'default', name = 'CosmicPtSlice'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddCosmicPtSliceArg(trf, inDic):
    # TODO: Convert to take allowed values from new cosmics SimFlags entries when they exist
    trf.add( CosmicPtSliceArg(['slice1', 'slice2', 'slice3', 'slice4', 'NONE']) )
    return
AddCosmicPtSliceArg.subSteps = ['e2h']
AddToConfigDic('CosmicPtSlice', AddCosmicPtSliceArg)

def AddDBContent(trf, inDic):
    trf.add( BasicStringArg(name = 'DBContent', help = 'Specify the tarball containing the Frozen Shower DB.') )
    return
AddDBContent.subSteps = ['e2h']
AddToConfigDic('DBContent', AddDBContent)

# TODO: Probably not needed any longer. Check that and remove if possible
class IgnoreConfigErrorArg(BoolArg):
    """Allow Resilience to catch exceptions from configuration errors"""
    def __init__(self, help = 'Allow Resilience to catch exceptions from configuration errors', name = 'IgnoreConfigError'):
        BoolArg.__init__(self, help, name)

    def isFullArgument(self):
        return True
def AddIgnoreConfigErrorArg(trf, inDic):
    trf.add( IgnoreConfigErrorArg() )
    return
AddToConfigDic('IgnoreConfigError', AddIgnoreConfigErrorArg)

class EnableLooperKillerArg(BoolArg):
    """Switch on the Looper Killer in G4Simulation (on by default)"""
    def __init__(self, help = 'default', name = 'enableLooperKiller'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddEnableLooperKillerArg(trf, inDic):
    trf.add( EnableLooperKillerArg() )#, default=True )
    return
AddEnableLooperKillerArg.subSteps = ['e2h']
AddToConfigDic('enableLooperKiller', AddEnableLooperKillerArg)

#######################


# Digitization specific arguments

class TestPileUpArg(BoolArg):
    """Calculates the number of background events that will be require for a given pile-up configuration"""
    def __init__(self, help = 'calculates the number of background events that will be require for a given pile-up configuration', name = 'testPileUpConfig'):
        BoolArg.__init__(self, help, name)

    def isFullArgument(self):
        return True
def AddTestPileUpArg(trf, inDic):
    trf.add( TestPileUpArg() )
    return
AddTestPileUpArg.subSteps = ['h2r']
AddToConfigDic('testPileUpConfig', AddTestPileUpArg)

def AddDigiSeedOffset1(trf, inDic):
    trf.add( RandomSeedArg(name = 'digiSeedOffset1', help = 'random seed offset for digitization') )
    return
AddDigiSeedOffset1.subSteps = ['h2r']
AddToConfigDic('digiSeedOffset1', AddDigiSeedOffset1)

def AddDigiSeedOffset2(trf, inDic):
    trf.add( RandomSeedArg(name = 'digiSeedOffset2', help = 'random seed offset for digitization') )
    return
AddDigiSeedOffset2.subSteps = ['h2r']
AddToConfigDic('digiSeedOffset2', AddDigiSeedOffset2)

def AddLowPtMinbiasHitsFile(trf, inDic):
    trf.add( InputHitsFileArg(name = 'LowPtMinbiasHitsFile', help = 'input hits for pile-up') )
    return
AddLowPtMinbiasHitsFile.subSteps = ['h2r']
AddToConfigDic('LowPtMinbiasHitsFile', AddLowPtMinbiasHitsFile)

def AddHighPtMinbiasHitsFile(trf, inDic):
    trf.add( InputHitsFileArg(name = 'HighPtMinbiasHitsFile', help = 'input hits for pile-up') )
    return
AddHighPtMinbiasHitsFile.subSteps = ['h2r']
AddToConfigDic('HighPtMinbiasHitsFile', AddHighPtMinbiasHitsFile)

def AddCavernHitsFile(trf, inDic):
    trf.add( InputHitsFileArg(name = 'cavernHitsFile',  help = 'input hits for cavern background') )
    return
AddCavernHitsFile.subSteps = ['h2r']
AddToConfigDic('cavernHitsFile', AddCavernHitsFile)

def AddBeamHaloHitsFile(trf, inDic):
    trf.add( InputHitsFileArg(name = 'beamHaloHitsFile', help = 'input hits for beam halo: side A - 98 percent, side C - 2 percent') )# , default='NONE' )
    return
AddBeamHaloHitsFile.subSteps = ['h2r']
AddToConfigDic('beamHaloHitsFile', AddBeamHaloHitsFile)

def AddBeamGasHitsFile(trf, inDic):
    trf.add( InputHitsFileArg(name = 'beamGasHitsFile', help = 'input hits for beam gas, already mixed and flipped C, H and O') )
    return
AddBeamGasHitsFile.subSteps = ['h2r']
AddToConfigDic('beamGasHitsFile', AddBeamGasHitsFile)

class DigiRndmSvcArg (StringChoicesArg):
    """random number service to use for digitization - AtRndmGenSvc uses Ranecu, AtRanluxGenSvc uses Ranlux64"""
    def __init__(self, choices, caseSensitive = False, help = 'default', name = 'digiRndmSvc'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddDigiRndmSvc(trf, inDic):
    trf.add( DigiRndmSvcArg( ['AtRndmGenSvc', 'AtRanluxGenSvc', 'AtDSFMTGenSvc']) )
    return
AddDigiRndmSvc.subSteps = ['h2r']
AddToConfigDic('digiRndmSvc', AddDigiRndmSvc)

class SamplingFractionDbTagArg (StringChoicesArg):
    """liquid argon calorimeter sampling fraction data base tag, passed on in jobOptions to LArfSamplG4Phys"""
    def __init__(self, choices, caseSensitive = False, help = 'default', name = 'samplingFractionDbTag'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddSamplingFractionDbTagArg(trf, inDic):
    trf.add( SamplingFractionDbTagArg(['QGSP_BERT', 'QGSP_EMV', 'QGSP', 'QGSP_BERT_EMV', 'FTFP_BERT', 'QGSP_FTFP_BERT', 'FTF_BIC', 'QGSP_BERT_CHIPS', 'QGSP_BIC', 'CHIPS']) )
    return
AddSamplingFractionDbTagArg.subSteps = ['h2r']
AddToConfigDic('samplingFractionDbTag', AddSamplingFractionDbTagArg)


#######################


## Pile-up Digitization specific arguments - repeat configuration usually done in digitization jobproperties

class NumberOfPileUpArg ( FloatArg ) :
    """Number of pile-up events"""
    def __init__(self, help = 'Number of pile-up events', name = 'default'):
        FloatArg.__init__(self, help, name)

    def isFullArgument(self):
        return True

def AddNumberOfLowPtMinBiasArg(trf, inDic):
    trf.add( NumberOfPileUpArg(name = 'numberOfLowPtMinBias', help = 'Number of low pt minbias events to add on average per bunch crossing') )
    return
AddNumberOfLowPtMinBiasArg.subSteps = ['h2r']
AddToConfigDic('numberOfLowPtMinBias', AddNumberOfLowPtMinBiasArg)

def AddNumberOfHighPtMinBiasArg(trf, inDic):
    trf.add( NumberOfPileUpArg(name = 'numberOfHighPtMinBias', help = 'Number of high pt minbias events to add on average per bunch crossing') )
    return
AddNumberOfHighPtMinBiasArg.subSteps = ['h2r']
AddToConfigDic('numberOfHighPtMinBias', AddNumberOfHighPtMinBiasArg)

def AddNumberOfBeamHaloArg(trf, inDic):
    trf.add( NumberOfPileUpArg(name = 'numberOfBeamHalo', help = 'Number of beam halo events to add on average per bunch crossing') )
    return
AddNumberOfBeamHaloArg.subSteps = ['h2r']
AddToConfigDic('numberOfBeamHalo', AddNumberOfBeamHaloArg)

def AddNumberOfBeamGasArg(trf, inDic):
    trf.add( NumberOfPileUpArg(name = 'numberOfBeamGas', help = 'Number of beam gas events to add on average per bunch crossing') )
    return
AddNumberOfBeamGasArg.subSteps = ['h2r']
AddToConfigDic('numberOfBeamGas', AddNumberOfBeamGasArg)

class NumberOfCavernBkgArg ( IntegerArg ) :
    """Number of cavern background events"""
    def __init__(self):
        IntegerArg.__init__(self, help = 'Number of cavern background events to add per bunch crossing', name = 'numberOfCavernBkg')

    def isFullArgument(self):
        return True
def AddNumberOfCavernBkgArg(trf, inDic):
    trf.add( NumberOfCavernBkgArg() )
    return
AddNumberOfCavernBkgArg.subSteps = ['h2r']
AddToConfigDic('numberOfCavernBkg', AddNumberOfCavernBkgArg)

class MeanBunchSpacingArg ( FloatArg ) :
    """The average spacing between filled bunches - using mean as this can will vary with bunch structure"""
    def __init__(self):
        FloatArg.__init__(self, help = 'The average spacing between filled bunches', name = 'bunchSpacing')

    def isFullArgument(self):
        return True

def AddMeanBunchSpacingArg(trf, inDic):
    trf.add( MeanBunchSpacingArg() )
    return
AddMeanBunchSpacingArg.subSteps = ['h2r']
AddToConfigDic('bunchSpacing', AddMeanBunchSpacingArg)

class RelativeBunchCrossingArg ( IntegerArg ) :
    """Bunch crossing number relative to signal interaction (in bunch crossing 0)"""
    def __init__(self, help = 'default', name = 'default'):
        IntegerArg.__init__(self, help, name)

    def isFullArgument(self):
        return True

def AddEarliestPileUpBunchCrossing(trf, inDic):
    trf.add( RelativeBunchCrossingArg(name = 'pileupInitialBunch', help = 'Initial (relative) bunch crossing to use pileup') )
    return
AddEarliestPileUpBunchCrossing.subSteps = ['h2r']
AddToConfigDic( 'pileupInitialBunch', AddEarliestPileUpBunchCrossing)

def AddLatestPileUpBunchCrossing(trf, inDic):
    trf.add( RelativeBunchCrossingArg(name = 'pileupFinalBunch', help = 'Final (relative) bunch crossing to use pileup') )
    return
AddLatestPileUpBunchCrossing.subSteps = ['h2r']
AddToConfigDic( 'pileupFinalBunch', AddLatestPileUpBunchCrossing)

#######################################################################

## def AddTriggerConfig(trf, inDic):
##     trf.add( TriggerConfigArg() )
##     return
## AddTriggerConfig.subSteps = ['h2r']
## AddToConfigDic('triggerConfig', AddTriggerConfig)

class NoiseControlArg (StringChoicesArg):
    """overall control of noise simualtion - useful for overlay jobs, will set doCaloNoise, doMuonNoise, doInDetNoise digitization jobproperties"""
    def __init__(self, choices, caseSensitive = False, help = 'override noise simulation flags in all subdetectors, [True/False]', name = 'doAllNoise'):
        StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True
def AddNoiseControlArg(trf, inDic):
    trf.add( NoiseControlArg(['True', 'False', 'NONE']) )
    return
AddNoiseControlArg.subSteps = ['h2r']
AddToConfigDic('doAllNoise', AddNoiseControlArg)

class AddCaloDigiArg(BoolArg):
    """Option to save Calo Digits too, not just RawChannels"""
    def __init__(self, help, name = 'AddCaloDigi'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddAddCaloDigiArg(trf, inDic):
    trf.add( AddCaloDigiArg ("True/False: Save CaloDigits as well, not just RawChannels") )
    return
AddAddCaloDigiArg.subSteps = ['h2r']
AddToConfigDic('AddCaloDigi', AddAddCaloDigiArg)

class DataRunNumberArg(IntegerArg):
    """Override existing run number with this value"""
    def __init__(self, help = 'Override existing run number with this value', name = 'DataRunNumber'):
        IntegerArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
def AddDataRunNumberArg(trf, inDic):
    trf.add( DataRunNumberArg() )
    return
AddDataRunNumberArg.subSteps = ['e2h','h2r']
AddToConfigDic('DataRunNumber', AddDataRunNumberArg)

class LucidOnArg(BoolArg):
    """Switch on lucid simulation/digitization"""
    def __init__(self, help, name = 'LucidOn'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddLucidOnArg(trf, inDic):
    trf.add( LucidOnArg ("Switch on lucid") )
    return
AddLucidOnArg.subSteps = ['e2h', 'h2r']
AddToConfigDic('LucidOn', AddLucidOnArg)

class AFPOnArg(BoolArg):
    """Switch on AFP simulation/digitization"""
    def __init__(self, help, name = 'AFPOn'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddAFPOnArg(trf, inDic):
    trf.add( AFPOnArg ("Switch on AFP") )
    return
AddAFPOnArg.subSteps = ['e2h', 'h2r']
AddToConfigDic('AFPOn', AddAFPOnArg)

class ALFAOnArg(BoolArg):
    """Switch on ALFA simulation/digitization"""
    def __init__(self, help, name = 'ALFAOn'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddALFAOnArg(trf, inDic):
    trf.add( ALFAOnArg ("Switch on ALFA") )
    return
AddALFAOnArg.subSteps = ['e2h', 'h2r']
AddToConfigDic('ALFAOn', AddALFAOnArg)

class ZDCOnArg(BoolArg):
    """Switch on ZDC simulation/digitization"""
    def __init__(self, help, name = 'ZDCOn'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddZDCOnArg(trf, inDic):
    trf.add( ZDCOnArg ("Switch on ZDC") )
    return
AddZDCOnArg.subSteps = ['e2h', 'h2r']
AddToConfigDic('ZDCOn', AddZDCOnArg)

class FwdRegionOnArg(BoolArg):
    """Switch on FwdRegion in simulation/digitization"""
    def __init__(self, help, name = 'FwdRegionOn'):
        BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'
def AddFwdRegionOnArg(trf, inDic):
    trf.add( FwdRegionOnArg ("Switch on FwdRegion") )
    return
AddFwdRegionOnArg.subSteps = ['e2h', 'h2r']
AddToConfigDic('FwdRegionOn', AddFwdRegionOnArg)


#######################


# RunDep specific arguments

class JobNumberArg(IntegerArg):
    """Number of this job in Task"""
    def __init__(self, help = 'default', name = 'default'):
        IntegerArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    def jobOrTask(self):
        return 'job'

def AddJobNumberArg(trf, inDic):
    trf.add(JobNumberArg())
    return
AddJobNumberArg.subSteps = ['e2h','h2r']
AddToConfigDic('jobNumber', AddJobNumberArg)


# FilterHit specific arguments

class TruthReductionSchemeArg(StringArg):
    """Method of Reducing the McEventCollection Size"""
    def __init__(self, help = 'default', name = 'TruthReductionScheme'):
        StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

def AddTruthReductionSchemeArg(trf, inDic):
    trf.add(TruthReductionSchemeArg())
    return
AddToConfigDic('TruthReductionScheme', AddTruthReductionSchemeArg)

# Help Functions

def addAllCommonArgs(trf, inDic):
    from PATJobTransforms.Configuration import AddBeamType, AddGeometryVersion, AddConditionsTag, AddMaxEvents, AddSkipEvents, AddPreInclude, AddPostInclude, AddPreExec, AddPostExec, AddDBRelease, AddAMITag
    AddMaxEvents(trf,inDic)
    AddSkipEvents(trf,inDic)
    AddPreInclude(trf,inDic)
    AddPostInclude(trf,inDic)
    AddPreExec(trf,inDic)
    AddPostExec(trf,inDic)
    AddGeometryVersion(trf,inDic)
    AddConditionsTag(trf,inDic)
    AddDBRelease(trf,inDic)
    AddBeamType(trf,inDic)
    AddAMITag(trf,inDic)
    AddDBContent(trf,inDic)
    AddIgnoreConfigErrorArg(trf,inDic)

def addAllFilterArgs(trf, inDic):
    from PATJobTransforms.Configuration import AddInputHITSFile, AddOutputHITFile
    AddInputHITSFile(trf,inDic)
    AddOutputHITFile(trf,inDic)
    addAllCommonArgs(trf, inDic)
    AddTruthReductionSchemeArg(trf, inDic)
    
def addAllG4Args(trf, inDic):
    from PATJobTransforms.Configuration import AddInputEvgenFile, AddOutputHITFile
    AddInputEvgenFile(trf,inDic)
    AddOutputHITFile(trf,inDic)
    addAllCommonArgs(trf, inDic)
    AddSimRandomSeedArg(trf,inDic)
    AddPhysicsListArg(trf,inDic)
    AddEnableLooperKillerArg(trf,inDic)
    AddCosmicFilterVolumeArg(trf,inDic)
    AddCosmicFilterVolume2Arg(trf,inDic)
    AddCosmicPtSliceArg(trf,inDic)
    AddUseISFArg(trf,inDic)
    AddFirstEventArg(trf,inDic)
    AddDataRunNumberArg(trf,inDic)
    AddLucidOnArg(trf,inDic)
    AddALFAOnArg(trf,inDic)
    AddZDCOnArg(trf,inDic)
    AddJobNumberArg(trf,inDic)
    AddAFPOnArg(trf,inDic)
    AddFwdRegionOnArg(trf,inDic)

def addAllDigiArgs(trf,inDic):
    from PATJobTransforms.Configuration import AddInputHITSFile, AddOutputRDOFile
    AddInputHITSFile(trf,inDic)
    AddOutputRDOFile(trf,inDic)
    addAllCommonArgs(trf, inDic)
    AddMeanBunchSpacingArg(trf,inDic)
    AddEarliestPileUpBunchCrossing(trf,inDic)
    AddLatestPileUpBunchCrossing(trf,inDic)
    AddNumberOfLowPtMinBiasArg(trf,inDic)
    AddNumberOfHighPtMinBiasArg(trf,inDic)
    AddNumberOfBeamHaloArg(trf,inDic)
    AddNumberOfBeamGasArg(trf,inDic)
    AddNumberOfCavernBkgArg(trf,inDic)
    AddLowPtMinbiasHitsFile(trf,inDic)
    AddHighPtMinbiasHitsFile(trf,inDic)
    AddCavernHitsFile(trf,inDic)
    AddBeamHaloHitsFile(trf,inDic)
    AddBeamGasHitsFile(trf,inDic)
    AddTestPileUpArg(trf,inDic)
    AddDigiSeedOffset1(trf,inDic)
    AddDigiSeedOffset2(trf,inDic)
    AddDigiRndmSvc(trf,inDic)
    AddSamplingFractionDbTagArg(trf,inDic)
    AddNoiseControlArg(trf,inDic)
    AddAddCaloDigiArg(trf,inDic)
    AddDataRunNumberArg(trf,inDic)
    AddLucidOnArg(trf,inDic)
    AddALFAOnArg(trf,inDic)
    AddZDCOnArg(trf,inDic)
    AddAFPOnArg(trf,inDic)
    AddFwdRegionOnArg(trf,inDic)
    AddJobNumberArg(trf,inDic)

def addAllSimArgs(trf,inDic):
    # Geant args
    from PATJobTransforms.Configuration import AddInputEvgenFile, AddOutputHITFile
    AddInputEvgenFile(trf,inDic)
    AddOutputHITFile(trf,inDic)
    addAllCommonArgs(trf, inDic)
    AddSimRandomSeedArg(trf,inDic)
    AddPhysicsListArg(trf,inDic)
    AddEnableLooperKillerArg(trf,inDic)
    AddCosmicFilterVolumeArg(trf,inDic)
    AddCosmicFilterVolume2Arg(trf,inDic)
    AddCosmicPtSliceArg(trf,inDic)
    AddFirstEventArg(trf,inDic)
    AddUseISFArg(trf,inDic)
    #Digitization args
    from PATJobTransforms.Configuration import AddInputHITSFile, AddOutputRDOFile
    AddInputHITSFile(trf,inDic)
    AddOutputRDOFile(trf,inDic)
    AddMeanBunchSpacingArg(trf,inDic)
    AddEarliestPileUpBunchCrossing(trf,inDic)
    AddLatestPileUpBunchCrossing(trf,inDic)
    AddNumberOfLowPtMinBiasArg(trf,inDic)
    AddNumberOfHighPtMinBiasArg(trf,inDic)
    AddNumberOfBeamHaloArg(trf,inDic)
    AddNumberOfBeamGasArg(trf,inDic)
    AddNumberOfCavernBkgArg(trf,inDic)
    AddLowPtMinbiasHitsFile(trf,inDic)
    AddHighPtMinbiasHitsFile(trf,inDic)
    AddCavernHitsFile(trf,inDic)
    AddBeamHaloHitsFile(trf,inDic)
    AddBeamGasHitsFile(trf,inDic)
    AddTestPileUpArg(trf,inDic)
    AddDigiSeedOffset1(trf,inDic)
    AddDigiSeedOffset2(trf,inDic)
    AddDigiRndmSvc(trf,inDic)
    AddSamplingFractionDbTagArg(trf,inDic)
    AddNoiseControlArg(trf,inDic)
    AddAddCaloDigiArg(trf,inDic)
    AddDataRunNumberArg(trf,inDic)
    AddLucidOnArg(trf,inDic)
    AddALFAOnArg(trf,inDic)
    AddZDCOnArg(trf,inDic)
    AddAFPOnArg(trf,inDic)
    AddFwdRegionOnArg(trf,inDic)
    AddJobNumberArg(trf,inDic)


## def Add(trf,inDic):
##     trf.add( BasicStringArg(name='AMITag', help='Store the AMITag for this step'), default='NONE' )
##     return
## Add.subSteps = ['h2r']
## AddToConfigDic('',)
