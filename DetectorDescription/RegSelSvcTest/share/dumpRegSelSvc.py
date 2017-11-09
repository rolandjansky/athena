# Joboptions fragement for backward compatability;
# please import configurables yourself instead.

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.FTK_setOn()
DetFlags.pixel_setOn()
DetFlags.SCT_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags._setOn()
#DetFlags._setOn()
#DetFlags._setOn()
#DetFlags._setOn()

include("TrigUpgradeTest/testHLT_MT.py")

# from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
# from AthenaCommon.AppMgr import ServiceMgr

# theRegSelSvc = RegSelSvcDefault("RegSelSvcDefault")
# ServiceMgr += theRegSelSvc



theApp.EvtMax = 1




from RegSelSvcTest.RegSelSvcTestConf import RegSelTestAlg
athAlgSeq += RegSelTestAlg()
athAlgSeq.RegSelTestAlg.Dump=True
athAlgSeq.RegSelTestAlg.OutputLevel=DEBUG




