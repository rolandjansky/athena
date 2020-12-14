# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from . import TrackD3PDMakerConf
for k, v in TrackD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

# Copy these here from TrackSummary.h so that we don't need to load
# all the EDM libraries to get these during configuration.
# (FIXME: The enums should be split into a separate dictionary
# with no EDM dependencies.)
# Electron probability from combining the below probabilities.
eProbabilityComb            = 0
# Electron probability from High Threshold (HT) information.
eProbabilityHT              = 1
# Electron probability from Time-Over-Threshold (ToT) information.
eProbabilityToT             = 2
# Electron probability from Brem fitting (DNA).
eProbabilityBrem            = 3
