# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getGauginosPhysicsTool(name="GauginosPhysicsTool", **kwargs):
    ##from AthenaCommon.SystemOfUnits import GeV,MeV,eplus

    ##kwargs.setdefault"GravitinoMass",        0.108e-04*GeV);
    ##kwargs.setdefault"GravitinoWidth",       0.0*GeV);
    ##kwargs.setdefault"GravitinoCharge",      0);
    ##kwargs.setdefault"GravitinoPDGCode",     1000039);
    ##kwargs.setdefault"GravitinoStable",      True);
    ##kwargs.setdefault"GravitinoLifetime",    -1);
    ##kwargs.setdefault"GravitinoShortlived",  False);

    ##kwargs.setdefault"NeutralinoMass",        118.848*GeV);
    ##kwargs.setdefault"NeutralinoWidth",       0.0*GeV);
    ##kwargs.setdefault"NeutralinoCharge",      0);
    ##kwargs.setdefault"NeutralinoPDGCode",     1000022);
    ##kwargs.setdefault"NeutralinoStable",      True);
    ##kwargs.setdefault"NeutralinoLifetime",    -1);
    ##kwargs.setdefault"NeutralinoShortlived",  False);
    return CfgMgr.GauginosPhysicsToolTool(name, **kwargs)
