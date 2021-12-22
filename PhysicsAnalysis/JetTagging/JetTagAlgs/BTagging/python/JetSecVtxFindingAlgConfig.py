# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.InDetImprovedJetFitterVxFinderConfig import InDetImprovedJetFitterVxFinderCfg
from BTagging.InDetVKalVxInJetToolConfig import InDetVKalVxInJetToolCfg

Analysis__JetSecVtxFindingAlg=CompFactory.Analysis.JetSecVtxFindingAlg

def JetSecVtxFindingAlgCfg(ConfigFlags, JetCollection, PrimaryVertexCollectionName="", SVFinder="", TracksToTag="", **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

    acc = ComponentAccumulator()

    if SVFinder == 'JetFitter':
        secVtxFinder = acc.popToolsAndMerge(InDetImprovedJetFitterVxFinderCfg(ConfigFlags, 'JFVxFinder'))
    elif SVFinder == 'JetFitterFlip': #Add the JetFitterFlip tagger (invert sign of signed track impact parameter)
        secVtxFinder = acc.popToolsAndMerge(InDetImprovedJetFitterVxFinderCfg(ConfigFlags, 'JFVxFinder', 'FLIP_SIGN'))
    elif SVFinder == 'SV1':
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg(ConfigFlags, "IDVKalVxInJet"))
    elif SVFinder == 'MSV':
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg(ConfigFlags, "IDVKalMultiVxInJet", MSV = True))
    else:
        return acc

    options = {}
    options.setdefault('SecVtxFinder', secVtxFinder)
    options.setdefault('vxPrimaryCollectionName', PrimaryVertexCollectionName)
    options['JetCollectionName'] = JetCollection.replace('Track', 'PV0Track') + 'Jets'
    options['TracksToTag'] = TracksToTag
    options['BTagVxSecVertexInfoName'] = SVFinder + 'VxSecVertexInfo_' + JetCollection
    options['name'] = (JetCollection + '_' + SVFinder + '_secvtxfinding').lower()

    # -- create the association algorithm
    acc.addEventAlgo(Analysis__JetSecVtxFindingAlg(**options))

    return acc
