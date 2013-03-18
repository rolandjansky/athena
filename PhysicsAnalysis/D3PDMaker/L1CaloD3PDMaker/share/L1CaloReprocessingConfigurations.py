### Define the configurations for the L1Calo Configuration Testing Tool

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ReprocessTriggerTowers as repoTT

### Class that defines the basics of all configurations
class L1CaloConfiguration(object):
  """Configure the L1CaloConfigurationTestingTool -  Author John Morris <john.morris@cern.ch>"""

  def __init__(self,name):
    self.name = name
    self.tts = repoTT(self.name)
    self.tts.doBCID=False
    self.tts.LUTStrategy = 1
    ### 2011 Settings
    ###self.tts.EMBThresh    = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    ###self.tts.EMECThresh   = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    ###self.tts.FCAL1Thresh  = [4000,4000,4000,4000]
    ###self.tts.TileThresh   = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    ###self.tts.HECThresh    = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    ###self.tts.FCAL23Thresh = [4000,4000,4000,4000]

    ### 2012 Settings
    self.tts.EMBThresh    = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    self.tts.EMECThresh   = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,11400,11700,10800,6100]
    self.tts.FCAL1Thresh  = [4500,13500,36500,44600]
    self.tts.TileThresh   = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    self.tts.HECThresh    = [4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
    self.tts.FCAL23Thresh = [6600,4100,28700,10900]    
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
  
  ### Let's change nothing and make sure we get the same results as
  ### the original. This is a sanity check.
  configSame = L1CaloConfiguration('same');
  algos.append(configSame)

  ### Example A - Set EMB to 95%
  configA = L1CaloConfiguration('configA')
  configA.tts.EMBScale     = [0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.95]
  algos.append(configA)

  ### Example B - Set EMB to 105%
  configB = L1CaloConfiguration('configB')
  configB.tts.EMBScale     = [1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05,1.05]
  algos.append(configB)

  ### Example C - lets do make up some crazy scales
  crazy = L1CaloConfiguration('crazy')
  crazy.tts.EMBScale     = [1.2,1.1,1.,0.9,0.8,0.9,1.,1.1,1.2,1.3,1.1,0.7,0.85,0.7,1.4]
  crazy.tts.EMECScale    = [0.7,1.4,1.2,0.8,1.6,1.2,1.3,0.95,0.8,0.6,1.1,1.1,1.3,1.3]
  algos.append(crazy)

  ### Example D - High noise in the FCAL
  fcalA = L1CaloConfiguration('fcalA')
  fcalA.tts.FCAL1Thresh  = [8000,8000,8000,8000]
  fcalA.tts.FCAL23Thresh = [8000,8000,8000,8000]
  algos.append(fcalA)

  ### Example E - High noise in the FCAL
  fcalB = L1CaloConfiguration('fcalB')
  fcalB.tts.FCAL1Thresh  = [12000,12000,12000,12000]
  fcalB.tts.FCAL23Thresh = [12000,12000,12000,12000]
  algos.append(fcalB)

  
  return algos

### Add your configurations to the jobSequence and return a list of names to feed into
### the D3PDMaker
def L1CaloConfig(job):
  algos = myConfigs()

  names = []
  for i in algos:
    names.append(i.name)
    job += i.tts
  return names

