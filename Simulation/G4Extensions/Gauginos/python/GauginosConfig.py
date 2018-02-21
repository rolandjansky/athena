# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getGauginosPhysicsTool(name="GauginosPhysicsTool", **kwargs):
    from AthenaCommon.SystemOfUnits import GeV,MeV,eplus,ns
    from G4AtlasApps.SimFlags import simFlags
    GMSBNeutralino = eval(simFlags.specialConfiguration.get_Value().get("GMSBNeutralino", None))
    GMSBTime = eval(simFlags.specialConfiguration.get_Value().get("GMSBLifeTime", None))
    kwargs.setdefault("NeutralinoMass",        GMSBNeutralino)
    ##kwargs.setdefault("NeutralinoWidth",       0.0*GeV);
    ##kwargs.setdefault("NeutralinoCharge",      0);
    ##kwargs.setdefault("NeutralinoPDGCode",     1000039);
    kwargs.setdefault("NeutralinoStable",      False);
    kwargs.setdefault("NeutralinoLifetime",    GMSBTime);
    ##kwargs.setdefault("NeutralinoShortlived",  False);

    if simFlags.specialConfiguration.get_Value().has_key("GMSBGravitino"):
        GMSBGravitino = eval(simFlags.specialConfiguration.get_Value().get("GMSBGravitino", None))
        kwargs.setdefault("GravitinoMass",       GMSBGravitino);
        ##kwargs.setdefault("GravitinoWidth",       0.0*GeV);
        ##kwargs.setdefault("GravitinoCharge",      0);
        ##kwargs.setdefault("GravitinoPDGCode",     1000022);
        ##kwargs.setdefault("GravitinoStable",      True);
        ##kwargs.setdefault("GravitinoLifetime",    -1);
        ##kwargs.setdefault("GravitinoShortlived",  False);
    return CfgMgr.GauginosPhysicsTool(name, **kwargs)


def getNeutralinoToPhotonGravitino(name="NeutralinoToPhotonGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_chi_0_1")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,gamma")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
