# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.SystemOfUnits import GeV,ns # noqa: F401


def GauginosPhysicsToolCfg(flags, name="GauginosPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    # Example specialConfiguration {'GMSBSlepton': '100.0*GeV', 'GMSBGravitino': '1e-07*GeV', 'GMSBSleptonTime': '0.01*ns'}
    GMSBNeutralino = eval(flags.Input.SpecialConfiguration.get("GMSBNeutralino", "0*GeV"))
    GMSBTime = eval(flags.Input.SpecialConfiguration.get("GMSBLifeTime", "0*GeV"))
    kwargs.setdefault("NeutralinoMass",        GMSBNeutralino)
    kwargs.setdefault("NeutralinoStable",      False)
    kwargs.setdefault("NeutralinoLifetime",    GMSBTime)

    if "GMSBGravitino" in flags.Input.SpecialConfiguration:
        GMSBGravitino = eval(flags.Input.SpecialConfiguration.get("GMSBGravitino", "0*GeV"))
        kwargs.setdefault("GravitinoMass",       GMSBGravitino)
    result.setPrivateTools( CompFactory.GauginosPhysicsTool(name, **kwargs) )
    return result


def NeutralinoToPhotonGravitinoCfg(flags, name="NeutralinoToPhotonGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_chi_0_1")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,gamma")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result
