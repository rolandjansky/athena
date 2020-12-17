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

taggerList = ['IP2D', 'IP3D', 'SV1', 'JetFitterNN', 'MV2c10']
#JetVertexCharge disable ATLASRECTS-4506
btagalg = BTagConf.setupJetBTaggerAlg(JetCollection="AntiKt4EMTopoJets", TaggerList=taggerList)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += btagalg
