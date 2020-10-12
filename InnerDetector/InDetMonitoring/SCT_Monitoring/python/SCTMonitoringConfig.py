#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def SCTMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        from .SCTErrMonAlg import SCTErrMonAlgConfig
        acc.merge(SCTErrMonAlgConfig(flags))
        from .SCTHitEffMonAlg import SCTHitEffMonAlgConfig
        acc.merge(SCTHitEffMonAlgConfig(flags))
        from .SCTHitsNoiseMonAlg import SCTHitsNoiseMonAlgConfig
        acc.merge(SCTHitsNoiseMonAlgConfig(flags))
        from .SCTLorentzMonAlg import SCTLorentzMonAlgConfig
        acc.merge(SCTLorentzMonAlgConfig(flags))
        from .SCTTracksMonAlg import SCTTracksMonAlgConfig
        acc.merge(SCTTracksMonAlgConfig(flags))

    return acc
