# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.NewJetFitterVxFinderConfig import NewJetFitterVxFinderCfg
from BTagging.InDetVKalVxInJetToolConfig import InDetVKalVxInJetToolCfg
from JetTagTools.JetFitterVariablesFactoryConfig import JetFitterVariablesFactoryCfg
from BTagging.MSVVariablesFactoryConfig import MSVVariablesFactoryCfg

from BTagging.BTaggingConf import Analysis__JetSecVertexingAlg

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
        secVtxFinder = acc.popToolsAndMerge(NewJetFitterVxFinderCfg(ConfigFlags, 'JFVxFinder'))
    elif SVFinder == 'SV1':
        OutputFilesname = "SecVtx"
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalVxInJet"))
    elif SVFinder == 'MSV':
        OutputFilesname = "MSV"
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalMultiVxInJet", MSV = True))
    else:
        return acc

    jetFitterVF = acc.popToolsAndMerge(JetFitterVariablesFactoryCfg('JFVarFactory'))

    varFactory = acc.popToolsAndMerge(MSVVariablesFactoryCfg("MSVVarFactory"))

    btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
    options = {}
    options.setdefault('SecVtxFinder', secVtxFinder)
    options.setdefault('SecVtxFinderxAODBaseName', secVtxFinderxAODBaseName)
    options.setdefault('PrimaryVertexName', BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('vxPrimaryCollectionName', BTaggingFlags.PrimaryVertexCollectionName)
    options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
    options['TrackToJetAssociatorName'] = Associator
    options['BTagJFVtxCollectionName'] = btagname + OutputFilesname
    options['BTagSVCollectionName'] = btagname + OutputFilesname
    options['JetSecVtxLinkName'] = JetSVLink
    options.setdefault('JetFitterVariableFactory', jetFitterVF)
    options.setdefault('MSVVariableFactory', varFactory)
    options['name'] = (jetcol + '_' + SVFinder + '_secvtx').lower()

    # -- create the association algorithm
    acc.addEventAlgo(Analysis__JetSecVertexingAlg(**options))

    return acc
