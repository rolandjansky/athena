import re
import os
import subprocess
import math
from MadGraphControl.MadGraphUtils import *

nevents = 10000
mode = 0

lambdalq = 0.3
beta1 = 0.5
beta2 = 0.5
beta3 = 0.0

JOname = runArgs.jobConfig[0]

generation = 0
if "LQ1" in JOname:
    generation = 1
elif "LQ2" in JOname:
    generation = 2
else:
    raise RuntimeError("Cannot determine LQ generation.")

matches = re.search("M([0-9]+).*\.py", JOname)
if matches is None:
    raise RuntimeError("Cannot find mass string.")
else:
    lqmass = float(matches.group(1))

matches = re.search("l([0-9]_[0-9]+)\.py", JOname)
if matches is None:
    raise RuntimeError("Cannot find coupling string.")
else:
    coupling = float(matches.group(1).replace("_", "."))

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
set group_subprocesses Auto
set ignore_six_quark_processes False
set loop_color_flows False
set gauge unitary
set complex_mass_scheme False
set max_npoint_for_channel 0
import model sm
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define p = p b b~
define j = j b b~\n""")

if generation == 1:
    fcard.write("""import model sleptoquark_ue_UFO\n""")
    fcard.write("""generate p p  > e+ e- u / z h a
output -f""")
    fcard.close()

elif generation == 2:
    fcard.write("""import model sleptoquark_umu_UFO\n""")
    fcard.write("""generate p p > mu+ mu- u / z h a
output -f""")
    fcard.close()

else:
    raise RuntimeError("generation indicator %i not recognised in these jobOptions." % generation)

beamEnergy = -999
if hasattr(runArgs, 'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()
extras = {'pdlabel': "'lhapdf'",
          'ktdurham': '1.0'}

try:
    os.remove('run_card.dat')
except OSError:
    pass

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat',
               nevts=nevents, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, extras=extras)

if os.path.exists("param_card.dat"):
    os.remove("param_card.dat")


#oldcard = open("MadGraph_param_card_SingleLQLight.dat", 'r')
param_card_name = 'MadGraph_param_card_SingleLQLight.dat'
param_card = subprocess.Popen(['get_files', '-data', param_card_name])
param_card.wait()
if not os.access(param_card_name, os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(param_card_name, 'r')
    newcard = open('param_card.dat', 'w')
    width = (lqmass*abs(coupling)**2)/(16.*math.pi)

    for line in oldcard:
        # if 'mu_r' in line:
        #     newcard.write('    1 %e # mu_r\n'%(lqmass))
        if '# Mlq' in line:
            newcard.write('  1104 %e # Mlq \n' % (lqmass))
        elif '# sdl : Mlq' in line:
            newcard.write('  1101 %e # sdl  : Mlq\n' % (lqmass))
        elif '# sul : Mlq' in line:
            newcard.write('  1102 %e # sul  : Mlq\n' % (lqmass))
        elif '# sdn : Mlq' in line:
            newcard.write('  1103 %e # sdn  : Mlq\n' % (lqmass))
        elif 'sdl : Wlq' in line:
            newcard.write('DECAY  1101 Auto # sdl : Wlq\n')
        elif 'sul : Wlq' in line:
            newcard.write('DECAY  1102 Auto # sul : Wlq\n')
        elif 'sdn : Wlq' in line:
            newcard.write('DECAY  1103 Auto # sdn : Wlq\n')
        elif 'sun : Wlq' in line:
            newcard.write('DECAY  1104 Auto # sun : Wlq\n')
        # # elif '# sdl : (Mlq*abs(Ylq)**2)/(16.*cmath.pi)' in line:
        # #     newcard.write('DECAY    1101 %e # sdl  :  (Mlq*abs(Ylq)**2)/(16.*cmath.pi)\n' % (width))
        # # elif '# sul : (Mlq*abs(Ylq)**2)/(16.*cmath.pi)' in line:
        # #     newcard.write('DECAY    1102 %e # sul  :  (Mlq*abs(Ylq)**2)/(16.*cmath.pi)\n' % (width))
        # # elif '# sdn : (Mlq*abs(Ylq)**2)/(16.*cmath.pi)' in line:
        # #     newcard.write('DECAY    1103 %e # sdn  :  (Mlq*abs(Ylq)**2)/(16.*cmath.pi)\n' % (width))
        # # elif '# sun : (Mlq*abs(Ylq)**2)/(16.*cmath.pi)' in line:
        # #     newcard.write('DECAY    1104 %e # sun  :  (Mlq*abs(Ylq)**2)/(16.*cmath.pi)\n' % (width))
        elif '# Ylq' in line:
            newcard.write('  1 %e # Ylq \n' % (coupling))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

print_cards()

runName = 'run_01'
process_dir = new_process()
generate(run_card_loc='run_card.dat',
         param_card_loc='param_card.dat',
         mode=mode,
         proc_dir=process_dir,
         run_name=runName)

arrange_output(run_name=runName, proc_dir=process_dir, outputDS=runName + '._00001.events.tar.gz', lhe_version=3,
               saveProcDir=True)

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

evgenConfig.description = 'Single production of scalar leptoquarks, generation: {0:d}, mLQ={1:d}'.format(int(generation),
                                                                                                         int(lqmass))
evgenConfig.keywords += ['BSM', 'exotic', 'leptoquark', 'scalar']
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> llj'
evgenConfig.contact = ["Marcus Matthias Morgenstern <marcus.matthias.morgenstern@cern.ch>"]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile = runName + '._00001.events.tar.gz'
