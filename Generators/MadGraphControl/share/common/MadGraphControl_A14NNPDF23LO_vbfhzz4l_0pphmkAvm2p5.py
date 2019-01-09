
#---------------------------------------------------------------------------------------------------
# parsing the Generate_tf.py arguments
#---------------------------------------------------------------------------------------------------

def parseargs(runArgs,safefactor=1.1,nevents=10000):
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

options = parseargs(safefactor=1.1,runArgs=runArgs)



if options["runNumber"] not in [343256]:
  raise RuntimeError("runNumber {0:d} not recognised in these jobOptions. Available options are: 343256".format(options["runNumber"]))



#---------------------------------------------------------------------------------------------------
# generating process cards in HC_NLO_X0_UFO model
#---------------------------------------------------------------------------------------------------

def writeproccard(content):
    fcard = open('proc_card_mg5.dat','w')
    fcard.write(content)
    fcard.close()


#---------------------------------------------------------------------------------------------------
# generating vbfhzz4l
#---------------------------------------------------------------------------------------------------

if options["runNumber"] == 343256:
  writeproccard(content="""import model HC_NLO_X0_UFO 
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
generate p p > j j x0 , x0 > l+ l- l+ l- QCD=0
output PROC_vbfhzz4l""")




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

writeparamcard(masses={'25': '1.250000e+02'},options=options,parameters={'frblock': {'kAbb': 0.0, 'kHaa': 0.0, 'kAaa': 0.0, 'cosa': 0.7071067811865475, 'kHdwR': 0.0, 'kAza': 0.0, 'kAll': 0.0, 'kHbb': 0.0, 'kHll': 0.0, 'kAzz': -2.5, 'kSM': 1.0, 'kHdwI': 0.0, 'kHdz': 0.0, 'kAww': -2.5, 'kAtt': 0.0, 'kHda': 0.0, 'kHza': 0.0, 'kHtt': 0.0, 'kHww': 0.0, 'kHzz': 0.0, 'Lambda': 1000.0}})




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
# creating run_card.dat for vbfhzz4l
#---------------------------------------------------------------------------------------------------

if options["runNumber"] == 343256:
  writeruncard(extras={'mmaamax': '-1', 'auto_ptj_mjj': 'F', 'ptl': '0', 'ptj': '0', 'drll': '0', 'ptb': '0', 'pta': '0', 'drbbmax': '-1', 'draamax': '-1', 'ptbmax': '-1', 'etal': '-1', 'mmaa': '0', 'drjjmax': '-1', 'etajmin': '0', 'ptl1min': '-1', 'dsqrt_q2fact2': '125', 'dsqrt_q2fact1': '125', 'bwcutoff': '50', 'cut_decays': 'T', 'drbjmax': '-1', 'ptl2min': '-1', 'lhe_version': '2.0', 'drabmax': '-1', 'etaonium': '-1', 'lhaid': '247000', 'drjj': '0', 'drbj': '0', 'ptonium': '0', 'drbb': '0', 'scale': '125', 'etaa': '-1', 'etab': '-1', 'drjl': '0', 'etaj': '-1', 'drajmax': '-1', 'ptamax': '-1', 'pdlabel': "'lhapdf'", 'parton_shower': 'PYTHIA8', 'etaamin': '0', 'draa': '0', 'drab': '0', 'dral': '0', 'mmbbmax': '-1', 'draj': '0', 'etabmin': '0', 'ptjmax': '-1', 'mmbb': '0'},options=options)

#---------------------------------------------------------------------------------------------------
# Calling Madgraph
#---------------------------------------------------------------------------------------------------

def callgenerate(options):
    from MadGraphControl.MadGraphUtils import print_cards,generate,arrange_output
    print_cards()
    generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=options["mode"],proc_dir=options["processdir"],run_name=options["runName"])
    arrange_output(run_name=options["runName"],proc_dir=options["processdir"],outputDS=options["runName"]+'._00001.events.tar.gz')

callgenerate(options=options)



#---------------------------------------------------------------------------------------------------
# Pythia8 Showering with A14_NNPDF23LO
#---------------------------------------------------------------------------------------------------

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")






evgenConfig.keywords = [ 'dijet','2jet','4lepton','4electron','4muon','BSM','diboson','Higgs','SM','scalar','VBF','VBFHiggs','proton','antiproton','resonance','inclusive','BSMHiggs','SMHiggs','spin0']
evgenConfig.contact = ['Katharina Ecker <Katharina.Maria.Ecker@cern.ch>']
evgenConfig.generators = ['MadGraph', 'Pythia8', 'EvtGen']
evgenConfig.description = """VBF 125 GeV Higgs production in the Higgs Characterization model decaying to zz4l."""
evgenConfig.inputfilecheck = options['runName']




#---------------------------------------------------------------------------------------------------
# For debugging purposes, test this joboption with the following command line:
# Generate_tf.py --ecmEnergy=13000 --firstEvent=1 --randomSeed=1 --jobConfig=MC15.343256.MadGraphPythia8EvtGen_A14NNPDF23LO_vbfhzz4l_0pphmkAvm2p5.py --outputEVNTFile=EVNT.00001.root --runNumber=343256 --maxEvents=10
#---------------------------------------------------------------------------------------------------

