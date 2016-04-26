# Set up the file reading:
import os
FNAME = os.environ['ASG_TEST_FILE_MC']

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

ToolSvc += CfgMgr.CP__EgammaCalibrationAndSmearingTool("egammaCalibTool",ESModel="es2015cPRE")
theJob = CfgMgr.AthSequencer("AthAlgSeq")

theJob += CfgMgr.CP__CalibratedEgammaProvider("ElectronCalibrator",Tool=ToolSvc.egammaCalibTool,Input="Electrons",Output="CalibratedElectrons",OutputLevel=VERBOSE)
theJob += CfgMgr.CP__CalibratedEgammaProvider("PhotonCalibrator",Tool=ToolSvc.egammaCalibTool,Input="Photons",Output="CalibratedPhotons",OutputLevel=VERBOSE)

#example of using auto-configuration of tool: don't pass a tool and the alg auto-configures one for you
theJob += CfgMgr.CP__CalibratedEgammaProvider("AutoElectronCalibrator",Input="Electrons",Output="AutoCalibratedElectrons",OutputLevel=VERBOSE)

theApp.EvtMax = 5
ServiceMgr.MessageSvc.defaultLimit = 1000000
