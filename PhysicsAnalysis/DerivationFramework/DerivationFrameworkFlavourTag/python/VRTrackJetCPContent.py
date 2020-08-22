# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

_aux_variables = [
    'pt','eta','phi','m',
    'HadronConeExclExtendedTruthLabelID', 'HadronConeExclTruthLabelID',
    'HadronGhostExtendedTruthLabelID', 'HadronGhostTruthLabelID',
    'has_ATLASRECTS_5027_bug','relativeDeltaRToVRJet']

_flavors = [
    'BHadronsFinal',
    'CHadronsFinal',
]


def getVRTrackJetCPContent(jc_name):
    labels = [
        'Ghost' + gp + 'Count' for gp in _flavors
    ] + [
        'Ghost' + gp + 'Pt' for gp in _flavors
    ]
    auxvars = '.'.join([jc_name + 'Aux'] + labels + _aux_variables + [''])
    return [ jc_name, auxvars ]
