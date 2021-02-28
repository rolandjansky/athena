#################################################################
#       preInclude.SLeptonsLLP.py - Emma Kuwertz, Dec 2017                #
#################################################################

def get_and_fix_PDGTABLE(replace):
    import os, shutil, re, sys

    # Download generic PDGTABLE (do not overwrite existing one if it exists, use existing one instead) 
    import ExtraParticles.PDGHelpers
    shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
    # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

    lines = open('PDGTABLE.MeV.org').readlines()
    for pdgid,mass,name,charge in replace:
        if not re.search(r'[MW]\s+'+str(pdgid)+'\s+\S+', ''.join(lines)):
            lines.append('M' + str(pdgid).rjust(8) +''.ljust(26) +
                         ('%11.5E' % mass).ljust(15) +
                         '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip()+''.rjust(20-len(name.strip())) + '\n')
            lines.append('W' + str(pdgid).rjust(8) +''.ljust(26) +
                         '0.E+00'.ljust(15) + '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip()+''.rjust(20-len(name.strip())) + '\n')
        else:
            for i in xrange(len(lines)):
                if re.search(r'M\s+'+str(pdgid)+'\s+\S+', lines[i]):
                    l = lines[i]
                    lines[i] = l[0:35] + ('%11.5E' % mass).ljust(14) + l[49:]

    update = open('PDGTABLE.MeV', 'w')
    update.write(''.join(lines))
    update.close()

    print 'modfied PDGTABLE\n%s\n' % ''.join(lines)
    sys.stdout.flush()

def load_files_for_sleptonLLP_scenario(simdict):

    if "GMSBSlepton" in simdict:
        get_and_fix_PDGTABLE([
                (2000011, eval(simdict.get("GMSBSlepton",'0')), '~e(R)', '-'),
                (2000013, eval(simdict.get("GMSBSlepton",'0')), '~mu(R)', '-'),            
                (1000011, eval(simdict.get("GMSBSlepton",'0')), '~e(L)', '-'),
                (1000013, eval(simdict.get("GMSBSlepton",'0')), '~mu(L)', '-'),
                ])
    if "GMSBStau" in simdict:
        get_and_fix_PDGTABLE([
                (2000015, eval(simdict.get("GMSBStau",'0')), '~tau(R)', '-'),
                (1000015, eval(simdict.get("GMSBStau",'0')), '~tau(L)', '-'),
                ])
    if "GMSBGravitino" in simdict:
        get_and_fix_PDGTABLE([
                (1000039, eval(simdict.get("GMSBGravitino",'0')), '~G', '0'),
                ])

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

load_files_for_sleptonLLP_scenario(simdict)

if doG4SimConfig:
    simFlags.PhysicsOptions += ["GauginosPhysicsTool","AllSleptonsPhysicsTool"]
    # Slepton decays from SleptonsConfig
    if "GMSBSlepton" in simdict:
        simFlags.PhysicsOptions += ["SElectronRPlusToElectronGravitino","SElectronLPlusToElectronGravitino"]
        simFlags.PhysicsOptions += ["SElectronRMinusToElectronGravitino","SElectronLMinusToElectronGravitino"]
        simFlags.PhysicsOptions += ["SMuonRPlusToMuonGravitino","SMuonLPlusToMuonGravitino"]
        simFlags.PhysicsOptions += ["SMuonRMinusToMuonGravitino","SMuonLMinusToMuonGravitino"]
    if "GMSBStau" in simdict:
        simFlags.PhysicsOptions += ["STauRPlusToTauGravitino","STauLPlusToTauGravitino"]
        simFlags.PhysicsOptions += ["STauRMinusToTauGravitino","STauLMinusToTauGravitino"]

del doG4SimConfig, simdict
