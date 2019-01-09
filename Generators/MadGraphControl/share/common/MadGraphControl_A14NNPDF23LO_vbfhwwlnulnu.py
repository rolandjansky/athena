from MadGraphControl.MadGraphUtils import *

mode=0

#---------------------------------------------------------------------------------------------------
# Setting EFT parameters in HC model
#---------------------------------------------------------------------------------------------------
parameters={
	'frblock':{ 
		'Lambda':'1.000000e+03',
		'cosa':  '1.000000e+00',
		'kSM':   '1.000000e+00',
		'kHtt':  '0.000000e+00',
		'kAtt':  '0.000000e+00',
		'kHbb':  '0.000000e+00',
		'kAbb':  '0.000000e+00',
		'kHll':  '0.000000e+00',
		'kAll':  '0.000000e+00',
		'kHaa':  '0.000000e+00',
		'kAaa':  '0.000000e+00',
		'kHza':  '0.000000e+00',
		'kAza':  '0.000000e+00',
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
# Setting higgs mass to 125 GeV for param_card.dat
#---------------------------------------------------------------------------------------------------
higgsMass={'25':'1.250000e+02'} #MH

#---------------------------------------------------------------------------------------------------
# Setting some parameters for run_card.dat
#---------------------------------------------------------------------------------------------------
extras = { 'lhe_version':'2.0', 
           'pdlabel':"'lhapdf'",
           'lhaid':'247000',
           'scale':'125',
           'dsqrt_q2fact1':'125',
           'dsqrt_q2fact2':'125',     
           'auto_ptj_mjj':'F',
           'parton_shower':'PYTHIA8',
           'bwcutoff':'50',
           'cut_decays':'T', 
           'ptj':'0',
           'ptb':'0',
           'pta':'0',
           'ptl':'0',
	   'ptl1min':'15',
	   'ptl2min':'5',
           'ptjmax':'-1',
           'ptbmax':'-1',
           'ptamax':'-1',
           'etaj':'-1',
           'etab':'-1',
           'etaa':'-1',
	   'etal':'2.6',
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
           'drll':'0',
           'drjl':'0',
           'dral':'0',
           'drjjmax':'-1',
           'drbbmax':'-1',
           'draamax':'-1',
           'drbjmax':'-1',
           'drajmax':'-1',
           'drabmax':'-1',
           'ptonium':'0',
           'etaonium':'-1'}

#---------------------------------------------------------------------------------------------------
# Generating VBF hwwlnulnu in HC model
#---------------------------------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
if (runArgs.runNumber == 342570):
	fcard.write("""
        import model HC_NLO_X0_UFO
        define p = g u c d s u~ c~ d~ s~
        define j = g u c d s u~ c~ d~ s~
        generate    p p > j j x0, x0 > w+ w- > e+ ve mu- vm~
        add process p p > j j x0, x0 > w+ w- > mu+ vm e- ve~
        add process p p > j j x0, x0 > w+ w- > e+ ve ta- vt~
        add process p p > j j x0, x0 > w+ w- > mu+ vm ta- vt~
        add process p p > j j x0, x0 > w+ w- > ta+ vt e- ve~
        add process p p > j j x0, x0 > w+ w- > ta+ vt mu- vm~
        add process p p > j j x0, x0 > w+ w- > ta+ vt ta- vt~
	output -f""")
	fcard.close()
else: 
	raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
	beamEnergy = runArgs.ecmEnergy / 2.
else: 
	raise RuntimeError("No center of mass energy found.")

#---------------------------------------------------------------------------------------------------
# Setting the number of generated events to 'safefactor' times maxEvents,
# to avoid crashing due to not having enough events
# Also putting protection to avoid from crashing when maxEvents=-1
#---------------------------------------------------------------------------------------------------
safefactor = 1.1
nevents    = 10000*safefactor
if runArgs.maxEvents > 0:
	nevents=runArgs.maxEvents*safefactor

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
# Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used
#---------------------------------------------------------------------------------------------------
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras,xqcut=0.)

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
# Used values given in "parameters" for EFT parameters, if not set there, default values are used
# Higgs mass is set to 125 GeV by "higgsMass"
#---------------------------------------------------------------------------------------------------
runName='run_01'
if runArgs.outputTXTFile:
	runName=runArgs.outputTXTFile

process_dir = new_process()
build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',masses=higgsMass,params=parameters)
print_cards()
generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

#---------------------------------------------------------------------------------------------------
# Pythia8 Showering with A14_NNPDF23LO, tau decay only leptonically
#---------------------------------------------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["15:onMode=off",
                            "15:onIfAny=11 13"]

#---------------------------------------------------------------------------------------------------
# EVGEN Configuration
#---------------------------------------------------------------------------------------------------
evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
if (runArgs.runNumber == 342570):
	evgenConfig.description = "VBF 125 GeV Higgs production in the Higgs Characterization model decaying to wwlnulnu."
	evgenConfig.keywords = ['BSM','Higgs','VBF','VBFHiggs','mH125','BSMHiggs','2lepton','dijet','resonance','WW']

evgenConfig.contact = ['Adam Kaluza <Adam.Kaluza@cern.ch>']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
