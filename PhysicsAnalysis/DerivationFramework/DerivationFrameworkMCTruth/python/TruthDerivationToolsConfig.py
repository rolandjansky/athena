# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Provides configs for the tools used for building the common truth collections
# Note that taus are handled separately (see MCTruthCommon.py)
# Note also that navigation info is dropped here and added separately
# Two kinds of config are defined here - for general config of the tools
# and for specific configurations, which implement the former.
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AllConfigFlags import ConfigFlags

def _simBarcodeOffset():
    if hasattr(ConfigFlags,'Sim') and hasattr(ConfigFlags.Sim,'SimBarcodeOffset'):
        return ConfigFlags.Sim.SimBarcodeOffset
    return 200000


#==============================================================================
# TruthCollectionMaker instances
#==============================================================================

def TruthCollectionMakerCfg(name,**kwargs):
    """Configure the TruthCollectionMaker tool"""
    acc = ComponentAccumulator()
    TruthCollectionMaker = CompFactory.DerivationFramework.TruthCollectionMaker
    acc.addPublicTool(TruthCollectionMaker(name = name,**kwargs),
                      primary = True)
    return acc

def DFCommonTruthMuonToolCfg():
    """Muon truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthMuonTool",
                                   NewCollectionName       = "TruthMuons",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(_simBarcodeOffset()))


def DFCommonTruthElectronToolCfg():
    """Electron truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthElectronTool",
                                   NewCollectionName       = "TruthElectrons",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(_simBarcodeOffset()))

def DFCommonTruthPhotonToolCfg():
    """Photon truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthPhotonTool",
                                   NewCollectionName       = "TruthPhotons",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(_simBarcodeOffset()))   

# this tool is needed for making TruthPhotons from sim samples, where extra cuts are needed. Origin 42 (pi0) and 23 (light meson) cut way down uninteresting photons
def DFCommonTruthPhotonToolSimCfg():
    """Tool for making TruthPhotons from sim samples"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthPhotonToolSim",
                                   NewCollectionName       = "TruthPhotons",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && ((TruthParticles.classifierParticleOrigin != 42 && TruthParticles.classifierParticleOrigin !=23) || (TruthParticles.pt > 20.0*GeV)) && ( TruthParticles.barcode < "+str(_simBarcodeOffset())+")")   

def DFCommonTruthNeutrinoToolCfg():
    """Neutrino truth collection maker"""
    neutrinoexpression = "(TruthParticles.isNeutrino && TruthParticles.status == 1) && TruthParticles.barcode < "+str(_simBarcodeOffset())
    return TruthCollectionMakerCfg(name = "DFCommonTruthNeutrinoTool",
                                   NewCollectionName       = "TruthNeutrinos",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = neutrinoexpression)

def DFCommonTruthBottomToolCfg():
    """B-quark truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthBottomTool",
                                   NewCollectionName       = "TruthBottom",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 5)",
                                   Do_Compress             = True)

def DFCommonTruthTopToolCfg():
    """Top-quark truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthTopTool",
                                   NewCollectionName       = "TruthTop",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 6)",
                                   Do_Compress             = True)
                                   
def DFCommonTruthBosonToolCfg():
    """Gauge bosons and Higgs truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthBosonTool",
                                   NewCollectionName       = "TruthBoson",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 23 || abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) == 25)",
                                   Do_Compress             = True,
                                   Do_Sherpa               = True)

def DFCommonTruthBSMToolCfg():
    """BSM particles truth collection maker"""
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthBSMTool",
                                   NewCollectionName       = "TruthBSM",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(TruthParticles.isBSM)",
                                   Do_Compress             = True)

# Superfluous lines below???
## Set up a tool to keep forward protons for AFP
#if not DerivationFrameworkRunningEvgen:
#    from RecExConfig.InputFilePeeker import inputFileSummary
#    if 'beam_energy' in inputFileSummary:
#        beam_energy = inputFileSummary['beam_energy']
#    elif '/TagInfo' in inputFileSummary and 'beam_energy' in inputFileSummary['/TagInfo']:
#        beam_energy = inputFileSummary['/TagInfo']['beam_energy']
#    elif 'metadata_itemsList' in inputFileSummary and 'tag_info' in inputFileSummary['metadata_itemsList'] and 'beam_energy' in inputFileSummary['metadata_itemsList']['tag_info']:
#        beam_energy = inputFileSummary['metadata_itemsList']['tag_info']['beam_energy']
#    else:
#        from AthenaCommon import Logging
#        dfcommontruthlog = Logging.logging.getLogger('DFCommonTruth')
#        dfcommontruthlog.warning('Could not find beam energy in input file. Using default of 6.5 TeV')
#        beam_energy = 6500000.0 # Sensible defaults
#else:
#    from AthenaCommon import Logging
#    dfcommontruthlog = Logging.logging.getLogger('DFCommonTruth')
#    dfcommontruthlog.warning('Could not find beam energy in input file - running evgen? Using default of 6.5 TeV')
#    beam_energy = 6500000.0 # Sensible defaults
#
## Weird formats in some metadata
#if type(beam_energy) is list: beam_energy = beam_energy[0]

def DFCommonTruthForwardProtonToolCfg(flags):
    """Forward proton truth collection maker"""
    beam_energy = flags.Beam.Energy
    return TruthCollectionMakerCfg(name                    = "DFCommonTruthForwardProtonTool",
                                   NewCollectionName       = "TruthForwardProtons",
                                   KeepNavigationInfo      = False,
                                   ParticleSelectionString = "(TruthParticles.status==1) && (abs(TruthParticles.pdgId)==2212) && (TruthParticles.e>0.8*"+str(beam_energy)+")",
                                   Do_Compress             = True) 

#==============================================================================
# Decoration tools
#==============================================================================

def TruthD2DecoratorCfg(name, **kwargs):
    """Configure the truth D2 decorator tool"""
    acc = ComponentAccumulator()
    TruthD2Decorator = CompFactory.DerivationFramework.TruthD2Decorator
    acc.addPublicTool(TruthD2Decorator(name, **kwargs), primary = True)
    return acc

def DFCommonMCTruthClassifierCfg():
    """Configure the MCTruthClassifier tool"""
    acc = ComponentAccumulator()
    MCTruthClassifier = CompFactory.MCTruthClassifier
    acc.addPublicTool(MCTruthClassifier(name = "DFCommonTruthClassifier", ParticleCaloExtensionTool = ""),
                      primary = True)
    return acc

def TruthClassificationDecoratorCfg(name, **kwargs):
    """Configure the TruthClassificationDecorator tool"""
    acc = ComponentAccumulator()
    TruthClassificationDecorator = CompFactory.DerivationFramework.TruthClassificationDecorator
    acc.addPublicTool(TruthClassificationDecorator(name = name, **kwargs),
                      primary = True)
    return acc

def TruthDressingToolCfg(name, **kwargs):
    """Configure the TruthDressingTool"""
    acc = ComponentAccumulator()
    TruthDressingTool = CompFactory.DerivationFramework.TruthDressingTool
    acc.addPublicTool(TruthDressingTool( name = name, **kwargs),
                      primary = True)   
    return acc

def TruthIsolationToolCfg(name, **kwargs):
    """Configure the truth isolation tool"""
    acc = ComponentAccumulator()
    TruthIsolationTool = CompFactory.DerivationFramework.TruthIsolationTool
    acc.addPublicTool(TruthIsolationTool(name = name, **kwargs),
                      primary = True)
    return acc 

def TruthQGDecorationToolCfg(name, **kwargs):
    """Configure the quark/gluon decoration tool"""
    acc = ComponentAccumulator()
    TruthQGDecorationTool = CompFactory.DerivationFramework.TruthQGDecorationTool
    acc.addPublicTool(TruthQGDecorationTool(name = name, **kwargs),
                      primary = True)
    return acc

def TruthNavigationDecoratorCfg(name, **kwargs):
    """Congigure the truth navigation decorator tool"""
    acc = ComponentAccumulator()
    TruthNavigationDecorator = CompFactory.DerivationFramework.TruthNavigationDecorator
    acc.addPublicTool(TruthNavigationDecorator(name = name, **kwargs),
                      primary = True)
    return acc

def TruthDecayCollectionMakerCfg(name, **kwargs):
    """Configure the truth decay collection maker"""
    acc = ComponentAccumulator()
    TruthDecayCollectionMaker = CompFactory.DerivationFramework.TruthDecayCollectionMaker
    acc.addPublicTool(TruthDecayCollectionMaker(name = name, **kwargs),
                      primary = True)
    return acc

def TruthBornLeptonCollectionMakerCfg(name, **kwargs):
    """Configure the truth Born lepton collection tool"""
    acc = ComponentAccumulator()
    TruthBornLeptonCollectionMaker = CompFactory.DerivationFramework.TruthBornLeptonCollectionMaker
    acc.addPublicTool(TruthBornLeptonCollectionMaker(name = name, **kwargs),
                      primary = True)
    return acc

def HardScatterCollectionMakerCfg(name, **kwargs):
    """Add a mini-collection for the hard scatter and N subsequent generations"""
    acc = ComponentAccumulator()
    HardScatterCollectionMaker = CompFactory.DerivationFramework.HardScatterCollectionMaker
    acc.addPublicTool(HardScatterCollectionMaker(name = name, **kwargs),
                      primary = True)
    return acc

#add the 'decoration' tool to dress the main truth collection with the classification
def DFCommonTruthClassificationToolCfg():
    """dress the main truth collection with the classification"""
    acc = ComponentAccumulator() 
    DFCommonTruthClassifier = acc.getPrimaryAndMerge(DFCommonMCTruthClassifierCfg())
    return TruthClassificationDecoratorCfg(name = "DFCommonTruthClassificationTool",
                                           ParticlesKey = "TruthParticles",
                                           MCTruthClassifier = DFCommonTruthClassifier)

#add the 'decoration' tools for dressing and isolation
def DFCommonTruthElectronDressingToolCfg(decorationName = "FIXME_WHATSHOULDTHISBE"):
    """Configure the electron truth dressing tool"""
    return TruthDressingToolCfg(name                  = "DFCommonTruthElectronDressingTool",
                                dressParticlesKey     = "TruthElectrons",
                                usePhotonsFromHadrons = False,
                                dressingConeSize      = 0.1,
                                particleIDsToDress    = [11],
                                decorationName        = decorationName)

def DFCommonTruthMuonDressingToolCfg(decorationName = "FIXME_WHATSHOULDTHISBE"):
    """Configure the muon truth dressing tool"""
    return TruthDressingToolCfg(name                  = "DFCommonTruthMuonDressingTool",
                                dressParticlesKey     = "TruthMuons",
                                usePhotonsFromHadrons = False,
                                dressingConeSize      = 0.1,
                                particleIDsToDress    = [13],
                                decorationName        = decorationName)

def DFCommonTruthTauDressingToolCfg():
    """Configure the tau truth dressing tool"""
    return TruthDressingToolCfg(name                  = "DFCommonTruthTauDressingTool",
                                dressParticlesKey     = "TruthTaus",
                                usePhotonsFromHadrons = False,
                                dressingConeSize      = 0.2, # Tau special
                                particleIDsToDress    = [15])

def DFCommonTruthElectronIsolationTool1Cfg():
    """Configure the electron isolation tool, cone=0.2"""
    return TruthIsolationToolCfg(name                   = "DFCommonTruthElectronIsolationTool1",
                                 isoParticlesKey        = "TruthElectrons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [11],
                                 IsolationConeSizes     = [0.2],
                                 IsolationVarNamePrefix = 'etcone',
                                 ChargedParticlesOnly   = False)

def DFCommonTruthElectronIsolationTool2Cfg():
    """Configure the electron isolation tool, cone=0.3"""
    return TruthIsolationToolCfg(name                   =  "DFCommonTruthElectronIsolationTool2",
                                 isoParticlesKey        = "TruthElectrons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [11],
                                 IsolationConeSizes     = [0.3],
                                 IsolationVarNamePrefix = 'ptcone',
                                 ChargedParticlesOnly   = True)

def DFCommonTruthMuonIsolationTool1Cfg():
    """Configure the muon isolation tool, cone=0.2"""
    return TruthIsolationToolCfg(name                   = "DFCommonTruthMuonIsolationTool1",
                                 isoParticlesKey        = "TruthMuons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [13],
                                 IsolationConeSizes     = [0.2],
                                 IsolationVarNamePrefix = 'etcone',
                                 ChargedParticlesOnly   = False) 

def DFCommonTruthMuonIsolationTool2Cfg():
    """Configure the muon isolation tool, cone=0.3"""
    return TruthIsolationToolCfg(name                   = "DFCommonTruthMuonIsolationTool2",
                                 isoParticlesKey        = "TruthMuons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [13],
                                 IsolationConeSizes     = [0.3],
                                 IsolationVarNamePrefix = 'ptcone',
                                 ChargedParticlesOnly   = True)

def DFCommonTruthPhotonIsolationTool1Cfg():
    """Configure the photon isolation tool, etcone"""
    return TruthIsolationToolCfg(name                   = "DFCommonTruthPhotonIsolationTool1",
                                 isoParticlesKey        = "TruthPhotons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [22],
                                 IsolationConeSizes     = [0.2],
                                 IsolationVarNamePrefix = 'etcone',
                                 ChargedParticlesOnly   = False)    


def DFCommonTruthPhotonIsolationTool2Cfg():
    """Configure the photon isolation tool, ptcone"""
    return  TruthIsolationToolCfg(name                   = "DFCommonTruthPhotonIsolationTool2",
                                  isoParticlesKey        = "TruthPhotons",
                                  allParticlesKey        = "TruthParticles",
                                  particleIDsToCalculate = [22],
                                  IsolationConeSizes     = [0.2],
                                  IsolationVarNamePrefix = 'ptcone',
                                  ChargedParticlesOnly   = True)

def DFCommonTruthPhotonIsolationTool3Cfg():
   """Configure the photon isolation tool, etcone=0.4"""
   return  TruthIsolationToolCfg(name                   = "DFCommonTruthPhotonIsolationTool3",
                                 isoParticlesKey        = "TruthPhotons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [22],
                                 IsolationConeSizes     = [0.4],
                                 IsolationVarNamePrefix = 'etcone',
                                 ChargedParticlesOnly   = False)

# Quark/gluon decoration for jets
def DFCommonTruthDressedWZQGLabelToolCfg():
    """Configure the QG decoration tool for AntiKt4TruthDressedWZJets"""
    return TruthQGDecorationToolCfg(name          = "DFCommonTruthDressedWZQGLabelTool",
                                    JetCollection = "AntiKt4TruthDressedWZJets")

#==============================================================================
# Other tools 
#==============================================================================
# Truth links on some objects point to the main truth particle container. 
# This re-points the links from the old container to the new container
def TruthLinkRepointToolCfg(ConfigFlags, name, **kwargs):
    """Configure the truth link repointing tool"""
    acc = ComponentAccumulator()
    TruthLinkRepointTool = CompFactory.DerivationFramework.TruthLinkRepointTool
    acc.addPublicTool(TruthLinkRepointTool(name, **kwargs),
                      primary = True)
    return acc   

# Makes a small collection of 'primary' vertices, one per event
# A bit like a collection of 'reconstructable' vertices
def TruthPVCollectionMakerCfg(ConfigFlags, name, **kwargs):
    """Configure the truth PV collection maker tool"""
    acc = ComponentAccumulator()
    TruthPVCollectionMaker = CompFactory.DerivationFramework.TruthPVCollectionMaker
    acc.addPublicTool(TruthPVCollectionMaker(name, **kwargs),
                      primary = True)
    return acc
