from MadGraphControl.MadGraphUtils import *
import fileinput
import shutil
import subprocess
import os

# General settings
nevents=10000
runName='schan'
runMode=2 # 0 = single machine, 1 = cluster, 2 = multicore
cluster_queue='8nh' #8nm 1nh 8nh 1nd 2nd 1nw 2nw (m = minute, d = day, w = week)
required_accuracy=0.001 #0.001

### PROCESS
mgproc="""define l+ = l+ ta+
define l- = l- ta-
generate p p > w+ > t b~ [QCD]
add process p p > w- > t~ b [QCD]"""
name='s-channel'
process="p p > t b"

##necessary gridpack settings
gridpack_mode=True
gridpack_dir='madevent/'

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(runName)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm-no_b_mass
"""+mgproc+"""
output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


# --------------------------------------------------------------
#  Start building the cards
# --------------------------------------------------------------

process_dir = new_process(grid_pack=gridpack_dir)

#run_card.dat and set parameters
#run_cardloc=process_dir+'/Cards/run_card.dat'
run_card_extras = {
    'pdlabel'                  : 'lhapdf',
	'lhaid'                    : 260000,
	'reweight_scale'           : False,
	'parton_shower'            :'HERWIGPP',
	'jetalgo'                  : '-1',
	'jetradius'               : '0.4',
	'ptj'                      : '0.1',
	'fixed_ren_scale'           : 'True',
  'fixed_fac_scale'           : 'True',
  'muR_ref_fixed'             : '172.5',
  'muF_ref_fixed'             : '172.5',
  'bwcutoff'                  : '50'}

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),
  run_card_new='run_card.dat',
	nevts=nevents,
  rand_seed=runArgs.randomSeed,
  beamEnergy=beamEnergy,
  xqcut=0.,
	extras=run_card_extras)

#param_card.dat
param_cardloc=process_dir+'/Cards/param_card.dat'

param_decay={
    '6': 'DECAY  6 0 # MT'}
#    '24' : 'DECAY  24 Auto # MW'}

build_param_card(param_card_old=param_cardloc,
  param_card_new='param_card.dat',
  masses={},
  decays={},
  extras={},
  params={})

# param card ATLAS recommended decay width (must be done like this)
paramNameDestination = 'param_card.dat'
mark  = '## INFORMATION FOR DECAY'
toKeep = True
for line in fileinput.input(paramNameDestination, inplace=1):
    if line.startswith(mark):
        toKeep = False
    if toKeep:
        print line,

fParamCard = open('param_card.dat','a') #append
fParamCard.write('''
#*************************
#      Decay widths      *
#*************************
#
#      PDG        Width
DECAY  1   0.000000e+00
#
#      PDG        Width
DECAY  2   0.000000e+00
#
#      PDG        Width
DECAY  3   0.000000e+00
#
#      PDG        Width
DECAY  4   0.000000e+00
#
#      PDG        Width
DECAY  5   0.000000e+00
#
#      PDG        Width
DECAY  6   1.320000e+00
#  BR             NDA  ID1    ID2   ...
   1.000000e+00   2    5  24 # 1.32
DECAY -6   1.320000e+00
#  BR             NDA  ID1    ID2   ...
   1.000000e+00   2   -5 -24 # 1.32
#
#      PDG        Width
DECAY  11   0.000000e+00
#
#      PDG        Width
DECAY  12   0.000000e+00
#
#      PDG        Width
DECAY  13   0.000000e+00
#
#      PDG        Width
DECAY  14   0.000000e+00
#
#      PDG        Width
DECAY  15   0.000000e+00
#
#      PDG        Width
DECAY  16   0.000000e+00
#
#      PDG        Width
DECAY  21   0.000000e+00
#
#      PDG        Width
DECAY  22   0.000000e+00
#
#      PDG        Width
DECAY  23   2.495200e+00
#
#      PDG        Width
DECAY  -24   2.085000e+00
#  BR             NDA  ID1    ID2   ...
      3.3370000e-01   2    1  -2
      3.337000e-01   2    3  -4
      1.082000e-01   2   11 -12
      1.082000e-01   2   13 -14
      1.082000e-01   2   15 -16
DECAY  24   2.085000e+00
#  BR             NDA  ID1    ID2   ...
      3.337000e-01   2   -1   2
      3.337000e-01   2   -3   4
      1.082000e-01   2  -11  12
      1.082000e-01   2  -13  14
      1.082000e-01   2  -15  16
#
#
#      PDG        Width
DECAY  25   6.382339e-03
#
#      PDG        Width
DECAY  82   0.000000e+00
''')
fParamCard.close()


#madspin card
madspin_card_loc='madspin_card.dat'

if not hasattr(runArgs, 'inputGenConfFile'):
    fMadSpinCard = open('madspin_card.dat','w')
    fMadSpinCard.write('import Events/'+runName+'/events.lhe.gz\n')
    fMadSpinCard.write('set ms_dir MadSpin\n')
else:
    os.unlink(gridpack_dir+'Cards/madspin_card.dat')
    fMadSpinCard = open(gridpack_dir+'Cards/madspin_card.dat','w')
    fMadSpinCard.write('import '+gridpack_dir+'Events/'+runName+'/events.lhe.gz\n')
    fMadSpinCard.write('set ms_dir '+gridpack_dir+'MadSpin\n')
    fMadSpinCard.write('set seed '+str(10000000+int(runArgs.randomSeed))+'\n')
fMadSpinCard.write('''set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event (default: 400)
decay t > w+ b, w+ > l+ vl
decay t~ > w- b~, w- > l- vl~
launch''')
fMadSpinCard.close()

print_cards() #prints the cards in the output (terminal)
cluster_size=100

#generate events with MG+MS
generate(run_card_loc='run_card.dat',
  param_card_loc='param_card.dat',
  mode=runMode,
  njobs=8,
  proc_dir=process_dir,
  run_name=runName,
  madspin_card_loc=madspin_card_loc,
  required_accuracy=required_accuracy,
  grid_pack=gridpack_mode,
  gridpack_dir=gridpack_dir,
  nevents=nevents,
  random_seed=runArgs.randomSeed,
  cluster_type="lsf",
  cluster_queue=cluster_queue)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(runName)

outputDS=arrange_output(run_name=runName,
    proc_dir=process_dir,
    outputDS=stringy+'._00001.events.tar.gz',
    lhe_version=3,
    saveProcDir=True)

runArgs.inputGeneratorFile=outputDS
