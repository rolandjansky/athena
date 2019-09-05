# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingConf import Analysis__BTagTool
from BTagging.BTaggingFlags import BTaggingFlags

def BTagToolCfg(ConfigFlags, jetcol, TaggerList, useBTagFlagsDefaults = True, **options):
      """Adds a new myBTagTool instance and registers it.

      input: jetcol:             The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             options:            Python dictionary of options to be passed to the BTagTool.
             (note the options storeSecondaryVerticesInJet is passed to the removal tool instead)

      The following default options exist:

      BTagLabelingTool                       default: None
      storeSecondaryVerticesInJet            default: BTaggingFlags.writeSecondaryVertices

      output: The btagtool for the desired jet collection."""

      acc=ComponentAccumulator()

      tagToolList = []

      if 'IP2D' in TaggerList:
          from JetTagTools.IP2DTagConfig import IP2DTagCfg
          ip2dtool = acc.popToolsAndMerge(IP2DTagCfg(ConfigFlags, 'IP2DTag'))
          tagToolList.append(ip2dtool)

      if 'IP3D' in TaggerList:
          from JetTagTools.IP3DTagConfig import IP3DTagCfg
          ip3dtool = acc.popToolsAndMerge(IP3DTagCfg(ConfigFlags, 'IP3DTag'))
          tagToolList.append(ip3dtool)

      if 'SV1' in TaggerList:
          from JetTagTools.SV1TagConfig import SV1TagCfg
          sv1tool = acc.popToolsAndMerge(SV1TagCfg('SV1Tag'))
          tagToolList.append(sv1tool)

      if 'RNNIP' in TaggerList:
          from JetTagTools.RNNIPTagConfig import RNNIPTagCfg
          rnniptool = acc.popToolsAndMerge(RNNIPTagCfg(ConfigFlags, 'RNNIPTag'))
          tagToolList.append(rnniptool)

      if 'JetFitterNN' in TaggerList:
          from JetTagTools.JetFitterTagConfig import JetFitterTagCfg
          jetfitterNNtool = acc.popToolsAndMerge(JetFitterTagCfg('JetFitterTagNN'))
          tagToolList.append(jetfitterNNtool)
      
      if 'SoftMu' in TaggerList:
          from JetTagTools.SoftMuonTagConfig import SoftMuonTagCfg
          softmutool = acc.popToolsAndMerge(SoftMuonTagCfg(ConfigFlags, 'SoftMuonTag'))
          tagToolList.append(softmutool)

      if 'MultiSVbb1' in TaggerList:
          from JetTagTools.MultiSVTagConfig import MultiSVTagCfg
          multisvbb1tool = acc.popToolsAndMerge(MultiSVTagCfg('MultiSVbb1Tag','MultiSVbb1'))
          tagToolList.append(multisvbb1tool)

      if 'MultiSVbb2' in TaggerList:
          from JetTagTools.MultiSVTagConfig import MultiSVTagCfg
          multisvbb2tool = acc.popToolsAndMerge(MultiSVTagCfg('MultiSVbb2Tag','MultiSVbb2'))
          tagToolList.append(multisvbb2tool)

      if 'JetVertexCharge' in TaggerList:
          from JetTagTools.JetVertexChargeConfig import JetVertexChargeCfg
          jvc = acc.popToolsAndMerge(JetVertexChargeCfg('JetVertexCharge'))
          tagToolList.append(jvc)

      if 'DL1' in TaggerList or 'DL1mu' in TaggerList or 'DL1rnn' in TaggerList:
          from JetTagTools.MultivariateTagManagerConfig import MultivariateTagManagerCfg
          mvtm = acc.popToolsAndMerge(MultivariateTagManagerCfg('mvtm', TaggerList = TaggerList))
          tagToolList.append(mvtm)

      options = dict(options)
      if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                     : BTaggingFlags.Runmodus,
                     'PrimaryVertexName'            : BTaggingFlags.PrimaryVertexCollectionName,
                     'BaselineTagger'               : BTaggingFlags.BaselineTagger,
                     'vxPrimaryCollectionName'      : BTaggingFlags.PrimaryVertexCollectionName,
                     'TagToolList'                  : tagToolList,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
      options['name'] = 'btag'
      btagtool = Analysis__BTagTool(**options)
      acc.setPrivateTools(btagtool)
 
      return acc
