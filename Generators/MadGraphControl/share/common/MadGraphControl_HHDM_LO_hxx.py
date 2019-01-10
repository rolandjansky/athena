from MadGraphControl.MadGraphUtils import *

#-----------------------------------------------------------------------------------------------
# Preliminaries
#-----------------------------------------------------------------------------------------------
efficiency = 1.0
safety_fac = 1.1
nevents = int(5000*safety_fac/efficiency)

def get_param_config(dsid):
    switcher = {
        342246: "MR275MX55BR50",
        342247: "MR275MX55BR60",
        342248: "MR275MX55BR70",
        342249: "MR275MX60BR50",
        342250: "MR275MX60BR60",
        342251: "MR275MX60BR70",
        342252: "MR285MX55BR50",
        342253: "MR285MX55BR60",
        342254: "MR285MX55BR70",
        342255: "MR285MX60BR50",
        342256: "MR285MX60BR60",
        342257: "MR285MX60BR70",
    }
    return switcher.get(dsid, "invalid_dsid")

mode=0
dsid_list=[342246,342247,342248,342249,342250,342251,342252,342253,342254,342255,342256,342257]

#-----------------------------------------------------------------------------------------------
# Create Process
#-----------------------------------------------------------------------------------------------

fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in dsid_list:
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    import model HHDM_intMET
    generate p p > h2, h2 > h n1 n1
    output -f""")
    fcard.close()
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

#-----------------------------------------------------------------------------------------------
# Card settings
#-----------------------------------------------------------------------------------------------
extras = { 'lhe_version':'2.0',
           'cut_decays':'F',
           'pdlabel':"'nn23lo1'",
           'parton_shower':'PYTHIA8' }
build_run_card(run_card_old='MadGraph_run_card_HHDM_def.dat',run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

print_cards()

#-----------------------------------------------------------------------------------------------
# Generation
#-----------------------------------------------------------------------------------------------

runName = get_param_config(runArgs.runNumber)

process_dir = new_process()
pc_dir = 'MadGraph_param_card_' + get_param_config(runArgs.runNumber) + '.dat';
generate(run_card_loc='run_card.dat',param_card_loc=pc_dir,mode=mode,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

#-----------------------------------------------------------------------------------------------
# Pythia8 Showering Dependencies
#-----------------------------------------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#-----------------------------------------------------------------------------------------------
# EVGEN
#-----------------------------------------------------------------------------------------------

evgenConfig.description = "Higgs production in association with DM through a heavy scalar"
evgenConfig.keywords = ["Higgs", "BSMHiggs", "diphoton"]
evgenConfig.contact = ['Stefan von Buddenbrock <stef.von.b@cern.ch>']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

#-----------------------------------------------------------------------------------------------
# Pythia8 Showering Commands
#-----------------------------------------------------------------------------------------------

genSeq.Pythia8.Commands += ["1000022:mayDecay no"] #switch off DM decays
genSeq.Pythia8.Commands += ["35:mayDecay no"] #switch off heavy scalar decays
genSeq.Pythia8.Commands += ["1000022:isVisible no"] #make DM invisible
genSeq.Pythia8.Commands += ["25:oneChannel = 1 1 0 22 22"] #gamma gamma decay
