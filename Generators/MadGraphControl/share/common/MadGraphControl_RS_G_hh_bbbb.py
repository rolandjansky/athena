from MadGraphControl.MadGraphUtils import *

c   = 1.0
mh  = 125
mhh = 1000

if ( runArgs.runNumber>=301486 and runArgs.runNumber<=301487 ):
    c = 0.5
elif ( runArgs.runNumber>=301488 and runArgs.runNumber<=301507 ):
    c = 1.0
elif ( runArgs.runNumber>=301508 and runArgs.runNumber<=301527 ):
    c = 2.0
elif ( runArgs.runNumber>=309038 and runArgs.runNumber<= 309041 ):
    c = 1.0
elif ( runArgs.runNumber>=309042 and runArgs.runNumber<= 309043 ):
    c = 2.0
else:
    print 'ERROR - no c parameter specified'

if ( runArgs.runNumber==301488 or runArgs.runNumber==301508 ):
    mhh = '300'
elif ( runArgs.runNumber==301489 or runArgs.runNumber==301509 ):
    mhh = '400'
elif ( runArgs.runNumber==301486 or  runArgs.runNumber==301490 or runArgs.runNumber==301510 ):
    mhh = '500'
elif ( runArgs.runNumber==301491 or runArgs.runNumber==301511 ):
    mhh = '600'
elif ( runArgs.runNumber==301492 or runArgs.runNumber==301512 ):
    mhh = '700'
elif ( runArgs.runNumber==301493 or runArgs.runNumber==301513 ):
    mhh = '800'
elif ( runArgs.runNumber==301494 or runArgs.runNumber==301514 ):
    mhh = '900'
elif ( runArgs.runNumber==301487 or runArgs.runNumber==301495 or runArgs.runNumber==301515 ):
    mhh = '1000'
elif ( runArgs.runNumber==301496 or runArgs.runNumber==301516 ):
    mhh = '1100'
elif ( runArgs.runNumber==301497 or runArgs.runNumber==301517 ):
    mhh = '1200'
elif ( runArgs.runNumber==301498 or runArgs.runNumber==301518 ):
    mhh = '1300'
elif ( runArgs.runNumber==301499 or runArgs.runNumber==301519 ):
    mhh = '1400'
elif ( runArgs.runNumber==301500 or runArgs.runNumber==301520 ):
    mhh = '1500'
elif ( runArgs.runNumber==301501 or runArgs.runNumber==301521 ):
    mhh = '1600'
elif ( runArgs.runNumber==301502 or runArgs.runNumber==301522 ):
    mhh = '1800'
elif ( runArgs.runNumber==301503 or runArgs.runNumber==301523 ):
    mhh = '2000'
elif ( runArgs.runNumber==301504 or runArgs.runNumber==301524 ):
    mhh = '2250'
elif ( runArgs.runNumber==301505 or runArgs.runNumber==301525 ):
    mhh = '2500'
elif ( runArgs.runNumber==301506 or runArgs.runNumber==301526 ):
    mhh = '2750'
elif ( runArgs.runNumber==301507 or runArgs.runNumber==301527 ):
    mhh = '3000'
elif ( runArgs.runNumber==309038 or runArgs.runNumber==309042 ):
    mhh = '260'
elif ( runArgs.runNumber==309039):
    mhh = '270'
elif ( runArgs.runNumber==309040 or runArgs.runNumber==309043 ):
    mhh = '280'
elif ( runArgs.runNumber==309041):
    mhh = '290'
else:
    print 'ERROR - no hh mass specified'

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


name = 'SMRS_c'+str(int(10*c))+'_G_hh_bbbb_m'+str(mhh)

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

generate p p > hh, (hh > h h)
output -f
""")
fcard1.close()

beamEnergy=6500
if hasattr(runArgs,'ecmEnergy'): 
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  raise RunTimeError("No center of mass energy found.")


build_run_card(run_card_old=get_default_runcard(isNLO=False), run_card_new='run_card.dat', xqcut=0,
               nevts=runArgs.maxEvents*1.1, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy,
               extras=extras)

print_cards()

runName='run_01'

# first process to compute widths and set parameters card
process_dir = new_process(card_loc='proc_card_mg5_step1.dat')
generate(run_card_loc='run_card.dat', param_card_loc=None, mode=0, njobs=1, 
         run_name='run_01', proc_dir=process_dir)
#generate(run_card_loc='run_card.dat', param_card_loc=None, mode=0, njobs=1, 
#        run_name='run_01', proc_dir=process_dir)

# second process to generate hh -> bb bb events using parameters from first process
process_dir = new_process(card_loc='proc_card_mg5_step2.dat')
generate(run_card_loc='run_card.dat', param_card_loc='SMRS_widths/Events/run_01/param_card.dat',
         mode=0, njobs=1, run_name=runName, proc_dir=process_dir)

stringy = 'madgraph.'+str(runArgs.runNumber)+'_'+str(name)

arrange_output(run_name=runName, proc_dir=process_dir, outputDS=stringy+'._00001.events.tar.gz')


#--------------------------------------------------------------
# Pythia8 showering with A14_NNPDF23LO
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["25:oneChannel = on 1.0 100 5 -5 "] #bb final state

#--------------------------------------------------------------
# MC15 metadata
#--------------------------------------------------------------
evgenConfig.contact = ["Stephane.Willocq@cern.ch"]
evgenConfig.description = "Bulk Randall-Sundrum model KK graviton -> hh -> bbbb with NNPDF2.3 LO A14 tune"
evgenConfig.inputfilecheck = stringy
evgenConfig.keywords = ["exotic", "RandallSundrum", "warpedED", "graviton", "Higgs"]
evgenConfig.process = "RS_G_hh_bbbb"
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

