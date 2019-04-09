# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.NewLikelihoodToolConfig import NewLikelihoodToolCfg

# import the SVTag configurable
from JetTagTools.JetTagToolsConf import Analysis__SVTag

def SV1TagCfg( name = 'SV1Tag', scheme = '', useBTagFlagsDefaults = True, **options ):
    """Sets up a SV1Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV1"
    jetCollectionList                   default: BTaggingFlags.Jets
    LikelihoodTool                      default: None
    SecVxFinderName                     default: "SV1"
    UseCHypo                            default: True

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    options['xAODBaseName'] = 'SV1'
    if useBTagFlagsDefaults:
        accLikelihood = NewLikelihoodToolCfg('SV1NewLikelihoodTool', 'SV1')
        likelihood = accLikelihood.popPrivateTools()
        acc.merge(accLikelihood)
        defaults = { 'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     'SVAlgType'                        : 'SV1',
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'SV1',
                     'UseCHypo'                         : True,
                     'LikelihoodTool'                   : likelihood }
        for option in defaults:
            options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__SVTag(**options))

    return acc
