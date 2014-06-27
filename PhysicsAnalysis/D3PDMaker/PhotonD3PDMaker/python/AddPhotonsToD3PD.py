# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PhotonD3PDMaker
import D3PDMakerCoreComps
import CaloD3PDMaker
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags
from D3PDMakerCoreComps.IndexAssociation             import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation        import IndexMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation            import SimpleAssociation
from PhotonD3PDMaker.PhotonD3PD                      import _args
from egammaD3PDMaker.PhotonD3PDObject                import PhotonD3PDObject
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

DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool

PhotonUDPrefix = "PhotonD3PD::PhotonUDPrefix_"

topo_iso_in_aod = at_least_version ('17.1.2') and not at_least_version ('17.3.0')

def AddPhotonsToD3PD(alg, **kw):
    from AthenaCommon.AppMgr      import ToolSvc
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # jet associations
    for i in PhotonD3PDMakerFlags.JetCollections():

        collection_name = "jet_"+i+"_"
        from PhotonD3PDMaker.DREtAssociation        import DREtAssociation
        PhotonJetD3PDAssoc = DREtAssociation (PhotonD3PDObject,
                                              'JetCollection',
                                              i,
                                              0.4,
                                              collection_name,
                                              level = 1,
                                              blockname = 'PhotonJetMatch_'+i)
        PhotonJetD3PDAssoc.defineBlock (1, 'PhotonJetIndex_'+i,
                                        D3PDMakerCoreComps.IndexFillerTool,
                                        Target = collection_name)
        

    # some other random stuff
    from egammaD3PDMaker.PhotonD3PDObject import PhotonClusterAssoc
    from CaloD3PDMaker                    import ClusterTimeFillerTool
    PhotonClusterAssoc.defineBlock(2, 'Time', ClusterTimeFillerTool)
    PhotonClusterAssoc.defineBlock(2, '', PhotonD3PDMaker.ShowerRadiusFillerTool)
 
    CellGetter = DVGetter ('PhotonSelectedCellGetter', 
                           TypeName = 'CaloCellContainer', 
                           SGKey = PhotonD3PDMakerFlags.SelectedCellsContainerName())
    ToolSvc += CellGetter
    PhotonCellAssoc = IndexMultiAssociation(PhotonClusterAssoc,
                                            CaloD3PDMaker.CaloClusterExtendedCellAssociationTool,
                                            "cell_",
                                            level=2,
                                            prefix="cell_",
                                            blockname = 'ClusterExtendedCells',
                                            allowMissing = True,
                                            Getter = CellGetter,
                                            DRCut=PhotonD3PDMakerFlags.CellDrCut())
    
    PhotonD3PDObject.defineBlock(2, "CoNoise",
                                 egammaDetailFillerTool,
                                 Details = [egammaParameters.etconoisedR04Sig2, 'etconoisedR04Sig2',
                                            egammaParameters.etconoisedR04Sig3, 'etconoisedR04Sig3']);


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
    ClusterTopoIsoGetter = DVGetter('egammaTopoAlgClusterGetter',
                                    TypeName = 'CaloClusterContainer',
                                    SGKey = D3PDMakerFlags.ClusterSGKey())
    PhotonTopoIsoGetter = DVGetter('ph_egammaTopoEMIsoAlgEgammaGetter',
                                   TypeName = 'PhotonContainer',
                                   SGKey = D3PDMakerFlags.PhotonSGKey())
    # calibrated clusters
    preseq += egammaD3PDAnalysis.egammaTopoIsoAlg("ph_egammaTopoCalIsoAlg",
                                                  EgammaGetter = PhotonTopoIsoGetter,
                                                  ClusterGetter = ClusterTopoIsoGetter,
                                                  TopoIsoTool = egammaCalTopoIsoTool,
                                                  UseEMScale = False,
                                                  ConeSizes = [.20, .30, .40, .50, .60],
                                                  AllowMissing = False,
                                                  UDPrefix = PhotonUDPrefix)
    # positive, uncalibrated clusters
    preseq += egammaD3PDAnalysis.egammaTopoIsoAlg("ph_egammaTopoPosEMIsoAlg",
                                                  EgammaGetter = PhotonTopoIsoGetter,
                                                  ClusterGetter = ClusterTopoIsoGetter,
                                                  TopoIsoTool = egammaTopoIsoTool,
                                                  UseEMScale = True,
                                                  PositiveEnergyClusters = True,
                                                  ConeSizes = [.20, .30, .40, .50, .60],
                                                  AllowMissing = False,
                                                  UDPrefix = PhotonUDPrefix)
    # positive, calibrated clusters
    preseq += egammaD3PDAnalysis.egammaTopoIsoAlg("ph_egammaTopoPosCalIsoAlg",
                                                  EgammaGetter = PhotonTopoIsoGetter,
                                                  ClusterGetter = ClusterTopoIsoGetter,
                                                  TopoIsoTool = egammaCalTopoIsoTool,
                                                  UseEMScale = False,
                                                  PositiveEnergyClusters = True,
                                                  ConeSizes = [.20, .30, .40, .50, .60],
                                                  AllowMissing = False,
                                                  UDPrefix = PhotonUDPrefix)
    # --- now fill them
    PhotonD3PDObject.defineBlock (2, 'PhotonD3PD_PhotonUDExtraTopoCones',
                                  D3PDMakerCoreComps.UserDataFillerTool,
                                  UDPrefix = PhotonUDPrefix,
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

    
    #----------------------------------------------
    # Temporarily add back in missing variables for Jean-Francois, while we wait
    # for Scott to fix something else.
    from egammaD3PDMaker.PhotonD3PDObject import ConversionVertexTrackTrackParticleAssociation
                                  
    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAU_ToT_tool
    ToTTool = PAU_ToT_tool(	name = "ToTTool" )
    ToolSvc += ToTTool

    from PhotonAnalysisUtils.PAU_AtlasExtrapolator import PAU_AtlasExtrapolator
    theAtlasExtrapolator = PAU_AtlasExtrapolator()

    from PhotonAnalysisUtils.PAU_ExtrapolateToCaloTool import PAU_ExtrapolateToCaloTool
    exToCalo = PAU_ExtrapolateToCaloTool(theAtlasExtrapolator = theAtlasExtrapolator)

    preseq += egammaD3PDAnalysis.ConversionTrackParticleCalculatorAlg("ConversionTrackParticleCalculator", 
                                                                      PhotonGetter = DVGetter ('ConversionVertexTrackParticlePhotonGetter', 
                                                                                               TypeName = 'PhotonContainer', 
                                                                                               SGKey = D3PDMakerFlags.PhotonSGKey()), 
                                                                      PAU_ToT_tool = ToTTool, 
                                                                      ExtrapolateToCaloTool = exToCalo, 
                                                                      UDPrefix        = PhotonUDPrefix) 
    
    ConversionVertexTrackTrackParticleAssociation.defineBlock(0, 'ConversionVertexTrackVariables', 
                                                              D3PDMakerCoreComps.UserDataFillerTool, 
                                                              UDPrefix=PhotonUDPrefix, 
                                                              Vars = ['ToT'                , '', 'float', 
                                                                      'DeltaEta_track_calo', '', 'float', 
                                                                      'DeltaPhi_track_calo', '', 'float'])
    
    # Track parameters at conversion vertex
    from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
    from egammaD3PDMaker.PhotonD3PDObject import ConversionVertexAssociation

    VxTrackAtConversionVertexAssociationTool = ContainedVectorMultiAssociation(ConversionVertexAssociation,
                                                                               PhotonD3PDMaker.VxTrackAtVertexAssociationTool,
                                                                               prefix="paramsAtConvVtx_")
    ConversionVertexTrackTrackParticlePerigeeAtVertexAssociationTool = SimpleAssociation(VxTrackAtConversionVertexAssociationTool,
                                                                                         PhotonD3PDMaker.ConversionPerigeeAssociationTool)
    ConversionVertexTrackTrackParticlePerigeeAtVertexAssociationTool.defineBlock (1, 'Trk',
                                                                                  TrackD3PDMaker.PerigeeFillerTool)
    TrackParticleCovarAssoc = SimpleAssociation (ConversionVertexTrackTrackParticlePerigeeAtVertexAssociationTool,
                                                 TrackD3PDMaker.PerigeeCovarianceAssociationTool)
    TrackParticleCovarAssoc.defineBlock (3, 'TrkCovDiag',
                                         TrackD3PDMaker.CovarianceFillerTool,
                                         IsTrackPerigee = True,
                                         Error = False,
                                         DiagCovariance = True)
    TrackParticleCovarAssoc.defineBlock (3, 'TrkCovOffDiag',
                                         TrackD3PDMaker.CovarianceFillerTool,
                                         IsTrackPerigee = True,
                                         Error = False,
                                         OffDiagCovariance = True)
    
    #  NeutralPerigee parameters 
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils
    conversionUtils = InDet__InDetJetFitterUtils(name                   = "conversionUtils",
                                                 LinearizedTrackFactory = None,
                                                 Extrapolator           = None)
    ToolSvc += conversionUtils

    NeutralPerigeeAtConversionVertexAssociationTool = SimpleAssociation(ConversionVertexAssociation,
                                                                        PhotonD3PDMaker.ConversionNeutralPerigeeAssociationTool,
                                                                        ConversionUtils = conversionUtils,
                                                                        prefix="neutralParamsAtConvVtx_")
    NeutralPerigeeAtConversionVertexAssociationTool.defineBlock (1, 'TrkNP',
                                                                 PhotonD3PDMaker.NeutralPerigeeFillerTool)
    NeutralTrackParticleCovarAssoc = SimpleAssociation (NeutralPerigeeAtConversionVertexAssociationTool,
                                                        PhotonD3PDMaker.NeutralPerigeeCovarianceAssociationTool)
    NeutralTrackParticleCovarAssoc.defineBlock (3, 'TrkCovDiagNP',
                                                TrackD3PDMaker.CovarianceFillerTool,
                                                IsTrackPerigee = True,
                                                Error = False,
                                                DiagCovariance = True)
    NeutralTrackParticleCovarAssoc.defineBlock (3, 'TrkCovOffDiagNP',
                                                TrackD3PDMaker.CovarianceFillerTool,
                                                IsTrackPerigee = True,
                                                Error = False,
                                                OffDiagCovariance = True)
    #----------------------------------------------


    from egammaD3PDMaker.PhotonD3PDObject import PhotonTopoD3PDAssoc
    PhotonTopoD3PDAssoc.defineBlock (1, 'PhotonTopoKinematicsEM',
                                     PhotonD3PDMaker.ClusterFillerTool, 
                                     Prefix = "EMsamplings_",
                                     UseCaloSamplings = [0,1,2,3,4,5,6,7])
    PhotonTopoD3PDAssoc.defineBlock (1, 'PhotonTopoKinematicsUncalibratedEM',
                                     PhotonD3PDMaker.ClusterFillerTool, 
                                     Prefix = "EMsamplings_uncalibrated_",
                                     SignalState = PyAthena.P4SignalState.UNCALIBRATED,
                                     UseCaloSamplings = [0,1,2,3,4,5,6,7])


    # now, add in some truth associations
    if rec.doTruth():
        # construct an association tool from an egamma* object to a genparticle
        # for now, at least, needs to use the PAU tools for object matching
        from PhotonAnalysisUtils.PAU_AtlasExtrapolator import PAU_AtlasExtrapolator
        theAtlasExtrapolator = PAU_AtlasExtrapolator()

        from PhotonAnalysisUtils.PAU_ExtrapolateToCaloTool import PAU_ExtrapolateToCaloTool
        exToCalo = PAU_ExtrapolateToCaloTool(theAtlasExtrapolator = theAtlasExtrapolator)

        from PhotonAnalysisUtils.TruthTools import TruthToolsWrapper
        PAUtruthTool = TruthToolsWrapper(exToCalo)

        PAUPhotonTruthParticleAssociationTool = IndexAssociation(PhotonD3PDObject,
                                                                 PhotonD3PDMaker.TruthParticleAssociationTool,
                                                                 target="mc_",
                                                                 prefix="mc_",
                                                                 level=1,
                                                                 blockname="PAUPhotonTruthParticleAssociationTool",
                                                                 allowMissing=False,
                                                                 PAUtruthTool = PAUtruthTool)

        PAUPhotonEgammaTruthParticleAssociationTool = IndexAssociation(PhotonD3PDObject,
                                                                       PhotonD3PDMaker.TruthParticleAssociationTool,
                                                                       target="egammaTruth_",
                                                                       prefix='egammaTruth_',
                                                                       level=1,
                                                                       blockname="PAUPhotonEgammaTruthParticleAssociationTool",
                                                                       allowMissing=False,
                                                                       PAUtruthTool = PAUtruthTool)

    conversion_track_collection_name="trk"
    GSFTrackParticlesInSample = testSGKey ('Rec::TrackParticleContainer', "GSFTrackParticleCandidate")
    if GSFTrackParticlesInSample: 
        conversion_track_collection_name = "GSF_trk" 
    alg += PhotonD3PDObject(**_args (20, 'Photon', kw,
                                     ConversionVertexTrackIndex_target = conversion_track_collection_name,
                                     include=["SumCellsGain", "NbCellsGain"]))
    
