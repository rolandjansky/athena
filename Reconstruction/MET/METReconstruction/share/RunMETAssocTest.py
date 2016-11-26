import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

filelist = [
    #"/atlas/data1/userdata/khoo/Data15/xAOD_20.7/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915894._000339.pool.root.1",
    #"user.valentem.mc15_13TeV.361107.merge.DAOD_JETM3.e3601_s2576_s2132_r7725_r7676.2016-08-31_EXT0/user.valentem.9333594.EXT0._000547.DAOD_JETM3.test.pool.root"
    #"DAOD_JETM3.MC15.pool.root"
    "/atlas/data1/userdata/valentem/PFlow/AODs/user.valentem.mc15_13TeV.361107.merge.DAOD_JETM3.e3601_s2576_s2132_r7725_r7676.2016-09-04_EXT0/user.valentem.9354614.EXT0._002886.DAOD_JETM3.test.pool.root"
    ]
athenaCommonFlags.FilesInput = filelist
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += CfgMgr.met__METAssocTestAlg("TestMETAssocEMTopo",
                                           #OutputLevel=VERBOSE,
                                           FailOnInconsistency=True)
topSequence += CfgMgr.met__METAssocTestAlg("TestMETAssocEMPFlow",
                                           #OutputLevel=VERBOSE,
                                           FailOnInconsistency=True,
                                           METMapSuffix="AntiKt4EMPFlow")

theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999
