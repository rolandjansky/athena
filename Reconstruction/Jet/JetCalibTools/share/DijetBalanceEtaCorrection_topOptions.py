from JetRec.JetGetters import *  

from JetRec.JetRecConf import JetAlgorithm
JetAlg = JetAlgorithm("MyAlgName")   # Create a JetAlgorithm
JetAlg.InputCollectionNames = [ "AntiKt4TopoJets" ]


from JetCalibTools.JetCalibToolsConf import *
#instantiate a calibration JetAlgTool

jetcalibrationtool = JetCalibrationTool("EtaCorrection")
jetcalibrationtool.Calibrator = DijetBalanceEtaCorrection()
jetcalibrationtool.Calibrator.CalibrationTag = "DijetPTBalanceEta"
jetcalibrationtool.Calibrator.WriteMoments = True
jetcalibrationtool.InplaceCorrection = True 
jetcalibrationtool.OutputLevel=6

jetcalibrationtool.Calibrator.EtaBins =[-3.0,-2.8,-2.6,-2.4,-2.2,-2.0,-1.8,-1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,0.,0.2, 0.4, 0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.2,2.4,2.6,2.8,3.0]

# AntiKt4TopoJets at em scale

jetcalibrationtool.Calibrator.CalibConstants="[[1.29146,1.22821,0.899143,0.823869,0.775549,0.862948,0.833784,0.965609,1.02129,0.714322,0.865088,0.957846,1.03681,0.885025,1.,1.,1.,1.02068,0.788418,1.05939,0.754231,0.729996,1.09421,0.950566,0.774641,0.768853,0.738289,0.910829,0.778498,0.867927,0.815018][-0.000660717,-0.000411808,0.000338774,0.000297661,0.000321868,1.61905e-05,0.000130294,-6.49354e-05,-1.23328e-05,0.000173153,0.00021238,-5.27866e-06,-6.35941e-05,0.000117292,-5.33953e-13,-4.49612e-13,-3.36587e-13,-5.82993e-05,0.000270939,-5.2871e-06,0.000311626,0.000295411,-0.000126589,-4.34092e-05,0.000154797,0.000204391,0.000338916,0.000235834,0.000539915,0.000331451,0.000741356][-1.35381,-1.29698,-0.199561,0.102692,0.354533,0.239333,0.428348,0.113625,0.212299,1.24094,0.647845,0.354048,-0.122398,0.412195,2.66527e-10,1.68158e-10,1.8355e-10,-0.107958,0.817558,-0.18274,1.0969,1.043,-0.0697594,0.151747,0.699638,0.560779,0.544435,-0.247577,0.253553,-0.0971621,0.151406]]"


JetAlg.AlgTools += [jetcalibrationtool]