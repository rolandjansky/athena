# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.NewJetFitterVxFinderConfig import NewJetFitterVxFinderCfg
from BTagging.InDetVKalVxInJetToolConfig import InDetVKalVxInJetToolCfg
from JetTagTools.JetFitterVariablesFactoryConfig import JetFitterVariablesFactoryCfg
from BTagging.MSVVariablesFactoryConfig import MSVVariablesFactoryCfg

from BTagging.BTaggingConf import Analysis__BTagSecVertexing

def BTagSecVtxToolCfg(flags, name, JetCollection, outputObjs = None, **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool.
    If outputObjs is set, then it is filled with objects written to SG."""
    acc = ComponentAccumulator()

    jetcol = JetCollection
    OutputFilesJFVxname = "JFVtx"
    OutputFilesSVname = "SecVtx"
    secVtxFinderList = []
    secVtxFinderTrackNameList = []
    secVtxFinderxAODBaseNameList = []

    newJetFitterVxFinder = acc.popToolsAndMerge(NewJetFitterVxFinderCfg(flags, 'JFVxFinder'))
    secVtxFinderList.append(newJetFitterVxFinder)
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
    secVtxFinderxAODBaseNameList.append('JetFitter')

    jetFitterVF = acc.popToolsAndMerge(JetFitterVariablesFactoryCfg('JFVarFactory'))

    inDetVKalVxInJetTool = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalVxInJet"))
    secVtxFinderList.append(inDetVKalVxInJetTool)
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
    secVtxFinderxAODBaseNameList.append('SV1')

    inDetVKalMultiVxInJetTool = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalMultiVxInJet", MSV = True))
    secVtxFinderList.append(inDetVKalMultiVxInJetTool)
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociatorBB')
    secVtxFinderxAODBaseNameList.append('MSV')

    varFactory = acc.popToolsAndMerge(MSVVariablesFactoryCfg("MSVVarFactory"))

    new_prefix = options.get('new_prefix', None)
    if new_prefix:
        btagname = new_prefix + jetcol
    else:
        btagname = flags.BTagging.OutputFiles.Prefix + jetcol

    options = {}
    options.setdefault('SecVtxFinderList', secVtxFinderList)
    options.setdefault('SecVtxFinderTrackNameList', secVtxFinderTrackNameList)
    options.setdefault('SecVtxFinderxAODBaseNameList', secVtxFinderxAODBaseNameList)
    options.setdefault('PrimaryVertexName',BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('vxPrimaryCollectionName',BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('BTagJFVtxCollectionName', btagname + OutputFilesJFVxname)
    options.setdefault('BTagSVCollectionName', btagname + OutputFilesSVname)
    options.setdefault('JetFitterVariableFactory', jetFitterVF)
    options.setdefault('MSVVariableFactory', varFactory)
    options['name'] = name
    if outputObjs:
        outputObjs['xAOD::VertexContainer'] = options['BTagSVCollectionName']
        outputObjs['xAOD::BTagVertexContainer'] = options['BTagJFVtxCollectionName']
    tool = Analysis__BTagSecVertexing(**options)

    acc.setPrivateTools(tool)

    return acc
