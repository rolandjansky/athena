# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.DL1TagConfig import DL1TagCfg
from JetTagTools.MV2TagConfig import MV2TagCfg

# import the MultivariateTagManager configurable
Analysis__MultivariateTagManager=CompFactory.Analysis.MultivariateTagManager

def MultivariateTagManagerCfg(flags, name = 'MultivariateTagManager', TaggerList = ['DL1', 'DL1rnn', 'DL1mu', 'MV2c10'], scheme = '', useBTagFlagsDefaults = True, **options):
    """Sets up a MultivariateTagManager tool and returns it.

    The following options have BTaggingFlags defaults:

    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1"
    inputIP2DSourceName                 default: "IP2D"
    inputIP3DSourceName                 default: "IP3D"
    inputJFSourceName                   default: "JetFitter"

    input:             name: The name of the tool (should be unique).
    output: The actual tool."""
    acc = ComponentAccumulator()
    mvtagtoollist = []
    MultivariateTagManagerAuxBranches = []
    MultivariateTagManagerAuxBranches += ['SMT_discriminant'] #ATLASRECTS-5381

    if 'DL1rnn' in TaggerList or 'MV2c10rnn' in TaggerList:
        #RNNIP output variables are needed
        rnnip_outputs = ['b','c','u','tau']
        MultivariateTagManagerAuxBranches += ['rnnip_p' + x for x in rnnip_outputs]

    if 'DL1' in TaggerList:
        dl1 = acc.popToolsAndMerge(DL1TagCfg(flags, 'DL1', scheme))
        mvtagtoollist.append(dl1)

    if 'DL1mu' in TaggerList:
        dl1 = acc.popToolsAndMerge(DL1TagCfg(flags, 'DL1mu', scheme))
        mvtagtoollist.append(dl1)

    if 'DL1rnn' in TaggerList:
        dl1 = acc.popToolsAndMerge(DL1TagCfg(flags, 'DL1rnn', scheme))
        mvtagtoollist.append(dl1)

    if 'MV2c10' in TaggerList:
        mv2 = acc.popToolsAndMerge(MV2TagCfg(flags, 'MV2c10', scheme))
        mvtagtoollist.append(mv2)

    if 'MV2c10mu' in TaggerList:
        mv2 = acc.popToolsAndMerge(MV2TagCfg(flags, 'MV2c10mu', scheme))
        mvtagtoollist.append(mv2)

    if 'MV2c10rnn' in TaggerList:
        mv2 = acc.popToolsAndMerge(MV2TagCfg(flags, 'MV2c10rnn', scheme))
        mvtagtoollist.append(mv2)

    if 'MV2c100' in TaggerList:
        mv2 = acc.popToolsAndMerge(MV2TagCfg(flags, 'MV2c100', scheme))
        mvtagtoollist.append(mv2)


    if 'MV2cl100' in TaggerList:
        mv2 = acc.popToolsAndMerge(MV2TagCfg(flags, 'MV2cl100', scheme))
        mvtagtoollist.append(mv2)

    #Check if input has been scheduled
    #if IP2D not added ....

    if useBTagFlagsDefaults:
        defaults = { 'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1',
                     'inputIP2DSourceName'              : 'IP2D',
                     'inputIP3DSourceName'              : 'IP3D',
                     'inputJFSourceName'                : 'JetFitter',
                     'MVTagToolList'                    : mvtagtoollist,
                     'arbitraryAuxData'                 : MultivariateTagManagerAuxBranches,
                     }
    for option in defaults:
        options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Analysis__MultivariateTagManager(**options))
    return acc
