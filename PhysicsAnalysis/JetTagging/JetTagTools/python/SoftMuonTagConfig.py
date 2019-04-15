# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.NewLikelihoodToolConfig import NewLikelihoodToolCfg
from JetTagTools.MuonSelectorToolConfig import MuonSelectorToolCfg

# import the SoftMuonTag configurable
from JetTagTools.JetTagToolsConf import Analysis__SoftMuonTag

def SoftMuonTagCfg( flags, name = 'SoftMu', useBTagFlagsDefaults = True, **options ):
    """Sets up a SoftMuonTag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    originalMuCollectionName            default: BTaggingFlags.MuonCollectionName
    BTagJetEtamin                       default: 2.5 (only if BTaggingFlags.Runmodus == 'reference')
    MuonQuality                         default: xAOD::Muon::Medium

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        accBTagTrackToVertexIPEstimator = BTagTrackToVertexIPEstimatorCfg(flags, 'TrkToVxIPEstimator')
        trackToVertexIPEstimator = accBTagTrackToVertexIPEstimator.popPrivateTools()
        acc.merge(accBTagTrackToVertexIPEstimator)
        accMuonSelector = MuonSelectorToolCfg('MuonSelectorTool')
        muonSelectorTool = accMuonSelector.popPrivateTools()
        acc.merge(accMuonSelector)
        accLikelihood = NewLikelihoodToolCfg('SoftMuonTagNewLikelihoodTool', 'SMT')
        likelihood = accLikelihood.popPrivateTools()
        acc.merge(accLikelihood)
        defaults = {
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'originalMuCollectionName'         : BTaggingFlags.MuonCollectionName,
                     'MuonQuality'                      : 2,
                     'muonSelectorTool'                 : muonSelectorTool,
                     'LikelihoodTool'                   : likelihood,
                     'TrackToVertexIPEstimator'         : trackToVertexIPEstimator, }
        if(BTaggingFlags.Runmodus == 'reference'):
            defaults['BTagJetEtamin'] = 2.5
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Analysis__SoftMuonTag( **options))

    return acc
