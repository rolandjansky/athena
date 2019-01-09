import re
import os
import math
import shutil
import subprocess

from MadGraphControl.MadGraphUtils import *

nevents = 10000
mode = 0


job_option_name = runArgs.jobConfig[0]

generation = 0
quark_flavour = job_option_name.split('S1T')[-1][0]
quark_flavours = ['d', 's', 'b']
quark_flavour_index = quark_flavours.index(quark_flavour) + 1
if quark_flavour not in quark_flavours:
    raise RuntimeError("Cannot determine quark flavour from job option name: {:s}.".format(job_option_name))
if "LQe" in job_option_name:
    generation = 1
elif "LQmu" in job_option_name:
    generation = 2
elif "LQtau" in job_option_name:
    generation = 3
else:
    raise RuntimeError("Cannot determine LQ generation from job option name: {:s}.".format(job_option_name))

matches = re.search("M([0-9]+).*\.py", job_option_name)
if matches is None:
    raise RuntimeError("Cannot find mass string in job option name: {:s}.".format(job_option_name))
else:
    lqmass = float(matches.group(1))

matches = re.search("l([0-9]_[0-9]+)\.py", job_option_name)
if matches is None:
    raise RuntimeError("Cannot find coupling string in job option name: {:s}.".format(job_option_name))
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
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~\n""")

if generation == 1:
    fcard.write("""import model ./LO_LQ_S1~\n""")
    fcard.write("""generate p p > e+ e- {:s} \n""".format(quark_flavour))
    fcard.write("""output -f\n""")
    fcard.close()

elif generation == 2:
    fcard.write("""import model ./LO_LQ_S1~\n""")
    fcard.write("""generate p p > mu+ mu- {:s} \n""".format(quark_flavour))
    fcard.write("""output -f\n""")
    fcard.close()

elif generation == 3:
    fcard.write("""import model ./LO_LQ_S1~\n""")
    fcard.write("""define p = g u c d s b u~ c~ d~ s~ b~\n""")
    fcard.write("""generate p p > ta+ ta- {:s} \n""".format(quark_flavour))
    fcard.write("""output -f""")
    fcard.close()

beamEnergy = -999
if hasattr(runArgs, 'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()
extras = {'pdlabel': "'lhapdf'",
	  'lhaid': 260000,
          'ktdurham': lqmass * 0.25}

try:
    os.remove('run_card.dat')
except OSError:
    pass

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat',
               nevts=nevents, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, extras=extras)

if os.path.exists("param_card.dat"):
    os.remove("param_card.dat")


param_card_name = 'MadGraph_param_card_TBS1Tilde_LO.dat'
#param_card = subprocess.Popen(['get_files', '-data', param_card_name])
#param_card.wait()
#try:
#    shutil.copy(os.path.join("/afs/cern.ch/user/m/morgens/afs_work/devarea/rel21/Multilepton/source/ELMultiLep/macros/misc/LQJO", param_card_name),
#                param_card_name)
#except:
#    try:
#        shutil.copy(os.path.join( "/user/mmorgens/devarea/rel21/Multilepton/source/ELMultiLep/macros/misc/LQJO",
#                                  param_card_name),
#                    param_card_name)
#    except:
#        exit(0)

if not os.access(param_card_name, os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(param_card_name, 'r')

    newcard = open('param_card.dat', 'w')
    width = (lqmass*abs(coupling)**2)/(16.*math.pi)
    # for line in oldcard:
    #     newcard.write(line)

    for line in oldcard:
        if '# yRR{:d}x{:d}'.format(quark_flavour_index, generation) in line:
            newcard.write('    {:d}  {:d}  {:e} # yRR{:d}x{:d}\n'.format(quark_flavour_index,
                                                                         generation,
                                                                         coupling,
                                                                         quark_flavour_index,
                                                                         generation))
        elif '# yRR' in line:
            indices = map(int, re.findall('\dx\d', line)[0].split('x'))
            newcard.write('    {:d}  {:d}  0.000000e+00 # yRR{:d}x{:d}\n'.format(indices[0],
                                                                                 indices[1],
                                                                                 indices[0],
                                                                                 indices[1]))
        elif '# MS1t' in line:
            newcard.write('  1 {:e} # MS1t \n'.format(lqmass))
        elif '# S1tm43 : MS1t' in line:
            newcard.write('  9000005 {:e} # S1tm43 : MS1t  \n'.format(lqmass))
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

evgenConfig.description = 'Toolbox scalar LO single production of S1~, generation: {0:d}, mLQ={1:d}'.format(
    int(generation), int(lqmass))
evgenConfig.keywords += ['BSM', 'exotic', 'leptoquark', 'scalar']
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> S1~ l'
evgenConfig.contact = ["Marcus Matthias Morgenstern <marcus.matthias.morgenstern@cern.ch>"]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile = runName + '._00001.events.tar.gz'
