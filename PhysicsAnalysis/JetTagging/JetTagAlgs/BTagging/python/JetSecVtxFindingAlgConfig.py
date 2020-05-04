# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.NewJetFitterVxFinderConfig import NewJetFitterVxFinderCfg
from BTagging.InDetVKalVxInJetToolConfig import InDetVKalVxInJetToolCfg

Analysis__JetSecVtxFindingAlg=CompFactory.Analysis.JetSecVtxFindingAlg

def JetSecVtxFindingAlgCfg(ConfigFlags, JetCollection, PrimaryVertexCollectionName="", SVFinder="", Associator="", **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

    acc = ComponentAccumulator()

    jetcol = JetCollection

    if SVFinder == 'JetFitter':
        secVtxFinder = acc.popToolsAndMerge(NewJetFitterVxFinderCfg(ConfigFlags, 'JFVxFinder'))
    elif SVFinder == 'SV1':
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalVxInJet"))
    elif SVFinder == 'MSV':
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalMultiVxInJet", MSV = True))
    else:
        return acc

    options = {}
    options.setdefault('SecVtxFinder', secVtxFinder)
    options.setdefault('vxPrimaryCollectionName', PrimaryVertexCollectionName)
    options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
    options['TrackToJetAssociatorName'] = options['JetCollectionName'] + '.' + Associator
    options['BTagVxSecVertexInfoName'] = SVFinder + 'VxSecVertexInfo_' + JetCollection
    options['name'] = (jetcol + '_' + SVFinder + '_secvtxfinding').lower()

    # -- create the association algorithm
    acc.addEventAlgo(Analysis__JetSecVtxFindingAlg(**options))

    return acc
