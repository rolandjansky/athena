########################################################
# This file is to generate SM tZq events using LO calculations
# There are two processes: trilepton including tau and inclusive decay excluding all Had decays
# contact alhroob@cern.ch
########################################################
from MadGraphControl.MadGraphUtils import *
import fileinput
import shutil
import subprocess
import os
import sys
# General settings
minevents=5000
nevents=5500

# MG merging settings
maxjetflavor=4
ickkw=0

dyn_scale = '0'

### DSID lists (extensions can include systematics samples)
Nominal=[410550,410560]

DSID = runArgs.runNumber

if DSID == 410550:
#   the following to generate events with three leptons including tau
    mgproc="""generate p p > t b~ j Z $$ W+ W-, (t > w+ b, w+ > l+ vl) , Z > l+ l-\n"""
    mgproc +="""add process p p > t~ b j Z $$ W+ W- , (t~ > w- b~, w- > l- vl~) , Z > l+ l-"""
    name='tZq'
    keyword=['SM','singleTop','tZ','lepton']
elif DSID == 410560:
#   the following to generate events with at least one lepton
    mgproc="""generate  p p > t b~ j Z $$ W+ W- , (t > w+ b , w+ > l+ vl) , Z > all all\n"""
    mgproc +="""add process  p p > t b~ j Z $$ W+ W- , (t > w+ b , w+ > j j) , Z > l+ l-\n"""

    mgproc +="""add process  p p > t~ b j Z $$ W+ W- , (t~ > w- b~ , w- > l- vl~) , Z > all all\n"""
    mgproc +="""add process  p p > t~ b j Z $$ W+ W- , (t~ > w- b~ , w- > j j) , Z > l+ l-"""
    name='tZq'
    keyword=['SM','singleTop','tZ','lepton']
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

get_param_card = subprocess.Popen(['get_files','-data','aMcAtNlo_param_card_tt.dat'])
get_param_card.wait()

fcard = open('proc_card_mg5.dat','w')
mode=1
cluster_type='condor'
cluster_queue='None'
fcard.write("""

define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~

define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~

define q = u c d s b
define q~ = u~ c~ d~ s~ b~

define all = q q~ l- l+ vl vl~

"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

pdflabel="lhapdf"
lhaid=262000
#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '3.0',
           'cut_decays'   : 'F',
           'pdlabel'      : "'"+pdflabel+"'",
           'lhaid'        : lhaid,
           'maxjetflavor' : maxjetflavor,
           'asrwgtflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 1,
           'ptb'          : 0,
           'mmjj'         : 0,
           'drjj'         : 0.4,
           'drll'         : 0.4,
           'drjl'         : 0.4,
           'ptl'          : 0,
           'etal'         : -1,
           'etab'         : -1,
           'etaj'         : 5,
           'use_syst' :'T',
           'sys_scalefact' :'1 0.5 2',
           'sys_alpsfact' : 'None',
           'sys_matchscale' : 'None',
           'sys_pdf' : 'None',
           'dynamical_scale_choice' : dyn_scale
 }


scalefact = 1.0
#if runArgs.runNumber in systScalefactUp:
#    name=name+'_facsc2_rensc2'
#    scalefact = 2.0
#elif runArgs.runNumber in systScalefactDn:
#    name=name+'_facsc0p5_rensc0p5'
#    scalefact = 0.5


process_dir = new_process()

# Cook the setscales file for the user defined dynamical scale
fileN = process_dir+'/SubProcesses/setscales.f'
mark  = '      elseif(dynamical_scale_choice.eq.0) then'
rmLines = ['ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc',
           'cc      USER-DEFINED SCALE: ENTER YOUR CODE HERE                                 cc',
           'cc      to use this code you must set                                            cc',
           'cc                 dynamical_scale_choice = 0                                    cc',
           'cc      in the run_card (run_card.dat)                                           cc',
           'write(*,*) "User-defined scale not set"',
           'stop 1',
           'temp_scale_id=\'User-defined dynamical scale\' ! use a meaningful string',
           'tmp = 0',
           'cc      USER-DEFINED SCALE: END OF USER CODE                                     cc',
	   'stop 21',
           'rscale = 0'
           ]

flag=0
for line in fileinput.input(fileN, inplace=1):
    toKeep = True
    for rmLine in rmLines:
        if line.find(rmLine) >= 0:
           toKeep = False
           break
    if toKeep:
        print line,
    if line.startswith(mark) and flag==0:
        flag +=1
        print """
c         Q^2= 4 * sqrt(mb^2 + pt(b)^2), b quark is the one coming from the gluon splitting. The b quark is always the last in the list, so for tZq at LO(only) and when the decay is done on-the-fly,
c         it is always the 6th particle in the list, otherwise we have to change the number or use a loop to find the last particle in the list
          rscale=0d0             !factorization scale**2 for pdf1
                                 !factorization scale**2 for pdf2
          xmtc=dot(P(0,6),P(0,6))
          rscale = 4d0 * sqrt(pt(P(0,6))**2+xmtc)

cc          An example how to find the last particle in the list
c          do i=3,nexternal
c          xmtc=dot(P(0,i),P(0,i))
c          if ( xmtc < 30 .and. xmtc > 10 ) then
c               rscale = 4d0 * sqrt(pt(P(0,i))**2+xmtc)
c          endif
c          enddo
         			 """

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras,scalefact=scalefact)

print_cards()

generate(run_card_loc='run_card.dat',
param_card_loc= 'aMcAtNlo_param_card_tt.dat',
mode=mode,
cluster_type=cluster_type,
cluster_queue=cluster_queue,
proc_dir=process_dir)

outputDS=arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')


## Shower
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
