# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.DL1TagConfig import DL1TagCfg
from JetTagTools.MV2TagConfig import MV2TagCfg

# import the MultivariateTagManager configurable
from JetTagTools.JetTagToolsConf import Analysis__MultivariateTagManager

def MultivariateTagManagerCfg(name = 'MultivariateTagManager', TaggerList = ['DL1', 'DL1rnn', 'DL1mu', 'MV2c10'], scheme = '', useBTagFlagsDefaults = True, **options):
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
    # list of taggers that use MultivariateTagManager
    mvtm_taggers = ['MV2c00','MV2c10','MV2c20','MV2c100','MV2cl100','MV2c10mu','MV2c10rnn','MV2m','MV2c10hp','DL1','DL1mu','DL1rnn']

    mvtm_active_taggers = list(set(mvtm_taggers) & set(TaggerList))

    if 'DL1' in mvtm_active_taggers:
        dl1 = acc.popToolsAndMerge(DL1TagCfg('DL1'))
        BTaggingFlags.MVTagToolList.append(dl1)

    if 'DL1mu' in mvtm_active_taggers:
        dl1 = acc.popToolsAndMerge(DL1TagCfg('DL1mu'))
        BTaggingFlags.MVTagToolList.append(dl1)

    if 'DL1rnn' in mvtm_active_taggers:
        dl1 = acc.popToolsAndMerge(DL1TagCfg('DL1rnn'))
        BTaggingFlags.MVTagToolList.append(dl1)

    if 'MV2c10' in mvtm_active_taggers:
        mv2 = acc.popToolsAndMerge(MV2TagCfg('MV2c10'))
        BTaggingFlags.MVTagToolList.append(mv2)

    if 'MV2c10mu' in mvtm_active_taggers:
        mv2 = acc.popToolsAndMerge(MV2TagCfg('MV2c10mu'))
        BTaggingFlags.MVTagToolList.append(mv2)

    if 'MV2c10rnn' in mvtm_active_taggers:
        mv2 = acc.popToolsAndMerge(MV2TagCfg('MV2c10rnn'))
        BTaggingFlags.MVTagToolList.append(mv2)

    if 'MV2c100' in mvtm_active_taggers:
        mv2 = acc.popToolsAndMerge(MV2TagCfg('MV2c100'))
        BTaggingFlags.MVTagToolList.append(mv2)


    if 'MV2cl100' in mvtm_active_taggers:
        mv2 = acc.popToolsAndMerge(MV2TagCfg('MV2cl100'))
        BTaggingFlags.MVTagToolList.append(mv2)

    #Check if input has been scheduled
    #if IP2D not added ....

    if useBTagFlagsDefaults:
        defaults = { 'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1',
                     'inputIP2DSourceName'              : 'IP2D',
                     'inputIP3DSourceName'              : 'IP3D',
                     'inputJFSourceName'                : 'JetFitter',
                     'MVTagToolList'                    : BTaggingFlags.MVTagToolList,
                     'arbitraryAuxData'                 : BTaggingFlags.MultivariateTagManagerAuxBranches,
                     }
    for option in defaults:
        options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Analysis__MultivariateTagManager(**options))
    return acc
