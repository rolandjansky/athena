def get_and_fix_PDGTABLE(replace):
    import os, shutil, re

    # Download generic PDGTABLE (overwrite existing one if it exists)
    os.system('get_files -remove -data PDGTABLE.MeV')
    shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
    # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

    lines = open('PDGTABLE.MeV.org').readlines()
    for pdgid,mass,name,charge in replace:
        if not re.search(r'[MW]\s+'+str(pdgid)+'\s+\S+', ''.join(lines)):
            lines.append('M' + str(pdgid).rjust(8) +''.ljust(26) +
                         ('%11.5E' % mass).ljust(15) +
                         '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip() + '\n')
            lines.append('W' + str(pdgid).rjust(8) +''.ljust(26) +
                         '0.E+00'.ljust(15) + '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip() + '\n')
        else:
            for i in xrange(len(lines)):
                if re.search(r'M\s+'+str(pdgid)+'\s+\S+', lines[i]):
                    l = lines[i]
                    lines[i] = l[0:35] + ('%11.5E' % mass).ljust(14) + l[49:]

    update = open('PDGTABLE.MeV', 'w')
    update.write(''.join(lines))
    update.close()


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

C1Mass = eval(simdict["AMSBC1Mass"])
N1Mass = eval(simdict["AMSBN1Mass"])
# patching PDGTABLE
get_and_fix_PDGTABLE([(1000022, N1Mass, '~chi(0,1)', '0'), (1000024, C1Mass, '~chi(+,1)', '+')])

if doG4SimConfig:
    from G4AtlasApps.SimFlags import simFlags
    simFlags.PhysicsOptions += ['CharginosPhysicsTool']
    # Add Chargino decays if necessary
    C1Lifetime = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1Lifetime", "-1.0"))
    if C1Lifetime > 0.0:
        simFlags.PhysicsOptions += ['CharginoPlusToPiPlusNeutralino']
        simFlags.PhysicsOptions += ['CharginoMinusToPiMinusNeutralino']
        C1ToEleBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToEleBR", "-1.0"))
        if C1ToEleBR > 0.0:
            simFlags.PhysicsOptions += ['CharginoPlusToEPlusNeutralino']
            simFlags.PhysicsOptions += ['CharginoMinusToEMinusNeutralino']
        C1ToMuBR = eval(simFlags.specialConfiguration.get_Value().get("AMSBC1ToMuBR", "-1.0"))
        if C1ToMuBR > 0.0:
            simFlags.PhysicsOptions += ['CharginoPlusToMuPlusNeutralino']
            simFlags.PhysicsOptions += ['CharginoMinusToMuMinusNeutralino']

del doG4SimConfig, simdict
