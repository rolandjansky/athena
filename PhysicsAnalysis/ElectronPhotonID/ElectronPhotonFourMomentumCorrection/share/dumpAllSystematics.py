import AthenaPoolCnvSvc.ReadAthenaPool  # TODO: best option?
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr

import cppyy
try :
    cppyy.loadDictionary("EgammaAnalysisInterfacesDict")
except :
    pass

from ROOT import EgammaCalibPeriodRunNumbersExample

### configure here (or configure calibration tool by yourself)
from glob import glob
ServiceMgr.EventSelector.InputCollections = glob("/afs/cern.ch/work/t/turra/mc15_13TeV_electrons/*")
#ServiceMgr.EventSelector.InputCollections = glob("/afs/cern.ch/work/t/turra/mc15_13TeV.423000.ParticleGun_single_electron_egammaET.merge.AOD.e3566_a766_a820_r7676_tid10416099_00/*")  # fast sim
#particle = "electron"  # electron or photon
#container_name = "Electrons"

ServiceMgr.EventSelector.InputCollections = glob("/afs/cern.ch/work/t/turra/mc15_13TeV.423001.ParticleGun_single_photon_egammaET.merge.AOD.e3566_s2726_r7772_r7676/*")
particle = "photon"
container_name = "Photons"

tools = [CfgMgr.CP__EgammaCalibrationAndSmearingTool("calib_es2016PRE",
                                                     ESModel="es2016PRE",
                                                     decorrelationModel="FULL_ETACORRELATED_v1", doSmearing=0, randomRunNumber=EgammaCalibPeriodRunNumbersExample.run_2016),
         CfgMgr.CP__EgammaCalibrationAndSmearingTool("calib_es2012c",
                                                     ESModel="es2012c",
                                                     decorrelationModel="FULL_ETACORRELATED_v1", doSmearing=0, randomRunNumber=EgammaCalibPeriodRunNumbersExample.run_2016),
         CfgMgr.CP__EgammaCalibrationAndSmearingTool("calib_es2017",
                                                     ESModel="es2016data_mc15c",
                                                     decorrelationModel="FULL_ETACORRELATED_v1", doSmearing=0, randomRunNumber=EgammaCalibPeriodRunNumbersExample.run_2016)]
#################

for tool in tools:
    ToolSvc += tool

dumper_alg = CfgMgr.DumpAllSystematics("DumpAllSystematics")
dumper_alg.particle = particle
dumper_alg.EgammaCalibrationAndSmearingTools = tools
dumper_alg.recoContainer = container_name

algSeq = CfgMgr.AthSequencer("AthAlgSeq")
algSeq += dumper_alg

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["DATASTREAM DATAFILE='output_sys.root' OPT='RECREATE'"]

theApp.EvtMax = 100
