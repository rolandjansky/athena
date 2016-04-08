## ProtosLHEF_i Interface 
from ProtosLHEF_i.ProtosLHEF_iConf import protos2lhef
topAlg += protos2lhef()
evgenConfig.generators += ["Lhef"] # ! need to define 'ProtosLHEF' in the lhefGenerators list
