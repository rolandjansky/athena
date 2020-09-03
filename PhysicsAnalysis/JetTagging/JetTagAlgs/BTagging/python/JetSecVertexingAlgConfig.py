# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.MSVVariablesFactoryConfig import MSVVariablesFactoryCfg

Analysis__JetSecVertexingAlg=CompFactory.Analysis.JetSecVertexingAlg

def JetSecVertexingAlgCfg(ConfigFlags, BTaggingCollection, JetCollection, PrimaryVertexCollectionName="", SVFinder="", Associator="", **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

    acc = ComponentAccumulator()

    jetcol = JetCollection
    btagname = BTaggingCollection

    if SVFinder == 'JetFitter':
        JetSVLink = 'JFVtx'
    if SVFinder == 'SV1':
        JetSVLink = 'SecVtx'
    if SVFinder == 'MSV':
        JetSVLink = 'MSecVtx' # Maybe no used

    varFactory = acc.popToolsAndMerge(MSVVariablesFactoryCfg("MSVVarFactory"))

    options = {}
    options.setdefault('SecVtxFinderxAODBaseName', SVFinder)
    options.setdefault('vxPrimaryCollectionName', PrimaryVertexCollectionName)
    options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
    options['BTagVxSecVertexInfoName'] = SVFinder + 'VxSecVertexInfo_' + JetCollection
    options['TrackToJetAssociatorName'] = options['JetCollectionName'] + '.' + Associator
    options['BTagJFVtxCollectionName'] = btagname + JetSVLink
    options['BTagSVCollectionName'] = btagname + JetSVLink
    options['JetSecVtxLinkName'] = options['JetCollectionName'] + '.' + JetSVLink
    options.setdefault('MSVVariableFactory', varFactory)
    options['name'] = (jetcol + '_' + SVFinder + '_secvtx').lower()

    # -- create the association algorithm
    acc.addEventAlgo(Analysis__JetSecVertexingAlg(**options))

    return acc
