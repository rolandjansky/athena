# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr      import ToolSvc
import PhotonD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags    import PAUflags
import EventCommonD3PDMaker
from AthenaCommon.Constants                          import WARNING, INFO, ERROR, VERBOSE
import TruthD3PDMaker
from TruthD3PDMaker.TruthParticleChildAssociation    import TruthParticleChildAssociation
from TruthD3PDMaker.TruthParticleParentAssociation   import TruthParticleParentAssociation
from D3PDMakerCoreComps.SimpleAssociation            import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation             import IndexAssociation

DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool

# ---------------------------------------------------------------------------------
# Now, setup a special collection for egamma truth objects.
# This is messy, hopefully someday I can come back and clean this up.
# 
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUcaloIsolationTool
from PhotonD3PDMaker.PAUCaloIsolationTool import EMCaloIsoPtCorrectionTool
PAUTruthCaloisolationtool = PAUcaloIsolationTool("PAUTruthCaloIsolationTool")
PAUTruthCaloisolationtool.DoAreaCorrections = PAUflags.DoReRunJetFinding()
PAUTruthCaloisolationtool.JetCollectionName = "Kt5TruthJets"
PAUTruthCaloisolationtool.JetAreaEtaBins    = PAUflags.JetAreaEtaBins()
PAUTruthCaloisolationtool.UseOldPtCorrections = PAUflags.UseOldPtCorrections()
PAUTruthCaloisolationtool.UseOldAreaCorrections = True
PAUTruthCaloisolationtool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool
PAUTruthCaloisolationtool.OutputLevel       = WARNING 
PAUTruthCaloisolationtool.JetAreaMomentName = PAUflags.JetAreaMomentName()
ToolSvc += PAUTruthCaloisolationtool

TruthParticlesSGKey = "SpclMC" # was D3PDMakerFlags.TruthParticlesSGKey() before...

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += PhotonD3PDMaker.EgammaTruthParticleFilter("EgammaTruthParticleFilter",
                                                    PhotonGetter  = DVGetter('TruthFilterPhotonGetter',
                                                                             TypeName = 'PhotonContainer',
                                                                             SGKey = D3PDMakerFlags.PhotonSGKey()),
                                                    ElectronGetter = DVGetter('TruthFilterElectronGetter',
                                                                              TypeName = 'ElectronContainer',
                                                                              SGKey = D3PDMakerFlags.ElectronSGKey()),
                                                    TruthGetter = DVGetter('TruthFilterTruthParticleGetter',
                                                                           TypeName = 'TruthParticleContainer',
                                                                           SGKey = TruthParticlesSGKey),
                                                    PAUtruthTool                = ToolSvc.PAUtruthTool,
                                                    OutputTruthCollectionName   = "FilteredEgammaTruthCollection")

preseq += PhotonD3PDMaker.TruthExplorerAlg("PhotonD3PDTruthExplorer",
                                           PhotonGetter  = DVGetter('TruthExplorerPhotonGetter',
                                                                    TypeName = 'PhotonContainer',
                                                                    SGKey = D3PDMakerFlags.PhotonSGKey()),
                                           ElectronGetter = DVGetter('TruthExplorerElectronGetter',
                                                                     TypeName = 'ElectronContainer',
                                                                     SGKey = D3PDMakerFlags.ElectronSGKey()),
                                           TruthParticleGetter = DVGetter('TruthExplorerTruthParticleGetter',
                                                                          TypeName = 'TruthParticleContainer',
                                                                          SGKey = "FilteredEgammaTruthCollection"),
                                           PAUtruthTool                = ToolSvc.PAUtruthTool,
                                           CalculateTruthCaloIsolation = PAUflags.FindLowPtTruthJets(),
                                           RemoveNHardestJets          = PAUflags.JetAreaJetsToRemove(),
                                           PAUCaloIsolationTool        = PAUTruthCaloisolationtool,
                                           UDPrefix                    = "PhotonD3PDTruthUD::")


EgammaTruthParticleD3PDObject = make_SGDataVector_D3PDObject \
                                ('TruthParticleContainer',
                                 D3PDMakerFlags.TruthParticlesSGKey(),
                                 'egammaTruth_',
                                 'EgammaTruthParticleD3PDObject')

EgammaTruthParticleD3PDObject.defineBlock (0, 'TruthKin',
                                           EventCommonD3PDMaker.FourMomFillerTool)

EgammaTruthParticleD3PDObject.defineBlock (0, 'TruthInfo',
                                           TruthD3PDMaker.TruthParticleFillerTool)

ProdVertexAssoc = SimpleAssociation \
                  (EgammaTruthParticleD3PDObject,
                   TruthD3PDMaker.TruthParticleProdVertexAssociationTool,
                   level = 1,
                   prefix = 'vx_',
                   blockname = 'ProdVert')
ProdVertexAssoc.defineBlock (1, 'ProdVertPos',
                             EventCommonD3PDMaker.GenVertexFillerTool)


ChildAssoc = TruthParticleChildAssociation(
    parent = EgammaTruthParticleD3PDObject,
    prefix = 'child_',
    # target = '', # filled by hook
    level = 0 )
def _TruthParticleChildAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_TruthParticleChildAssociation', None)
    if assoc:
        # This produces warnings, I'm not sure how to configure this properly to avoid the warnings....
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
        return
    EgammaTruthParticleD3PDObject.defineHook(_TruthParticleChildAssocHook)
    
ParentAssoc = TruthParticleParentAssociation(
    parent = EgammaTruthParticleD3PDObject,
    prefix = 'parent_',
    # target = '', # filled by hook
    level = 0 )
def _TruthParticleParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_parent_TruthParticleParentAssociation', None)
    if assoc:
        # This produces warnings, I'm not sure how to configure this properly to avoid the warnings....
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
        return
    EgammaTruthParticleD3PDObject.defineHook(_TruthParticleParentAssocHook)

# Link to the truth particle's parents in the mc_ block.
doSimpleIndexing=False
if doSimpleIndexing:
    # in principle I feel like this should work, but I wonder if things are getting
    # screwed up because I'm playing games with sorting the mc_ block.
    EgammaTruthParticleD3PDObject.defineBlock(1, "MCIndex",
                                              D3PDMakerCoreComps.IndexFillerTool,
                                              Target="mc_",
                                              prefix="mc_",
                                              AllowMissing=True)
else:
    # something more complicated, this seems to do the association a bit better.
    EgammaTruthParticleGenParticleAssociation = SimpleAssociation(EgammaTruthParticleD3PDObject,
                                                                  TruthD3PDMaker.TruthParticleGenParticleAssociationTool,
                                                                  level = 1)

    IndexAssociation(EgammaTruthParticleGenParticleAssociation,
                     TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
                     target="mc_",
                     Target="mc_",
                     prefix="mc_",
                     level=1,
                     blockname="EgammaTruthParticleMCIndexAssociation",
                     allowMissing=True)
    
    
EgammaTruthParticleD3PDObject.defineBlock (1, 'TruthIDInfo',
                                           D3PDMakerCoreComps.UserDataFillerTool,
                                           UDPrefix = "PhotonD3PDTruthUD::",
                                           Vars = ['TruthConv' , '', 'int',
                                                   'XTruthConv', '', 'float',
                                                   'YTruthConv', '', 'float',
                                                   'ZTruthConv', '', 'float',
                                                   'RTruthConv', '', 'float',
                                                   'isHardProcPhoton', '', 'int',
                                                   'isBremPhoton'    , '', 'int',
                                                   'isPromptPhoton'  , '', 'int',
                                                   'PartonIsolation15_UE', '', 'float',
                                                   'PartonIsolation20_UE', '', 'float',
                                                   'PartonIsolation25_UE', '', 'float',
                                                   'PartonIsolation30_UE', '', 'float',
                                                   'PartonIsolation35_UE', '', 'float',
                                                   'PartonIsolation40_UE', '', 'float',
                                                   'PartonIsolation45_UE', '', 'float',
                                                   'PartonIsolation60_UE', '', 'float',
                                                   'PartonIsolation15', '', 'float',
                                                   'PartonIsolation20', '', 'float',
                                                   'PartonIsolation30', '', 'float',
                                                   'PartonIsolation25', '', 'float',
                                                   'PartonIsolation35', '', 'float',
                                                   'PartonIsolation40', '', 'float',
                                                   'PartonIsolation45', '', 'float',
                                                   'PartonIsolation60', '', 'float',
                                                   'ParticleIsolation40', '', 'float',
                                                   'ParticleIsolation30', '', 'float',
                                                   'ParticleIsolation20', '', 'float',
                                                   'ParticleIsolation40_ED_corrected', '', 'float',
                                                   'ParticleIsolation30_ED_corrected', '', 'float',
                                                   'ParticleIsolation20_ED_corrected', '', 'float',
                                                   'mother_pdgId', '', 'int'])
     
