# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getG4ScoringProcessTool(name="G4ScoringProcessTool", **kwargs):
    #Add any future configuration here
    return CfgMgr.G4ScoringProcessTool(name, **kwargs)
