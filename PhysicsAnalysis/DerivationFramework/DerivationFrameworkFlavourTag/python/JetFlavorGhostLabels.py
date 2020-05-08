# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

_flavors = [
    'BHadronsFinal',
    'CHadronsFinal',
]

def getJetFlavorGhostLabels(flavors=_flavors):
    labels = [
        'Ghost' + gp + 'Count' for gp in _flavors
    ] + [
        'Ghost' + gp + 'Pt' for gp in _flavors
    ]
    return '.'.join(labels)
