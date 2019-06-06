# Joboptions fragement for backward compatability;
# please import configurables yourself instead.

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.FTK_setOn()
DetFlags.pixel_setOn()
DetFlags.SCT_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

include("TrigUpgradeTest/testHLT_MT.py")

from RegSelSvcTest.RegSelSvcTestConf import RegSelTestAlg
from AthenaCommon.AppMgr import athAlgSeq
athAlgSeq += RegSelTestAlg()
athAlgSeq.RegSelTestAlg.OutputLevel=DEBUG
if "TestType" not in dir():
    raise "Specify the test type, either \"dump\" or \"mt\""
if TestType == "dump":
    athAlgSeq.RegSelTestAlg.Dump=True

if TestType == "mt":
    athAlgSeq.RegSelTestAlg.Mt=True
