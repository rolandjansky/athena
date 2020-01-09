# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.JetFitterVariablesFactoryConfig import JetFitterVariablesFactoryCfg
from BTagging.MSVVariablesFactoryConfig import MSVVariablesFactoryCfg

Analysis__JetSecVertexingAlg=CompFactory.Analysis__JetSecVertexingAlg

def JetSecVertexingAlgCfg(ConfigFlags, JetCollection, ParticleCollection="", SVFinder="", Associator="", JetSVLink="", **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

    acc = ComponentAccumulator()

    jetcol = JetCollection
    secVtxFinderxAODBaseName = SVFinder

    if SVFinder == 'JetFitter':
        OutputFilesname = "JFVtx"
    elif SVFinder == 'SV1':
        OutputFilesname = "SecVtx"
    elif SVFinder == 'MSV':
        OutputFilesname = "MSV"
    else:
        return acc

    jetFitterVF = acc.popToolsAndMerge(JetFitterVariablesFactoryCfg('JFVarFactory'))

    varFactory = acc.popToolsAndMerge(MSVVariablesFactoryCfg("MSVVarFactory"))

    btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
    options = {}
    options.setdefault('SecVtxFinderxAODBaseName', secVtxFinderxAODBaseName)
    options.setdefault('PrimaryVertexName', BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('vxPrimaryCollectionName', BTaggingFlags.PrimaryVertexCollectionName)
    options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
    options['BTagVxSecVertexInfoName'] = SVFinder + 'VxSecVertexInfo'
    options['TrackToJetAssociatorName'] = Associator
    options['BTagJFVtxCollectionName'] = btagname + OutputFilesname
    options['BTagSVCollectionName'] = btagname + OutputFilesname
    options['JetSecVtxLinkName'] = JetSVLink
    options.setdefault('JetFitterVariableFactory', jetFitterVF)
    options.setdefault('MSVVariableFactory', varFactory)
    options['name'] = (jetcol + '_' + SVFinder + '_secvtx').lower()
    #options['OutputLevel'] = 1

    # -- create the association algorithm
    acc.addEventAlgo(Analysis__JetSecVertexingAlg(**options))

    return acc
