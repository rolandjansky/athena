from MadGraphControl.MadGraphUtils import *

mode=0

# extract dataset short name from filename, should be of the form MC15.999999.MadGraphPythia8EvtGen_A14NNPDF23LO_ZpHA_Y1toHy_bby_1000_NW.py
shortname=runArgs.jobConfig[0].split('/')[-1].split('.')[2].split('_')
# decode dataset short name, should be of the form MadGraphPythia8EvtGen_A14NNPDF23LO_ZpHA_Y1toHy_bby_1000_NW (split by '_')
Y1decay=shortname[3]
Hdecay=shortname[4]
resonanceMass=int(shortname[5])
resonanceWidthPercent=shortname[6]

print 'Y1decay: %s' % Y1decay
print 'Hdecay: %s' % Hdecay
print 'resonanceMass: %d' % resonanceMass
print 'resonanceWidthPercent: %s' % resonanceWidthPercent

#is_qq = runArgs.jobConfig[0].find('qq')
#is_gg = runArgs.jobConfig[0].find('gg')
#is_pp = runArgs.jobConfig[0].find('pp')

resonanceWidth = 0

if resonanceWidthPercent == "NW":
    resonanceWidth = 0.0042  #GeV
else:
    resonanceWidthPercent = float(resonanceWidthPercent[1:-1]) * 0.01 #Converting string to percent of mass
    resonanceWidth = resonanceWidthPercent * resonanceMass

if resonanceWidth == 0:
    raise RuntimeError("Resonance width not correctly interpreted from JO name")


#---------------------------------------------------------------------------------------------------
# Setting X0 mass and width for param_card.dat
#---------------------------------------------------------------------------------------------------
Y1Mass  = {'5000001': '%e # MY1'%resonanceMass}               #Mass
Y1Decay = {'5000001':'DECAY 5000001 %e # WY1'%resonanceWidth} #Width


#---------------------------------------------------------------------------------------------------
# Generating pp(qq) -> Y1 -> Hy  in DM spin-1 Z' effective coupling model
#---------------------------------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
if ("bb" in Hdecay):
    fcard.write("""
    import model ZpHA
    define q = u c d s b u~ c~ d~ s~ b~
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    generate p p > y1 > h a, h > b b~
    output -f""")
elif ("tt" in Hdecay):
    fcard.write("""
    import model ZpHA
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define q = u c d s b u~ c~ d~ s~ b~
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    generate p p > y1 > h a, h > t t~
    output -f""")
elif ("ll" in Hdecay):
    fcard.write("""
    import model ZpHA
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define q = u c d s b u~ c~ d~ s~ b~
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    generate p p > y1 > h a, z > l+ l-
    output -f""")
elif ("ww" in Hdecay):
    fcard.write("""
    import model ZpHA
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define q = u c d s b u~ c~ d~ s~ b~
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    generate p p > y1 > h a, h > w+ w-
    output -f""")
elif ("zz" in Hdecay):
    fcard.write("""
    import model ZpHA
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define q = u c d s b u~ c~ d~ s~ b~
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    generate p p > y1 > h a, h > z z
    output -f""")
else:
    raise RuntimeError("Not a valid decay: need to be bb/tt/ww/zz/ll.")
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
          'lhaid'      : '247000',
          'sys_pdf'    : 'NNDF23_lo_as_0130_qed',
          'parton_shower' :'PYTHIA8',
          'reweight_scale':'.false.',
          'bwcutoff'   : '1000'}

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
#---------------------------------------------------------------------------------------------------

build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',masses=Y1Mass,decays=Y1Decay)

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
# Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used
#---------------------------------------------------------------------------------------------------
extras={}
extras["scale"] = resonanceMass
extras["dsqrt_q2fact1"] = resonanceMass
extras["dsqrt_q2fact2"] = resonanceMass
extras["pdlabel"]='lhapdf'
extras["lhaid"]=247000
build_run_card(run_card_old='MadGraph_run_card_ZpHA_LO.dat',
               run_card_new='run_card.dat',
               xqcut=0,
               nevts=nevents,
               rand_seed=runArgs.randomSeed,
               beamEnergy=beamEnergy,
               scalefact=1.0,
               alpsfact=1.0,
               extras=extras)

print_cards()


runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3)

#### Shower

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.description = 'Dark Matter Zprime model LO Spin-1 qq->Y1->Higgs+gamma narrow width resonance in b-bbar decay model'
evgenConfig.keywords = ["exotic", "Higgs", "photon", "LO", "bbbar", "spin1"]
evgenConfig.inputfilecheck = runName
evgenConfig.contact = ['Shu Li <Shu.Li@cern.ch>']

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
