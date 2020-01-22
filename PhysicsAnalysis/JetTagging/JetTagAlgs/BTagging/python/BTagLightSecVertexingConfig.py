# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.JetFitterVariablesFactoryConfig import JetFitterVariablesFactoryCfg
#from BTagging.MSVVariablesFactoryConfig import MSVVariablesFactoryCfg

from BTagging.BTaggingConf import Analysis__BTagLightSecVertexing

def BTagLightSecVtxToolCfg(flags, Name, JetCollection, TimeStamp = "", **options):
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
    VxSecVertexInfoNameList = []
    secVtxFinderTrackNameList = []
    secVtxFinderxAODBaseNameList = []
    if TimeStamp:
        TimeStamp = '_' + TimeStamp

    VxSecVertexInfoNameList.append('JetFitter'+'VxSecVertexInfo')
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
    secVtxFinderxAODBaseNameList.append('JetFitter')

    jetFitterVF = acc.popToolsAndMerge(JetFitterVariablesFactoryCfg('JFVarFactory'))

    VxSecVertexInfoNameList.append('SV1'+'VxSecVertexInfo')
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
    secVtxFinderxAODBaseNameList.append('SV1')

    #Keep this commented out for the moment: ATLASRECTS-5247
    #inDetVKalMultiVxInJetTool = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg("IDVKalMultiVxInJet", MSV = True))
    #secVtxFinderList.append(inDetVKalMultiVxInJetTool)
    #secVtxFinderTrackNameList.append('BTagTrackToJetAssociatorBB')
    #secVtxFinderxAODBaseNameList.append('MSV')
    #varFactory = acc.popToolsAndMerge(MSVVariablesFactoryCfg("MSVVarFactory"))

    options = {}
    options.setdefault('SecVtxFinderTrackNameList', secVtxFinderTrackNameList)
    options.setdefault('SecVtxFinderxAODBaseNameList', secVtxFinderxAODBaseNameList)
    options['BTagVxSecVertexInfoNames'] = VxSecVertexInfoNameList
    options.setdefault('PrimaryVertexName', BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('vxPrimaryCollectionName', BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('JetFitterVariableFactory', jetFitterVF)
    options['JetSecVtxLinkName'] = jetcol + 'Jets.' + OutputFilesSVname
    options['JetJFVtxLinkName'] = jetcol + 'Jets.' + OutputFilesJFVxname
    #options.setdefault('MSVVariableFactory', varFactory)
    options['name'] = Name+TimeStamp

    tool = Analysis__BTagLightSecVertexing(**options)

    acc.setPrivateTools(tool)

    return acc
