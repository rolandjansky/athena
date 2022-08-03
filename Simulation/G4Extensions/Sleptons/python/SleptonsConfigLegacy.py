# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon.SystemOfUnits import GeV,ns # noqa: F401
# Example specialConfiguration {'GMSBSlepton': '100.0*GeV', 'GMSBGravitino': '1e-07*GeV', 'GMSBSleptonTime': '0.01*ns'}

"""
Defining default settings for slepton/staus. Possible options are:
G4ParticleMass (default 0.0*GeV)
G4ParticleWidth (default 0.0*GeV)
G4ParticleCharge (default +/-1.*eplus)
G4ParticlePDGCode (default sparticle pdgid)
G4ParticleStable (default True)
G4ParticleLifetime (default -1)
G4ParticleShortlived (default False)
where Particle = [STau1Minus, STau1Plus, STau2Minus, STau2Plus, SElectronRMinus, SElectronRLinus, SElectronRPlus, SElectronLPlus, SMuonRMinus, SMuonLMinus, SMuonRPlus, SMuonLPlus]
"""
def getSleptonsPhysicsTool(name="SleptonsPhysicsTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    GMSBStau    = eval(simFlags.specialConfiguration.get_Value().get("GMSBStau", None))
    kwargs.setdefault("G4STau1MinusMass",             GMSBStau)
    kwargs.setdefault("G4STau1PlusMass",              GMSBStau)

    if "GMSBSlepton" in simFlags.specialConfiguration.get_Value():
        GMSBSlepton = eval(simFlags.specialConfiguration.get_Value().get("GMSBSlepton", None))

        kwargs.setdefault("G4SElectronRMinusMass",        GMSBSlepton)
        kwargs.setdefault("G4SElectronRPlusMass",         GMSBSlepton)
        kwargs.setdefault("G4SMuonRMinusMass",            GMSBSlepton)
        kwargs.setdefault("G4SMuonRPlusMass",             GMSBSlepton)

    return CfgMgr.SleptonsPhysicsTool(name, **kwargs)

def getAllSleptonsPhysicsTool(name="AllSleptonsPhysicsTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if "GMSBStau" in simFlags.specialConfiguration.get_Value():
        GMSBStau    = eval(simFlags.specialConfiguration.get_Value().get("GMSBStau", None))
        GMSBStauTime    = eval(simFlags.specialConfiguration.get_Value().get("GMSBStauTime", None))
        kwargs.setdefault("G4STau1MinusMass",             GMSBStau)
        kwargs.setdefault("G4STau1MinusPDGCode",          1000015)
        kwargs.setdefault("G4STau1MinusStable",           False)
        kwargs.setdefault("G4STau1MinusLifetime",         GMSBStauTime)

        kwargs.setdefault("G4STau1PlusMass",              GMSBStau)
        kwargs.setdefault("G4STau1PlusPDGCode",           -1000015)
        kwargs.setdefault("G4STau1PlusStable",            False)
        kwargs.setdefault("G4STau1PlusLifetime",          GMSBStauTime)

        kwargs.setdefault("G4STau2MinusMass",             GMSBStau)
        kwargs.setdefault("G4STau2MinusPDGCode",          2000015)
        kwargs.setdefault("G4STau2MinusStable",           False)
        kwargs.setdefault("G4STau2MinusLifetime",         GMSBStauTime)

        kwargs.setdefault("G4STau2PlusMass",              GMSBStau)
        kwargs.setdefault("G4STau2PlusPDGCode",           -2000015)
        kwargs.setdefault("G4STau2PlusStable",            False)
        kwargs.setdefault("G4STau2PlusLifetime",          GMSBStauTime)

    if "GMSBSlepton" in simFlags.specialConfiguration.get_Value():
        GMSBSlepton = eval(simFlags.specialConfiguration.get_Value().get("GMSBSlepton", None))
        GMSBSleptonTime = eval(simFlags.specialConfiguration.get_Value().get("GMSBSleptonTime", None))

        kwargs.setdefault("G4SElectronLMinusMass",        GMSBSlepton)
        kwargs.setdefault("G4SElectronLMinusPDGCode",     1000011)
        kwargs.setdefault("G4SElectronLMinusStable",      False)
        kwargs.setdefault("G4SElectronLMinusLifetime",    GMSBSleptonTime)

        kwargs.setdefault("G4SElectronLPlusMass",         GMSBSlepton)
        kwargs.setdefault("G4SElectronLPlusPDGCode",      -1000011)
        kwargs.setdefault("G4SElectronLPlusStable",       False)
        kwargs.setdefault("G4SElectronLPlusLifetime",     GMSBSleptonTime)

        kwargs.setdefault("G4SMuonLMinusMass",            GMSBSlepton)
        kwargs.setdefault("G4SMuonLMinusPDGCode",         1000013)
        kwargs.setdefault("G4SMuonLMinusStable",          False)
        kwargs.setdefault("G4SMuonLMinusLifetime",        GMSBSleptonTime)

        kwargs.setdefault("G4SMuonLPlusMass",             GMSBSlepton)
        kwargs.setdefault("G4SMuonLPlusPDGCode",          -1000013)
        kwargs.setdefault("G4SMuonLPlusStable",           False)
        kwargs.setdefault("G4SMuonLPlusLifetime",         GMSBSleptonTime)

        kwargs.setdefault("G4SElectronRMinusMass",        GMSBSlepton)
        kwargs.setdefault("G4SElectronRMinusPDGCode",     2000011)
        kwargs.setdefault("G4SElectronRMinusStable",      False)
        kwargs.setdefault("G4SElectronRMinusLifetime",    GMSBSleptonTime)


        kwargs.setdefault("G4SElectronRPlusMass",         GMSBSlepton)
        kwargs.setdefault("G4SElectronRPlusPDGCode",      -2000011)
        kwargs.setdefault("G4SElectronRPlusStable",       False)
        kwargs.setdefault("G4SElectronRPlusLifetime",     GMSBSleptonTime)

        kwargs.setdefault("G4SMuonRMinusMass",            GMSBSlepton)
        kwargs.setdefault("G4SMuonRMinusPDGCode",         2000013)
        kwargs.setdefault("G4SMuonRMinusStable",          False)
        kwargs.setdefault("G4SMuonRMinusLifetime",        GMSBSleptonTime)

        kwargs.setdefault("G4SMuonRPlusMass",             GMSBSlepton)
        kwargs.setdefault("G4SMuonRPlusPDGCode",          -2000013)
        kwargs.setdefault("G4SMuonRPlusStable",           False)
        kwargs.setdefault("G4SMuonRPlusLifetime",         GMSBSleptonTime)


    return CfgMgr.SleptonsPhysicsTool(name, **kwargs)


def getSElectronRPlusToElectronGravitino(name="SElectronRPlusToElectronGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_e_plus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
def getSElectronRMinusToElectronGravitino(name="SElectronRMinusToElectronGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_e_minus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)

def getSMuonRPlusToMuonGravitino(name="SMuonRPlusToMuonGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_mu_plus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
def getSMuonRMinusToMuonGravitino(name="SMuonRMinusToMuonGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_mu_minus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)

def getSTauLPlusToTauGravitino(name="STauLPlusToTauGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_tau_plus_1")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
def getSTauLMinusToTauGravitino(name="STauLMinusToTauGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_tau_minus_1")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)

def getSElectronLPlusToElectronGravitino(name="SElectronLPlusToElectronGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_e_plus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
def getSElectronLMinusToElectronGravitino(name="SElectronLMinusToElectronGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_e_minus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)

def getSMuonLPlusToMuonGravitino(name="SMuonLPlusToMuonGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_mu_plus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
def getSMuonLMinusToMuonGravitino(name="SMuonLMinusToMuonGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_mu_minus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)

def getSTauRPlusToTauGravitino(name="STauRPlusToTauGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_tau_plus_2")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau+")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
def getSTauRMinusToTauGravitino(name="STauRMinusToTauGravitino", **kwargs):
    kwargs.setdefault("ParticleName","s_tau_minus_2")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau-")
    return CfgMgr.AddPhysicsDecayTool(name, **kwargs)
