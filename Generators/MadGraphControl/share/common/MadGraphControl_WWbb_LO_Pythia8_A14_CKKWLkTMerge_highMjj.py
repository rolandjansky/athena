from MadGraphControl.MadGraphUtils import *

# General settings
mode=0
nevents=500000

# MG Particle cuts
mllcut=-1

# MG merging settings
maxjetflavor=5
ickkw=0


# Pythia8 merging settings
nJetMax=2
ktdurham=30
dparameter=0.4


### DSID lists (extensions can include systematics samples)

tt_Np0=[343852]
tt_Np1=[343853]
tt_Np2=[343854]

if runArgs.runNumber in tt_Np0:
    print "ardvark"
    minevents=50
    mgproc="""generate p p > t t~"""
    name='tt_Np0'
    process="pp>tt~"
    nJetMax=2
    keyword=['SM','ttbar']
elif runArgs.runNumber in tt_Np1:
    minevents=200
    mgproc="""generate p p > t t~ j"""
    name='tt_Np1'
    process="pp>tt~"
    nJetMax=2
    keyword=['SM','ttbar']
elif runArgs.runNumber in tt_Np2:
    minevents=1000
    mgproc="""generate p p > t t~ j j"""
    name='tt_Np2'
    process="pp>tt~"
    nJetMax=2
    keyword=['SM','ttbar']

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define w = w+ w-
define ttbar = t t~
define zonshell = u c d s b vl
define zonshell~ = u~ c~ d~ s~ b~ vl~
define leptonicDecay = e+ mu+ ta+ ve vm vt e- mu- ta- ve~ vm~ vt~
"""+mgproc+"""
output -f
""")
fcard.close()

#Now for madspin

madspin_card_loc='madspin_card.dat'
mscard = open(madspin_card_loc,'w')
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
#Some options (uncomment to apply)
#
# set seed 1
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
# set BW_cut 15                # cut on how far the particle can be off-shell
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
set seed %i
# specify the decay for the final state particles
decay t > w+ b, w+ > leptonicDecay leptonicDecay
decay t~ > w- b~, w- > leptonicDecay leptonicDecay
decay w+ > leptonicDecay leptonicDecay
decay w- > leptonicDecay leptonicDecay
# running the actual code
launch"""%runArgs.randomSeed)
mscard.close()

#end madspin

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")




process_dir = new_process()

pdflabel="nn23lo1"

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '2.0',
           'cut_decays'   : 'F', 
           'pdlabel'      : "'"+pdflabel+"'",
           'maxjetflavor' : maxjetflavor,
           'asrwgtflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 20,
           'ptb'          : 20,
           'mmll'         : mllcut,      
           'mmjj'         : 0,
           'drjj'         : 0,
           'drll'         : 0,
           'drjl'         : 0.4,
           'ptl'          : 0,
           'etal'         : 10,
           'etab'         : 6,
           'etaj'         : 6,
           'ktdurham'     : ktdurham,    
           'dparameter'   : dparameter,
           'use_syst'     : 'F',}


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)


#build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
#build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat', 
#               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
#               scalefact=scalefact,alpsfact=alpsfact,extras=extras)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,madspin_card_loc=madspin_card_loc)
#generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir)

arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')



#### Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword 
evgenConfig.inputfilecheck = stringy
evgenConfig.minevents = minevents
evgenConfig.generators  = [ "MadGraph", "Pythia8", "EvtGen"] 
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


PYTHIA8_TMS=ktdurham
PYTHIA8_nJetMax=nJetMax
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
genSeq.Pythia8.Commands+=["Merging:mayRemoveDecayProducts=on"]


