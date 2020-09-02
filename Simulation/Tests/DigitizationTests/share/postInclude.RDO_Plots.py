from DigitizationTests.DigitizationTestsConf import DigiTestAlg, McEventCollectionTestTool, PixelRDOsTestTool, SCT_RDOsTestTool, TRT_RDOsTestTool
from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
#ServiceMgr.THistSvc.Output  = ["atlasTest DATAFILE='atlasTest.muons.histo.root' OPT='RECREATE'"];
ServiceMgr.THistSvc.Output = ["truth DATAFILE='RDO_truth.root' OPT='RECREATE'"]

job = AlgSequence()
job += DigiTestAlg()
if DetFlags.Truth_on():
    job.DigiTestAlg.DigiTestTools += [CfgGetter.getPublicTool(
        "McEventCollectionTestTool", checkType=True)]
if DetFlags.Truth_on():
    if DetFlags.pileup.any_on():
        job.DigiTestAlg.DigiTestTools += [
            CfgGetter.getPublicTool("EventInfoTestTool", checkType=True)]
if DetFlags.pixel_on():
    job.DigiTestAlg.DigiTestTools += [
        CfgGetter.getPublicTool("PixelRDOsTestTool", checkType=True)]
if DetFlags.SCT_on():
    job.DigiTestAlg.DigiTestTools += [
        CfgGetter.getPublicTool("SCT_RDOsTestTool", checkType=True)]
if DetFlags.TRT_on():
    job.DigiTestAlg.DigiTestTools += [
        CfgGetter.getPublicTool("TRT_RDOsTestTool", checkType=True)]
