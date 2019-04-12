# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.DL1TagConfig import DL1TagCfg

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
        from JetTagTools.DL1TagConfig import DL1TagCfg
        accDL1 = DL1TagCfg('DL1')
        dl1 = accDL1.popPrivateTools()
        acc.merge(accDL1)
        BTaggingFlags.MVTagToolList.append(dl1)

    if 'DL1mu' in mvtm_active_taggers:
        from JetTagTools.DL1TagConfig import DL1TagCfg
        accDL1 = DL1TagCfg('DL1mu')
        dl1 = accDL1.popPrivateTools()
        acc.merge(accDL1)
        BTaggingFlags.MVTagToolList.append(dl1)

    if 'DL1rnn' in mvtm_active_taggers:
        from JetTagTools.DL1TagConfig import DL1TagCfg
        accDL1 = DL1TagCfg('DL1rnn')
        dl1 = accDL1.popPrivateTools()
        acc.merge(accDL1)
        BTaggingFlags.MVTagToolList.append(dl1)

    if 'MV2c10' in mvtm_active_taggers:
        from JetTagTools.MV2TagConfig import MV2TagCfg
        accMV2 = MV2TagCfg('MV2c10')
        mv2 = accMV2.popPrivateTools()
        acc.merge(accMV2)
        BTaggingFlags.MVTagToolList.append(mv2)

    if 'MV2c10mu' in mvtm_active_taggers:
        from JetTagTools.MV2TagConfig import MV2TagCfg
        accMV2 = MV2TagCfg('MV2c10mu')
        mv2 = accMV2.popPrivateTools()
        acc.merge(accMV2)
        BTaggingFlags.MVTagToolList.append(mv2)

    if 'MV2c10rnn' in mvtm_active_taggers:
        from JetTagTools.MV2TagConfig import MV2TagCfg
        accMV2 = MV2TagCfg('MV2c10rnn')
        mv2 = accMV2.popPrivateTools()
        acc.merge(accMV2)
        BTaggingFlags.MVTagToolList.append(mv2)

    if 'MV2c100' in mvtm_active_taggers:
        from JetTagTools.MV2TagConfig import MV2TagCfg
        accMV2 = MV2TagCfg('MV2c100')
        mv2 = accMV2.popPrivateTools()
        acc.merge(accMV2)
        BTaggingFlags.MVTagToolList.append(mv2)


    if 'MV2cl100' in mvtm_active_taggers:
        from JetTagTools.MV2TagConfig import MV2TagCfg
        accMV2 = MV2TagCfg('MV2cl100')
        mv2 = accMV2.popPrivateTools()
        acc.merge(accMV2)
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
