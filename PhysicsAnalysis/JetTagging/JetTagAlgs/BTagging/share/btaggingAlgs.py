# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#Run btagging on AntiKt4EMTopoJets

from BTagging.BTaggingConfiguration import getConfiguration
BTagConf = getConfiguration()
BTagConf.PrefixxAODBaseName(False)
BTagConf.PrefixVertexFinderxAODBaseName(False)

BTagConf.doNotCheckForTaggerObstacles()

#Rename the existing btaggingLink from Jet container to BTagging container
from SGComps.SGCompsConf import AddressRemappingSvc
AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.btaggingLink->AntiKt4EMTopoJets.oldbtaggingLink']
svcMgr+= AddressRemappingSvc

taggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2rmu', 'MV2r', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1r', 'DL1rmu','DL1mu', 'RNNIP']
btagalg = BTagConf.setupJetBTaggerAlg(JetCollection="AntiKt4EMTopoJets", TaggerList=taggerList)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += btagalg
