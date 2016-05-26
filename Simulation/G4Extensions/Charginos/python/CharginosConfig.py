# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getCharginosPhysicsTool(name="CharginosPhysicsTool", **kwargs):
    ##from AthenaCommon.SystemOfUnits import GeV,MeV,eplus

    ##kwargs.setdefault"CharginoPlusMass",        101.0*GeV);
    ##kwargs.setdefault"CharginoPlusWidth",       0.0*MeV);
    ##kwargs.setdefault"CharginoPlusCharge",      1.*eplus);
    ##kwargs.setdefault"CharginoPlusPDGCode",     1000024);
    ##kwargs.setdefault"CharginoPlusStable",      True);
    ##kwargs.setdefault"CharginoPlusLifetime",    -1);
    ##kwargs.setdefault"CharginoPlusShortlived",  False);

    ##kwargs.setdefault"CharginoMinusMass",       101.0*GeV);
    ##kwargs.setdefault"CharginoMinusWidth",      0.0*MeV);
    ##kwargs.setdefault"CharginoMinusCharge",     -1.*eplus);
    ##kwargs.setdefault"CharginoMinusPDGCode",    -1000024);
    ##kwargs.setdefault"CharginoMinusStable",     True);
    ##kwargs.setdefault"CharginoMinusLifetime",   -1);
    ##kwargs.setdefault"CharginoMinusShortlived", False);

    ##kwargs.setdefault"NeutralinoMass",          100.0*GeV);
    ##kwargs.setdefault"NeutralinoWidth",         0.0*MeV);
    ##kwargs.setdefault"NeutralinoCharge",        0.*eplus);
    ##kwargs.setdefault"NeutralinoPDGCode",       1000022);
    ##kwargs.setdefault"NeutralinoStable",        True);
    ##kwargs.setdefault"NeutralinoLifetime",      -1);
    ##kwargs.setdefault"NeutralinoShortlived",    False);

    return CfgMgr.CharginosPhysicsToolTool(name, **kwargs)
