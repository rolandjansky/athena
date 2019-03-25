
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Authors: joao victor pinto <jodafons@cern.ch>


from TrigMultiVarHypo.TrigMultiVarHypoConf        import TrigL2CaloRingerFex, TrigL2CaloRingerHypo
from AthenaCommon.SystemOfUnits                   import GeV
from TrigMultiVarHypo.TrigL2CaloRingerHypoTool    import _GetPath
from TrigMultiVarHypo.TrigL2CaloRingerHypoMonitoring import TrigL2CaloRingerHypoValidationMonitoring, \
                                                            TrigL2CaloRingerHypoOnlineMonitoring,\
                                                            TrigL2CaloRingerHypoTimeMonitoring,\
                                                            TrigL2CaloRingerFexTimeMonitoring
 

# Call all Trigger Ringer Fast configurations here!
class TrigL2CaloRingerFexBase( TrigL2CaloRingerFex ):
  __slots__ = []
  def __init__(self, name):
    super(TrigL2CaloRingerFexBase,self).__init__( name )

    self.HltFeature = "TrigRingerNeuralFex"
    self.Feature    = "TrigT2CaloEgamma"
    
    #time       = TrigL2CaloRingerFexTimeMonitoring()
    #self.AthenaMonTools = [ time ]
    #self.doTiming = True


#  The multi layer perceptron and preprocessing step is called by
#  Fex because we store the output network. So to complete the trigger
#  chain you must uses the Fex and Hypo together.
class TrigL2CaloRingerFex( TrigL2CaloRingerFexBase ):
  __slots__ = []
  def __init__(self, name, threshold, IDinfo, trigType):
    super( TrigL2CaloRingerFex, self ).__init__( name ) 

    from AthenaCommon.AppMgr import ToolSvc
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
    ToolSvc += LuminosityToolOnline()
    pconstants,pthresholds=_GetPath(trigType,IDinfo)
    #if this is empty, the fex will work like EtCut
    self.CalibPath =pconstants
   



class TrigL2CaloRingerFex_Empty( TrigL2CaloRingerFexBase ):
  __slots__ = []
  def __init__(self, name):
    super( TrigL2CaloRingerFex_Empty, self ).__init__( name ) 



#  Hypothesis group for electron trigger, NoCut step (bypass) and
#  photons chains for future. Because the neural output store,
#  you must put the Fex and Hypo together in the same sequence
#  list. For example:
#    L2SequenceList = [theT2CaloEgamma_Ringer, 
#    theTrigL2CaloRingerFex, theTrigL2CaloRingerHypo]
#
#  the same configuration must be passed to fex and hypo when
#  you create the objects.

class TrigL2CaloRingerHypo_e_ID(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name, threshold, IDinfo, trigType):
    super( TrigL2CaloRingerHypo_e_ID, self ).__init__( name ) 

    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = False
    self.EmEtCut    = (float(threshold) - 3)*GeV
    pconstants,pthresholds=_GetPath(trigType,IDinfo)
    self.CalibPath = pthresholds
    #validation = TrigL2CaloRingerHypoValidationMonitoring()       
    #online     = TrigL2CaloRingerHypoOnlineMonitoring()
    #time       = TrigL2CaloRingerHypoTimeMonitoring()
    #self.AthenaMonTools = [ time, validation, online ]
    #self.doTiming = True



class TrigL2CaloRingerHypo_g_ID(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name, threshold, IDinfo, trigType):
    super( TrigL2CaloRingerHypo_g_ID, self ).__init__( name ) 

    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = False
    self.EmEtCut    = (float(threshold) - 3)*GeV
    pconstants,pthresholds=_GetPath(trigType,IDinfo)
    self.CalibPath = pthresholds
    #validation = TrigL2CaloRingerHypoValidationMonitoring()       
    #online     = TrigL2CaloRingerHypoOnlineMonitoring()
    #time       = TrigL2CaloRingerHypoTimeMonitoring()
    #self.AthenaMonTools = [ time, validation, online ]
    #self.doTiming = True



class TrigL2CaloRingerHypo_NoCut(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name):
    super( TrigL2CaloRingerHypo_NoCut, self ).__init__( name ) 
    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = True
    #validation = TrigL2CaloRingerHypoValidationMonitoring()       
    #online     = TrigL2CaloRingerHypoOnlineMonitoring()
    #time       = TrigL2CaloRingerHypoTimeMonitoring()
    #self.AthenaMonTools = [ time, validation, online ]
    #self.doTiming = True


class TrigL2CaloRingerHypo_EtCut(TrigL2CaloRingerHypo):
  __slots__ = []

  def __init__(self, name, threshold):
    super( TrigL2CaloRingerHypo_EtCut, self ).__init__( name ) 
    self.HltFeature = 'TrigRingerNeuralFex'
    self.AcceptAll  = False
    self.EmEtCut    = (float(threshold) - 3)*GeV
    #validation = TrigL2CaloRingerHypoValidationMonitoring()       
    #online     = TrigL2CaloRingerHypoOnlineMonitoring()
    #time       = TrigL2CaloRingerHypoTimeMonitoring()
    #self.AthenaMonTools = [ time, validation, online ]
    #self.doTiming = True




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










