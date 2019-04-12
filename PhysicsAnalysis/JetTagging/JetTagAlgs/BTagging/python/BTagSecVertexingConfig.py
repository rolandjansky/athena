# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.NewJetFitterVxFinderConfig import NewJetFitterVxFinderCfg
from JetTagTools.JetFitterVariablesFactoryConfig import JetFitterVariablesFactoryCfg

from BTagging.BTaggingConf import Analysis__BTagSecVertexing

def BTagSecVtxToolCfg(flags, name, JetCollection, Verbose = False, outputObjs = None, **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           Verbose:            Whether to print detailed information about the tool.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool.
    If outputObjs is set, then it is filled with objects written to SG."""
    acc = ComponentAccumulator()

    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

    jetcol = JetCollection
    secVtxFinderList = []
    secVtxFinderTrackNameList = []
    secVtxFinderxAODBaseNameList = []

    #newJetFitterVxFinder = NewJetFitterVxFinderCfg(flags, 'JFVxFinder')
    accJetFitterVxFinder = NewJetFitterVxFinderCfg(flags, 'JFVxFinder')
    newJetFitterVxFinder = accJetFitterVxFinder.popPrivateTools()
    secVtxFinderList.append(newJetFitterVxFinder)
    acc.merge(accJetFitterVxFinder)
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
    secVtxFinderxAODBaseNameList.append('JetFitter')
    #jetFitterVF = JetFitterVariablesFactoryCfg('JFVarFactory')
    accJetFitterVF = JetFitterVariablesFactoryCfg('JFVarFactory')
    jetFitterVF = accJetFitterVF.popPrivateTools()
    acc.merge(accJetFitterVF)
    

    from BTagging.InDetVKalVxInJetToolConfig import InDetVKalVxInJetToolCfg
    accInDetVKalVxInJetTool = InDetVKalVxInJetToolCfg("IDVKalVxInJet")
    inDetVKalVxInJetTool = accInDetVKalVxInJetTool.popPrivateTools()
    #inDetVKalVxInJetTool = InDetVKalVxInJetToolCfg("IDVKalVxInJet")
    secVtxFinderList.append(inDetVKalVxInJetTool)
    acc.merge(accInDetVKalVxInJetTool)
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
    secVtxFinderxAODBaseNameList.append('SV1')

    #inDetVKalMultiVxInJetTool = InDetVKalVxInJetToolCfg("IDVKalMultiVxInJet", MSV = True)
    accInDetVKalVxInJetTool = InDetVKalVxInJetToolCfg("IDVKalMultiVxInJet", MSV = True)
    inDetVKalMultiVxInJetTool = accInDetVKalVxInJetTool.popPrivateTools()
    secVtxFinderList.append(inDetVKalMultiVxInJetTool)
    acc.merge(accInDetVKalVxInJetTool)
    secVtxFinderTrackNameList.append('BTagTrackToJetAssociatorBB')
    secVtxFinderxAODBaseNameList.append('MSV')

    from BTagging.MSVVariablesFactoryConfig import MSVVariablesFactoryCfg
    accMSVVariablesFactory = MSVVariablesFactoryCfg("MSVVarFactory")
    varFactory =  accMSVVariablesFactory.popPrivateTools()
    acc.merge(accMSVVariablesFactory)
    #varFactory = MSVVariablesFactoryCfg("MSVVarFactory")

    options = dict(options)
    options.setdefault('SecVtxFinderList', secVtxFinderList)
    options.setdefault('SecVtxFinderTrackNameList', secVtxFinderTrackNameList)
    options.setdefault('SecVtxFinderxAODBaseNameList', secVtxFinderxAODBaseNameList)
    options.setdefault('PrimaryVertexName',BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('vxPrimaryCollectionName',BTaggingFlags.PrimaryVertexCollectionName)
    options.setdefault('BTagJFVtxCollectionName', ConfInstance.getOutputFilesPrefix() + jetcol + ConfInstance._OutputFilesJFVxname)
    options.setdefault('BTagSVCollectionName', ConfInstance.getOutputFilesPrefix() + jetcol + ConfInstance._OutputFilesSVname)
    options.setdefault('JetFitterVariableFactory', jetFitterVF)
    options.setdefault('MSVVariableFactory', varFactory)
    options['name'] = name
    if outputObjs:
        outputObjs['xAOD::VertexContainer'] = options['BTagSVCollectionName']
        outputObjs['xAOD::BTagVertexContainer'] = options['BTagJFVtxCollectionName']
    tool = Analysis__BTagSecVertexing(**options)

    acc.setPrivateTools(tool)

    return acc
