# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
''' A basic postInclude file to test high lossy float compression in DAOD_PHYS production  '''

# Get a handle to the StreamDAOD_PHYS
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
myStream = MSMgr.GetStream( derivationFlags.WriteDAOD_PHYSStream.StreamName )

# All float and vector<float> variables (except pt, eta, phi, and m) 
# will be compressed to 7-bit mantissa for these containers
myHighLossyFloatCompressionList = [
    'TruthCharm',
    'BTagging_AntiKt4EMPFlow',
    'TruthMuons',
    'TruthNeutrinos',
    'TruthBottom',
    'MuonSpectrometerTrackParticles',
    'DiTauJetsLowPt',
    'TruthBoson',
    'MET_Track',
    'TauSecondaryVertices',
    'TruthBosonsWithDecayVertices',
    'TauJets',
    'TruthElectrons',
    'xTrigDecision',
    'ExtrapolatedMuonTrackParticles',
    'CombinedMuonTrackParticles',
    'TruthTaus',
    'TruthPhotons',
    'AntiKt10TruthTrimmedPtFrac5SmallR20Jets',
    'TruthBosonsWithDecayParticles',
    'PrimaryVertices',
    'Muons',
    'AntiKt4TruthDressedWZJets',
    'TruthHFWithDecayVertices',
    'METAssoc_AntiKt4EMPFlow',
    'METAssoc_AntiKt4EMTopo',
    'HardScatterVertices',
    'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets',
    'Photons',
    'Electrons',
    'InDetTrackParticles',
    'GSFTrackParticles',
    'AntiKt10LCTopoJets',
    'TruthHFWithDecayParticles',
    'egammaClusters',
    'HardScatterParticles',
    'AntiKt4EMPFlowJets',
    'AntiKt4EMTopoJets' 
]

# Set the compression list for the underlying stream object
if myStream is not None:
    fullAuxList = [ item for item in myStream.GetItems() if 'Aux.' in item ]
    for cont in myHighLossyFloatCompressionList:
        for item in fullAuxList:
            if cont in item:
                compInput = f"{item.split('.')[0]}.-pt.-eta.-phi.-m"
                myStream.Stream.CompressionListHigh += [ compInput ]
                break

# Don't clutter the global namespace
del myStream, myHighLossyFloatCompressionList
