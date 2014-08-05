# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TruthD3PDMaker/python/Atlfast1D3PDMakerFlags.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Feb, 2010
# @brief Common flags for Atlfast1 D3PD making.
#


"""Common flags for Atlfast1 D3PD making.
"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


class Atlfast1D3PDMakerFlags (JobPropertyContainer):
    """Common flags for Atlfast1 D3PD making.
"""
    pass


jobproperties.add_Container (Atlfast1D3PDMakerFlags)


# Helper to create a SG key property.
def _sgkey_prop (name, val):
    prop = type (name, (JobProperty,), {})
    prop.statusOn = True
    prop.allowedTypes = ['str']
    prop.StoredValue = val
    jobproperties.Atlfast1D3PDMakerFlags.add_JobProperty(prop)
    return

_sgkey_prop ('IsoElectronSGKey',  'AtlfastIsoElectronCollection')
_sgkey_prop ('IsoPhotonSGKey',    'AtlfastIsoPhotonCollection')
_sgkey_prop ('IsoMuonSGKey',      'AtlfastIsoMuonCollection')
_sgkey_prop ('ContainerPrefix',   '')


class UseAtlfast1Correction (JobProperty):
    """If true, use Atlfast1 corrected containers in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.Atlfast1D3PDMakerFlags.add_JobProperty(UseAtlfast1Correction)


Atlfast1D3PDMakerFlags = jobproperties.Atlfast1D3PDMakerFlags
