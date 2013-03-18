### Define the configurations for the L1Calo Configuration Testing Tool
include("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ReprocessTriggerTowers as repoTT

### Class that defines the basics of all configurations
class L1CaloConfiguration(object):
  """Configure the L1CaloConfigurationTestingTool -  Author John Morris <john.morris@cern.ch>"""

  def __init__(self,name):
    self.name = name
    self.tts = repoTT(self.name)
    self.tts.doBCID=False
    self.tts.LUTStrategy = 1
    self.tts.emThreshold  = 4000
    self.tts.hadThreshold = 4000
    self.tts.EMBScale     = [1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.]
    self.tts.EMECScale    = [1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.]
    self.tts.FCAL1Scale   = [1.,1.,1.,1.]
    self.tts.TileScale    = [1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.]
    self.tts.HECScale     = [1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.]
    self.tts.FCAL23Scale  = [1.,1.,1.,1.]
    self.tts.OutputTriggerTowerLocation = 'TriggerTowers_' + self.name


### Setup your list of configurations here
### A default configuration will match the class above
### Pick a name and then overload the values with what you want
### Remember to append your configuration to the algos list
def myConfigs():
  algos = []

  # unscaled DM Corrections
  DM = L1CaloConfiguration('dm')
  dmExt = 1.00001
  DM.tts.EMBScale           = [1.0013,1.0001,0.9993,0.9979,1.0009,1.0006,1.0069,1.0192,1.0574,1.0615,1.0717,1.0796,1.0724,1.0893,1.0]
  DM.tts.EMECScale          = [1.0033,1.0905,0.9860,1.0123,1.0048,0.9956,1.0017,1.0040,1.0098,1.0130,dmExt,dmExt,dmExt,dmExt,dmExt]
  DM.tts.FCAL1Scale         = [dmExt,dmExt,dmExt,dmExt]
  algos.append(DM)
  
  # Scaled DM Corrections
  DMScaled = L1CaloConfiguration('dmScaled')
  dmsExt = 1.05976
  DMScaled.tts.EMBScale     = [1.06113,1.05988,1.059,1.05749,1.06069,1.06037,1.0671,1.08011,1.12057,1.12491,1.13572,1.14411,1.13653,1.15444,1.0]
  DMScaled.tts.EMECScale    = [1.0633,1.15572,1.04493,1.07278,1.06488,1.0551,1.06157,1.06399,1.07013,1.07353,dmsExt,dmsExt,dmsExt,dmsExt,dmsExt]
  DMScaled.tts.FCAL1Scale   = [dmsExt,dmsExt,dmsExt,dmsExt]
  algos.append(DMScaled)
  
  return algos

### Add your configurations to the jobSequence and return a list of names to feed into
### the D3PDMaker
def L1CaloConfig(job):
  algos = myConfigs()

  names = []
  for i in algos:
    names.append(i.name)
    job += i.tts
  pass

