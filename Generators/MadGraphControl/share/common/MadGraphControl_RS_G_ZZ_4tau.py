from MadGraphControl.MadGraphUtils import *

c   = 1.0
mh  = 125
mhh = 1000


mhh = runArgs.jobConfig[0].split('_M')[1].split('.')[0]
c = float( runArgs.jobConfig[0].split('tau_c')[1].split('_')[0] ) * 0.1

# Change defaults for run_card.dat
extras = { 'lhe_version':'2.0',
           'cut_decays':'F',
           'pdlabel':"'nn23lo1'",
           'lhaid':'247000',
           'scale':'91.18',
           'dsqrt_q2fact1':'91.18',
           'dsqrt_q2fact2':'91.18',      
           'parton_shower':'PYTHIA8',
           'ptj':'0',
           'ptb':'0',
           'pta':'0',
           'ptjmax':'-1',
           'ptbmax':'-1',
           'ptamax':'-1',
           'etaj':'-1',
           'etab':'-1',
           'etaa':'-1',
           'etajmin':'0',
           'etabmin':'0',
           'etaamin':'0',
           'mmaa':'0',
           'mmaamax':'-1',
           'mmbb':'0',
           'mmbbmax':'-1',
           'drjj':'0',
           'drbb':'0',
           'draa':'0',
           'drbj':'0',
           'draj':'0',
           'drab':'0',
           'drjjmax':'-1',
           'drbbmax':'-1',
           'draamax':'-1',
           'drbjmax':'-1',
           'drajmax':'-1',
           'drabmax':'-1'}


name = 'SMRS_c'+str(int(10*c))+'_G_ZZ_4tau_m'+str(mhh)

if not hasattr(runArgs,'runNumber'): 
  raise RunTimeError("No run number found.")

#--------------------------------------------------------------
# MG5 Proc card
#--------------------------------------------------------------
fcard0 = open('proc_card_mg5_step1.dat','w')
fcard0.write("""
set group_subprocesses Auto
set ignore_six_quark_processes False
set gauge unitary
set complex_mass_scheme False
import model SMRS_Decay
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~

#Calculate widths first:
generate hh > all all
output SMRS_widths
launch
"""
)
fcard0.write('set c %f \n'%(c))
fcard0.write('set Mh %f \n'%(mh))
fcard0.write('set Mhh %s \n'%(mhh))
fcard0.write('set Whh Auto')
fcard0.close()

fcard1 = open('proc_card_mg5_step2.dat','w')
fcard1.write("""
set group_subprocesses Auto
set ignore_six_quark_processes False
set gauge unitary
set complex_mass_scheme False
import model SMRS_Decay
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~

generate p p > hh, (hh > Z Z)
output -f
""")
fcard1.close()

beamEnergy=6500
if hasattr(runArgs,'ecmEnergy'): 
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  raise RunTimeError("No center of mass energy found.")


process_dir = new_process(card_loc='proc_card_mg5_step1.dat')

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat', xqcut=0,
               nevts=runArgs.maxEvents*90, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy,
               extras=extras)

print_cards()

runName='run_01'

# first process to compute widths and set parameters card
generate(run_card_loc='run_card.dat', param_card_loc=None, mode=0, njobs=1, 
         run_name='run_01', proc_dir=process_dir)

# second process to generate hh -> 4tau events using parameters from first process
process_dir = new_process(card_loc='proc_card_mg5_step2.dat')
generate(run_card_loc='run_card.dat', param_card_loc='SMRS_widths/Cards/param_card.dat',
         mode=0, njobs=1, run_name=runName, proc_dir=process_dir)

stringy = 'madgraph.'+str(runArgs.runNumber)+'_'+str(name)

arrange_output(run_name=runName, proc_dir=process_dir, outputDS=stringy+'._00001.events.tar.gz')


#--------------------------------------------------------------
# Pythia8 showering with A14_NNPDF23LO
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#---------------------------------------------------------------------------------------------------
# Generator Filters
#---------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------------

# Decaying hh to 4tau with Pythia8

#---------------------------------------------------------------------------------------------------

genSeq.Pythia8.Commands += ["23:onMode=off", 
                            "23:onIfAny=15" ]


#--------------------------------------------------------------
# MC15 metadata
#--------------------------------------------------------------
evgenConfig.contact = ["Edson.Carquin@cern.ch"]
evgenConfig.description = "Bulk Randall-Sundrum model KK graviton -> ZZ -> 4tau with NNPDF2.3 LO A14 tune"
evgenConfig.inputfilecheck = stringy
evgenConfig.keywords = ["exotic", "RandallSundrum", "warpedED", "graviton", "Z","BSM"]
evgenConfig.process = "RS_G_ZZ_4tau"
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

