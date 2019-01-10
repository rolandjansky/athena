from MadGraphControl.MadGraphUtils import *

# Generate A->Zh, h->bb, Z->ll or Z->vv
# Assume job inputs of the form:
# MC15.runNumber.MadGraphPythia8EvtGen_PDF_ggA[massA]_Zh[massh]_[Zdecay]bb.py


# parse the job arguments to get mA,mh and Z decay mode
mA = float(runArgs.jobConfig[0].split('.')[2].split('_')[2][3:])
mh = float(runArgs.jobConfig[0].split('.')[2].split('_')[3][2:])
decay = (runArgs.jobConfig[0].split('.')[2].split('_')[4][0:2])
print "mh",mh
print "mA",mA
print "decay",decay

# Zll/Znunu
isZll=True 
if decay=='vv':
    isZll=False

print "isZll",isZll

# here we deal with the need to generate more events to cope with job failures
nevents=int(runArgs.maxEvents*1.4) 
runNumber=runArgs.runNumber 
mode=0 

print 'nevents,runNumber',nevents,runNumber

# create the process string to be copied to proc_card_mg5.dat
process=""
if isZll:
    process="""
import model 2HDM_GF
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~ a
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
# Define multiparticle labels
# Specify process(es) to run
generate     g g > h3,     (h3 > z h1, (z > l- l+, h1 > b b~) )
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
generate     g g > h3,     (h3 > z h1, (z > vl vl~, h1 > b b~) )
output -f
"""

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


#Fetch default LO run_card.dat and set parameters (extras)
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'lhapdf'",
           'lhaid':"'247000'"} # nn23lo1
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
runName='run_01'     

# set up process
#process_dir = new_process(card_loc='proc_card_mg5.dat')
process_dir = new_process(process) 

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
if isZll:
    evgenConfig.description = 'non SM higgs mA(%s GeV)->Zh(125 GeV)->llbb' % (mA)
else:
    evgenConfig.description = 'non SM higgs mA(%s GeV)->Zh(125 GeV)->nunubb' % (mA)
evgenConfig.keywords+=['ZHiggs','BSMHiggs','bottom']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

genSeq.Pythia8.Commands += ['25:onMode = off',#decay of Higgs
                            '25:onIfMatch = 5 5'
                            ]
