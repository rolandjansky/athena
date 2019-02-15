# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from BTagging.BTaggingFlags import BTaggingFlags

from BTagging.BTaggingConf import Analysis__BTagSecVertexing

def BTagSecVtxToolCfg(name, JetCollection, Verbose = False, options={}, outputObjs = None):
      """Adds a SecVtxTool instance and registers it.

      input: name:               The tool's name.
             JetCollection       The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
             options:            Python dictionary of options to be passed to the SecVtxTool.
      output: The tool.
      If outputObjs is set, then it is filled with objects written to SG."""
      from BTagging.BTaggingConfiguration import getConfiguration
      ConfInstance = getConfiguration()

      jetcol = JetCollection
      secVtxFinderList = []
      secVtxFinderTrackNameList = []
      secVtxFinderxAODBaseNameList = []
      from BTagging.NewJetFitterVxFinder_SVConfig import NewJetFitterVxFinder_SVCfg

      newJetFitterVxFinder = NewJetFitterVxFinder_SVCfg('JFVxFinder')
      secVtxFinderList.append(newJetFitterVxFinder)
      secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
      secVtxFinderxAODBaseNameList.append('JetFitter')
      from BTagging.BTaggingConfiguration_NewJetFitterCollection import toolJetFitterVariablesFactory
      jetFitterVF = toolJetFitterVariablesFactory('JFVarFactory')

      from BTagging.BTaggingConfiguration_InDetVKalVxInJetTool import toolInDetVKalVxInJetTool
      inDetVKalVxInJetTool = toolInDetVKalVxInJetTool("IDVKalVxInJet")
      secVtxFinderList.append(inDetVKalVxInJetTool)
      secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
      secVtxFinderxAODBaseNameList.append('SV1')

      from BTagging.BTaggingConfiguration_InDetVKalVxMultiVxInJetTool import toolInDetVKalMultiVxInJetTool
      inDetVKalMultiVxInJetTool = toolInDetVKalMultiVxInJetTool("IDVKalMultiVxInJet")
      secVtxFinderList.append(inDetVKalMultiVxInJetTool)
      secVtxFinderTrackNameList.append('BTagTrackToJetAssociatorBB')
      secVtxFinderxAODBaseNameList.append('MSV')

      from BTagging.BTaggingConfiguration_InDetVKalVxMultiVxInJetTool import toolMSVVariablesFactory
      varFactory = toolMSVVariablesFactory("MSVVarFactory")

      options = dict(options)
      options.setdefault('SecVtxFinderList', secVtxFinderList)
      options.setdefault('SecVtxFinderTrackNameList', secVtxFinderTrackNameList)
      options.setdefault('SecVtxFinderxAODBaseNameList', secVtxFinderxAODBaseNameList)
      options.setdefault('PrimaryVertexName',BTaggingFlags.PrimaryVertexCollectionName)
      options.setdefault('vxPrimaryCollectionName',BTaggingFlags.PrimaryVertexCollectionName)
      options.setdefault('BTagJFVtxCollectionName', ConfInstance.getOutputFilesPrefix() + jetcol + ConfInstance._OutputFilesJFVxname)
      options.setdefault('BTagSVCollectionName', ConfInstance.getOutputFilesPrefix() + jetcol + ConfInstance._OutputFilesSVname)
      options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
      options.setdefault('JetFitterVariableFactory', jetFitterVF)
      options.setdefault('MSVVariableFactory', varFactory)
      options['name'] = name
      if outputObjs:
          outputObjs['xAOD::VertexContainer'] = options['BTagSVCollectionName']
          outputObjs['xAOD::BTagVertexContainer'] = options['BTagJFVtxCollectionName']
      tool = Analysis__BTagSecVertexing(**options)
      return tool
