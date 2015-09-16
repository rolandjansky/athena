# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgGetter,CfgMgr
from G4AtlasApps.SimFlags import simFlags
from AthenaCommon.BeamFlags import jobproperties

# this creates the USerActionSvc with the default actions
# additional actions must be added to the service by the
# client packages, through G4AtlasUserActionConfig.UAStore
def getUserActionSvc(name="UserActionSvc", **kwargs):
    # placeholder for more advanced config, if needed
    return CfgMgr.UserActionSvc(name, **kwargs)
