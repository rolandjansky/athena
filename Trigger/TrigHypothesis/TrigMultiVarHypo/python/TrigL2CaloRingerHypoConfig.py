
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# Authors: joao victor pinto <jodafons@cern.ch>

from TrigMultiVarHypo.TrigMultiVarHypoConf        import TrigL2CaloRingerFex, TrigL2CaloRingerHypo
from AthenaCommon.SystemOfUnits                   import GeV



class TrigL2CaloRingerPidConfs:

  """
    This class will be responsible to build all calib
    paths and point the hypo to the ATLAS calib area files.
    This will be implemented here for now. To switch between
    tuning, you just need add this in your job option
    e.g:
    from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
    EgammaSliceFlags.ringerVersion = 'RingerSelectorTools/file_of_your_ringer_confs'
  """

  _signatureDict = {
      'signature' : ['e','g'],
  }

  _pidMap = {
      'tight'   : 'tight' , 
      'medium'  :'medium' , 
      'loose'   :'loose'  , 
      'vloose'  :'vloose' ,
      'lhtight' : 'tight' , 
      'lhmedium': 'medium', 
      'lhloose' :'loose'  , 
      'lhvloose': 'vloose'
      }

  # The default path for ringer selectors. The standard cuts will be the ringer v6 tuning for now.
  # Last modification in: 2017/02/21
  # MC15c tuning if pileup corrention using data 2016 (periods A to K) to fix all thresholds values
  _default_basepath = 'RingerSelectorTools/TrigL2_20170221_v6'

  def __init__(self):
    
    from AthenaCommon.Logging import logging
    self._logger = logging.getLogger("TrigMultiVarHypo.TrigL2CaloRingerPidConfs")

    #TODO: Use this to configurate the calib path from triggerMenu flags
    from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
    if EgammaSliceFlags.ringerVersion():
      self._basePath = EgammaSliceFlags.ringerVersion()
      self._logger.info('TrigMultiVarHypo version: %s',self._basePath)
    else:
      self._basePath = self._default_basepath
      self._logger.info('TrigMultiVarHypo version: %s (default)',self._basePath)

    # Electron files
    self._electronConstants = {
        'vloose' : 'TrigL2CaloRingerElectronVeryLooseConstants.root',
        'loose'  : 'TrigL2CaloRingerElectronLooseConstants.root',
        'medium' : 'TrigL2CaloRingerElectronMediumConstants.root',
        'tight'  : 'TrigL2CaloRingerElectronTightConstants.root',
        }
    self._electronCutDefs = {
        'vloose' : 'TrigL2CaloRingerElectronVeryLooseThresholds.root',
        'loose'  : 'TrigL2CaloRingerElectronLooseThresholds.root',
        'medium' : 'TrigL2CaloRingerElectronMediumThresholds.root',
        'tight'  : 'TrigL2CaloRingerElectronTightThresholds.root',
        }

    #TODO: photon paths for future
    self._photonConstants = {
        'vloose' : 'TrigL2CaloRingerPhotonVeryLooseConstants.root',
        'loose'  : 'TrigL2CaloRingerPhotonLooseConstants.root',
        'medium' : 'TrigL2CaloRingerPhotonMediumConstants.root',
        'tight'  : 'TrigL2CaloRingerPhotonTightConstants.root',
        }
    self._photonCutDefs = {
        'vloose' : 'TrigL2CaloRingerPhotonVeryLooseThresholds.root',
        'loose'  : 'TrigL2CaloRingerPhotonLooseThresholds.root',
        'medium' : 'TrigL2CaloRingerPhotonMediumThresholds.root',
        'tight'  : 'TrigL2CaloRingerPhotonTightThresholds.root',
        }

    
  def get_constants_path(self, trigType, IDinfo):
    if not (trigType[0] in self._signatureDict['signature']):
      raise RuntimeError('Bad signature')
    # is Electron
    if self._signatureDict['signature'][0] in trigType:
      return self._basePath + '/' +self._electronConstants[self._pidMap[IDinfo]]
    else: #is Photon
      #TODO: this will be uncoment when we have photon tuning 
      #return self._basePath + '/' +self._photonConstants[self._pidMap[IDinfo]]
      return str()

  def get_cutDefs_path(self, trigType, IDinfo):
    if not (trigType[0] in self._signatureDict['signature']):
      raise RuntimeError('Bad signature')
    # is Electron
    if self._signatureDict['signature'][0] in trigType:
      return self._basePath + '/' +self._electronCutDefs[self._pidMap[IDinfo]]
    else: #is Photon
      #TODO: this will be uncoment when we have photon tuning 
      #return self._basePath + '/' +self._photonCutDefs[self._pidMap[IDinfo]]
      return str() 

# Instance object here!
# For future use this object to call all paths
theRingerConfig = TrigL2CaloRingerPidConfs()


# Call all Trigger Ringer Fast configurations here!
class TrigL2CaloRingerFexBase( TrigL2CaloRingerFex ):
  __slots__ = []
  def __init__(self, name):
    super(TrigL2CaloRingerFexBase,self).__init__( name )

    self.HltFeature = "TrigRingerNeuralFex"
    self.Feature    = "TrigT2CaloEgamma"
    from TrigMultiVarHypo.TrigL2CaloRingerHypoMonitoring import TrigL2CaloRingerHypoValidationMonitoring, \
                                                                TrigL2CaloRingerHypoOnlineMonitoring,\
                                                                TrigL2CaloRingerHypoTimeMonitoring
    validation = TrigL2CaloRingerHypoValidationMonitoring()       
    online     = TrigL2CaloRingerHypoOnlineMonitoring()
    time       = TrigL2CaloRingerHypoTimeMonitoring()
    
    self.AthenaMonTools = [ time, validation, online ]
    self.doTiming = True

'''
  The multi layer perceptron and preprocessing step is called by
  Fex because we store the output network. So to complete the trigger
  chain you must uses the Fex and Hypo together.
'''
class TrigL2CaloRingerFex( TrigL2CaloRingerFexBase ):
  __slots__ = []
  def __init__(self, name, threshold, IDinfo, trigType):
    super( TrigL2CaloRingerFex, self ).__init__( name ) 

    from AthenaCommon.AppMgr import ToolSvc
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
    ToolSvc += LuminosityToolOnline()
    #if this is empty, the fex will work like EtCut
    self.CalibPath = theRingerConfig.get_constants_path(trigType, IDinfo) 
   
    # The standard preproc to normalize the rings
    from TrigMultiVarHypo.TrigRingerPreprocessorDefs import Norm1
    preproc = Norm1()

    self.SectionRings = preproc.SectionRings
    self.NormalisationRings =  preproc.NormalisationRings
    self.NRings = preproc.NRings



class TrigL2CaloRingerFex_Empty( TrigL2CaloRingerFexBase ):
  __slots__ = []
  def __init__(self, name):
    super( TrigL2CaloRingerFex_Empty, self ).__init__( name ) 



'''
  Hypothesis group for electron trigger, NoCut step (bypass) and
  photons chains for future. Because the neural output store,
  you must put the Fex and Hypo together in the same sequence
  list. For example:
    L2SequenceList = [theT2CaloEgamma_Ringer, 
    theTrigL2CaloRingerFex, theTrigL2CaloRingerHypo]

  the same configuration must be passed to fex and hypo when
  you create the objects.
'''
class TrigL2CaloRingerHypo_e_ID(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name, threshold, IDinfo, trigType):
    super( TrigL2CaloRingerHypo_e_ID, self ).__init__( name ) 

    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = False
    self.EmEtCut    = (float(threshold) - 3)*GeV
    self.CalibPath = theRingerConfig.get_cutDefs_path(trigType, IDinfo) 



class TrigL2CaloRingerHypo_g_ID(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name, threshold, IDinfo, trigType):
    super( TrigL2CaloRingerHypo_g_ID, self ).__init__( name ) 

    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = False
    self.EmEtCut    = (float(threshold) - 3)*GeV
    self.CalibPath = theRingerConfig.get_cutDefs_path(trigType, IDinfo) 


class TrigL2CaloRingerHypo_NoCut(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name):
    super( TrigL2CaloRingerHypo_NoCut, self ).__init__( name ) 
    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = True

class TrigL2CaloRingerHypo_EtCut(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name, threshold):
    super( TrigL2CaloRingerHypo_EtCut, self ).__init__( name ) 
    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = False
    self.EmEtCut    = (float(threshold) - 3)*GeV



#helper function for combined
def TrigL2CaloRingerFexHypo_e_ID(threshold, IDinfo, trigType):
  return  TrigL2CaloRingerFex("TrigL2CaloRingerFex_e"+str(threshold)+'_'+IDinfo, threshold,IDinfo,trigType),\
          TrigL2CaloRingerHypo_e_ID("TrigL2CaloRingerHypo_e"+str(threshold)+'_'+str(IDinfo), threshold,IDinfo,trigType)

#helper function for combined
def TrigL2CaloRingerFexHypo_g_ID(threshold, IDinfo, trigType):
  return  TrigL2CaloRingerFex("TrigL2CaloRingerFex_g"+str(threshold)+'_'+IDinfo, threshold,IDinfo,trigType),\
          TrigL2CaloRingerHypo_g_ID("TrigL2CaloRingerHypo_g"+str(threshold)+'_'+str(IDinfo), threshold,IDinfo,trigType)

#helper function for combined
def TrigL2CaloRingerFexHypo_e_NoCut(threshold):
  return  TrigL2CaloRingerFex_Empty("TrigL2CaloRingerFex_e"+str(threshold)+'_Empty'),\
          TrigL2CaloRingerHypo_NoCut("TrigL2CaloRingerHypo_e"+str(threshold)+'_NoCut')

#helper function for combined
def TrigL2CaloRingerFexHypo_e_EtCut(threshold):
  return  TrigL2CaloRingerFex_Empty("TrigL2CaloRingerFex_e"+str(threshold)+'_Empty'),\
          TrigL2CaloRingerHypo_EtCut("TrigL2CaloRingerHypo_e"+str(threshold)+'_EtCut', threshold)

#helper function for combined
def TrigL2CaloRingerFexHypo_g_NoCut(threshold):
  return  TrigL2CaloRingerFex_Empty("TrigL2CaloRingerFex_g"+str(threshold)+'_Empty'),\
          TrigL2CaloRingerHypo_NoCut("TrigL2CaloRingerHypo_g"+str(threshold)+'_NoCut')

#helper function for combined
def TrigL2CaloRingerFexHypo_g_EtCut(threshold):
  return  TrigL2CaloRingerFex_Empty("TrigL2CaloRingerFex_g"+str(threshold)+'_Empty'),\
          TrigL2CaloRingerHypo_EtCut("TrigL2CaloRingerHypo_g"+str(threshold)+'_EtCut', threshold)











