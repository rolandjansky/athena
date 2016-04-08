# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getSleptonsPhysicsTool(name="SleptonsPhysicsTool", **kwargs):
    ##from AthenaCommon.SystemOfUnits import GeV,MeV,eplus

    ##kwargs.setdefault"G4SElectronMinusMass",        100.0*GeV);
    ##kwargs.setdefault"G4SElectronMinusWidth",       0.0*GeV);
    ##kwargs.setdefault"G4SElectronMinusCharge",      -1.*eplus);
    ##kwargs.setdefault"G4SElectronMinusPDGCode",     2000011);
    ##kwargs.setdefault"G4SElectronMinusStable",      True);
    ##kwargs.setdefault"G4SElectronMinusLifetime",    -1);
    ##kwargs.setdefault"G4SElectronMinusShortlived",  False);

    ##kwargs.setdefault"G4SElectronPlusMass",         100.0*GeV);
    ##kwargs.setdefault"G4SElectronPlusWidth",        0.0*GeV);
    ##kwargs.setdefault"G4SElectronPlusCharge",       1.*eplus);
    ##kwargs.setdefault"G4SElectronPlusPDGCode",      -2000011);
    ##kwargs.setdefault"G4SElectronPlusStable",       True);
    ##kwargs.setdefault"G4SElectronPlusLifetime",     -1);
    ##kwargs.setdefault"G4SElectronPlusShortlived",   False);

    ##kwargs.setdefault"G4SMuonMinusMass",            100.0*GeV);
    ##kwargs.setdefault"G4SMuonMinusWidth",           0.0*GeV);
    ##kwargs.setdefault"G4SMuonMinusCharge",          -1.*eplus);
    ##kwargs.setdefault"G4SMuonMinusPDGCode",         2000013);
    ##kwargs.setdefault"G4SMuonMinusStable",          True);
    ##kwargs.setdefault"G4SMuonMinusLifetime",        -1);
    ##kwargs.setdefault"G4SMuonMinusShortlived",      False);

    ##kwargs.setdefault"G4SMuonPlusMass",             100.0*GeV);
    ##kwargs.setdefault"G4SMuonPlusWidth",            0.0*GeV);
    ##kwargs.setdefault"G4SMuonPlusCharge",           1.*eplus);
    ##kwargs.setdefault"G4SMuonPlusPDGCode",          -2000013);
    ##kwargs.setdefault"G4SMuonPlusStable",           True);
    ##kwargs.setdefault"G4SMuonPlusLifetime",         -1);
    ##kwargs.setdefault"G4SMuonPlusShortlived",       False);

    ##kwargs.setdefault"G4STauMinusMass",             100.0*GeV);
    ##kwargs.setdefault"G4STauMinusWidth",            0.0*GeV);
    ##kwargs.setdefault"G4STauMinusCharge",           -1.*eplus);
    ##kwargs.setdefault"G4STauMinusPDGCode",          1000015);
    ##kwargs.setdefault"G4STauMinusStable",           True);
    ##kwargs.setdefault"G4STauMinusLifetime",         -1);
    ##kwargs.setdefault"G4STauMinusShortlived",       False);

    ##kwargs.setdefault"G4STauPlusMass",              100.0*GeV);
    ##kwargs.setdefault"G4STauPlusWidth",             0.0*GeV);
    ##kwargs.setdefault"G4STauPlusCharge",            1.*eplus);
    ##kwargs.setdefault"G4STauPlusPDGCode",           -1000015);
    ##kwargs.setdefault"G4STauPlusStable",            True);
    ##kwargs.setdefault"G4STauPlusLifetime",          -1);
    ##kwargs.setdefault"G4STauPlusShortlived",        False);
    return CfgMgr.SleptonsPhysicsToolTool(name, **kwargs)
