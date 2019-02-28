import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from glob import glob
filelist = glob("/atlas/data1/userdata/khoo/Data16/AOD_r21/data16_13TeV.00302347.express_express.recon.AOD.r9112/*")

athenaCommonFlags.FilesInput = filelist
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from GaudiSequencer.PyComps import PyEvtFilter
masterSeq = CfgMgr.AthSequencer("AthMasterSeq")
#the following lines are examples, pick one...
masterSeq += PyEvtFilter("PVSoftTrkTail", evt_list=[
        106239409,
        103677144,
        210091212,
        647377331,
        649083254,
        679943194,
        676957592,
        931088975,
        930867220,
        932105457,
        932761543,
        500249532,
        498552012,
        8093981,
        7747623,
        9713934,
        ])
topSequence += masterSeq

masterSeq += CfgMgr.met__METAssocTestAlg("TestMETAssocEMTopo",
                                         OutputLevel=VERBOSE,
                                         FailOnInconsistency=True)
masterSeq += CfgMgr.met__METAssocTestAlg("TestMETAssocEMPFlow",
                                         OutputLevel=VERBOSE,
                                         FailOnInconsistency=True,
                                         METMapSuffix="AntiKt4EMPFlow")

theApp.EvtMax = 5000
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999
