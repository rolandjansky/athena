# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingConf import Analysis__BTagTool
from BTagging.BTaggingFlags import BTaggingFlags

def BTagToolCfg(ConfigFlags, jetcol, TaggerList, useBTagFlagsDefaults = True, Verbose = False, options = {}):
      """Adds a new myBTagTool instance and registers it.

      input: jetcol:             The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
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
          accIP2D = IP2DTagCfg(ConfigFlags, 'IP2DTag')
          ip2dtool = accIP2D.popPrivateTools()
          acc.merge(accIP2D)
          tagToolList.append(ip2dtool)

      if 'IP3D' in TaggerList:
          from JetTagTools.IP3DTagConfig import IP3DTagCfg
          accIP3D = IP3DTagCfg(ConfigFlags, 'IP3DTag')
          ip3dtool = accIP3D.popPrivateTools()
          acc.merge(accIP3D)
          tagToolList.append(ip3dtool)

      if 'SV1' in TaggerList:
          from JetTagTools.SV1TagConfig import SV1TagCfg
          accSV1 = SV1TagCfg('SV1Tag')
          sv1tool = accSV1.popPrivateTools()
          acc.merge(accSV1)
          tagToolList.append(sv1tool)

      if 'RNNIP' in TaggerList:
          from JetTagTools.RNNIPTagConfig import RNNIPTagCfg
          accRNNIP = RNNIPTagCfg(ConfigFlags, 'RNNIPTag')
          rnniptool = accRNNIP.popPrivateTools()
          acc.merge(accRNNIP)
          tagToolList.append(rnniptool)

      if 'JetFitterNN' in TaggerList:
          from JetTagTools.JetFitterTagConfig import JetFitterTagCfg
          accJetFitter = JetFitterTagCfg('JetFitterTagNN')
          jetfitterNNtool = accJetFitter.popPrivateTools()
          acc.merge(accJetFitter)
          tagToolList.append(jetfitterNNtool)
      
      if 'SoftMu' in TaggerList:
          from JetTagTools.SoftMuonTagConfig import SoftMuonTagCfg
          accSoftMu = SoftMuonTagCfg(ConfigFlags, 'SoftMuonTag')
          softmutool = accSoftMu.popPrivateTools()
          acc.merge(accSoftMu)
          tagToolList.append(softmutool)

      if 'MultiSVbb1' in TaggerList:
          from JetTagTools.MultiSVTagConfig import MultiSVTagCfg
          accMultiSV = MultiSVTagCfg('MultiSVbb1Tag','MultiSVbb1')
          multisvbb1tool = accMultiSV.popPrivateTools()
          acc.merge(accMultiSV)
          tagToolList.append(multisvbb1tool)

      if 'MultiSVbb2' in TaggerList:
          from JetTagTools.MultiSVTagConfig import MultiSVTagCfg
          accMultiSV = MultiSVTagCfg('MultiSVbb2Tag','MultiSVbb2')
          multisvbb2tool = accMultiSV.popPrivateTools()
          acc.merge(accMultiSV)
          tagToolList.append(multisvbb2tool)

      if 'JetVertexCharge' in TaggerList:
          from JetTagTools.JetVertexChargeConfig import JetVertexChargeCfg
          accJVC = JetVertexChargeCfg('JetVertexCharge')
          jvc = accJVC.popPrivateTools()
          acc.merge(accJVC)
          tagToolList.append(jvc)

      if 'DL1' in TaggerList or 'DL1mu' in TaggerList or 'DL1rnn' in TaggerList:
          from JetTagTools.MultivariateTagManagerConfig import MultivariateTagManagerCfg
          accMVTM = MultivariateTagManagerCfg('mvtm', TaggerList = TaggerList)
          mvtm = accMVTM.popPrivateTools()
          acc.merge(accMVTM)
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
