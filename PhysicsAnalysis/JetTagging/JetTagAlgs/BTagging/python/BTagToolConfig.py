# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
Analysis__BTagTool=CompFactory.Analysis.BTagTool

def BTagToolCfg(ConfigFlags, TaggerList, PrimaryVertexCollectionName="", scheme = '', useBTagFlagsDefaults = True):
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
          ip2dtool = acc.popToolsAndMerge(IP2DTagCfg(ConfigFlags, 'IP2DTag', scheme))
          tagToolList.append(ip2dtool)

      if 'IP3D' in TaggerList:
          from JetTagTools.IP3DTagConfig import IP3DTagCfg
          ip3dtool = acc.popToolsAndMerge(IP3DTagCfg(ConfigFlags, 'IP3DTag', PrimaryVertexCollectionName, scheme))
          tagToolList.append(ip3dtool)

      if 'SV1' in TaggerList:
          from JetTagTools.SV1TagConfig import SV1TagCfg
          sv1tool = acc.popToolsAndMerge(SV1TagCfg(ConfigFlags, 'SV1Tag', scheme))
          tagToolList.append(sv1tool)

      if 'RNNIP' in TaggerList:
          from JetTagTools.RNNIPTagConfig import RNNIPTagCfg
          rnniptool = acc.popToolsAndMerge(RNNIPTagCfg(ConfigFlags, 'RNNIPTag', scheme))
          tagToolList.append(rnniptool)

      if 'JetFitterNN' in TaggerList:
          from JetTagTools.JetFitterTagConfig import JetFitterTagCfg
          jetfitterNNtool = acc.popToolsAndMerge(JetFitterTagCfg(ConfigFlags, 'JetFitterTagNN', scheme))
          tagToolList.append(jetfitterNNtool)
      
      if 'SoftMu' in TaggerList:
          from JetTagTools.SoftMuonTagConfig import SoftMuonTagCfg
          softmutool = acc.popToolsAndMerge(SoftMuonTagCfg(ConfigFlags, 'SoftMuonTag', scheme))
          tagToolList.append(softmutool)

      if 'MultiSVbb1' in TaggerList:
          from JetTagTools.MultiSVTagConfig import MultiSVTagCfg
          multisvbb1tool = acc.popToolsAndMerge(MultiSVTagCfg(ConfigFlags,'MultiSVbb1Tag','MultiSVbb1', scheme))
          tagToolList.append(multisvbb1tool)

      if 'MultiSVbb2' in TaggerList:
          from JetTagTools.MultiSVTagConfig import MultiSVTagCfg
          multisvbb2tool = acc.popToolsAndMerge(MultiSVTagCfg(ConfigFlags, 'MultiSVbb2Tag','MultiSVbb2', scheme))
          tagToolList.append(multisvbb2tool)

      if 'JetVertexCharge' in TaggerList:
          from JetTagTools.JetVertexChargeConfig import JetVertexChargeCfg
          jvc = acc.popToolsAndMerge(JetVertexChargeCfg(ConfigFlags, 'JetVertexCharge', scheme))
          tagToolList.append(jvc)

      # list of taggers that use MultivariateTagManager
      mvtm_taggers = ['MV2c00','MV2c10','MV2c20','MV2c100','MV2cl100','MV2c10mu','MV2c10rnn','MV2m','MV2c10hp','DL1','DL1mu','DL1rnn']
      if 'RNNIP' not in TaggerList: #some taggers needs RNNIP variables
          mvtm_taggers.remove('DL1rnn')
          mvtm_taggers.remove('MV2c10rnn')
          mvtm_taggers.remove('MV2c100')
          mvtm_taggers.remove('MV2cl100')
      mvtm_active_taggers = list(set(mvtm_taggers) & set(TaggerList))
      if len(mvtm_active_taggers) > 0:
          from JetTagTools.MultivariateTagManagerConfig import MultivariateTagManagerCfg
          mvtm = acc.popToolsAndMerge(MultivariateTagManagerCfg(ConfigFlags, 'mvtm', TaggerList = mvtm_active_taggers, scheme = scheme))
          tagToolList.append(mvtm)

      options = {}
      if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                     : ConfigFlags.BTagging.RunModus,
                     'vxPrimaryCollectionName'      : PrimaryVertexCollectionName,
                     'TagToolList'                  : tagToolList,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
      options['name'] = 'btag'
      btagtool = Analysis__BTagTool(**options)
      acc.setPrivateTools(btagtool)
 
      return acc
