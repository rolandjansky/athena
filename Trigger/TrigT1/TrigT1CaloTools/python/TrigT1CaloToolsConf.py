from AthenaCommon.Configurable import *

class LVL1__L1EmTauTools( ConfigurableAlgTool ) :
  __slots__ = { 
    'OutputLevel' : 7, # int
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditFinalize' : False, # bool
    'TriggerTowerLocation' : 'LVL1TriggerTowers', # str
  }
  _propertyDocDct = { 
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(LVL1__L1EmTauTools, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TrigT1CaloTools'
  def getType( self ):
      return 'LVL1::L1EmTauTools'
  pass # class LVL1__L1EmTauTools

class LVL1__L1JetTools( ConfigurableAlgTool ) :
  __slots__ = { 
    'OutputLevel' : 7, # int
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditFinalize' : False, # bool
    'JetElementLocation' : 'LVL1JetElements', # str
  }
  _propertyDocDct = { 
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(LVL1__L1JetTools, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TrigT1CaloTools'
  def getType( self ):
      return 'LVL1::L1JetTools'
  pass # class LVL1__L1JetTools
