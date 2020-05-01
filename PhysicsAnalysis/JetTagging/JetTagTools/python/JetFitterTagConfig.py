# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.JetFitterNtupleWriterNNConfig import JetFitterNtupleWriterNNCfg
from JetTagTools.JetFitterNNToolConfig import JetFitterNNToolCfg

# import the JetFitterTag configurable
Analysis__JetFitterTag=CompFactory.Analysis.JetFitterTag

def JetFitterTagCfg(flags, name = 'JetFitterTagNN', scheme = '', CombinedIPNN = False, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagNN tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTagging.RunModus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitter"
    useForcedCalibration                default: False
    ipinfoTaggerName                    default: ""

    If CombinedIPNN is True
    ipinfoTaggerName                    default: "IP3D"


    input:             name: The name of the tool (should be unique).
         useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    options['xAODBaseName'] = 'JetFitter'

    if (scheme == ""):
        if useBTagFlagsDefaults:
            if not CombinedIPNN:
                jetFitterNtupleWriterNN = acc.popToolsAndMerge(JetFitterNtupleWriterNNCfg('JetFitterNtupleWriterNN'))
                jetfitterClassifier = acc.popToolsAndMerge(JetFitterNNToolCfg('JetFitterNNTool'))
                defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : "",
                     'jetfitterNtupleWriter'            : jetFitterNtupleWriterNN,
                     'jetfitterClassifier'              : jetfitterClassifier,
                     }
            else:
                defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : 'IP3D',
                     }
    elif (scheme == "Flip"): #Prepare Flip mode tagger, JetFitterFlip not defined yet
            # JetFitterFlip should be a flip version of JetFitter created in setupSecVtxTool
        if useBTagFlagsDefaults:
            defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitterFlip',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : 'IP3D',
                }
    for option in defaults:
        options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__JetFitterTag(**options))

    return acc
