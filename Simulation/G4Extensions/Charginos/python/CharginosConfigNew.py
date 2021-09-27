# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AccumulatorCache import AccumulatorCache
import shutil, re
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.SystemOfUnits import MeV, ns, GeV # noqa: F401
from AthenaConfiguration.Enums import ProductionStep
from ExtraParticles.PDGHelpers import getPDGTABLE
from G4AtlasServices.G4AtlasServicesConfigNew import PhysicsListSvcCfg


@AccumulatorCache
def get_and_fix_PDGTABLE_AMSB(replace):

    # Download generic PDGTABLE (overwrite existing one if it exists)
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


def CharginosPhysicsToolCfg(flags, name="CharginosPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    # Example specialConfiguration: {'AMSBC1Mass': '1200.16*GeV', 'AMSBN1Mass': '1200.0*GeV', 'AMSBC1Lifetime': '0.2*ns'}
    C1Mass = eval(flags.Input.SpecialConfiguration.get("AMSBC1Mass", None))
    N1Mass = eval(flags.Input.SpecialConfiguration.get("AMSBN1Mass", None))
    C1Lifetime = eval(flags.Input.SpecialConfiguration.get("AMSBC1Lifetime", "-1.0"))

    kwargs.setdefault("CharginoPlusMass", C1Mass)
    kwargs.setdefault("CharginoPlusStable",      (C1Lifetime < 0))
    if not (C1Lifetime < 0):
        kwargs.setdefault("CharginoPlusLifetime",    C1Lifetime)

    kwargs.setdefault("CharginoMinusMass", C1Mass)
    kwargs.setdefault("CharginoMinusStable",      (C1Lifetime < 0))
    if not (C1Lifetime < 0):
        kwargs.setdefault("CharginoMinusLifetime",    C1Lifetime)

    kwargs.setdefault("NeutralinoMass",          N1Mass)

    result.setPrivateTools(CompFactory.CharginosPhysicsTool(name, **kwargs))
    return result


def CharginoPlusToPiPlusNeutralinoCfg(flags, name="CharginoPlusToPiPlusNeutralino", **kwargs):
    result = ComponentAccumulator()
    C1ToPiBR = 1.0
    C1ToEleBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToPiBR -= C1ToEleBR
    C1ToMuBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToPiBR -= C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_plus_1")
    kwargs.setdefault("BR",C1ToPiBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,pi+")
    result.setPrivateTools(CompFactory.AddPhysicsDecayTool(name, **kwargs))
    return result


def CharginoMinusToPiMinusNeutralinoCfg(flags, name="CharginoMinusToPiMinusNeutralino", **kwargs):
    result = ComponentAccumulator()
    C1ToPiBR = 1.0
    C1ToEleBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToPiBR -= C1ToEleBR
    C1ToMuBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToPiBR -= C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_minus_1")
    kwargs.setdefault("BR",C1ToPiBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,pi-")
    result.setPrivateTools(CompFactory.AddPhysicsDecayTool(name, **kwargs))
    return result


def CharginoPlusToEPlusNeutralinoCfg(flags, name="CharginoPlusToEPlusNeutralino", **kwargs):
    result = ComponentAccumulator()
    C1ToEBR = 0.0
    C1ToEleBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToEBR = C1ToEleBR
    kwargs.setdefault("ParticleName","s_chi_plus_1")
    kwargs.setdefault("BR",C1ToEBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,nu_e,e+")
    result.setPrivateTools(CompFactory.AddPhysicsDecayTool(name, **kwargs))
    return result


def CharginoMinusToEMinusNeutralinoCfg(flags, name="CharginoMinusToEMinusNeutralino", **kwargs):
    result = ComponentAccumulator()
    C1ToEBR = 0.0
    C1ToEleBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToEleBR", "-1.0"))
    if C1ToEleBR > 0.0:
        C1ToEBR = C1ToEleBR
    kwargs.setdefault("ParticleName","s_chi_minus_1")
    kwargs.setdefault("BR",C1ToEBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,anti_nu_e,e-")
    result.setPrivateTools(CompFactory.AddPhysicsDecayTool(name, **kwargs))
    return result


def CharginoPlusToMuPlusNeutralinoCfg(flags, name="CharginoPlusToMuPlusNeutralino", **kwargs):
    result = ComponentAccumulator()
    C1ToMuonBR = 0.0
    C1ToMuBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToMuonBR = C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_plus_1")
    kwargs.setdefault("BR",C1ToMuonBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,nu_mu,mu+")
    result.setPrivateTools(CompFactory.AddPhysicsDecayTool(name, **kwargs))
    return result


def CharginoMinusToMuMinusNeutralinoCfg(flags, name="CharginoMinusToMuMinusNeutralino", **kwargs):
    result = ComponentAccumulator()
    C1ToMuonBR = 0.0
    C1ToMuBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToMuBR", "-1.0"))
    if C1ToMuBR > 0.0:
        C1ToMuonBR = C1ToMuBR
    kwargs.setdefault("ParticleName","s_chi_minus_1")
    kwargs.setdefault("BR",C1ToMuonBR) #Branching Ratio
    kwargs.setdefault("Daughters","s_chi_0_1,anti_nu_mu,mu-")
    result.setPrivateTools(CompFactory.AddPhysicsDecayTool(name, **kwargs))
    return result


def AMSB_Cfg(flags):
    result = ComponentAccumulator()
    C1Mass = eval(flags.Input.SpecialConfiguration["AMSBC1Mass"])
    N1Mass = eval(flags.Input.SpecialConfiguration["AMSBN1Mass"])
    # patching PDGTABLE
    get_and_fix_PDGTABLE_AMSB([(1000022, N1Mass, '~chi(0,1)', '0'), (1000024, C1Mass, '~chi(+,1)', '+')])
    if flags.Common.ProductionStep == ProductionStep.Simulation:
        result.merge(PhysicsListSvcCfg(flags))
        physicsOptions = [ result.popToolsAndMerge(CharginosPhysicsToolCfg(flags)) ]
        # Add Chargino decays if necessary
        C1Lifetime = eval(flags.Input.SpecialConfiguration.get("AMSBC1Lifetime", "-1.0"))
        if C1Lifetime > 0.0:
            physicsOptions += [ result.popToolsAndMerge(CharginoPlusToPiPlusNeutralinoCfg(flags)) ]
            physicsOptions += [ result.popToolsAndMerge(CharginoMinusToPiMinusNeutralinoCfg(flags)) ]
            C1ToEleBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToEleBR", "-1.0"))
            if C1ToEleBR > 0.0:
                physicsOptions += [ result.popToolsAndMerge(CharginoPlusToEPlusNeutralinoCfg(flags)) ]
                physicsOptions += [ result.popToolsAndMerge(CharginoMinusToEMinusNeutralinoCfg(flags)) ]
            C1ToMuBR = eval(flags.Input.SpecialConfiguration.get("AMSBC1ToMuBR", "-1.0"))
            if C1ToMuBR > 0.0:
                physicsOptions += [ result.popToolsAndMerge(CharginoPlusToMuPlusNeutralinoCfg(flags)) ]
                physicsOptions += [ result.popToolsAndMerge(CharginoMinusToMuMinusNeutralinoCfg(flags)) ]
        result.getService("PhysicsListSvc").PhysOption += physicsOptions

    return result
