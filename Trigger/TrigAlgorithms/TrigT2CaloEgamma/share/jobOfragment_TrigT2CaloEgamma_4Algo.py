#----------------------------------------------------
#  T2CaloEgamma options to run in four steps
#----------------------------------------------------
theApp.Dlls += [ "TrigT2CaloEgamma" ]

include( "TrigT2CaloCommon/jobOfragment_TrigT2CaloCommon_4Algo.py" )

T2CaloEgamma_g4_L2 = Algorithm( "T2CaloEgamma_g4_L2" )
T2CaloEgamma_g4_L2.EtaWidth = 0.2
T2CaloEgamma_g4_L2.PhiWidth = 0.2
T2CaloEgamma_g4_L2.T2CaloTools=["CaloSamp2FexEgamma","T2CaloL1Sim"]
T2CaloEgamma_g4_L2.TimerNtuple="T2Calo.T2CaEgSmp2"
#T2CaloEgamma_g4_L2.T2CaloTools=["T2CaloL1Sim"]
T2CaloSamp1En_1Egamma_L2 = Algorithm( "T2CaloSamp1En_1Egamma_L2" )
T2CaloSamp1En_1Egamma_L2.T2CaloTools=["CaloSamp1FexEgamma"]
T2CaloSamp1En_1Egamma_L2.TimerNtuple="T2Calo.T2CaEgSmp1"
T2CaloSamp1En_1Egamma_L2.TriggerOn="Egamma"
T2CaloSamp1En_2Egamma_L2 = Algorithm( "T2CaloSamp1En_2Egamma_L2" )
T2CaloSamp1En_2Egamma_L2.T2CaloTools=["CaloEmEnFexEgamma"]
T2CaloSamp1En_2Egamma_L2.TimerNtuple="T2Calo.T2CaEgEmEn"
T2CaloSamp1En_2Egamma_L2.TriggerOn="Egamma"
T2CaloSamp1En_3Egamma_L2 = Algorithm( "T2CaloSamp1En_3Egamma_L2" )
T2CaloSamp1En_3Egamma_L2.T2CaloTools=["CaloHadEnFexEgamma"]
T2CaloSamp1En_3Egamma_L2.TimerNtuple="T2Calo.T2CaEgHdEn"
T2CaloSamp1En_3Egamma_L2.TriggerOn="Egamma"
T2CaloEgamma_g4_L2.EMShowerMinsKey="T2CaloEMShowerMins"
T2CaloEgamma_g4_L2.CaloClustersKey="T2CaloEgammaCaloClusters"
T2CaloEgamma_g4_L2.OutputLevel = INFO


