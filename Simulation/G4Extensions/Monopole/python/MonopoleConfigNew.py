# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AccumulatorCache import AccumulatorCache
import os
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import ProductionStep
from G4AtlasServices.G4AtlasServicesConfigNew import PhysicsListSvcCfg
from ExtraParticles.PDGHelpers import getPDGTABLE


@AccumulatorCache
def load_files_for_monopole_scenario(MASS, GCHARGE):
    if getPDGTABLE('PDGTABLE.MeV'):
        ALINE1="M 4110000                         {intmass}.E+03       +0.0E+00 -0.0E+00 Monopole         0".format(intmass=int(MASS))
        ALINE2="W 4110000                          0.E+00         +0.0E+00 -0.0E+00 Monopole         0"
        BLINE1="4110000 {intmass}.00 0.0 {gcharge} # Monopole".format(intmass=int(MASS), gcharge=GCHARGE)
        BLINE2="-4110000 {intmass}.00 0.0 -{gcharge} # MonopoleBar".format(intmass=int(MASS), gcharge=GCHARGE)

        f=open('PDGTABLE.MeV','a')
        f.writelines(str(ALINE1))
        f.writelines('\n')
        f.writelines(str(ALINE2))
        f.writelines('\n')
        f.close()
        partmod = os.path.isfile('particles.txt')
        if partmod is True:
            os.remove('particles.txt')
        f=open('particles.txt','w')
        f.writelines(str(BLINE1))
        f.writelines('\n')
        f.writelines(str(BLINE2))
        f.writelines('\n')
        f.close()

        del ALINE1
        del ALINE2
        del BLINE1
        del BLINE2


@AccumulatorCache
def load_files_for_qball_scenario(MASS, CHARGE):
    if getPDGTABLE('PDGTABLE.MeV'):
        CODE=10000000+int(float(CHARGE)*100)

        ALINE1="M {code}                         {intmass}.E+03       +0.0E+00 -0.0E+00 Qball           +".format(code=CODE,intmass=int(MASS))
        ALINE2="W {code}                         0.E+00         +0.0E+00 -0.0E+00 Qball           +".format(code=CODE)
        BLINE1="{code}  {intmass}.00  {charge}  0.0 # Qball".format(code=CODE,intmass=int(MASS), charge=CHARGE)
        BLINE2="-{code}  {intmass}.00  -{charge}  0.0 # QballBar".format(code=CODE,intmass=int(MASS), charge=CHARGE)

        f=open('PDGTABLE.MeV','a')
        f.writelines(str(ALINE1))
        f.writelines('\n')
        f.writelines(str(ALINE2))
        f.writelines('\n')
        f.close()
        partmod = os.path.isfile('particles.txt')
        if partmod is True:
            os.remove('particles.txt')
        f=open('particles.txt','w')
        f.writelines(str(BLINE1))
        f.writelines('\n')
        f.writelines(str(BLINE2))
        f.writelines('\n')
        f.close()

        del ALINE1
        del ALINE2
        del BLINE1
        del BLINE2


@AccumulatorCache
def load_files_for_fcp_scenario(MASS, CHARGE, X, Y):
    CODE=int(20000000)+int(X)*1000+int(Y)*10
    print("Trying to load %s, %s for particle with code %s" % (X, Y, CODE))

    pdgLine1="M {code}                         {intmass}.E+03       +0.0E+00 -0.0E+00 fcp           +\n".format(code=CODE,intmass=int(MASS))
    pdgLine2="W {code}                         0.E+00         +0.0E+00 -0.0E+00 fcp           +\n".format(code=CODE)
    particleLine1="{code}  {intmass}.00  {fcharge}  0.0 # fcp\n".format(code=CODE,intmass=int(MASS), fcharge=float(CHARGE))
    particleLine2="-{code}  {intmass}.00  -{fcharge}  0.0 # fcpBar\n".format(code=CODE,intmass=int(MASS), fcharge=float(CHARGE))

    # retreive the PDGTABLE file
    if getPDGTABLE('PDGTABLE.MeV'):
        f=open('PDGTABLE.MeV','a')
        f.writelines(str(pdgLine1))
        f.writelines(str(pdgLine2))
        f.close()
        partmod = os.path.isfile('particles.txt')
        if partmod is True:
            os.remove('particles.txt')
        f=open('particles.txt','w')
        f.writelines(str(particleLine1))
        f.writelines(str(particleLine2))
        f.close()

    del pdgLine1
    del pdgLine2
    del particleLine1
    del particleLine2


def MonopolePhysicsToolCfg(flags, name="MonopolePhysicsTool", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools( CompFactory.MonopolePhysicsTool(name, **kwargs) )
    return result


def G4mplEqMagElectricFieldToolCfg(flags, name="G4mplEqMagElectricField", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools( CompFactory.G4mplEqMagElectricFieldTool(name, **kwargs) )
    return result


def fcpPreInclude(flags):
    simdict = flags.Input.SpecialConfiguration
    if flags.Common.ProductionStep == ProductionStep.Simulation:
        if "InteractingPDGCodes" not in simdict: #FIXME This code would ideally update the ConfigFlag itself
            assert "CHARGE" in simdict
            assert "X" in simdict
            assert "Y" in simdict
            CODE=int(20000000)+int(simdict["X"])*1000+int(simdict["Y"])*10
            simdict['InteractingPDGCodes'] = str([CODE,-1*CODE])


def fcpCfg(flags):
    result = ComponentAccumulator()
    simdict = flags.Input.SpecialConfiguration
    load_files_for_fcp_scenario(simdict["MASS"], simdict["CHARGE"], simdict["X"], simdict["Y"])

    if flags.Common.ProductionStep == ProductionStep.Simulation:
        result.merge(PhysicsListSvcCfg(flags))
        physicsOptions = [ result.popToolsAndMerge(MonopolePhysicsToolCfg(flags)) ]
        result.getService("PhysicsListSvc").PhysOption += physicsOptions
        # add monopole-specific configuration for looper killer
        #simFlags.OptionalUserActionList.addAction('G4UA::MonopoleLooperKillerTool') #FIXME missing functionality
        # add default HIP killer
        #simFlags.OptionalUserActionList.addAction('G4UA::HIPKillerTool') #FIXME missing functionality

    return result


def QballPreInclude(flags):
    simdict = flags.Input.SpecialConfiguration
    if flags.Common.ProductionStep == ProductionStep.Simulation:
            if "InteractingPDGCodes" not in simdict:
                assert "CHARGE" in simdict
                CODE=10000000+int(float(simdict["CHARGE"])*100)
                simdict['InteractingPDGCodes'] = str([CODE,-1*CODE])


def QballCfg(flags):
    result = ComponentAccumulator()
    simdict = flags.Input.SpecialConfiguration
    if flags.Common.ProductionStep == ProductionStep.Simulation:
            if "InteractingPDGCodes" not in simdict:
                assert "CHARGE" in simdict
                CODE=10000000+int(float(simdict["CHARGE"])*100)
                simdict['InteractingPDGCodes'] = str([CODE,-1*CODE])

    assert "MASS" in simdict
    assert "CHARGE" in simdict
    load_files_for_qball_scenario(simdict["MASS"], simdict["CHARGE"])

    if flags.Common.ProductionStep == ProductionStep.Simulation:
        result.merge(PhysicsListSvcCfg(flags))
        physicsOptions = [ result.popToolsAndMerge(MonopolePhysicsToolCfg(flags)) ]
        result.getService("PhysicsListSvc").PhysOption += physicsOptions
        # add monopole-specific configuration for looper killer
        #simFlags.OptionalUserActionList.addAction('G4UA::MonopoleLooperKillerTool') #FIXME missing functionality
        # add default HIP killer
        #simFlags.OptionalUserActionList.addAction('G4UA::HIPKillerTool') #FIXME missing functionality

    return result


def MonopolePreInclude(flags):
    if flags.Common.ProductionStep == ProductionStep.Simulation:
        flags.Sim.G4Stepper = 'ClassicalRK4'
        flags.Sim.G4EquationOfMotion = "G4mplEqMagElectricField" #Monopole Equation of Motion
        flags.Sim.TightMuonStepping = False
        simdict = flags.Input.SpecialConfiguration
        if "InteractingPDGCodes" not in simdict:
            simdict['InteractingPDGCodes'] = str([4110000,-4110000])


def MonopoleCfg(flags):
    result = ComponentAccumulator()
    simdict = flags.Input.SpecialConfiguration

    assert "MASS" in simdict
    assert "CHARGE" in simdict
    load_files_for_qball_scenario(simdict["MASS"], simdict["CHARGE"])

    if flags.Common.ProductionStep == ProductionStep.Simulation:
        result.merge(PhysicsListSvcCfg(flags))
        physicsOptions = [ result.popToolsAndMerge(MonopolePhysicsToolCfg(flags)) ]
        result.getService("PhysicsListSvc").PhysOption += physicsOptions
        # add monopole-specific configuration for looper killer
        #simFlags.OptionalUserActionList.addAction('G4UA::MonopoleLooperKillerTool') #FIXME missing functionality
        # add default HIP killer
        #simFlags.OptionalUserActionList.addAction('G4UA::HIPKillerTool') #FIXME missing functionality
    return result
