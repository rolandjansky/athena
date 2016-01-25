#########################################################################
#       preInclude.Monopole.py - Chiara Debenedetti, 3 Jun 2011      #
#########################################################################

def load_files_for_monopole_scenario(MASS, GCHARGE):
    import os, shutil, sys

    ALINE1="M 4110000                         {intmass}.E+03       +0.0E+00 -0.0E+00 Monopole         0".format(intmass=int(MASS))
    ALINE2="W 4110000                          0.E+00         +0.0E+00 -0.0E+00 Monopole         0"
    BLINE1="4110000 {intmass}.00 0.0 {gcharge} # Monopole".format(intmass=int(MASS), gcharge=GCHARGE)
    BLINE2="-4110000 {intmass}.00 0.0 -{gcharge} # MonopoleBar".format(intmass=int(MASS), gcharge=GCHARGE)

    pdgmod = os.path.isfile('PDGTABLE.MeV')
    if pdgmod is True:
        os.remove('PDGTABLE.MeV')
    os.system('get_files -data PDGTABLE.MeV')
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

doG4SimConfig = True
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as af
try:
    f = af.fopen(athenaCommonFlags.FilesInput()[0])

    if "StreamHITS" in f.infos["stream_names"]:
        from Digitization.DigitizationFlags import digitizationFlags
        simdict = digitizationFlags.specialConfiguration.get_Value()
        doG4SimConfig = False
    else:
        from G4AtlasApps import AtlasG4Eng
        simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
except:
    from G4AtlasApps import AtlasG4Eng
    simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
    
assert "MASS" in simdict
assert "GCHARGE" in simdict
load_files_for_monopole_scenario(simdict["MASS"], simdict["GCHARGE"])

if doG4SimConfig:
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.log.info("Unlocking simFlags.EquationOfMotion to reset the value for Monopole simulation.")
    from G4AtlasApps.SimFlags import simFlags
    # FIXME ideally would include this file early enough, so that the unlocking is not required
    simFlags.EquationOfMotion.unlock() 
    simFlags.EquationOfMotion.set_On()
    simFlags.EquationOfMotion.set_Value_and_Lock("MonopoleEquationOfMotion")

    def monopole_setupg4():
        from G4AtlasApps import PyG4Atlas, AtlasG4Eng
        AtlasG4Eng.G4Eng.load_Lib("Monopole")

    simFlags.InitFunctions.add_function("preInitPhysics", monopole_setupg4)

    def setup_stepper_classicalrk4():
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng._ctrl.fldMenu.UseStepper('ClassicalRK4')
        AtlasG4Eng.G4Eng._ctrl.fldMenu.UseStepper('IDET::IDET', 'ClassicalRK4')

    simFlags.InitFunctions.add_function("postInit", setup_stepper_classicalrk4)

    # configure the looper killer
    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    from AthenaCommon.CfgGetter import getPublicToolClone
    # use specific configuration
    lkAction = getPublicToolClone("LooperKillerMonopole", "LooperKiller", PrintSteps=2, MaxSteps=2000000, VerboseLevel=0)
    UAStore.addAction(lkAction,['Step'])
    # add HIP killer
    UAStore.addAction('HIPKiller',['Step'])



del doG4SimConfig, simdict
