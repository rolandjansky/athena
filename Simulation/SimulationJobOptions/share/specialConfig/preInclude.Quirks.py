
import os

import ExtraParticles.PDGHelpers

quirk_firststring=1e-6 #mm
quirk_maxboost=1e-2
quirk_maxmerge=1e-6 #mm
quirk_debugenabled=False
quirk_debugdist=1000 #mm
quirk_debugsteps=1000

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
        simdict = simFlags.specialConfiguration.get_Value()
except:
    from G4AtlasApps.SimFlags import simFlags
    simdict = simFlags.specialConfiguration.get_Value()

quirk_mass = float(simdict["MASS"])
quirk_charge = float(simdict["CHARGE"])
quirk_pdgid = int(simdict["PDGID"])
quirk_stringforce = float(simdict["STRINGFORCE"])

f = open('PDGTABLE.MeV', 'a')
f.write("M%8d                          %.8E +0.0E+00 -0.0E+00 Quirk               +\n" % (quirk_pdgid, quirk_mass))
f.write("W%8d                          0.E+00         +0.0E+00 -0.0E+00 Quirk               +\n" % quirk_pdgid)
f.close()

if doG4SimConfig:
    f = open('quirks_setup.txt', 'w')
    for x in [quirk_mass, quirk_charge, quirk_pdgid, quirk_stringforce, quirk_firststring, quirk_maxboost, quirk_maxmerge, quirk_maxmerge]:
        f.write(repr(x) + "\n")
    if quirk_debugenabled:
        f.write("1\n")
        f.write(repr(quirk_debugdist) + "\n")
        f.write(repr(quirk_debugsteps) + "\n")
    else:
        f.write("0\n")
    f.close()

    simFlags.PhysicsOptions += ["QuirksPhysicsTool"]
del quirk_firststring, quirk_maxboost, quirk_maxmerge, quirk_debugenabled, quirk_debugdist, quirk_debugsteps, doG4SimConfig, simdict, f, quirk_mass, quirk_charge, quirk_pdgid, quirk_stringforce
