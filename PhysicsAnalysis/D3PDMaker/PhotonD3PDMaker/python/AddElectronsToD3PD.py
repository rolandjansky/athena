# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PhotonD3PDMaker
import D3PDMakerCoreComps
import CaloD3PDMaker
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags
from D3PDMakerCoreComps.IndexAssociation             import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation        import IndexMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation            import SimpleAssociation
from PhotonD3PDMaker.PhotonD3PD                      import _args
from egammaD3PDMaker.ElectronD3PDObject              import ElectronD3PDObject
from egammaD3PDMaker                                 import egammaDetailFillerTool
from ROOT                                            import egammaParameters
import egammaD3PDMaker
import egammaD3PDAnalysis
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from egammaD3PDMaker.defineBlockAndAlg               import defineAlgLODFunc, defineBlockAndAlg
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr      import ToolSvc
from AthenaCommon.Include     import include
from D3PDMakerCoreComps.release_version              import at_least_version
import TrackD3PDMaker
from AthenaPython                                    import PyAthena
from RecExConfig.RecFlags                            import rec
from D3PDMakerCoreComps.resolveSGKey                 import testSGKey
from CaloD3PDMaker                                   import ClusterTimeFillerTool
import EventCommonD3PDMaker

DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool
ElectronUDPrefix = "PhotonD3PD::ElectronUDPrefix_"


topo_iso_in_aod = at_least_version ('17.1.2') and not at_least_version ('17.3.0')

def AddElectronsToD3PD(alg, **kw):
    from AthenaCommon.AppMgr      import ToolSvc
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # some electron->jet matching
    for i in PhotonD3PDMakerFlags.JetCollections():

        collection_name = "jet_"+i+"_"

        # this jet matching for electrons is different than for photons.
        # PAU doesn't do any jet matching for electrons, so I elected to keep this
        # matching consistent with what the rest of ATLAS is doing, rather than
        # keep the PAU convention for photons.
        from EventCommonD3PDMaker.DRAssociation     import DRAssociation
        EleJetD3PDAssoc = DRAssociation (ElectronD3PDObject,
                                         'JetCollection',
                                         i,
                                         0.2,
                                         collection_name,
                                         level = 1,
                                         blockname = 'ElectronJetMatch_'+i)
        EleJetD3PDAssoc.defineBlock (1, 'ElectronJetIndex_'+i,
                                     D3PDMakerCoreComps.IndexFillerTool,
                                     Target = collection_name)

    # stuff that's not going to make it in there otherwise
    #
    additionalElectronIncludes=["SumCellsGain", "NbCellsGain"]
    if (PhotonD3PDMakerFlags.ElectronDumpingLevel() < 2):
        additionalElectronIncludes += ["Rings",
                                       "Pointing",
                                       "EMTrackFitDetailsBrem",
                                       "EMTrackFitDetailsRefit",
                                       "IsoPtCorrected",
                                       "TrackIsoIP",
                                       "Samplings",
                                       "AllSamplings",
                                       "ClusterKin",
                                       "RawSamplings",
                                       "RawClusterKin",
                                       "SumCellsGain",
                                       "NbCellsGain"]
                                    
        
    from egammaD3PDMaker.ElectronD3PDObject import ElectronClusterAssoc
    ElectronClusterAssoc.defineBlock(1, 'Time', ClusterTimeFillerTool)
    ElectronClusterAssoc.defineBlock(2, '', PhotonD3PDMaker.ShowerRadiusFillerTool)
 
    CellGetter = DVGetter ('ElectronSelectedCellGetter', 
                           TypeName = 'CaloCellContainer', 
                           SGKey = PhotonD3PDMakerFlags.SelectedCellsContainerName())
    ToolSvc += CellGetter
    ElectronCellAssoc = IndexMultiAssociation(ElectronClusterAssoc,
                                              CaloD3PDMaker.CaloClusterExtendedCellAssociationTool,
                                              "cell_",
                                              level=1,
                                              prefix="cell_",
                                              blockname = 'ClusterExtendedCells',
                                              allowMissing = True,
                                              Getter = CellGetter,
                                              DRCut=PhotonD3PDMakerFlags.CellDrCut())

    ElectronD3PDObject.defineBlock(1, "CoNoise",
                                   egammaDetailFillerTool,
                                   Details = [egammaParameters.etconoisedR04Sig2, 'etconoisedR04Sig2',
                                              egammaParameters.etconoisedR04Sig3, 'etconoisedR04Sig3']);
    
    # Topo cluster associations.
    ElectronTopoD3PDAssoc = DRAssociation (ElectronD3PDObject,
                                           'CaloClusterContainer',
                                           D3PDMakerFlags.ClusterSGKey(),
                                           0.1,
                                           'topo',
                                           level = PhotonD3PDMakerFlags.ElectronDumpingLevel(),
                                           blockname = 'TopoMatch')
    ElectronTopoD3PDAssoc.defineBlock (PhotonD3PDMakerFlags.ElectronDumpingLevel(), 'TopoKinematics',
                                       EventCommonD3PDMaker.FourMomFillerTool,
                                       WriteM = False)
    ElectronTopoD3PDAssoc.defineBlock (1, 'ElectronTopoKinematicsEM',
                                       PhotonD3PDMaker.ClusterFillerTool,
                                       Prefix = "EMsamplings_",
                                       UseCaloSamplings = [0,1,2,3,4,5,6,7])
    ElectronTopoD3PDAssoc.defineBlock (1, 'ElectronTopoKinematicsUncalibratedEM',
                                       PhotonD3PDMaker.ClusterFillerTool,
                                       Prefix = "EMsamplings_uncalibrated_",
                                       SignalState = PyAthena.P4SignalState.UNCALIBRATED,
                                       UseCaloSamplings = [0,1,2,3,4,5,6,7])
    
    # Some tools for topocluster isolatoin
    from egammaCaloTools.egammaTopoIsoToolBase   import egammaTopoIsoToolBase
    egammaCalTopoIsoTool = egammaTopoIsoToolBase("egammaCalTopoIsoToolBase")
    egammaCalTopoIsoTool.UseEMScale = False
    egammaTopoIsoTool = egammaTopoIsoToolBase("egammTopoIsoToolBase")

    if egammaTopoIsoTool not in ToolSvc:
        ToolSvc += egammaTopoIsoTool
    if egammaCalTopoIsoTool not in ToolSvc:
        ToolSvc += egammaCalTopoIsoTool

    #----------------------------------------------
    # TopoCluster isolation variables, requested by Sandrine, Jean-Baptiste, and Jessica

    # Add additional TopoCluster Isolation variables
    # -- first, schedule them to be calculated
    ClusterTopoIsoGetter = DVGetter('el_egammaTopoAlgClusterGetter',
                                    TypeName = 'CaloClusterContainer',
                                    SGKey = D3PDMakerFlags.ClusterSGKey())
    ElectronTopoIsoGetter =  DVGetter('el_egammaTopoPosEMIsoAlgEgammaGetter',
                               TypeName = 'ElectronContainer',
                               SGKey = D3PDMakerFlags.ElectronSGKey())
    preseq += egammaD3PDAnalysis.egammaTopoIsoAlg("el_egammaTopoPosEMIsoAlg",
                                                  EgammaGetter = ElectronTopoIsoGetter,
                                                  ClusterGetter = ClusterTopoIsoGetter,
                                                  TopoIsoTool = egammaTopoIsoTool,
                                                  PositiveEnergyClusters = True, 
                                                  UseEMScale = True,
                                                  ConeSizes = [.20, .30, .40, .50, .60],
                                                  AllowMissing = False,
                                                  UDPrefix = ElectronUDPrefix)
    preseq += egammaD3PDAnalysis.egammaTopoIsoAlg("el_egammaTopoCalIsoAlg",
                                                  EgammaGetter = ElectronTopoIsoGetter,
                                                  ClusterGetter = ClusterTopoIsoGetter,
                                                  TopoIsoTool = egammaCalTopoIsoTool,
                                                  PositiveEnergyClusters = False,
                                                  UseEMScale = False,
                                                  ConeSizes = [.20, .30, .40, .50, .60],
                                                  AllowMissing = False,
                                                  UDPrefix = ElectronUDPrefix)
    preseq += egammaD3PDAnalysis.egammaTopoIsoAlg("el_egammaTopoPosCalIsoAlg",
                                                  EgammaGetter = ElectronTopoIsoGetter,
                                                  ClusterGetter = ClusterTopoIsoGetter,
                                                  TopoIsoTool = egammaCalTopoIsoTool,
                                                  PositiveEnergyClusters = True,
                                                  UseEMScale = False,
                                                  ConeSizes = [.20, .30, .40, .50, .60],
                                                  AllowMissing = False,
                                                  UDPrefix = ElectronUDPrefix)
    # now fill them
    ElectronD3PDObject.defineBlock (PhotonD3PDMakerFlags.ElectronDumpingLevel(),
                                    'PhotonD3PD_ElectronExtraUDTopoCones',
                                    D3PDMakerCoreComps.UserDataFillerTool,
                                    UDPrefix = ElectronUDPrefix,
                                    Vars = ['topoCalEtcone20', '', 'float',
                                            'topoCalEtcone30', '', 'float',
                                            'topoCalEtcone40', '', 'float',
                                            'topoCalEtcone50', '', 'float',
                                            'topoCalEtcone60', '', 'float',
                                            'topoPosEtcone20', '', 'float',
                                            'topoPosEtcone30', '', 'float',
                                            'topoPosEtcone40', '', 'float',
                                            'topoPosEtcone50', '', 'float',
                                            'topoPosEtcone60', '', 'float',
                                            'topoPosCalEtcone20', '', 'float',
                                            'topoPosCalEtcone30', '', 'float',
                                            'topoPosCalEtcone40', '', 'float',
                                            'topoPosCalEtcone50', '', 'float',
                                            'topoPosCalEtcone60', '', 'float',
                                            ])
    #----------------------------------------------
    
    # some truth associations
    if rec.doTruth():
        # construct an association tool from an egamma* object to a genparticle
        # for now, at least, needs to use the PAU tools for object matching
        from PhotonAnalysisUtils.PAU_AtlasExtrapolator import PAU_AtlasExtrapolator
        theAtlasExtrapolator = PAU_AtlasExtrapolator()

        from PhotonAnalysisUtils.PAU_ExtrapolateToCaloTool import PAU_ExtrapolateToCaloTool
        exToCalo = PAU_ExtrapolateToCaloTool(theAtlasExtrapolator = theAtlasExtrapolator)

        from PhotonAnalysisUtils.TruthTools import TruthToolsWrapper
        PAUtruthTool = TruthToolsWrapper(exToCalo)

        PAUElectronTruthParticleAssociationTool = IndexAssociation(ElectronD3PDObject,
                                                                   PhotonD3PDMaker.TruthParticleAssociationTool,
                                                                   target="mc_",
                                                                   prefix="mc_",
                                                                   level=1,
                                                                   blockname="PAUElectronTruthParticleAssociationTool",
                                                                   allowMissing=False,
                                                                   PAUtruthTool = PAUtruthTool)
        
        PAUElectronEgammaTruthParticleAssociationTool = IndexAssociation(ElectronD3PDObject,
                                                                         PhotonD3PDMaker.TruthParticleAssociationTool,
                                                                         target="egammaTruth_",
                                                                         prefix='egammaTruth_',
                                                                         level=1,
                                                                         blockname="PAUElectronEgammaTruthParticleAssociationTool",
                                                                         allowMissing=False,
                                                                         PAUtruthTool = PAUtruthTool)

        

    # Finally, associate the electron to a track
    ElectronTrackParticleIndexAssociation = IndexAssociation(ElectronD3PDObject,
                                                             egammaD3PDMaker.egammaTrackParticleAssociationTool,
                                                             target="trk",
                                                             prefix="trk_",
                                                             level=1,
                                                             blockname="TrackIndex",
                                                             allowMissing=False)

    GSFTrackParticlesInSample = testSGKey ('Rec::TrackParticleContainer', "GSFTrackParticleCandidate")
    if GSFTrackParticlesInSample:
        ElectronTrackParticleIndexAssociation = IndexAssociation(ElectronD3PDObject,
                                                                 egammaD3PDMaker.egammaTrackParticleAssociationTool,
                                                                 target="GSF_trk",
                                                                 prefix="GSF_trk_",
                                                                 level=1,
                                                                 blockname="GSFTrackIndex",
                                                                 allowMissing=False)
        

    ed3pd = ElectronD3PDObject        (**_args ( PhotonD3PDMakerFlags.ElectronDumpingLevel(),
                                                 'Electron', kw,
                                                 include=additionalElectronIncludes,
                                                 exclude=['JetMatch'],
                                                 EgammaJetSignedIPAndPTRel_target='jet_AntiKt4TopoEMJets_'))
    alg += ed3pd
    
    # GSF electrons?
    GSFElectronsInSample = testSGKey ('ElectronContainer', D3PDMakerFlags.GSFElectronSGKey())
    if GSFElectronsInSample:
        alg += GSFElectronD3PDObject(**_args ( PhotonD3PDMakerFlags.ElectronDumpingLevel(),
                                               'GSFElectron', kw,
                                               sgkey = D3PDMakerFlags.GSFElectronSGKey(),
                                               EgammaJetSignedIPAndPTRel_target='jet_AntiKt4TopoEMJets_',
                                               prefix = 'el_gsf_'))

