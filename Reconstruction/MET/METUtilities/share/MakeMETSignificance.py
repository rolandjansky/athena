import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from glob import glob
filelist = ["/eos/atlas/atlascerngroupdisk/penn-ww/MET/mc16_13TeV.364105.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV70_140_BFilter.deriv.DAOD_JETM3.e5271_s3126_r9364_r9315_p3141/DAOD_JETM3.11443210._000018.pool.root.1"]
#filelist = ["DAOD_JETM2.test.TruthAssoc.pool.root"]
#filelist = ["/afs/cern.ch/work/r/rsmith/public/METUtilities_testfiles/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s1982_s1964_r6006_tid04628718_00/AOD.04628718._000158.pool.root.1"]
ServiceMgr.EventSelector.InputCollections = filelist

#from METUtilities.METMakerConfig import getMETMakerAlg
#metAlg = getMETMakerAlg('AntiKt4EMTopo',20e3)
#metAlg.METName = 'MET_Reco_AntiKt4EMTopo'

from METUtilities.METSignificanceConfig import getMETSigificanceAlg
signifAlg = getMETSigificanceAlg()
signifAlg.METName = 'MET_Reco_AntiKt4EMTopo'

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#topSequence += metAlg
topSequence += signifAlg

write_xAOD = True
if write_xAOD:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.METSignificance.pool.root" )
    #
    #xaodStream.AddItem('xAOD::MissingETContainer#MET_Reco_AntiKt4EMTopo')
    #xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Reco_AntiKt4EMTopoAux.')
    xaodStream.Print()

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

ServiceMgr.MessageSvc.defaultLimit = 9999999
theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0

printint = max(theApp.EvtMax/250,1) if theApp.EvtMax>0 else 1000
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=printint)
