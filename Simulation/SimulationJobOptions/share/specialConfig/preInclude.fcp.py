#########################################################################
#       preInclude.fcp.py - Wendy Taylor 23 Jun 2011      #
#########################################################################

def load_files_for_fcp_scenario(MASS, CHARGE, X, Y):
    import os, shutil, sys

    CODE=int(20000000)+int(X)*1000+int(Y)*10
    print("Trying to load %d, %d for particle with code %d" % (X, Y, CODE))

    pdgLine1="M {code}                         {intmass}.E+03       +0.0E+00 -0.0E+00 fcp           +\n".format(code=CODE,intmass=int(MASS))
    pdgLine2="W {code}                         0.E+00         +0.0E+00 -0.0E+00 fcp           +\n".format(code=CODE)
    particleLine1="{code}  {intmass}.00  {fcharge}  0.0 # fcp\n".format(code=CODE,intmass=int(MASS), fcharge=float(CHARGE))
    particleLine2="-{code}  {intmass}.00  -{fcharge}  0.0 # fcpBar\n".format(code=CODE,intmass=int(MASS), fcharge=float(CHARGE))

    pdgmod = os.path.isfile('PDGTABLE.MeV')
    if pdgmod is True:
        os.remove('PDGTABLE.MeV')
    os.system('get_files -data PDGTABLE.MeV')
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
        from G4AtlasApps.SimFlags import simFlags
        if not "InteractingPDGCodes" in simFlags.specialConfiguration.get_Value():
            assert "CHARGE" in simFlags.specialConfiguration.get_Value()
            assert "X" in simFlags.specialConfiguration.get_Value()
            assert "Y" in simFlags.specialConfiguration.get_Value()
            CODE=int(20000000)+int(simFlags.specialConfiguration.get_Value()["X"])*1000+int(simFlags.specialConfiguration.get_Value()["Y"])*10
            simFlags.specialConfiguration.get_Value()['InteractingPDGCodes'] = str([CODE,-1*CODE])
        simdict = simFlags.specialConfiguration.get_Value()
except:
    from G4AtlasApps.SimFlags import simFlags
    simdict = simFlags.specialConfiguration.get_Value()

assert "MASS" in simdict
assert "CHARGE" in simdict
assert "X" in simdict
assert "Y" in simdict
load_files_for_fcp_scenario(simdict["MASS"], simdict["CHARGE"], simdict["X"], simdict["Y"])

if doG4SimConfig:
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.log.info("Unlocking simFlags.EquationOfMotion to reset the value for Monopole simulation.")
    from G4AtlasApps.SimFlags import simFlags
    simFlags.PhysicsOptions += ["MonopolePhysicsTool"]
    # add monopole-specific configuration for looper killer
    simFlags.OptionalUserActionList.addAction('G4UA::MonopoleLooperKillerTool')
    # add default HIP killer
    simFlags.OptionalUserActionList.addAction('G4UA::HIPKillerTool')

del doG4SimConfig, simdict
