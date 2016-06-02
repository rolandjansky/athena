# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getGauginosPhysicsTool(name="GauginosPhysicsTool", **kwargs):
    from AthenaCommon.SystemOfUnits import GeV,MeV,eplus,ns
    from G4AtlasApps import AtlasG4Eng
    GMSBNeutralino = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBNeutralino"])
    GMSBTime = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBLifeTime"])
    kwargs.setdefault("GravitinoMass",        GMSBNeutralino)
    ##kwargs.setdefault("GravitinoWidth",       0.0*GeV);
    ##kwargs.setdefault("GravitinoCharge",      0);
    ##kwargs.setdefault("GravitinoPDGCode",     1000039);
    kwargs.setdefault("GravitinoStable",      False);
    kwargs.setdefault("GravitinoLifetime",    GMSBTime);
    ##kwargs.setdefault("GravitinoShortlived",  False);

    if AtlasG4Eng.G4Eng.Dict_SpecialConfiguration.has_key("GMSBGravitino"):
        GMSBGravitino = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBGravitino"])
        kwargs.setdefault("NeutralinoMass",       GMSBGravitino);
        ##kwargs.setdefault("NeutralinoWidth",       0.0*GeV);
        ##kwargs.setdefault("NeutralinoCharge",      0);
        ##kwargs.setdefault("NeutralinoPDGCode",     1000022);
        ##kwargs.setdefault("NeutralinoStable",      True);
        ##kwargs.setdefault("NeutralinoLifetime",    -1);
        ##kwargs.setdefault("NeutralinoShortlived",  False);
    return CfgMgr.GauginosPhysicsToolTool(name, **kwargs)
