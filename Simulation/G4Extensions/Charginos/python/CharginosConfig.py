# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getCharginosPhysicsTool(name="CharginosPhysicsTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.SystemOfUnits import GeV,MeV,eplus,ns
    C1Mass = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1Mass", None))
    N1Mass = eval(simFlags.specialConfiguration.get_Value().get("AMSBN1Mass", None))
    C1Lifetime = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1Lifetime", "-1.0"))

    kwargs.setdefault("CharginoPlusMass", C1Mass*MeV) # TODO check units
    kwargs.setdefault("CharginoPlusStable",      (C1Lifetime < 0))
    if not (C1Lifetime < 0):
        kwargs.setdefault("CharginoPlusLifetime",    C1Lifetime*ns)
    ##kwargs.setdefault("CharginoPlusMass",        101.0*GeV)
    ##kwargs.setdefault("CharginoPlusWidth",       0.0*MeV)
    ##kwargs.setdefault("CharginoPlusCharge",      1.*eplus)
    ##kwargs.setdefault("CharginoPlusPDGCode",     1000024)
    ##kwargs.setdefault("CharginoPlusStable",      True)
    ##kwargs.setdefault("CharginoPlusLifetime",    -1)
    ##kwargs.setdefault("CharginoPlusShortlived",  False)

    kwargs.setdefault("CharginoMinusMass", C1Mass*MeV) # TODO check units
    kwargs.setdefault("CharginoMinusStable",      (C1Lifetime < 0))
    if not (C1Lifetime < 0):
        kwargs.setdefault("CharginoMinusLifetime",    C1Lifetime*ns)
    ##kwargs.setdefault("CharginoMinusMass",        101.0*GeV)
    ##kwargs.setdefault("CharginoMinusWidth",       0.0*MeV)
    ##kwargs.setdefault("CharginoMinusCharge",      1.*eplus)
    ##kwargs.setdefault("CharginoMinusPDGCode",     1000024)
    ##kwargs.setdefault("CharginoMinusStable",      True)
    ##kwargs.setdefault("CharginoMinusLifetime",    -1)
    ##kwargs.setdefault("CharginoMinusShortlived",  False)

    kwargs.setdefault("NeutralinoMass",          N1Mass*MeV) # TODO check units
    ##kwargs.setdefault("NeutralinoWidth",         0.0*MeV)
    ##kwargs.setdefault("NeutralinoCharge",        0.*eplus)
    ##kwargs.setdefault("NeutralinoPDGCode",       1000022)
    ##kwargs.setdefault("NeutralinoStable",        True)
    ##kwargs.setdefault("NeutralinoLifetime",      -1)
    ##kwargs.setdefault("NeutralinoShortlived",    False)

    return CfgMgr.CharginosPhysicsTool(name, **kwargs)


def getCharginoPlusToPiPlusNeutralino(name="CharginoPlusToPiPlusNeutralino", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    C1ToPiBR = 1.0
    C1ToEleBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToPiBR -= C1ToEleBR
    C1ToMuBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToPiBR -= C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_plus_1")
    kwargs.setdefault("BR",C1ToPiBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,pi+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)


def getCharginoMinusToPiMinusNeutralino(name="CharginoMinusToPiMinusNeutralino", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    C1ToPiBR = 1.0
    C1ToEleBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToPiBR -= C1ToEleBR
    C1ToMuBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToPiBR -= C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_minus_1")
    kwargs.setdefault("BR",C1ToPiBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,pi-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)


def getCharginoPlusToEPlusNeutralino(name="CharginoPlusToEPlusNeutralino", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    C1ToEBR = 0.0
    C1ToEleBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToEBR = C1ToEleBR
    kwargs.setdefault("ParticleName","s_chi_plus_1")
    kwargs.setdefault("BR",C1ToEBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,nu_e,e+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)


def getCharginoMinusToEMinusNeutralino(name="CharginoMinusToEMinusNeutralino", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    C1ToEBR = 0.0
    C1ToEleBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToEBR = C1ToEleBR
    kwargs.setdefault("ParticleName","s_chi_minus_1")
    kwargs.setdefault("BR",C1ToEBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,anti_nu_e,e-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)


def getCharginoPlusToMuPlusNeutralino(name="CharginoPlusToMuPlusNeutralino", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    C1ToMuonBR = 0.0
    C1ToMuBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToMuonBR = C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_plus_1")
    kwargs.setdefault("BR",C1ToMuonBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,nu_mu,mu+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)


def getCharginoMinusToMuMinusNeutralino(name="CharginoMinusToMuMinusNeutralino", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    C1ToMuonBR = 0.0
    C1ToMuBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToMuonBR = C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_minus_1")
    kwargs.setdefault("BR",C1ToMuonBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,anti_nu_mu,mu-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
