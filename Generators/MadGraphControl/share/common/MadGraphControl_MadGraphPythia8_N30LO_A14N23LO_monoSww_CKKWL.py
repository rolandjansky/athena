from MadGraphControl.MadGraphUtils import *
import math

safefactor=2.

# define process and specify W decay modes (**all** decays, **had**ronic decays (no b-quarks), **semilep**tonic decays)
# + number of additional partons jets
# default to "had" if nothing specified
try: assert process in ["all", "had", "semilep"]
except NameError: 
    print("process not set, defaulting to 'had'")
    process = "had"

# default to "01" (meaning 0 + 1 parton in matrix element) if nothing specified
try: assert jets in ["01", "0", "1"]
except NameError:
    print("jets variable not set, defaulting to '01'")
    jets = "01"
# ----------------------------------------------------------------------------
# Full decay of W bosons
# ----------------------------------------------------------------------------
if process == "all":
    process_string = """
# define standard model particles W decay products (e, mu, tau, neutrinos, jets, b-jets)
define wdecay = j b b~ l+ l- ta+ ta- vl vl~
"""
    if jets == "01":
        process_string += """
generate p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w+ > wdecay wdecay, w- > wdecay wdecay) @0
add process p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w+ > wdecay wdecay, w- > wdecay wdecay) @1
"""
    elif jets == "0":
        process_string += """
generate p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w+ > wdecay wdecay, w- > wdecay wdecay) @0
"""
    elif jets == "1":
        process_string += """
generate p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w+ > wdecay wdecay, w- > wdecay wdecay) @1
"""
# ----------------------------------------------------------------------------
# Hadronic decay of W bosons
# ----------------------------------------------------------------------------
elif (process == "had"):
    process_string = ""
    if jets == "01":
        process_string = """
generate p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w+ > j j, w- > j j) @0
add process p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w+ > j j, w- > j j) @1
"""
    elif jets == "0":
        process_string = """
generate p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w+ > j j, w- > j j) @0
"""
    elif jets == "1":
        process_string = """
generate p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w+ > j j, w- > j j) @1
"""
# ----------------------------------------------------------------------------
# Semileptonic decay of W bosons
# ----------------------------------------------------------------------------
elif (process == "semilep"):
    process_string = """
define lepton = l+ l-
define neutrino = vl vl~
"""
    if jets == "01":
        process_string = """
generate p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w+ > j j, w- > lepton neutrino) @0
add process p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w- > j j, w+ > lepton neutrino) @0
add process p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w+ > j j, w- > lepton neutrino) @1
add process p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w- > j j, w+ > lepton neutrino) @1
"""
    elif jets == "0":
        process_string = """
generate p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w+ > j j, w- > lepton neutrino) @0
add process p p > zp > n1 n1 hs QED<=2, (hs > w+ w-, w- > j j, w+ > lepton neutrino) @0
"""
    elif jets == "1":
        process_string = """
generate p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w+ > j j, w- > lepton neutrino) @1
add process p p > zp > n1 n1 hs j QED<=2, (hs > w+ w-, w- > j j, w+ > lepton neutrino) @1
"""

# write process card
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model DarkHiggs2MDM
{process}
output -f
""".format(process=process_string))
fcard.close()

# require beam energy to be set as argument
beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()

# determine ktdurham cut from dark Higgs mass
# (ktdurham cut sets scale at which event description is split between parton shower and matrix element) 
try:
    ktdurham = int(mhs / 4)
    assert ktdurham > 40
except AssertionError:
    ktdurham = 40

# fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'3.0',
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : 263000,
          'parton_shower' :'PYTHIA8',
          'cut_decays': 'F',
          'event_norm': 'sum',
          'drjj': "0.0",         # required for CKKW-L jet matching
          'ickkw': 0,            # required for CKKW-L jet matching
          'ktdurham': ktdurham,  # required for CKKW-L jet matching
          'dparameter': "0.4"    # required for CKKW-L jet matching
          }

# from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MadGraph5aMCatNLOForAtlas#Problems_with_run_card_dat_in_ne
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', xqcut=0.0,
               nevts=int(runArgs.maxEvents*safefactor),rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
print_cards()

# write parameter card
paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_DarkHiggs2MDM.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_DarkHiggs2MDM.dat', os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat', os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_DarkHiggs2MDM.dat','r')
    newcard = open('param_card.dat','w')

    for line in oldcard:
        if '{__COUPLING_GQ__}' in line:
            newcard.write('   1 %e # gq \n'%(gq))
        elif '{__COUPLING_GX__}' in line:
            newcard.write('   2 %e # gx \n'%(gx))
        elif '{__MIXING_TH__}' in line:
            newcard.write('   3 %e # th \n'%(th))
        elif '{__MASS_MDM__}' in line:
            newcard.write('  1000022 %e # MDM \n'%(mdm))
        elif '{__MASS_MHs__}' in line:
            newcard.write('  54 %e # MHs \n'%(mhs))
        elif '{__MASS_MZp__}' in line:
            newcard.write('  55 %e # MZP \n'%(mzp))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)
# multi-core capability
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

# option: disable TestHepMC
# if hasattr(testSeq, "TestHepMC"):
#     testSeq.remove(TestHepMC())

# showering with Pythia 8
evgenConfig.description = "Dark Higgs (WW) Dark Matter from 2MDM UFO"
evgenConfig.keywords = ["exotic","BSM"]
if (process == "all"):
    evgenConfig.process = "generate p p > zp > n1 n1 hs, (hs > w+ w- > all)"
elif (process == "had"):
    evgenConfig.process = "generate p p > zp > n1 n1 hs, (hs > w+ w- > j j j j)"
elif (process == "semilep"):
    evgenConfig.process = "generate p p > zp > n1 n1 hs, (hs > w+ w- > l vl j j)"
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ["Paul Philipp Gadow <pgadow@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

# Pythia settings: make the dark matter invisible
# syntax: particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["SLHA:allowUserOverride = on",
                            "1000022:all = chi chi 2 0 0 %d 0.0 0.0 0.0 0.0" %(mdm),
                            "1000022:isVisible = false"]

# CKKW-L jet matching
PYTHIA8_nJetMax=1
PYTHIA8_Dparameter=float(extras['dparameter'])
PYTHIA8_Process="guess"
PYTHIA8_TMS=float(extras['ktdurham'])
PYTHIA8_nQuarksMerge=4
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
genSeq.Pythia8.Commands+=["Merging:mayRemoveDecayProducts=on"]
# modification of merging to allow pythia to guess the hard process with "guess" syntax
if "UserHooks" in genSeq.Pythia8.__slots__.keys():
    genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
else:
    genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'

# # MET filter
# include("GeneratorFilters/MissingEtFilter.py")
# filtSeq.MissingEtFilter.METCut = 50*GeV
# filtSeq.Expression = "(MissingEtFilter)"
