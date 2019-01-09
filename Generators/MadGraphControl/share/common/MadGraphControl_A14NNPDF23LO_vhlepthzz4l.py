#---------------------------------------------------------------------------------------------------
# parsing the Generate_tf.py arguments
#---------------------------------------------------------------------------------------------------

def parseargs(runArgs,safefactor=1.1,nevents=20000):
    options = {}
    if not hasattr(runArgs,'runNumber'):
        raise RuntimeError("no runNumber supplied as argument")
    else: 
        options["runNumber"] = runArgs.runNumber

    if hasattr(runArgs,'ecmEnergy'):
        options["beamEnergy"] = runArgs.ecmEnergy / 2.
    else: 
        raise RuntimeError("No center of mass energy found.")

    if hasattr(runArgs,"maxEvents") and runArgs.maxEvents > 0:
        options["nevents"]=runArgs.maxEvents*safefactor
    else:
        options["nevents"]=nevents*safefactor

    if hasattr(runArgs,"randomSeed"):
        options["randomSeed"] = runArgs.randomSeed
    else:
        raise RuntimeError("No random seed provided.")

    if hasattr(runArgs,"outputTXTFile"):
        options["runName"]=runArgs.outputTXTFile
    else:
        options["runName"]='run_01'

    options["mode"] = 0

    runArgs.inputGeneratorFile=options["runName"]+'._00001.events.tar.gz'
    options["ingenfile"] = runArgs.inputGeneratorFile

    return options

options = parseargs(safefactor=1.5,runArgs=runArgs)



if ( (options["runNumber"] not in range(345050,345053)) and (options["runNumber"] not in range(344135,344152)) ):
  raise RuntimeError("runNumber {0:d} not recognised in these jobOptions. Available options are: 100001".format(options["runNumber"]))




#---------------------------------------------------------------------------------------------------
# defining the dsid- eft parameters correspondence
#---------------------------------------------------------------------------------------------------
parameters=[1.0,1.0,0.0,0.0]
#ksm, cos, khvv, kAvv


if options["runNumber"] == 344135 : 
    parameters=[1.0,1.0,0.0,0.0]
if options["runNumber"] == 344136 : 
    parameters=[1.0,1.0,5.0,0.0]
if options["runNumber"] == 344137 :
    parameters=[1.0,1.0,-5.0,0.0]
if options["runNumber"] == 344138 :
    parameters=[1.0,1.0,2.5,0.0]
if options["runNumber"] == 344139 :
    parameters=[1.0,1.0,-2.5,0.0]
if options["runNumber"] == 344140 :
    parameters=[0.0,1.0,10.0,0.0]
if options["runNumber"] == 344141 :
    parameters=[1.0,0.7071,0.0,5.0]
if options["runNumber"] == 344142 :
    parameters=[1.0,0.7071,0.0,-5.0]
if options["runNumber"] == 344143 :
    parameters=[1.0,0.7071,0.0,2.5]
if options["runNumber"] == 344144 :
    parameters=[1.0,0.7071,0.0,-2.5]
if options["runNumber"] == 344145 :
    parameters=[0.0,0.7071,0.0,15.0]

# for 2D
if options["runNumber"] == 344146 :
    parameters=[1.0,0.7071,2.5,-5.0]
if options["runNumber"] == 344147 :
    parameters=[1.0,0.7071,-2.5,-5.0]
if options["runNumber"] == 344148 :
    parameters=[1.0,0.7071,-5.0,5.0]
if options["runNumber"] == 344149 :
    parameters=[1.0,0.7071,-5.0,-6.0]
if options["runNumber"] == 344150 :
    parameters=[1.0,0.7071,5.0,6.0]
if options["runNumber"] == 344151 :
    parameters=[1.0,0.7071,5.0,5.0]

# for 2D, additional samples
if options["runNumber"] == 345050 :
    parameters=[1.0,0.7071,5.0,10.0]
if options["runNumber"] == 345051 :
    parameters=[1.0,0.7071,-10.0,10.0]
if options["runNumber"] == 345052 :
    parameters=[1.0,0.7071,10.0,-10.0]



#---------------------------------------------------------------------------------------------------
# generating process cards in HC_NLO_X0_UFO model
#---------------------------------------------------------------------------------------------------

def writeproccard(content):
    fcard = open('proc_card_mg5.dat','w')
    fcard.write(content)
    fcard.close()


#---------------------------------------------------------------------------------------------------
# generating vhzz4l
#---------------------------------------------------------------------------------------------------

writeproccard(content="""import model HC_NLO_X0_UFO
    define p,j = g u c d s u~ c~ d~ s~
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    generate p p > x0 l+ l- /a
    add process p p > x0 vl vl~ 
    add process p p > x0 l- vl~ 
    add process p p > x0 l+ vl 
output PROC_vhlepthzz4l""")




#---------------------------------------------------------------------------------------------------
# Setting EFT parameters in HC model
#---------------------------------------------------------------------------------------------------

def writeparamcard(options,masses,parameters):
    from MadGraphControl.MadGraphUtils import new_process
    process_dir = new_process()
    options["processdir"] = process_dir
    
    from MadGraphControl.MadGraphUtils import build_param_card
    from os.path import join as pathjoin
    build_param_card(param_card_old=pathjoin(options["processdir"],'Cards/param_card.dat'),param_card_new='param_card_new.dat',
                     masses=masses,params=parameters)


writeparamcard(masses={'25': '1.250000e+02'},options=options,parameters={'frblock': {'kAbb': 0.0, 
										     'kHaa': 0.0, 
										     'kAaa': 0.0, 
										     'cosa': parameters[1], 
										     'kHdwR': 0.0, 
										     'kAza': 0.0, 
										     'kAll': 0.0, 
										     'kHbb': 0.0, 
										     'kHll': 0.0, 
										     'kAzz': parameters[3], 
										     'kSM': parameters[0], 
										     'kHdwI': 0.0, 
										     'kHdz': 0.0, 
										     'kAww': parameters[3], 
										     'kAtt': 0.0, 
										     'kHda': 0.0, 
										     'kHza': 0.0, 
										     'kHtt': 0.0, 
										     'kHww': parameters[2], 
										     'kHzz': parameters[2], 
										     'Lambda': 1000.0}})



#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
#---------------------------------------------------------------------------------------------------

def writeruncard(options,extras):
    from MadGraphControl.MadGraphUtils import build_run_card,get_default_runcard
    rand_seed_tmp=int(options["randomSeed"])% (30081*30081) 
    build_run_card(run_card_old=get_default_runcard(options["processdir"]),run_card_new='run_card.dat',
                   nevts=options["nevents"],rand_seed=rand_seed_tmp,
                   beamEnergy=options["beamEnergy"],extras=extras,xqcut=0.)


#---------------------------------------------------------------------------------------------------
# creating run_card.dat for vhzz4l
#---------------------------------------------------------------------------------------------------


writeruncard(extras={'lhe_version':'2.0',
		     'mmaamax': '-1', 
		     'auto_ptj_mjj': 'F', 
		     'ptl': '0', 
		     'ptj': '0', 
		     'drll': '0', 
		     'ptb': '0', 
		     'pta': '0', 
		     'drbbmax': '-1', 
		     'draamax': '-1', 
		     'ptbmax': '-1', 
		     'etal': '-1', 
		     'mmaa': '0', 
		     'drjjmax': '-1', 
		     'etajmin': '0', 
		     'ptl1min': '-1', 
		     'dsqrt_q2fact2': '125', 
		     'dsqrt_q2fact1': '125', 
		     'bwcutoff': '50', 
		     'cut_decays': 'F', 
		     'drbjmax': '-1', 
		     'ptl2min': '-1', 
		     'drabmax': '-1', 
		     'etaonium': '-1', 
		     'lhaid': '247000', 
		     'drjj': '0', 
		     'drbj': '0', 
		     'ptonium': '0', 
		     'drbb': '0', 
		     'scale': '125', 
		     'etaa': '-1', 
		     'etab': '-1', 
		     'drjl': '0', 
		     'etaj': '-1', 
		     'drajmax': '-1', 
		     'ptamax': '-1', 
		     'pdlabel': "'lhapdf'", 
		     'parton_shower': 'PYTHIA8', 
		     'etaamin': '0', 
		     'draa': '0', 
		     'drab': '0', 
		     'dral': '0', 
		     'mmbbmax': '-1', 
		     'draj': '0', 
		     'etabmin': '0', 
		     'ptjmax': '-1', 
		     'mmbb': '0',
		     'use_syst': 'False'},options=options)


#
# creating mad spin card
#

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
# set seed %i
# specify the decay for the final state particles
set spinmode none
define l+ = e+ mu+
define l- = e- mu-
decay x0 > l+ l- l+ l-
# running the actual code
launch"""%runArgs.randomSeed)                                                                                                                                              
mscard.close()



#---------------------------------------------------------------------------------------------------
# Calling Madgraph
#---------------------------------------------------------------------------------------------------

def callgenerate(options):
    from MadGraphControl.MadGraphUtils import print_cards,generate,arrange_output
    import shutil
    print_cards()
    generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=options["mode"],proc_dir=options["processdir"],run_name=options["runName"],madspin_card_loc=madspin_card_loc)    
    fname_in = options["processdir"]+"/Events/"+options["runName"]+"/unweighted_events.lhe"
    fname_tmp = options["processdir"]+"/Events/"+options["runName"]+"/tmp.lhe"
    shutil.move(fname_in,fname_tmp)
    arrange_output(run_name=options["runName"],proc_dir=options["processdir"],outputDS=options["runName"]+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)
    
callgenerate(options=options)



#---------------------------------------------------------------------------------------------------
# Pythia8 Showering with A14_NNPDF23LO
#---------------------------------------------------------------------------------------------------

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")




evgenConfig.keywords = [ 'leptonic','2jet','4lepton','4electron','4muon','BSM','diboson','Higgs','SM','scalar','VBF','VBFHiggs','proton','antiproton','resonance','inclusive','BSMHiggs','SMHiggs','spin0']
evgenConfig.contact = ['Katharina Ecker <Katharina.Ecker.cern.ch>, Karolos Potamianos <Karolos.Potamianos@cern.ch>, Roberto Di Nardo <roberto.di.nardo@cern.ch>, Fabio Cerutti <Fabio.Cerutti@cern.ch>']
evgenConfig.generators = ['MadGraph', 'Pythia8', 'EvtGen']
evgenConfig.description = """VH leptonic 125 GeV Higgs production in the Higgs Characterization model decaying to zz4l."""
evgenConfig.inputfilecheck = options['runName']


