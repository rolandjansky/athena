# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for SubDetectorEnvelopes
Elmar Ritsch, 27/09/2013
"""

from AthenaCommon.CfgGetter import addService

# Common tools, services and algorithms used by jobs:
addService("SubDetectorEnvelopes.SubDetectorEnvelopesConfig.getEnvelopeDefSvc", "AtlasGeometry_EnvelopeDefSvc")
