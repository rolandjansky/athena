import os, sys

from MadGraphControl.MadGraphUtils import *
mode=0

###
# Read Mass from joboption name
###
int(s) 

mh2_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS H AND THE PRODUCTION MODE WE WANT TO SIMULATE IN FORMAT LIKE: *_ggH750_* -> H=750GeV
mh2=0
prodmode=""
for s in mh2_str.split("_"):
    if "ggH" in s:
        ss=s.replace("ggH","")              # in GeV
        if ss.isdigit():
            mh2 = int(ss)
            prodmode="ggH"
            break
    if "ttH" in s:
        ss=s.replace("ttH","")              # in GeV
        if ss.isdigit():
            mh2 = int(ss)        
            prodmode="ttH"
            break
if mh2==0:
   raise RuntimeError("Heavy Higgs mass not set, mh2=0, check joOption name %s"%mh2_str)
###

ma1_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS a WE WANT TO SIMULATE IN FORMAT LIKE: *_2a2_* -> a01=2GeV
ma1=0
for s in ma1_str.split("_"):
    if s.replace("2a","").isdigit():  
        if "2a0" in s: 
            ss=s.replace("2a0","0.")     # decimal in GeV
        else:
            ss=s.replace("2a","")        # int in GeV  
        ma1 = float(ss)        
if ma1==0:
   raise RuntimeError("light Higgs-pseudoscalar mass not set, ma1=0, check joOption name %s"%ma1_str)
###

wh2_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE WIDTH OF H (in %) WE WANT TO SIMULATE IN FORMAT LIKE: *_W10_* -> wh2=mh2*0.1 or _NWA_ -> wh2=4.08MeV
wh2=0
for s in wh2_str.split("_"):
    if "NWA" in s:
        wh2=4.088e-03
        break
    ss=s.replace("p","")
    if ss.replace("W","").isdigit():  
        if "W0" in ss: 
            ss=ss.replace("W0","0.")  # decimal in GeV
        else:
            ss=ss.replace("W","")     # int in GeV  
        wh2 = mh2*float(ss)/100.       
if wh2==0:
   raise RuntimeError("Heavy Higgs width not set, wh2=0, check joOption name %s"%wh2_str)
###

decaymode_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE DECAY MODE OF THE a WE WANT TO SIMULATE IN FORMAT LIKE: *_4y or *_6pi0
decaymode=""
if "_4y" in decaymode_str:
    decaymode="36:addChannel 1 1 0 22 22"
elif "_6pi0" in decaymode_str:
    decaymode="36:addChannel 1 1 101 111 111 111"
else:
    raise RuntimeError("Pseudo scalar decay mode (4y or 6pi0) not set, decaymode=\"\", check joOption name %s"%decaymode_str)
###

#---------------------------------------------------------------------------------------------------
# Setting EFT parameters in HC model
#---------------------------------------------------------------------------------------------------
parameters={
    'frblock':{ 
        'Lambda':'1.000000e+03',
        'cosa':  '1.000000e+00',
        'kSM':   '1.000000e+00',
        'kHaa':  '1.000000e+00',
        'kHll':  '0.000000e+00',
        'kAll':  '0.000000e+00',
        'kAaa':  '0.000000e+00',
        'kHza':  '0.000000e+00',
        'kAza':  '0.000000e+00',
        'kHgg':  '1.000000e+00',
        'kAgg':  '0.000000e+00',
        'kHzz':  '0.000000e+00',
        'kAzz':  '0.000000e+00',
        'kHww':  '0.000000e+00',
        'kAww':  '0.000000e+00',
        'kHda':  '0.000000e+00',
        'kHdz':  '0.000000e+00',
        'kHdwR': '0.000000e+00',
        'kHdwI': '0.000000e+00'}
    }

#---------------------------------------------------------------------------------------------------
# Setting H mass and width for param_card.dat 
#---------------------------------------------------------------------------------------------------
higgsMass  = {'25':str(mh2)+' # MH'                  }        #Mass
higgsDecay = {'25':'DECAY  25 '+str(wh2)+' # H Width'}        #Width

#---------------------------------------------------------------------------------------------------
# Generating pp -> H in HC model
#---------------------------------------------------------------------------------------------------
# ggH:
fcard = open('proc_card_mg5.dat','w')
if prodmode=="ggH": 
    fcard.write("""
import model HC_NLO_MOD_GammaGamma-heft
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
generate p p > x0 > a a / t [QCD]
output -f""")
    fcard.close()




beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()


#---------------------------------------------------------------------------------------------------
# Setting some parameters for run_card.dat
#---------------------------------------------------------------------------------------------------
#Fetch default NLO run_card.dat and set parameters
extras = {'lhe_version':'3.0', 
          'cut_decays' :'F', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : '260000',
          'sys_pdf'    : 'NNPDF30_nlo_as_0118',
          'parton_shower' :'PYTHIA8',
          'reweight_scale':'.false.'}

#---------------------------------------------------------------------------------------------------
# Setting the number of generated events to 'safefactor' times maxEvents,
# to avoid crashing due to not having enough events
# Also putting protection to avoid from crashing when maxEvents=-1
#---------------------------------------------------------------------------------------------------
safefactor = 1.1
nevents    = 5000*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
# Used values given in "parameters" for EFT parameters, if not set there, default values are used
# Higgs mass is set to 125 GeV by "higgsMass"
#---------------------------------------------------------------------------------------------------

build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',masses=higgsMass,decays=higgsDecay,params=parameters)

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
# Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used
#---------------------------------------------------------------------------------------------------
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

print_cards()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3)

# LHE hack: 
# - Change PDG ID of SM Higgs to BSM Higgs-like scalar, for Pythia8 
# - Remove decay products (photons)
# - Correct the number of states on each event (max expected 5: 2 initial partons + 1 Higgs + max 2jets)
# - Remove events where no Higgs is found (10% FiltEff already considered earlier on)
 
infile = "tmp_"+runName+'._00001.events.events'

f1 = open( infile )
lines = f1.readlines()
f1.close()

newfile = infile+'.temp'
os.system('rm %s ' %newfile)
f2 = open(newfile,'w')

line_iter = iter(lines)
for line in line_iter:
    if line.startswith       ('       25  2'):
        f2.write(line.replace('       25  2','       35  1'))
    elif line.startswith     ('  7      0'):
        f2.write(line.replace('  7      0','  5      0'))
    elif line.startswith     ('  6      0'):
        f2.write(line.replace('  6      0','  4      0'))
    elif line.startswith     ('  5      0'):
        f2.write(line.replace('  5      0','  3      0'))
    elif line.startswith     ('  4      0'):
        for i in range(0,6): next(line_iter) 
    elif line.startswith     ('       22  1'):
        continue
    else:
        f2.write(line)
        
f2.close()

os.system('rm %s ' %infile)
os.system('mv %s %s ' %(newfile,infile) )

#### Shower

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

#--------------------------------------------------------------
# Pythia8 main31 update
#--------------------------------------------------------------
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 1' ]

#--------------------------------------------------------------
# Higgs Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += [ "35:onMode = off",             # decay of our Higgs
                             "35:m0 = "+str(mh2),           # Higgs mass (just in case)
                             "35:mWidth = "+str(wh2),       # Higgs width (just in case)
                             "35:addChannel 1 1 101 36 36",
                             "35:onIfMatch = 36 36",        # decay H --> AA    
                             "36:onMode = off",
                             decaymode,
                             "36:isResonance = off",
                             "36:m0 = "+str(ma1),           # set A mass
                             "36:mMin = 0.005",
                             "36:mWidth = 0.001",           # Narrow width -> 1MeV
                             "36:onIfMatch = 22 22"         # decay A --> 2photon
                             ]


#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.generators = ["aMcAtNlo", "Pythia8", "EvtGen"]
evgenConfig.description = "MG5+PYTHIA8+EVTGEN, ggH H->aa->4y  NLO HC model"
evgenConfig.process     = "gg->H->aa->4photons"
evgenConfig.keywords    = [ "Higgs", "BSMHiggs", "4photon" ]
evgenConfig.inputfilecheck = runName
evgenConfig.contact     = [ 'montoya@cern.ch' ]

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'



