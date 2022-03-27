# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod


def IPDetailedTrackGradeFactoryCfg( flags, name = 'IPDetailedTrackGradeFactory', useBTagFlagsDefaults = True, **options ):
    """Sets up a IPDetailedTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    useSharedHitInfo                    default: True
    useDetailSharedHitInfo              default: True
    useRun2TrackGrading                 default: False
    hitBLayerGrade                      default: True

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()

    btagrun23 = flags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3]

    if useBTagFlagsDefaults:
        defaults = { 'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'useRun2TrackGrading'    : btagrun23,
                     'useInnerLayers0HitInfo' : btagrun23,
                     'useDetailSplitHitInfo'  : btagrun23,
                     'useITkTrackGrading'     : flags.GeoModel.Run >= LHCPeriod.Run4,
                     'hitBLayerGrade'         : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(CompFactory.Analysis.DetailedTrackGradeFactory(**options))

    return acc
