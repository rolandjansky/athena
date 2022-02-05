# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AccumulatorCache import AccumulatorCache
import sys, shutil, re
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.SystemOfUnits import GeV,ns # noqa: F401
from AthenaConfiguration.Enums import ProductionStep
from Gauginos.GauginosConfigNew import GauginosPhysicsToolCfg, NeutralinoToPhotonGravitinoCfg
from G4AtlasServices.G4AtlasServicesConfigNew import PhysicsListSvcCfg

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


@AccumulatorCache
def get_and_fix_PDGTABLE_GMSB(replace):

    # Download generic PDGTABLE (overwrite existing one if it exists)
    from ExtraParticles.PDGHelpers import getPDGTABLE
    if getPDGTABLE('PDGTABLE.MeV'):
        shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

        # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
        # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

        lines = open('PDGTABLE.MeV.org').readlines()
        for pdgid,mass,name,charge in replace:
            if not re.search(r'[MW]\s+'+str(pdgid)+r'\s+\S+', ''.join(lines)):
                lines.append('M' + str(pdgid).rjust(8) +''.ljust(26) +
                             ('%11.5E' % mass).ljust(15) +
                             '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                             name.strip() + ''.ljust(6) + charge.strip()+''.rjust(20-len(name.strip())) + '\n')
                lines.append('W' + str(pdgid).rjust(8) +''.ljust(26) +
                             '0.E+00'.ljust(15) + '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                             name.strip() + ''.ljust(6) + charge.strip()+''.rjust(20-len(name.strip())) + '\n')
            else:
                from past.builtins import xrange # Temporary workaround for python3 compatibility use range in CA-based config
                for i in xrange(len(lines)):
                    if re.search(r'M\s+'+str(pdgid)+r'\s+\S+', lines[i]):
                        l = lines[i]
                        lines[i] = l[0:35] + ('%11.5E' % mass).ljust(14) + l[49:]

        update = open('PDGTABLE.MeV', 'w')
        update.write(''.join(lines))
        update.close()

        print ('modfied PDGTABLE\n%s\n' % ''.join(lines))
        sys.stdout.flush()


@AccumulatorCache
def load_files_for_GMSB_scenario(simdict):

    GMSBIndex = int(simdict["GMSBIndex"])

    if GMSBIndex == 1:
        get_and_fix_PDGTABLE_GMSB([
                              (1000022, eval(simdict["GMSBNeutralino"]), '~chi(0,1)', '0'),
                              (1000039, eval(simdict.get("GMSBGravitino",'0')), '~G', '0')
                            ])

    elif GMSBIndex == 2:
        m_stau    = eval(simdict["GMSBStau"])
        m_slepton = eval(simdict["GMSBSlepton"])
        get_and_fix_PDGTABLE_GMSB([
                              (1000015, m_stau, '~tau(L)', '-'),
                              (2000011, m_slepton, '~e(R)', '-'),
                              (2000013, m_slepton, '~mu(R)', '-')
                            ])

    elif GMSBIndex == 3:
        m_stau = eval(simdict["GMSBStau"])
        m_slepton = eval(simdict["GMSBSlepton"])
        m_squark = eval(simdict["SQUARKMASS"])
        m_neutralino = eval(simdict["NEUTRALINOMASS"])
        m_gluino = eval(simdict["GLUINOMASS"])
        get_and_fix_PDGTABLE_GMSB([
                              (1000001, m_squark, '~d(L)', '-1/3'), (2000001, m_squark, '~d(R)', '-1/3'),
                              (1000002, m_squark, '~u(L)', '+2/3'), (2000002, m_squark, '~u(R)', '+2/3'),
                              (1000003, 1.00E+04, '~s(L)', '-1/3'), (2000003, 1.00E+04, '~s(R)', '-1/3'),
                              (1000004, 1.00E+04, '~c(L)', '+2/3'), (2000004, 1.00E+04, '~c(R)', '+2/3'),
                              (1000005, 1.00E+04, '~b(1)', '-1/3'), (2000005, 1.00E+04, '~b(2)', '-1/3'),
                              (1000006, 1.00E+04, '~t(1)', '+2/3'), (2000006, 1.00E+04, '~t(2)', '+2/3'),
                              (1000011, 2.50E+02, '~e(L)', '-'), (2000011, m_slepton, '~e(R)', '-'),
                              (1000012, 1.00E+04, '~nu(e,L)', '0'),
                              (1000013, 2.50E+02, '~mu(L)', '-'), (2000013, m_slepton, '~mu(R)', '-'),
                              (1000014, 1.00E+04, '~nu(e,L)', '0'),
                              (1000015, m_stau, '~tau(L)', '-'), (2000015, 2.50E+02, '~tau(R)', '-'),
                              (1000016, 1.00E+04, '~nu(tau,L)', '0'),
                              (1000021, m_gluino, '~g', '0'),
                              (1000022, m_neutralino, '~chi(0,1)', '0'),
                              (1000023, 1.00E+04, '~chi(0,2)', '0'),
                              (1000024, 1.00E+04, '~chi(+,1)', '+'),
                              (1000025, -1.00E+04, '~chi(0,3)', '0'),
                              (1000035, 1.00E+04, '~chi(0,4)', '0'),
                              (1000037, 1.00E+04, '~chi(+,2)', '+')
                            ])

    elif GMSBIndex == 4:
        get_and_fix_PDGTABLE_GMSB([
                              (1000015, m_stau, '~tau(L)', '-')
                            ])

    else:
        print ('GMSBIndex %i not supported' % GMSBIndex)
        raise


@AccumulatorCache
def get_and_fix_PDGTABLE_sleptons(flags, replace):

    # Download generic PDGTABLE (do not overwrite existing one if it exists, use existing one instead)
    from ExtraParticles.PDGHelpers import getPDGTABLE
    if getPDGTABLE('PDGTABLE.MeV'):
        shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
        # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

        lines = open('PDGTABLE.MeV.org').readlines()
        for pdgid,mass,name,charge in replace:
            if not re.search(r'[MW]\s+'+str(pdgid)+r'\s+\S+', ''.join(lines)):
                lines.append('M' + str(pdgid).rjust(8) +''.ljust(26) +
                             ('%11.5E' % mass).ljust(15) +
                             '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                             name.strip() + ''.ljust(6) + charge.strip()+''.rjust(20-len(name.strip())) + '\n')
                lines.append('W' + str(pdgid).rjust(8) +''.ljust(26) +
                             '0.E+00'.ljust(15) + '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                             name.strip() + ''.ljust(6) + charge.strip()+''.rjust(20-len(name.strip())) + '\n')
            else:
                from past.builtins import xrange # Temporary workaround for python3 compatibility use range in CA-based config
                for i in xrange(len(lines)):
                    if re.search(r'M\s+'+str(pdgid)+r'\s+\S+', lines[i]):
                        l = lines[i]
                        lines[i] = l[0:35] + ('%11.5E' % mass).ljust(14) + l[49:]

        update = open('PDGTABLE.MeV', 'w')
        update.write(''.join(lines))
        update.close()

        print('modfied PDGTABLE\n%s\n' % ''.join(lines))
        sys.stdout.flush()


def load_files_for_sleptonLLP_scenario(flags):
    simdict = flags.Input.SpecialConfiguration
    if "GMSBSlepton" in simdict:
        get_and_fix_PDGTABLE_sleptons(flags, [
                (2000011, eval(simdict.get("GMSBSlepton",'0')), '~e(R)', '-'),
                (2000013, eval(simdict.get("GMSBSlepton",'0')), '~mu(R)', '-'),
                (1000011, eval(simdict.get("GMSBSlepton",'0')), '~e(L)', '-'),
                (1000013, eval(simdict.get("GMSBSlepton",'0')), '~mu(L)', '-'),
                ])
    if "GMSBStau" in simdict:
        get_and_fix_PDGTABLE_sleptons(flags, [
                (2000015, eval(simdict.get("GMSBStau",'0')), '~tau(R)', '-'),
                (1000015, eval(simdict.get("GMSBStau",'0')), '~tau(L)', '-'),
                ])
    if "GMSBGravitino" in simdict:
        get_and_fix_PDGTABLE_sleptons(flags, [
                (1000039, eval(simdict.get("GMSBGravitino",'0')), '~G', '0'),
                ])


def SleptonsPhysicsToolCfg(flags, name="SleptonsPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    GMSBStau    = eval(flags.Input.SpecialConfiguration.get("GMSBStau", None))
    kwargs.setdefault("G4STau1MinusMass",             GMSBStau)
    kwargs.setdefault("G4STau1PlusMass",              GMSBStau)

    if "GMSBSlepton" in flags.Input.SpecialConfiguration:
        GMSBSlepton = eval(flags.Input.SpecialConfiguration.get("GMSBSlepton", None))

        kwargs.setdefault("G4SElectronRMinusMass",        GMSBSlepton)
        kwargs.setdefault("G4SElectronRPlusMass",         GMSBSlepton)
        kwargs.setdefault("G4SMuonRMinusMass",            GMSBSlepton)
        kwargs.setdefault("G4SMuonRPlusMass",             GMSBSlepton)

    result.setPrivateTools( CompFactory.SleptonsPhysicsTool(name, **kwargs) )
    return result


def AllSleptonsPhysicsToolCfg(flags, name="AllSleptonsPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    if "GMSBStau" in flags.Input.SpecialConfiguration:
        GMSBStau    = eval(flags.Input.SpecialConfiguration.get("GMSBStau", None))
        GMSBStauTime    = eval(flags.Input.SpecialConfiguration.get("GMSBStauTime", None))
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

    if "GMSBSlepton" in flags.Input.SpecialConfiguration:
        GMSBSlepton = eval(flags.Input.SpecialConfiguration.get("GMSBSlepton", None))
        GMSBSleptonTime = eval(flags.Input.SpecialConfiguration.get("GMSBSleptonTime", None))

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
    result.setPrivateTools( CompFactory.SleptonsPhysicsTool(name, **kwargs) )
    return result


def SElectronRPlusToElectronGravitinoCfg(flags, name="SElectronRPlusToElectronGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_e_plus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e+")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SElectronRMinusToElectronGravitinoCfg(flags, name="SElectronRMinusToElectronGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_e_minus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e-")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SMuonRPlusToMuonGravitinoCfg(flags, name="SMuonRPlusToMuonGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_mu_plus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu+")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SMuonRMinusToMuonGravitinoCfg(flags, name="SMuonRMinusToMuonGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_mu_minus_R")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu-")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def STauLPlusToTauGravitinoCfg(flags, name="STauLPlusToTauGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_tau_plus_1")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau+")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def STauLMinusToTauGravitinoCfg(flags, name="STauLMinusToTauGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_tau_minus_1")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau-")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SElectronLPlusToElectronGravitinoCfg(flags, name="SElectronLPlusToElectronGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_e_plus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e+")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SElectronLMinusToElectronGravitinoCfg(flags, name="SElectronLMinusToElectronGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_e_minus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,e-")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SMuonLPlusToMuonGravitinoCfg(flags, name="SMuonLPlusToMuonGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_mu_plus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu+")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def SMuonLMinusToMuonGravitinoCfg(flags, name="SMuonLMinusToMuonGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_mu_minus_L")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,mu-")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def STauRPlusToTauGravitinoCfg(flags, name="STauRPlusToTauGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_tau_plus_2")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau+")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result


def STauRMinusToTauGravitinoCfg(flags, name="STauRMinusToTauGravitino", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleName","s_tau_minus_2")
    kwargs.setdefault("BR", 1.0) # Branching Ratio
    kwargs.setdefault("Daughters","s_G,tau-")
    result.setPrivateTools( CompFactory.AddPhysicsDecayTool(name, **kwargs) )
    return result

def SleptonsLLPCfg(flags):
    result = ComponentAccumulator()
    load_files_for_sleptonLLP_scenario(flags)

    if flags.Common.ProductionStep == ProductionStep.Simulation:
        result.merge(PhysicsListSvcCfg(flags))
        physicsOptions = [ result.popToolsAndMerge(GauginosPhysicsToolCfg(flags)) ]
        physicsOptions += [ result.popToolsAndMerge(AllSleptonsPhysicsToolCfg(flags)) ]
        # Slepton decays from SleptonsConfig
        if "GMSBSlepton" in flags.Input.SpecialConfiguration:
            physicsOptions += [ result.popToolsAndMerge(SElectronRPlusToElectronGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SElectronLPlusToElectronGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SElectronRMinusToElectronGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SElectronLMinusToElectronGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SMuonRPlusToMuonGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SMuonLPlusToMuonGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SMuonRMinusToMuonGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(SMuonLMinusToMuonGravitinoCfg(flags)) ]
        if "GMSBStau" in flags.Input.SpecialConfiguration:
            physicsOptions += [ result.popToolsAndMerge(STauRPlusToTauGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(STauLPlusToTauGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(STauRMinusToTauGravitinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(STauLMinusToTauGravitinoCfg(flags)) ]
    result.getService("PhysicsListSvc").PhysOption += physicsOptions
    return result


def GMSB_Cfg(flags):
    result = ComponentAccumulator()
    simdict = flags.Input.SpecialConfiguration
    assert "GMSBIndex" in simdict

    load_files_for_GMSB_scenario(simdict)

    if flags.Common.ProductionStep == ProductionStep.Simulation:
        GMSBIndex = int(simdict["GMSBIndex"])
        result.merge(PhysicsListSvcCfg(flags))
        physicsOptions = []
        if GMSBIndex == 1: # generic neutralino to photon scenario
            physicsOptions = [ result.popToolsAndMerge(GauginosPhysicsToolCfg(flags)) ]
            physicsOptions = [ result.popToolsAndMerge(NeutralinoToPhotonGravitinoCfg(flags)) ]
        elif GMSBIndex == 2 or GMSBIndex == 3 or GMSBIndex == 4: # generic stau scenario
            physicsOptions = [ result.popToolsAndMerge(SleptonsPhysicsToolCfg(flags)) ]
        else:
            print ('GMSBIndex %i not supported' % GMSBIndex)
            raise
        del GMSBIndex
    result.getService("PhysicsListSvc").PhysOption += physicsOptions
    return result
