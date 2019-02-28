import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from glob import glob
#filelist = ['../../athxaod/files/mc14_8TeV.147806.PowhegPythia8_AU2CT10_Zee.merge.AOD.e1169_s1896_s1912_r5591_r5625_tid01512387_00/AOD.01512387._001662.pool.root.1']
filelist = ["/afs/cern.ch/user/b/boliu/workdir/dataset/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.merge.AOD.e1727_s1933_s1911_r5591_r5625_tid01507243_00/AOD.01507243._011158.pool.root.1"]

#filelist = ['../../athxaod/newfiler.root']
#filelist = ['xAOD.sklim.pool.root']
#filelist = ['/afs/cern.ch/user/s/schaffer/public/public1/work-19.x.EventSizes/build/run/devval_0_0809_100evts/myAOD.pool.root']
#filelist = ['/afs/cern.ch/user/c/christos/public/AOD_forRob.pool.root']
ServiceMgr.EventSelector.InputCollections = filelist

verbose = False

######################################################################
# Rerun MET reco to fix track-vertex associations
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig, getMETRecoAlg

rf_builders = [BuildConfig('Ele'),
               BuildConfig('Gamma'),
               BuildConfig('Tau'),
               BuildConfig('Jet'),
               BuildConfig('Muon'),
               BuildConfig('SoftClus'),
               BuildConfig('SoftTrk')]
rf_refiners = [RefConfig('TrackFilter','PVSoftTrk'),
               RefConfig('JetFilter','RefJet_JVFCut')]
cfg_mrf = METConfig('RefFinalFix',
                    rf_builders,
                    rf_refiners,
                    doSum=True,
                    doTracks=True
                    )
cfg_mrf.builders['Tau'].MinWet=1.
metconfigs = {
    'RefFinalFix':cfg_mrf
    }
metAlg = getMETRecoAlg('METReconstruction',metconfigs)

# The tools are accessible via the configurations in metFlags
from AthenaCommon.AppMgr import ToolSvc
metMaker = CfgMgr.met__METMaker('METMaker',
                                JetPtCut=0e3,
                                DoJetJvfCut=False,
                                );
ToolSvc += metMaker

makerAlg = CfgMgr.met__METMakerAlg('METMakerAlg',
                                   METCoreName='MET_Core',
                                   METName='MET_RefAssoc',
                                   Maker=metMaker)

## Set up default configurations
from METUtilities.METUtilConfig import getMETUtilAlg,METUtilConfig,getMETRebuilder
csttool = getMETRebuilder(METUtilConfig('RefFinalFix',{'Soft':'SoftClus'}, {},'RefCST'))
tsttool = getMETRebuilder(METUtilConfig('RefFinalFix',{'Soft':'PVSoftTrk'},{},'RefTST'))
utilAlg = getMETUtilAlg('METUtilAlg')
csttool.CalibJetPtCut=0e3
tsttool.CalibJetPtCut=0e3
csttool.DoJetJVFCut=False
tsttool.DoJetJVFCut=False
ToolSvc += csttool
ToolSvc += tsttool
utilAlg.Rebuilders = [csttool,tsttool]


pfotool = CfgMgr.CP__RetrievePFOTool('MET_PFOTool')
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += pfotool

trkseltool=CfgMgr.InDet__InDetTrackSelectionTool(CutLevel="TightPrimary")
ToolSvc += trkseltool



assocAlg = CfgMgr.met__METAssocAlg('METAssocAlg',PFOTool=pfotool, TrackSelectorTool=trkseltool)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from GaudiSequencer.PyComps import PyEvtFilter
masterSeq = CfgMgr.AthSequencer('AthMasterSeq')
#the following lines are examples, pick one...
#masterSeq += PyEvtFilter(evt_list=[1980130,287866,1706011,2226793,146377,146604,1904442,1297008,1298451,1072953]) #will execute main sequence only for these eventnumbers
#masterSeq += PyEvtFilter(evt_list=[1108077])
masterSeq += metAlg
masterSeq += assocAlg
masterSeq += utilAlg
masterSeq += makerAlg

if verbose:
    metAlg.OutputLevel=VERBOSE
    assocAlg.OutputLevel=VERBOSE
    makerAlg.OutputLevel=VERBOSE
    metMaker.OutputLevel=VERBOSE
    cfg_mrf.builders['Ele'].OutputLevel=VERBOSE
    cfg_mrf.builders['Gamma'].OutputLevel=VERBOSE
    cfg_mrf.builders['Tau'].OutputLevel=VERBOSE
    cfg_mrf.builders['Muon'].OutputLevel=VERBOSE
    cfg_mrf.builders['Jet'].OutputLevel=VERBOSE
    cfg_mrf.builders['SoftTrk'].OutputLevel=VERBOSE
    cfg_mrf.refiners['TrackFilter'].OutputLevel=VERBOSE

#from Valkyrie.JobOptCfg import ValgrindSvc
#svcMgr += ValgrindSvc( OutputLevel = DEBUG,
#                       ProfiledAlgs = ['METAssocAlg','METAssocAlg'],
#                       ProfiledIntervals = ['METAssocAlg.execute'])

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( 'StreamAOD', 'xAOD.METAssoc.testassoc.pool.root' )
xaodStream.AddItem('xAOD::MissingETAssociationMap_v1#METAssoc_Akt4LC')
xaodStream.AddItem('xAOD::MissingETAuxAssociationMap_v1#METAssoc_Akt4LCAux.')
xaodStream.AddItem('xAOD::EventInfo_v1#EventInfo')
xaodStream.AddItem('xAOD::EventAuxInfo_v1#EventInfoAux.')
#xaodStream.AddItem('xAOD::ElectronContainer#ElectronCollection')
#xaodStream.AddItem('xAOD::ElectronAuxContainer#ElectronCollectionAux.')
#xaodStream.AddItem('xAOD::PhotonContainer#PhotonCollection')
#xaodStream.AddItem('xAOD::PhotonAuxContainer#PhotonCollectionAux.')
#xaodStream.AddItem('xAOD::TauJetContainer_v1#TauRecContainer')
#xaodStream.AddItem('xAOD::TauJetAuxContainer_v1#TauRecContainerAux.')
#xaodStream.AddItem('xAOD::JetContainer_v1#AntiKt4LCTopoJets')
#xaodStream.AddItem('xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux.')
#xaodStream.AddItem('xAOD::MuonContainer_v1#Muons')
#xaodStream.AddItem('xAOD::MuonAuxContainer_v1#MuonsAux.')
#xaodStream.AddItem('xAOD::MissingETComponentMap_v1#METMap_RefFinalFix')
#xaodStream.AddItem('xAOD::MissingETAuxComponentMap_v1#METMap_RefFinalFixAux.')
#xaodStream.AddItem('xAOD::CaloClusterContainer_v1#egClusterCollection');
#xaodStream.AddItem('xAOD::CaloClusterAuxContainer_v1#egClusterCollectionAux.');
#xaodStream.AddItem('xAOD::CaloClusterContainer_v1#CaloCalTopoCluster');
#xaodStream.AddItem('xAOD::CaloClusterAuxContainer_v1#CaloCalTopoClusterAux.');
#xaodStream.AddItem('xAOD::TrackParticleContainer_v1#*')
#xaodStream.AddItem('xAOD::TrackParticleAuxContainer_v1#*')
#xaodStream.AddItem('xAOD::VertexContainer_v1#*')
#xaodStream.AddItem('xAOD::VertexAuxContainer_v1#*')
xaodStream.AddItem('xAOD::MissingETContainer_v1#MET_Core')
xaodStream.AddItem('xAOD::MissingETAuxContainer_v1#MET_CoreAux.')
xaodStream.AddItem('xAOD::MissingETContainer_v1#MET_RefAssoc')
xaodStream.AddItem('xAOD::MissingETAuxContainer_v1#MET_RefAssocAux.')
xaodStream.AddItem('xAOD::MissingETContainer_v1#MET_RefFinalFix')
xaodStream.AddItem('xAOD::MissingETAuxContainer_v1#MET_RefFinalFixAux.')
xaodStream.AddItem('xAOD::MissingETContainer_v1#MET_RefCST')
xaodStream.AddItem('xAOD::MissingETAuxContainer_v1#MET_RefCSTAux.')
xaodStream.AddItem('xAOD::MissingETContainer_v1#MET_RefTST')
xaodStream.AddItem('xAOD::MissingETAuxContainer_v1#MET_RefTSTAux.')
xaodStream.AcceptAlgs(['PyEvtFilter'])

theApp.EvtMax = 100
ServiceMgr.EventSelector.SkipEvents = 0
#ServiceMgr.MessageSvc.defaultLimit = 9999
