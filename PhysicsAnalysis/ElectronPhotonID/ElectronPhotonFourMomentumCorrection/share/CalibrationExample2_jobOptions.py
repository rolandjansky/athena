# Set up the file reading:
FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

ToolSvc += CfgMgr.CP__EgammaCalibrationAndSmearingTool("egammaCalibTool",ESModel="es2012c")
theJob = CfgMgr.AthSequencer("AthAlgSeq")

theJob += CfgMgr.CP__CalibratedEgammaProvider("ElectronCalibrator",Tool=ToolSvc.egammaCalibTool,Input="ElectronCollection",Output="CalibratedElectrons",OutputLevel=VERBOSE)
theJob += CfgMgr.CP__CalibratedEgammaProvider("PhotonCalibrator",Tool=ToolSvc.egammaCalibTool,Input="PhotonCollection",Output="CalibratedPhotons",OutputLevel=VERBOSE)

#example of using auto-configuration of tool: don't pass a tool and the alg auto-configures one for you
theJob += CfgMgr.CP__CalibratedEgammaProvider("AutoElectronCalibrator",Input="ElectronCollection",Output="AutoCalibratedElectrons",OutputLevel=VERBOSE)

theApp.EvtMax = 5
ServiceMgr.MessageSvc.defaultLimit = 1000000
