# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: tauPerf.py 540918 2013-03-14 12:01:49Z pmalecki $
#
# @file PhysicsAnalysis/D3PDMaker/TauD3PDMaker/python/tauPerf.py
# @author Dugan O'Neil <doneil@sfu.ca> and Ryan Reece <ryan.reece@cern.ch>
# @date December, 2009
# @brief Construct a tau performance D3PD.
#

# Athena imports
from RecExConfig.RecFlags                           import rec
from AthenaCommon.AlgSequence                       import AlgSequence
topSequence = AlgSequence()

# D3PDMaker imports
import D3PDMakerCoreComps
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 1
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 1
TrackD3PDFlags.storeTrackPredictionAtBLayer = False
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
TrackD3PDFlags.vertexPositionLevelOfDetails=1
TrackD3PDFlags.storeVertexTrackAssociation = False

from D3PDMakerConfig.D3PDMakerFlags                 import D3PDMakerFlags

# truth stuff
if rec.doTruth():

    D3PDMakerFlags.TruthParticlesSGKey = 'TruthHadrons'
    
    from TruthD3PDAnalysis.truthParticleConfig          import truthParticleConfig
    from TruthD3PDMaker.GenEventD3PDObject              import GenEventD3PDObject
    from TruthD3PDMaker.TruthParticleD3PDObject         import TruthParticleD3PDObject
    from TauD3PDMaker.TruthTauD3PDObject                import TruthTauD3PDObject
    from TauD3PDMaker.TruthTauFullD3PDObject            import TruthTauFullD3PDObject
    from MuonD3PDMaker.TruthMuonD3PDObject              import TruthMuonD3PDObject
    from egammaD3PDMaker.egammaTruthD3PDObject          import egammaTruthD3PDObject
    
    # more for egamma truth
    from TruthD3PDMaker.MCTruthClassifierConfig         import D3PDMCTruthClassifier
    from D3PDMakerCoreComps.SimpleAssociation           import SimpleAssociation
    from D3PDMakerCoreComps.IndexAssociation            import IndexAssociation
    import TruthD3PDMaker

# tau stuff
from TauD3PDMaker.EventTauD3PDObject                import EventTauD3PDObject
from TauD3PDMaker.TauD3PDObject                     import TauD3PDObject
import TauD3PDMaker

# D3PDObjects for eflowObjects
from TauD3PDMaker.EflowObjectsD3PDObject             import EflowObjectsD3PDObject
from TauD3PDMaker.EflowObjectsFromTruePi0sD3PDObject import EflowObjectsFromTruePi0sD3PDObject

# egamma stuff
from egammaD3PDAnalysis.egammaUserDataConfig        import egammaUserDataConfig
from egammaD3PDMaker.ElectronD3PDObject             import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject               import PhotonD3PDObject

# Muon stuff
from MuonD3PDMaker.MuonD3PDObject                   import MuonD3PDObject

# MET stuff
from MissingETD3PDMaker.MissingETD3PDMakerFlags     import MissingETD3PDMakerFlags
MissingETD3PDMakerFlags.doCellOutEflow = True
MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey = 'AntiKt4LCTopoJets'
MissingETD3PDMakerFlags.METDefaultJetPrefix = "jet_AntiKt4LCTopo_MET_"
from MissingETD3PDMaker.MissingETD3PDObject         import MissingETD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject         import MissingETCompositionD3PDObject


# Track stuff
from TrackD3PDMaker.TrackD3PDObject                 import TrackParticleD3PDObject
from TauD3PDMaker.makeTrackFilterAlg                import makeTrackFilterAlg
from TrackD3PDMaker.VertexD3PDObject                import PrimaryVertexD3PDObject

# Trigger stuff
if D3PDMakerFlags.DoTrigger():
    from TauD3PDMaker.TauPerfTrigger                    import TauPerfTriggerD3PDObjects
    from TrigTauD3PDMaker.TrigTauD3PD                   import TrigTauD3PDObjects
    from TauD3PDMaker.TauAnalysisTriggerBitsD3PDObject  import TauAnalysisTriggerBitsD3PDObject
    from egammaD3PDMaker.egammaTriggerBitsD3PDObject    import egammaTriggerBitsD3PDObject
    from TrigEgammaD3PDMaker.EFElectronD3PDObject       import EFElectronD3PDObject
    from TrigEgammaD3PDMaker.TrigEgammaD3PD             import TrigEgammaD3PDObjects
    from TrigEgammaD3PDMaker.TrigElectronD3PDObject     import TrigElectronD3PDObject
    from MuonD3PDMaker.MuonTriggerBitsD3PDObject        import MuonTriggerBitsD3PDObject
    from TrigMuonD3PDMaker.TrigMuonD3PD                 import TrigMuonD3PDObjects
    from TrigMissingETD3PDMaker.TrigMETD3PD             import TrigMETD3PDObjects
    from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import METD3PDTriggerBitsObject
    from TrigJetD3PDMaker.TrigJetD3PD                   import TrigJetD3PDObjects
    from TriggerD3PDMaker.TrigDecisionD3PDObject        import TrigDecisionD3PDObject
    from TriggerD3PDMaker.JetROID3PDObject              import JetROID3PDObject
    from CaloD3PDMaker.MBTSTriggerBitsD3PDObject        import MBTSTriggerBitsD3PDObject


# Top stuff
from TopInputsD3PDMaker.HforD3PDObject              import HforD3PDObject
import BoostedTopD3PDMaker

# Temporary to get Jet ROI pT
import EventCommonD3PDMaker

# Jet stuff
from JetD3PDMaker.JetD3PDObject                     import JetD3PDObject
from JetTagD3PDMaker.JetTagD3PDMakerKeys            import JetTagD3PDKeys
from QcdD3PDMaker.QcdEventShapeD3PDObject           import EventShapeD3PDObject

# Calorimeter and event stuff
from CaloD3PDMaker.ClusterD3PDObject                import ClusterD3PDObject
from CaloD3PDMaker.MBTSD3PDObject                   import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject               import MBTSTimeD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject       import LArCollisionTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject      import CollisionDecisionD3PDObject
from EventCommonD3PDMaker.EventInfoD3PDObject       import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig          import LBMetadataConfig

# associations and tools
from EventCommonD3PDMaker.DRIndexAssociation        import DRIndexAssociation
from EventCommonD3PDMaker.DRIndexMultiAssociation   import DRIndexMultiAssociation

# from TauD3PDMaker import TrackCaloParamsFillerTool

#PanTau flags to switch PanTau specifics on/off
from PanTauAnalysis.PanTauFlags import panTauFlags

#----------------------------------------------------------
def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw



#----------------------------------------------------------
def tauPerf (alg=None,
                   level = 10,
                   file='tauPerf.root',
                   doPhys = True,
                   doCl = True,
                   doCell = False,
                   doTrk = True,
                   doHad = True,
                   doOtherTrk = True,
                   tuplename = 'tau',
                   seq = topSequence,
                   D3PDSvc = 'D3PD::RootD3PDSvc',
                   doMETSkimVariables = False,
                   streamName = None,
                   **kw):

    #
    # use track filter!
    #
    filter = makeTrackFilterAlg()
    from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
    
    from AthenaCommon.AlgSequence import AlgSequence
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    preseq +=filter
    #--------------------------------------------------------------------------
    # Additional Variables, set the LOD for some blocks to lower values
    #--------------------------------------------------------------------------
    from egammaD3PDMaker.ElectronD3PDObject import ElectronClusterAssoc
    for block in ElectronClusterAssoc._blocks:
        if block.name == 'Samplings' or block.name == 'ClusterKin':
            block.lod = 0
    from egammaD3PDMaker.ElectronD3PDObject import ElectronRawClusterAssoc
    for block in ElectronRawClusterAssoc._blocks:
        if block.name == 'RawClusterKin':
            block.lod = 0

    for block in MuonD3PDObject._blocks:
        if block.name == 'MuonHitSummary':
            block.lod = 0
        if block.name == 'L1InfoIndex':
            block.lod = 0
            
            
    from MuonD3PDMaker.MuonD3PDObject import MuonFitQualityAssoc
    for block in MuonFitQualityAssoc._blocks:
        if block.name == 'TrkFitQuality':
            block.lod = 0            
    #--------------------------------------------------------------------------
    # Reco Object Overlap Association
    #--------------------------------------------------------------------------
    TauMuonAssoc = DRIndexMultiAssociation(
        parent = TauD3PDObject,
        type_name = 'Analysis::MuonContainer',
        default_sgkey = D3PDMakerFlags.MuonSGKey(),
        default_drcut = 0.2,
        prefix = 'muAssoc_',
        target = 'mu_' )
    TauElectronAssoc = DRIndexMultiAssociation(
        parent = TauD3PDObject,
        type_name = 'ElectronContainer',
        default_sgkey = D3PDMakerFlags.ElectronSGKey(),
        default_drcut = 0.2,
        prefix = 'elAssoc_',
        target = 'el_' )
    TauJetAssoc = DRIndexMultiAssociation(
        parent = TauD3PDObject,
        type_name = 'JetCollection',
        default_sgkey = D3PDMakerFlags.JetSGKey(),
        default_drcut = 0.4,
        prefix = 'jetAssoc_',
        target = 'jet_' )
    JetMuonAssoc = DRIndexMultiAssociation(
        parent = JetD3PDObject,
        type_name = 'Analysis::MuonContainer',
        default_sgkey = D3PDMakerFlags.MuonSGKey(),
        default_drcut = 0.4,
        prefix = 'muAssoc_',
        target = 'mu_' )
    JetElectronAssoc = DRIndexMultiAssociation(
        parent = JetD3PDObject,
        type_name = 'ElectronContainer',
        default_sgkey = D3PDMakerFlags.ElectronSGKey(),
        default_drcut = 0.4,
        prefix = 'elAssoc_',
        target = 'el_' )
    JetTauAssoc = DRIndexMultiAssociation(
        parent = JetD3PDObject,
        type_name = 'Analysis::TauJetContainer',
        default_sgkey = D3PDMakerFlags.TauSGKey(),
        default_drcut = 0.4,
        prefix = 'tauAssoc_',
        target = 'tau_' )

    #associate an EFO to a tau
    # EFOToTauAssoc = DRIndexAssociation(
    #     parent = EflowObjectsD3PDObject,            #Parent: D3PD Object that should be associated to other objects
    #     type_name = 'Analysis::TauJetContainer',    #type_name: Type of target container that has the object to assoc.
    #     default_sgkey = D3PDMakerFlags.TauSGKey(),
    #     default_drcut = 0.4,
    #     prefix = 'EFOtoTauAssoc_',
    #     target = 'tau_' )
        
    # #associate a tau to the efo's in a cone
    # TauToEFOMultiAssoc = DRIndexMultiAssociation(
    #     parent = TauD3PDObject,
    #     type_name = 'eflowObjectContainer',
    #     default_sgkey = 'eflowObjects_tauMode',
    #     default_drcut = 0.4,
    #     prefix = 'TauToEFOAssoc_',
    #     target = 'efo_' )
      
    #############################################
    # Now configure truth-only configs          #
    #############################################
    if rec.doTruth():

        if doHad:
            truthParticleConfig(seq, sgkey=D3PDMakerFlags.TruthParticlesSGKey(), writeHadrons=True, writePartons=True, writeGeant=False)
            
        # tau_trueTauAssoc_index
        TauTrueTauAssoc = DRIndexAssociation(
            parent = TauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = 'TrueHadronicTaus',
            default_drcut = 0.2,
            prefix = 'trueTauAssoc_',
            target = 'trueTau_' )
        # trueTau_tauAssoc_index
        TrueTauTauAssoc = DRIndexAssociation(
            parent = TruthTauD3PDObject,
            type_name = 'Analysis::TauJetContainer',
            default_sgkey = D3PDMakerFlags.TauSGKey(),
            default_drcut = 0.2,
            prefix = 'tauAssoc_',
            target = 'tau_' )
        # tau_truthAssoc_index
        TauTruthAssoc = DRIndexMultiAssociation(
            parent = TauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.45,
            prefix = 'truthAssoc_',
            target = 'mc_' )    
        # trueTau_truthAssoc_index
        TrueTauTruthAssoc = DRIndexMultiAssociation(
            parent = TruthTauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.45,
            prefix = 'truthAssoc_',
            target = 'mc_' )

        # tau_truthTauAssoc_index (!) associate taus to truthTaus (the ones with complete & additional information, not only hadronic ones)
        # TauTrueTauAssoc = DRIndexAssociation(
#             parent = TauD3PDObject,
#             type_name = 'TruthParticleContainer',
#             default_sgkey = 'TruthTaus',
#             default_drcut = 0.2,
#             prefix = 'truthTauAssoc_',
#             target = 'truthTau_' )
        
        ## truthTau_tauAssoc_index (!) associate truth taus to Taus
#        TrueTauTauAssoc = DRIndexAssociation(
#             parent = TruthTauFullD3PDObject,
#             type_name = 'Analysis::TauJetContainer',
#             default_sgkey = D3PDMakerFlags.TauSGKey(),
#            default_drcut = 0.2,
#             prefix = 'tauAssoc_',
#             target = 'tau_' )

        #associate EFO to EFO matched to true pi0 (and vice versa)
        #if panTauFlags.ExtractAllFeatures():
        #    EFOToEFOWithMatchAssoc = DRIndexAssociation(
        #        parent = EflowObjectsD3PDObject,
        #        type_name = 'eflowObjectContainer',
        #        default_sgkey = 'EFOsMatchedToNeutralPions',
        #        default_drcut = 0.05,
        #        prefix = 'EFOtoEFOWithMatch_',
        #        target = 'efoFromTruePi0s_' )
        #    
        #    EFOToTauAssoc = DRIndexAssociation(
        #        parent = EflowObjectsFromTruePi0sD3PDObject,
        #        type_name = 'eflowObjectContainer',
        #        default_sgkey = 'eflowObjects_tauMode',
        #        default_drcut = 0.05,
        #        prefix = 'EFOWithMatchToEFO_',
        #        target = 'efo_' )

        MuonTruthAssoc = DRIndexMultiAssociation(
            parent = MuonD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.2,
            prefix = 'truthAssoc_',
            target = 'mc_' )
        JetTruthAssoc = DRIndexMultiAssociation(
            parent = JetD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.4,
            prefix = 'truthAssoc_',
            target = 'mc_' )
        # truth to reco
        TruthMuonAssoc = DRIndexMultiAssociation(
            parent = TruthParticleD3PDObject,
            type_name = 'Analysis::MuonContainer',
            default_sgkey = D3PDMakerFlags.MuonSGKey(),
            default_drcut = 0.2,
            prefix = 'muAssoc_',
            target = 'mu_' )
        #reco tau to truth egamma: tau_truthEgammaAssoc_index
        TauTruthEgammaAssoc = DRIndexAssociation(
            parent = TauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = 'egammaTruth',
            default_drcut = 0.2,
            prefix = 'truthEgammaAssoc_',
            target = 'egtruth_')
        
        #add egtruth block
        egammaTruthD3PDObject.defineBlock (0, 'TruthEgammaClassification',
                                           TauD3PDMaker.TruthEgammaFillerTool,
                                           Classifier = D3PDMCTruthClassifier)
        egammaTruthGenParticleAssociation = SimpleAssociation(egammaTruthD3PDObject,
                                                              TruthD3PDMaker.TruthParticleGenParticleAssociationTool,
                                                              level = 0)
        IndexAssociation(egammaTruthGenParticleAssociation,
                         TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
                         target="mc_",
                         Target="mc_",
                         level=0,
                         blockname="EgammaTruthParticleMCIndexAssociation",
                         allowMissing=True)
         
    
    #------------------------------
    # Retrieve Jet ROI pT

    if D3PDMakerFlags.DoTrigger():
        JetROID3PDObject.defineBlock( 0, "myKinematics",
                                      EventCommonD3PDMaker.FourMomFillerTool,
                                      WriteM = False, WritePt = True,
                                      prefix = "myKin_")

#    TrackParticleD3PDObject.defineBlock( 0, "TJVAperigee",
#                                         TauD3PDMaker.TrackParticleAtTJVAFillerTool, prefix = 'atTJVA_')


    #------------------------------
    # Special muon isolation variables

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Include import include

    include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

    MuonD3PDObject.defineBlock(0, "miniiso10", BoostedTopD3PDMaker.MiniIsolationMuonFillerTool, MuIsolationTool=ToolSvc.MuonIsolationTool, KtDist=10., prefix="MI10_")
    MuonD3PDObject.defineBlock(0, "miniiso15", BoostedTopD3PDMaker.MiniIsolationMuonFillerTool, MuIsolationTool=ToolSvc.MuonIsolationTool, KtDist=15., prefix="MI15_")
    
    
    #--------------------------------------------------------------------------
    # Make the D3PD
    #--------------------------------------------------------------------------

    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(streamName, file, tuplename)

    ###################################
    ## Tau objects
    alg += EventTauD3PDObject(level)
    TauExcludeList = ['OtherTrk00', 'OtherTrkTJVA', 'OtherTrkPV', 'OtherIDHits']
    if doOtherTrk:
        TauExcludeList = []
    if not doCell:
        TauExcludeList.append('TauCellsDetails')
    alg += TauD3PDObject (level, exclude=TauExcludeList)

    #alg += EflowObjectsD3PDObject(level)

    ###################################
    ## Truth
    if rec.doTruth():
        #if panTauFlags.ExtractAllFeatures():
        #    alg += EflowObjectsFromTruePi0sD3PDObject(level)
        alg += TruthParticleD3PDObject (10, sgkey=D3PDMakerFlags.TruthParticlesSGKey())
        alg += GenEventD3PDObject (1)
        alg += TruthTauD3PDObject(10)
        #alg += TruthTauFullD3PDObject(10)
        alg += TruthMuonD3PDObject (**_args ( 2, 'TruthMuon', kw))
        alg += JetD3PDObject          (**_args ( 3, 'AK4TruthJet', kw,
                                                 sgkey='AntiKt4TruthJets',
                                                 prefix='jet_antikt4truth_',
                                                 allowMissing = True))
        alg += egammaTruthD3PDObject(0)

    ###################################
    ## Egamma
    PhotonIncludeList=['Iso',
                       'IsoPtCorrected',
                       'PAU',
                       'PAUVariables',
                       'Pointing',
                       'ClusterKin',
                       'HadLeakage',
                       'Layer1Shape',
                       'Layer2Shape',
                       'Samplings']
    ElectronIncludeList=['Pointing',
                         'EMTrackFitDetailsBrem',
                         'EMTrackFitDetailsRefit']
    alg += ElectronD3PDObject (1, include=ElectronIncludeList)
    alg += PhotonD3PDObject (0, include=PhotonIncludeList)

    ###################################
    ## Calorimeter, Jets and MET
    if doCl:
        alg += ClusterD3PDObject (-1, include='Kinematics')

    JetIncludeList = ['BTagDefault',
                      JetTagD3PDKeys.BTagWeightsBlockName(),
                      JetTagD3PDKeys.TruthInfoBlockName(),
                      'JetVertexFraction']

    JetMV3IncludeList = [JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
                         JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName()]

    alg += JetD3PDObject (1, "AntiKt4Topo", "jet_", '', '', sgkey='AntiKt4LCTopoJetsReTagged', include=JetIncludeList + ['ConstituentScale', 'TracksMoments', 'Samplings'] + JetMV3IncludeList)
    alg += JetD3PDObject (1, "AntiKt4TopoEM", "jet_AntiKt4TopoEM_", '', '', sgkey='AntiKt4TopoEMJets', include=JetIncludeList)
    alg += JetD3PDObject (0, "AntiKt6LCTopo", "jet_AntiKt6LCTopo_", '', '', sgkey='AntiKt6LCTopoJets', include=JetIncludeList + ['ConstituentScale'])

    
    alg += EventShapeD3PDObject(**_args(0, 'rho', kw))

    
    METIncludeList=['MET_MuonBoy',
                    'MET_Track',
                    'MET_RefGamma',
                    'MET_RefEle',
                    'MET_RefJet',
                    'MET_RefMuon',
                    'MET_RefMuon_Track',
                    'MET_RefTau',
                    'MET_RefFinal',
                    'MET_RefFinal_em',
                    'MET_SoftJets',
                    'MET_SoftJets_em',
                    'MET_Cryo',
                    'MET_CellOut',
                    'MET_CellOut_Eflow',
                    'MET_Muid',
                    'MET_Muon',
                    'MET_Final',
                    'MET_Topo',
                    'MET_Topo_Samplings',
                    'MET_Topo_Regions',
                    'MET_LocHadTopo',
                    'MET_LocHadTopo_Samplings',
                    'MET_LocHadTopo_Regions',
                    'MET_Truth_NonInt',
                    'METComposition']
    
    alg += MissingETD3PDObject (**_args (10, 'MissingET', kw, allowMissing = True, include=METIncludeList))

    ## Other MET
    customMETList = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet', 'MET_CellOut', 'MET_SoftJets', 'MET_RefMuon_Staco','MET_Muon_Total_Staco','MET_Muon_Total_Muid']
     
    customMETConf = ['BDTMedium','AntiKt4LCTopo_tightpp']

    ## METcomposition map for tau METRefFinal ( _BDTMedium_ )
    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_BDTMedium', suffix='BDTMedium_', allowMissing =True, jetSGKey='AntiKt4LCTopoJets', jetPrefix='jet_AntiKt4LCTopo_MET_' )

    ## METcomposition map for H+ MetRefFinal ( _em_tightpp_ )
    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_AntiKt4LCTopo_tightpp', suffix='AntiKt4LCTopo_tightpp_', allowMissing =True, jetSGKey='AntiKt4LCTopoJets', jetPrefix='jet_AntiKt4LCTopo_tightpp_MET_' )
    
    
    for conf in customMETConf:
        for item in customMETList:
            alg += MissingETD3PDObject (**_args(0, 'MissingET'+'_'+conf, kw, sgkey= item+'_'+conf, prefix=item+'_'+conf, include='METComposition', allowMissing = True))
            
       
    ###################################
    ## Muons
    alg += MuonD3PDObject (**_args (10, 'Muon', kw,
                                    sgkey='Muons',
                                    prefix='mu_'))
            
    alg += MuonD3PDObject (**_args (10, 'MuidMuon', kw,
                                    sgkey='MuidMuonCollection',
                                    prefix='mu_muid_'))
    
    alg += MuonD3PDObject (**_args (10, 'StacoMuon', kw,
                                    sgkey='StacoMuonCollection',
                                    prefix='mu_staco_'))
    
    alg += MuonD3PDObject (**_args (10, 'CaloMuon', kw,
                                    sgkey='CaloMuonCollection',
                                    prefix='mu_calo_'))

    ###################################
    ## Top
    alg += HforD3PDObject (**_args (0, 'HforInfo', kw))

    ###################################
    ## Tracks
    if doTrk:
        alg += TrackParticleD3PDObject (0, sgkey="TauSelectedTracks")
    alg += PrimaryVertexD3PDObject (0, allowMissing = True,
                                    sgkey = D3PDMakerFlags.VertexSGKey(),
                                    prefix = 'vxp_')

    
    ###################################
    ## General event info and cleaning
    alg += EventInfoD3PDObject (10)
    alg += CollisionDecisionD3PDObject (10)
    alg.MetadataTools += [LBMetadataConfig()]

    
    ###################################
    ## Trigger info
    if D3PDMakerFlags.DoTrigger():

        #Bits
        alg += TrigDecisionD3PDObject (2)
        
        #Tau
        alg += TauAnalysisTriggerBitsD3PDObject (level)
        #TauPerfTriggerD3PDObjects(alg, 0)
        TrigTauD3PDObjects (alg, 2)

        #Egamma
        alg += egammaTriggerBitsD3PDObject (0)
        alg += EFElectronD3PDObject(level, include=['Triggers'],allowMissing = True)
        alg += TrigElectronD3PDObject(level)

        #Muons
        alg += MuonTriggerBitsD3PDObject (1)
        TrigMuonD3PDObjects( alg, 1)

        #MET
        alg += METD3PDTriggerBitsObject (0)
        TrigMETD3PDObjects (alg, 1)

        #Jets
        alg += JetROID3PDObject (0)
        TrigJetD3PDObjects(alg, level=2, addNaviInfo = False, addConfigInfo = False)

        #BJets
        from TrigBJetD3PDMaker.TrigBJetD3PD import TrigBJetD3PDObjects
        TrigBJetD3PDObjects(alg, 2, False, False, True, False)

        ###################################
        ## bunch structure metadata
        from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
        addBunchStructureMetadata( alg )

    return alg
