# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
SCT_DetectorElementCondAlg = CompFactory.SCT_DetectorElementCondAlg


def SCT_DetectorElementCondAlgCfg(flags, name='SCT_DetectorElementCondAlg', **kwargs):
    def merge_lists(a, b):
        a.extend([item for item in b if item not in a])
        return a

    alg = SCT_DetectorElementCondAlg(name, **kwargs)
    alg._descriptors['MuonManagerKey'].semantics.merge = merge_lists
    alg._descriptors['TRT_DetEltContKey'].semantics.merge = merge_lists
    alg._descriptors['PixelAlignmentStore'].semantics.merge = merge_lists

    from SCT_ConditionsAlgorithms.SCT_AlignCondAlgConfig import SCT_AlignCondAlgCfg
    acc = SCT_AlignCondAlgCfg(flags)
    acc.addCondAlgo(alg)
    return acc


def ITkStripDetectorElementCondAlgCfg(flags, name='ITkStripDetectorElementCondAlg', **kwargs):
    kwargs.setdefault('DetManagerName', 'ITkStrip')
    kwargs.setdefault('ReadKey', 'ITkStripAlignmentStore')
    kwargs.setdefault('WriteKey', 'ITkStripDetectorElementCollection')

    alg = SCT_DetectorElementCondAlg(name, **kwargs)

    from SCT_ConditionsAlgorithms.SCT_AlignCondAlgConfig import ITkStripAlignCondAlgCfg
    acc = ITkStripAlignCondAlgCfg(flags)
    acc.addCondAlgo(alg)
    return acc
