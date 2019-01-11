## ProtosLHEF_i Interface 
from ProtosLHEF_i.ProtosLHEF_iConf import protos2lhef

genSeq += protos2lhef()
evgenConfig.generators += ["ProtosLHEF"] 
