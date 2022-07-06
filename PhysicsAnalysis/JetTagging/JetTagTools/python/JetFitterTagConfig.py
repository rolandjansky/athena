# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.JetFitterNtupleWriterNNConfig import JetFitterNtupleWriterNNCfg
from JetTagTools.JetFitterNNToolConfig import JetFitterNNToolCfg

def JetFitterTagCfg(flags, name = 'JetFitterTagNN', scheme = '', CombinedIPNN = False, useBTagFlagsDefaults = True, runNN=False, **options):
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

    if scheme == "" or scheme == "Trig":
        if useBTagFlagsDefaults:
            if not CombinedIPNN:
                if runNN:
                    jetfitterClassifier = acc.popToolsAndMerge(JetFitterNNToolCfg(flags, 'JetFitterNNTool', scheme))
                else:
                    # FIXME: there has to be an easier way to instance
                    # a #@$%ing tool :'(
                    inneracc = ComponentAccumulator()
                    DummyClassifier = (
                        CompFactory.Analysis.JetFitterDummyClassifier)
                    inneracc.setPrivateTools(DummyClassifier(
                        name='JetFitterDummyClassifier'))
                    jetfitterClassifier = acc.popToolsAndMerge(inneracc)

                jetFitterNtupleWriterNN = acc.popToolsAndMerge(JetFitterNtupleWriterNNCfg('JetFitterNtupleWriterNN'))
                defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : [], #used only in reference mode
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : "",
                     'jetfitterNtupleWriter'            : jetFitterNtupleWriterNN,
                     'jetfitterClassifier'              : jetfitterClassifier,
                     }
            else:
                defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : [], #used only in reference mode
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : 'IP3D',
                     }
    elif (scheme == "Flip"): #Prepare Flip mode tagger, JetFitterFlip not defined yet
            # JetFitterFlip should be a flip version of JetFitter created in setupSecVtxTool
        if useBTagFlagsDefaults:
            defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : [], #used only in reference mode
                     'SecVxFinderName'                  : 'JetFitterFlip',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : 'IP3D',
                }
    for option in defaults:
        options.setdefault(option, defaults[option])
    acc.setPrivateTools(CompFactory.Analysis.JetFitterTag(**options))

    return acc
