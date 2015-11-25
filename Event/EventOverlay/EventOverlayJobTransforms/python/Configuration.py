# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## -*- python -*-

from PyJobTransformsCore.full_trfarg import *
from PATJobTransforms.Configuration import *

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
