import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

filelist = [#"valid2.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s2578_r7226/AOD.06803710._000047.pool.root.1",
            #"valid2.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s2578_r7226/AOD.06803710._000254.pool.root.1",
#    "myESD.pool.root"
#    "/atlas/data1/userdata/khoo/Data15/mc15_ESD/mc15_13TeV.422008.ParticleGun_single_ele_Pt100.recon.ESD.e4459_s2726_r7143_tid06642056_00/ESD.06642056._000021.pool.root.1"
    "/atlas/data1/userdata/khoo/Data15/ESD/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.ESD.e3099_s2578_r7579_r7578_r7579/ESD.07670800._003079.pool.root.1",
    "/atlas/data1/userdata/khoo/Data15/ESD/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.ESD.e3099_s2578_r7579_r7578_r7579/ESD.07670800._003152.pool.root.1"
    ]
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = filelist
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

############################################################################
# Set up detector description for cell access

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
print inputFileSummary
if inputFileSummary['evt_type'][0] == 'IS_DATA':
    globalflags.DataSource = 'data'
else:
    globalflags.DataSource = 'geant4'

globalflags.DetDescrVersion = inputFileSummary['geometry']

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc
#from AthenaCommon.CfgGetter import getService
#getService("AtlasTrackingGeometrySvc")

include('RecExCond/AllDet_detDescr.py')
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

############################################################################
# Rerun the AntiKt4EMTopoJets with newer config

from JetRec.JetRecStandard import jtm
jtm.addJetFinder("NewAntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter= 5000, calibOpt="ar")
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(eventShapeTools=[])

############################################################################
# Set up muon and egamma topocluster links
egammaTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("EgammaTCLinks",
                                                 ClustersToDecorate="egammaClusters")
egammatopoTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("TopoEgammaTCLinks",
                                                     ClustersToDecorate="egammaTopoSeededClusters")
muonTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("MuonTCLinks",
                                               ClustersToDecorate="MuonClusterCollection",
                                               UseLeadCellEtaPhi=True)
topSequence += egammaTCLinkAlg
topSequence += egammatopoTCLinkAlg
topSequence += muonTCLinkAlg

# Set up default configurations
#import METReconstruction.METConfig_Associator

#include('RecExCond/AllDet_detDescr.py')
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


############################################################################
# Set up an extra associator for testing
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig
JetType = 'EMJet'
#todo check pflow

associators = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Tau'),
               AssocConfig('Soft')]
cfg_akt4em = METAssocConfig('NewAntiKt4EMTopo',
                            associators,
                            doPFlow=False
                            )

metFlags.METAssocConfigs()[cfg_akt4em.suffix] = cfg_akt4em
metFlags.METAssocOutputList().append(cfg_akt4em.suffix)

associators = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Tau'),
               AssocConfig('Soft')]
cfg_akt4emoc = METAssocConfig('NewAntiKt4EMTopo_OriginCorr',
                              associators,
                              doPFlow=False,
                              doOriginCorrClus=True
                              )

metFlags.METAssocConfigs()[cfg_akt4emoc.suffix] = cfg_akt4emoc
metFlags.METAssocOutputList().append(cfg_akt4emoc.suffix)

from METReconstruction.METAssocConfig import getMETAssocAlg

# Get the configuration directly from METRecoFlags
# Can also provide a dict of configurations or list of RecoTools or both
metAlg = getMETAssocAlg('METAssociation')
topSequence += metAlg

from METUtilities.METMakerConfig import getMETMakerAlg
for key,conf in metFlags.METAssocConfigs().iteritems():
    if not conf.doTruth:
        makerAlg = getMETMakerAlg(conf.suffix,jetColl="NewAntiKt4EMTopoJets")
        topSequence += makerAlg
#ToolSvc.METMaker_NewAntiKt4EMTopo.OutputLevel = DEBUG
#ToolSvc.METMaker_NewAntiKt4EMTopo_OriginCorr.OutputLevel = DEBUG

from METReconstruction.METRecoConfig import METConfig,BuildConfig

cfg_lht = METConfig('LocHadTopo_OriginCorr',[BuildConfig('LCOCSoftClus','LocHadTopo')],
                    doRegions=True,
                    doOriginCorrClus=True
                    )

metFlags.METConfigs()[cfg_lht.suffix] = cfg_lht
metFlags.METOutputList().append(cfg_lht.suffix)
metFlags.METOutputList().append(cfg_lht.suffix+'Regions')

from METReconstruction.METRecoConfig import getMETRecoAlg
metAlg2 = getMETRecoAlg('METReconstruction')
topSequence += metAlg2

from Valkyrie.JobOptCfg import ValgrindSvc
svcMgr += ValgrindSvc( OutputLevel = INFO,
                      ProfiledAlgs = ["METAssociation"],
                      ProfiledIntervals = ["METAssociation.execute"])

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

write_xAOD = True
if write_xAOD:

    # The list of output containers/maps is autogenerated and stored in metFlags
    # This jO extracts them with the appropriate formatting
    from AthenaCommon.Resilience import protectedInclude
    protectedInclude("METReconstruction/METReconstructionOutputAODList_jobOptions.py")

    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_SPLITLEVEL ='99'" ]
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
    for item in MissingETAODList:
        xaodStream.AddItem(item)

    xaodStream.AddItem('xAOD::MissingETContainer#MET_LocHadTopo')
    xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_LocHadTopoAux.')

    for jetrec in jtm.jetrecs:
      xaodStream.AddItem("xAOD::JetContainer#" + jetrec.name())
      xaodStream.AddItem("xAOD::JetAuxContainer#" + jetrec.name() + "Aux.")

      xaodStream.AddItem("xAOD::CaloClusterContainer#EMOriginTopoClusters")
      xaodStream.AddItem("xAOD::CaloClusterContainer#LCOriginTopoClusters")
      xaodStream.AddItem("xAOD::ShallowAuxContainer#LCOriginTopoClustersAux.")
      xaodStream.AddItem("xAOD::ShallowAuxContainer#EMOriginTopoClustersAux.")

from Valkyrie.JobOptCfg import ValgrindSvc
svcMgr += ValgrindSvc( OutputLevel = VERBOSE,
                       ProfiledAlgs = ["jetalg","METAssociation"] )

theApp.EvtMax = 100
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999
