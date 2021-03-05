# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import getPublicTool


def getActsSimServiceID(name="ISF_ActsSimSvc", **kwargs):
    kwargs.setdefault("Identifier",     "Fatras")
    kwargs.setdefault("SimulatorTool",  "ISF_ActsSimulatorToolST")
    from AthenaCommon import CfgMgr
    return CfgMgr.ISF__LegacySimSvc(name, **kwargs )


def getActsSimulatorToolST(name="ISF_ActsSimulatorToolST", **kwargs):
    kwargs.setdefault("IDSimulationTool", getPublicTool('ISF_ActsFatrasSimTool'))
    kwargs.setdefault("SimulationTool",   getPublicTool('ISF_ActsFatrasSimTool'))
    # register Fatras random number stream if not already registered
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "FatrasRnd" ):
      simFlags.RandomSeedList.addSeed( "FatrasRnd", 81234740, 23474923 )
    from AthenaCommon import CfgMgr
    return CfgMgr.ISF__ActsSimTool(name, **kwargs )

