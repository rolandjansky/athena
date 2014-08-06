#----------------------------------------------------
#  T2CaloEgamma options
#----------------------------------------------------
theApp.Dlls += [ "TrigT2CaloEgamma" ]

include( "TrigT2CaloCommon/jobOfragment_TrigT2CaloCommon.py" )
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaSamp2Fex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaSamp1Fex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaEmEnFex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaHadEnFex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgamma_g4_L2

#T2CaloEgamma_g4_L2 = Algorithm( "T2CaloEgamma_g4_L2" )
#T2CaloEgamma_g4_L2.EtaWidth = 0.2
#T2CaloEgamma_g4_L2.PhiWidth = 0.2
#T2CaloEgamma_g4_L2.IAlgToolList=[EgammaSamp2Fex("1"),
#				 EgammaSamp1Fex("2"),
#				 EgammaEmEnFex("3") ,
#				 EgammaHadEnFex("4")]
#T2CaloEgamma_g4_L2.TimerNtuple="T2CaloEgamma.T2CaEgtTot"
#T2CaloEgamma_g4_L2.TrigEMClusterKey="T2CaloTrigEMCluster"
#T2CaloEgamma_g4_L2.Monitoring=False
#T2CaloEgamma_g4_L2.OutputLevel = INFO

#include( "TrigT2CaloEgamma/jobOfragment_TrigT2CaloEgamma_Calib.py")
