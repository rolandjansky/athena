#################################################################
#	preInclude.AMSB.py - Chiara Debenedetti, 3 Jun 2011	#
#################################################################


def get_and_fix_PDGTABLE(replace):
    import os, shutil

    # Download generic PDGTABLE (overwrite existing one if it exists)
    os.system('get_files -remove -data PDGTABLE.MeV')
    shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
    # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

    update = open('PDGTABLE.MeV', 'w')
    for l in open('PDGTABLE.MeV.org'):

        for r in replace:
            if l.find(r[1]) > -1:
                ll = l.split()
            
                if ll[0] == r[0] and ll[1] == r[1]:
                    l = l[0:35] + ('%11.5E' % r[2]).strip().ljust(14) + l[49:]
                    continue
        
        update.write(l)
    update.close()


def load_files_for_AMSB_scenario(simdict):
    C1Mass = eval(simdict["AMSBC1Mass"])
    N1Mass = eval(simdict["AMSBN1Mass"])
    # patching PDGTABLE
    get_and_fix_PDGTABLE([('M', '1000022', N1Mass), ('M', '1000024', C1Mass)])


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

load_files_for_AMSB_scenario(simdict)

if doG4SimConfig:
    from G4AtlasApps.SimFlags import simFlags
    def amsb_processlist():
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng.gbl.G4Commands().process.list()
	
    simFlags.InitFunctions.add_function("postInit", amsb_processlist)

    def amsb_setparams():
        from G4AtlasApps import AtlasG4Eng
        from GaudiKernel.SystemOfUnits import GeV, ns
        C1Mass = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["AMSBC1Mass"])
        N1Mass = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["AMSBN1Mass"])
        C1Lifetime = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["AMSBC1Lifetime"])


        AtlasG4Eng.G4Eng._ctrl.load("Charginos")
        charginoPlus = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_plus_1")
        charginoPlus.SetParticleMass(C1Mass)
        if C1Lifetime == -1:
            charginoPlus.Stable(True)
        else:
            charginoPlus.SetParticleLifeTime(C1Lifetime)
            charginoPlus.Stable(False)
            charginoPlus.AddDecayChannel("s_chi_plus_1", 1., "s_chi_0_1=pi+")

        charginoMinus = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_minus_1")
        charginoMinus.SetParticleMass(C1Mass)
        if C1Lifetime == -1:
            charginoMinus.Stable(True)
        else:
            charginoMinus.SetParticleLifeTime(C1Lifetime)
            charginoMinus.Stable(False)
            charginoMinus.AddDecayChannel("s_chi_minus_1", 1., "s_chi_0_1=pi-")

        neutralino = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_0_1")
        neutralino.SetParticleMass(N1Mass)
        neutralino.Stable(True)


    simFlags.InitFunctions.add_function("preInitPhysics", amsb_setparams)

    def amsb_applycalomctruthstrategy():
## Applying the MCTruth strategies: add decays in the Calorimeter
        from G4AtlasApps import AtlasG4Eng
        myDecay = AtlasG4Eng.G4Eng.Dict_MCTruthStrg.get('Decay')
        myDecay.add_Volumes('CALO::CALO', 1)

    simFlags.InitFunctions.add_function("postInit", amsb_applycalomctruthstrategy)

del doG4SimConfig, simdict
