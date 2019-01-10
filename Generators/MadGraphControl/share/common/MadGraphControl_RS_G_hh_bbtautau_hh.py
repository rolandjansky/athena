from MadGraphControl.MadGraphUtils import *

c   = 1.0
mh  = 125
mhh = 1000

if ( runArgs.runNumber==303349 or runArgs.runNumber==303350 ):
    c = 0.5
elif (runArgs.runNumber>=303351 and runArgs.runNumber<=303371):
    c = 1.0
elif (runArgs.runNumber>=303372 and runArgs.runNumber<=303392):
    c = 2.0
else:
    print 'ERROR - no c parameter specified'

if ( runArgs.runNumber==303351 or runArgs.runNumber==303372 ):
    mhh = '260'
elif ( runArgs.runNumber==303352 or runArgs.runNumber==303373 ):
    mhh = '300'
elif ( runArgs.runNumber==303353 or runArgs.runNumber==303374 ):
    mhh = '400'
elif ( runArgs.runNumber==303354 or  runArgs.runNumber==303375 or runArgs.runNumber==303349):
    mhh = '500'
elif ( runArgs.runNumber==303355 or runArgs.runNumber==303376 ):
    mhh = '600'
elif ( runArgs.runNumber==303356 or runArgs.runNumber==303377 ):
    mhh = '700'
elif ( runArgs.runNumber==303357 or runArgs.runNumber==303378 ):
    mhh = '800'
elif ( runArgs.runNumber==303358 or runArgs.runNumber==303379 ):
    mhh = '900'
elif ( runArgs.runNumber==303359 or runArgs.runNumber==303380 or runArgs.runNumber==303350):
    mhh = '1000'
elif ( runArgs.runNumber==303360 or runArgs.runNumber==303381 ):
    mhh = '1100'
elif ( runArgs.runNumber==303361 or runArgs.runNumber==303382 ):
    mhh = '1200'
elif ( runArgs.runNumber==303362 or runArgs.runNumber==303383 ):
    mhh = '1300'
elif ( runArgs.runNumber==303363 or runArgs.runNumber==303384 ):
    mhh = '1400'
elif ( runArgs.runNumber==303364 or runArgs.runNumber==303385 ):
    mhh = '1500'
elif ( runArgs.runNumber==303365 or runArgs.runNumber==303386 ):
    mhh = '1600'
elif ( runArgs.runNumber==303366 or runArgs.runNumber==303387 ):
    mhh = '1800'
elif ( runArgs.runNumber==303367 or runArgs.runNumber==303388 ):
    mhh = '2000'
elif ( runArgs.runNumber==303368 or runArgs.runNumber==303389 ):
    mhh = '2250'
elif ( runArgs.runNumber==303369 or runArgs.runNumber==303390 ):
    mhh = '2500'
elif ( runArgs.runNumber==303370 or runArgs.runNumber==303391 ):
    mhh = '2750'
elif ( runArgs.runNumber==303371 or runArgs.runNumber==303392 ):
    mhh = '3000'
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


name = 'SMRS_c'+str(int(10*c))+'_G_hh_bbtautau_hh_m'+str(mhh)

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


build_run_card(run_card_old=get_default_runcard(), run_card_new='run_card.dat', xqcut=0,
               nevts=runArgs.maxEvents*50, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy,
               extras=extras)

print_cards()

runName='run_01'

# first process to compute widths and set parameters card
process_dir = new_process(card_loc='proc_card_mg5_step1.dat')
generate(run_card_loc='run_card.dat', param_card_loc=None, mode=0, njobs=1, 
         run_name='run_01', proc_dir=process_dir)

# second process to generate hh -> bb tautau events using parameters from first process
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

#---------------------------------------------------------------------------------------------------
# Generator Filters
#---------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------------

# Decaying hh to bbtautau with Pythia8

#---------------------------------------------------------------------------------------------------

genSeq.Pythia8.Commands += ["25:onMode=off",
                            "25:onIfAny=5 15" ]

# Require Higgs(25) -> b(5) b~(-5) tau(15) tau~(-15) independent of status codes (recommended)
from GeneratorFilters.GeneratorFiltersConf import ParentTwoChildrenFilter
filtSeq += ParentTwoChildrenFilter("HiggsToBBTauTauFilter")
filtSeq.HiggsToBBTauTauFilter.PDGParent = [25]
filtSeq.HiggsToBBTauTauFilter.PDGChild = [5,15]

#---------------------------------------------------------------------------------------------------
# Filter for hadhad
#---------------------------------------------------------------------------------------------------
from GeneratorFilters.GeneratorFiltersConf import XtoVVDecayFilterExtended
filtSeq += XtoVVDecayFilterExtended("TauTautoHadHadFilter")
filtSeq.TauTautoHadHadFilter.PDGGrandParent = 25
filtSeq.TauTautoHadHadFilter.PDGParent = 15
filtSeq.TauTautoHadHadFilter.StatusParent = 2
filtSeq.TauTautoHadHadFilter.PDGChild1 = [111,130,211,221,223,310,311,321,323]
filtSeq.TauTautoHadHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]

filtSeq.Expression = "HiggsToBBTauTauFilter and TauTautoHadHadFilter"

#--------------------------------------------------------------
# MC15 metadata
#--------------------------------------------------------------
evgenConfig.contact = ["Edson.Carquin@cern.ch"]
evgenConfig.description = "Bulk Randall-Sundrum model KK graviton -> hh -> bbtautau_hh with NNPDF2.3 LO A14 tune"
evgenConfig.inputfilecheck = stringy
evgenConfig.keywords = ["exotic", "RandallSundrum", "warpedED", "graviton", "Higgs","BSM"]
evgenConfig.process = "RS_G_hh_bbtautau_hh"
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

