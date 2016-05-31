import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

filelist = [#"valid2.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s2578_r7226/AOD.06803710._000047.pool.root.1",
            #"valid2.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s2578_r7226/AOD.06803710._000254.pool.root.1",
    #"myESD.pool.root"
#    "/atlas/data1/userdata/khoo/Data15/mc15_ESD/mc15_13TeV.422008.ParticleGun_single_ele_Pt100.recon.ESD.e4459_s2726_r7143_tid06642056_00/ESD.06642056._000021.pool.root.1"
    "/atlas/data1/userdata/khoo/Data15/mc15_ESD/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.ESD.e3698_s2608_s2183_r7509_tid07497143_00/ESD.07497143._000004.pool.root.1"
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

include('RecExCond/AllDet_detDescr.py')
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

############################################################################
# Set up muon and egamma topocluster links
muonTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("MuonTCLinks",
                                               ClustersToDecorate="MuonClusterCollection",
                                               UseLeadCellEtaPhi=True)
egammaTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("EgammaTCLinks",
                                                 ClustersToDecorate="egammaClusters")
egammatopoTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("TopoEgammaTCLinks",
                                                     ClustersToDecorate="egammaTopoSeededClusters")
topSequence += muonTCLinkAlg
topSequence += egammaTCLinkAlg
topSequence += egammatopoTCLinkAlg

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
for assoc in cfg_akt4em.assoclist:
    assoc.IgnoreJetConst = True
    assoc.ForwardColl = "CellTowers"

associators2 = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Tau'),
               AssocConfig('Soft')]
cfg_akt4em2 = METAssocConfig('NewAntiKt4EMTopo2',
                            associators2,
                            doPFlow=False
                            )
for assoc in cfg_akt4em2.assoclist:
    assoc.IgnoreJetConst = True
    assoc.ClusColl = "CellTowers"


metFlags.METAssocConfigs()[cfg_akt4em.suffix] = cfg_akt4em
metFlags.METAssocOutputList().append(cfg_akt4em.suffix)

metFlags.METAssocConfigs()[cfg_akt4em2.suffix] = cfg_akt4em2
metFlags.METAssocOutputList().append(cfg_akt4em2.suffix)

#from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocConfig import getMETAssocAlg

# Get the configuration directly from METRecoFlags
# Can also provide a dict of configurations or list of RecoTools or both
metAlg = getMETAssocAlg('METAssociation')
topSequence += metAlg

from METUtilities.METMakerConfig import getMETMakerAlg
for key,conf in metFlags.METAssocConfigs().iteritems():
    if not conf.doTruth:
        makerAlg = getMETMakerAlg(conf.suffix,jetColl="AntiKt4EMTopoJets")
        topSequence += makerAlg
ToolSvc.METMaker_NewAntiKt4EMTopo.OutputLevel = 1

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

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
    for item in MissingETAODList:
        xaodStream.AddItem(item)

theApp.EvtMax = 10
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999
