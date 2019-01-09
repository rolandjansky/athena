from MadGraphControl.MadGraphUtils import *

evgenConfig.contact  = [ 'pedro.henrique.sales.de.bruin@cern.ch' ]
evgenConfig.keywords+=['ZHiggs','BSMHiggs','bottom', 'tau']

# Generate A->Zh, h->bb, Z->ll or Z->vv
# Assume job inputs of the form (NOTICE ../ !!):
# MC15.runNumber.MadGraphPythia8EvtGen_PDF_ggA[massA]_Zh[massh]_[Zdecay]bb.py  


# parse the job arguments to get mA,mh and Z decay mode
print runArgs.jobConfig[0]
print runArgs.jobConfig[0].split('.')[2]
print runArgs.jobConfig[0].split('.')[2].split('_')[2]
print runArgs.jobConfig[0].split('.')[2].split('_')[2][3:]

mA = float(runArgs.jobConfig[0].split('.')[2].split('_')[2][3:])
mh = float(runArgs.jobConfig[0].split('.')[2].split('_')[3][2:])
Z_decay = (runArgs.jobConfig[0].split('.')[2].split('_')[4][0:2])
h_decay = ''
tautau_decay = (runArgs.jobConfig[0].split('.')[2].split('_')[5][0:2])

# can be tautaubb or bbtautau (also works with ttbb,bbtt)
isZtautau=True
if Z_decay=='bb':
    isZtautau=False
    h_decay = 'tautau'
else:
    Z_decay = 'tautau'
    h_decay = 'bb'

# lephad or hadhad decay?
isLepHad=True
if (tautau_decay=='hh'):
    isLepHad=False

print "mh",mh
print "mA",mA
print "Z decays to ",Z_decay
print "h decays to ",h_decay
print "tautau decay mode is ", tautau_decay

print "isZtautau",isZtautau

# here we deal with the need to generate more events to cope with job failures
nevents=int(runArgs.maxEvents*30) 
runNumber=runArgs.runNumber 
mode=0 

print 'nevents,runNumber',nevents,runNumber

# create the process string to be copied to proc_card_mg5.dat
process=""
if isZtautau:
    process="""
import model 2HDM_GF
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
# Define multiparticle labels
# Specify process(es) to run
generate     g g > h3,     (h3 > z h1, (z > ta- ta+), (h1 > b b~) )
output -f
"""
else: 
    process="""
import model 2HDM_GF
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~ a
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
# Define multiparticle labels
# Specify process(es) to run
generate     g g > h3,     (h3 > z h1, (z > b b~), (h1 > ta- ta+) )
output -f
"""

# Lines I used during inclusive production
# generate     g g > h3,     (h3 > z h1, (z > ta- ta+), (h1 > b b~) ) 
# generate     g g > h3,     (h3 > z h1, (z > b b~), (h1 > ta- ta+) )

print 'process string:',process

print 'mA',mA

#---------------------------------------------------------------------------------------------------
# Set masses and widths in param_card.dat
#---------------------------------------------------------------------------------------------------
mh1=mh
mh2=mA
mh3=mA
mhc=mA
print 'mh1,mh2,mh3,mhc',mh1,mh2,mh3,mhc
masses ={'25':mh1,  
         '35':mh2,
         '36':mh3, 
         '37':mhc} 

print masses

decays ={'25':'DECAY 25 4.070000e-03 # Wh1',  
         '35':'DECAY 35 1.000000e+00 # Wh2',  
         '36':'DECAY 36 1.000000e+00 # Wh3',  
         '37':'DECAY 37 1.000000e+00 # whc'}  

print decays


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

# PEDRO: new_process needs to be called before build_run_card to avoid an error
# set up process
#process_dir = new_process(card_loc='proc_card_mg5.dat')
process_dir = new_process(process)


#Fetch default LO run_card.dat and set parameters (extras)
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'lhapdf'",
#           'lhaid':"'247000'"} # nn23lo1
           'lhaid':247000} # nn23lo1
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

runName='run_01'     

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
#---------------------------------------------------------------------------------------------------
build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',masses=masses,decays=decays)
    
print_cards()

# and the generation
generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')  


#### Pythia8 Showering with A14_NNPDF23LO
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_LHEF.py")
   
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
if isZtautau:
    evgenConfig.description = 'non SM higgs mA(%s GeV)->Zh(125 GeV)->tautaubb' % (mA)
else:
    evgenConfig.description = 'non SM higgs mA(%s GeV)->Zh(125 GeV)->bbtautau' % (mA)

evgenConfig.minevents = 5000
evgenConfig.inputfilecheck = runName

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

if isZtautau:
    genSeq.Pythia8.Commands += ['25:onMode = off',  # decay of Higgs to bb
                            '25:onIfMatch = 5 5'
                            ]
else:
    genSeq.Pythia8.Commands += ['25:onMode = off',  # decay of Higgs to tautau
                            '25:onIfMatch = 15 15'
                            ]

# Should this be here?
#if isZtautau:
#    genSeq.Pythia8.Commands += ['23:onMode = off',# decay of Z to tautau
#                            '23:onIfMatch = 15 15'
#                            ]
#else:
#    genSeq.Pythia8.Commands += ['23:onMode = off',#decay of Z to bb
#                            '23:onIfMatch = 5 5'
#                            ]

#---------------------------------------------------------------------------------------------------
# Filter for bbtautau/tautaubb
#---------------------------------------------------------------------------------------------------
include("GeneratorFilters/XtoVVDecayFilterExtended.py")
from GeneratorFilters.GeneratorFiltersConf import XtoVVDecayFilterExtended

## Already done with the process definition? TODO: double-check this!
#if isZtautau:
#    filtSeq += XtoVVDecayFilterExtended("TauTauBBFilter")
#    filtSeq.BBTauTauFilter.PDGGrandParent = 36  # used to be 1560 for di-higgs sample
#    filtSeq.BBTauTauFilter.PDGParent = 25
#    filtSeq.BBTauTauFilter.StatusParent = 22
#    filtSeq.BBTauTauFilter.PDGChild1 = [5]
#    filtSeq.BBTauTauFilter.PDGChild2 = [15]
#else:
#    filtSeq += XtoVVDecayFilterExtended("BBTauTauFilter")
#    filtSeq.TauTauBBFilter.PDGGrandParent = 36  # used to be 1560 for di-higgs sample
#    filtSeq.TauTauBBFilter.PDGParent = 25
#    filtSeq.TauTauBBFilter.StatusParent = 22
#    filtSeq.TauTauBBFilter.PDGChild1 = [15]
#    filtSeq.TauTauBBFilter.PDGChild2 = [5]

#---------------------------------------------------------------------------------------------------
# Filter for lephad and hadhad
#---------------------------------------------------------------------------------------------------
if isZtautau:
   if isLepHad:
       filtSeq += XtoVVDecayFilterExtended("TauTauLepHadFilter")
       filtSeq.TauTauLepHadFilter.PDGGrandParent = 23
       filtSeq.TauTauLepHadFilter.PDGParent = 15
       filtSeq.TauTauLepHadFilter.StatusParent = 2
       filtSeq.TauTauLepHadFilter.PDGChild1 = [11,13]
       filtSeq.TauTauLepHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]
   else:
       filtSeq += XtoVVDecayFilterExtended("TauTauHadHadFilter")
       filtSeq.TauTauHadHadFilter.PDGGrandParent = 23
       filtSeq.TauTauHadHadFilter.PDGParent = 15
       filtSeq.TauTauHadHadFilter.StatusParent = 2   # What status should this be?
       filtSeq.TauTauHadHadFilter.PDGChild1 = [111,130,211,221,223,310,311,321,323]
       filtSeq.TauTauHadHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]
else:
   if isLepHad:
       filtSeq += XtoVVDecayFilterExtended("TauTauLepHadFilter")
       filtSeq.TauTauLepHadFilter.PDGGrandParent = 25
       filtSeq.TauTauLepHadFilter.PDGParent = 15
       filtSeq.TauTauLepHadFilter.StatusParent = 2
       filtSeq.TauTauLepHadFilter.PDGChild1 = [11,13]
       filtSeq.TauTauLepHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]
   else:
       filtSeq += XtoVVDecayFilterExtended("TauTauHadHadFilter")
       filtSeq.TauTauHadHadFilter.PDGGrandParent = 25
       filtSeq.TauTauHadHadFilter.PDGParent = 15
       filtSeq.TauTauHadHadFilter.StatusParent = 2
       filtSeq.TauTauHadHadFilter.PDGChild1 = [111,130,211,221,223,310,311,321,323]
       filtSeq.TauTauHadHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]

#---------------------------------------------------------------------------------------------------
# Filter for 2 leptons (inc tau(had)) with pt cuts on e/mu and tau(had)
#---------------------------------------------------------------------------------------------------
from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
filtSeq += MultiElecMuTauFilter("LepTauPtFilter")
filtSeq.LepTauPtFilter.IncludeHadTaus = True
filtSeq.LepTauPtFilter.NLeptons = 2
filtSeq.LepTauPtFilter.MinPt = 13000.
filtSeq.LepTauPtFilter.MinVisPtHadTau = 15000.
filtSeq.LepTauPtFilter.MaxEta = 5.

## Create filtSeq
#if isZtautau:
#    filtSeq.Expression = "TauTauBBFilter"
#else:
#    filtSeq.Expression = "BBTauTauFilter"

if isLepHad:
    filtSeq.Expression = "TauTauLepHadFilter"
else:
    filtSeq.Expression = "TauTauHadHadFilter"

filtSeq.Expression += " and LepTauPtFilter"

print "filter used: ",filtSeq.Expression

