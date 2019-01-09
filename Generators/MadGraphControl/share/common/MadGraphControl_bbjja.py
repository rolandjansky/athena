from MadGraphControl.MadGraphUtils import *

# General settings
nevents = 5500
gridpack_mode = True
gridpack_dir = 'madevent/'

if runArgs.runNumber in [343388]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > h a j j"
    process     = "ppTOHbbajj_SM"
    description = "VBF and VH Higgs production with an additional photon"
    keywords    = ['Higgs', 'bottom', 'VBFHiggs', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.343388.Hbbajj_SM.TXT.mc15_v1.tar.gz'

elif runArgs.runNumber in [343389]:
    model       = "heft"
    mg_proc     = "generate p p > h a j j"
    process     = "ppTOHbbajj_HEFT"
    description = "ggF and other Higgs production with an additional photon in the HEFT model : gluon and photon Higgs couplings"
    keywords    = ['Higgs', 'bottom', 'gluonFusionHiggs', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.343389.Hbbajj_HEFT.TXT.mc15_v1.tar.gz'

elif runArgs.runNumber in [343390]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > z a j j QCD=0, z > b b~"
    process     = "ppTOZbbajj_EWK"
    description = "Z+Gamma production with two additional jets and no QCD couplings"
    keywords    = ['Z', 'bottom', 'VBF', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.343390.Zbbajj_EWK.TXT.mc15_v1.tar.gz'

elif runArgs.runNumber in [343391]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > z a j j QED=2 QCD=10, z > b b~"
    process     = "ppTOZbbajj_QCD"
    description = "Z+Gamma production with two additional jets and at most two QED couplings"
    keywords    = ['Z', 'bottom', 'QCD', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.343391.Zbbajj_QCD.TXT.mc15_v1.tar.gz'

elif runArgs.runNumber in [343392]:
    model       = "sm-no_b_mass"
    model       = "sm"
    mg_proc     = "generate p p > b b~ j j a $ z h"
    process     = "ppTObbajj_nonRes"
    description = "Non-resonant production of two b-jets two additional jets and a photon"
    keywords    = ['bottom', 'photon', 'QCD']
    gridpack    = 'group.phys-gener.MGPy8EG.343392.bbajj_nonRes.TXT.mc15_v1.tar.gz'

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


if hasattr(runArgs, 'inputGenConfFile'):
    if not gridpack in runArgs.inputGenConfFile:
        raise RuntimeError("Please use the correct gridpack for this DSID.")

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(process)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model %s
""" %model)
if "no_b_mass" in model:
    fcard.write("""\ndefine p = j b b~\ndefine j = p""")
fcard.write("""
%s
output %s
""" %(mg_proc, process))
fcard.close()

process_dir = new_process(grid_pack=gridpack_dir)

extras = { 'pdlabel' : 'lhapdf',
           'lhaid'   : str(("no_b_mass" in model)*263000 + (model=="sm")*263400 + (model=="heft")*263400),
           'ptj'     : '15.0',
           'ptb'     : '15.0',
           'pta'     : '15.0',
           'etaj'    : '-1.0',
           'etaa'    : '3.0',
           'drbb'    : '0.4',
           'drbj'    : '0.4',
           'drab'    : '0.4',
           }

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), 
               run_card_new='run_card.dat',
               xqcut=0,
               nevts=nevents,
               rand_seed=runArgs.randomSeed,
               beamEnergy=beamEnergy,
               extras=extras)

generate(run_card_loc='run_card.dat', param_card_loc=None, 
         mode=0, njobs=1, proc_dir=process_dir,
         grid_pack=gridpack_mode, gridpack_dir=gridpack_dir,
         nevents=nevents, random_seed=runArgs.randomSeed)

arrange_output(proc_dir=process_dir, outputDS=stringy+'._00001.events.tar.gz')

#### Shower
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts


evgenConfig.description = description
evgenConfig.keywords = keywords
evgenConfig.process = process
evgenConfig.generators = ['MadGraph', 'Pythia8']
evgenConfig.inputfilecheck = stringy
evgenConfig.contact = ["prose@ucsc.edu"]

runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


if runArgs.runNumber in [343388, 343389]:
    include("Pythia8_i/Pythia8_SMHiggs125_inc.py")
    genSeq.Pythia8.Commands += ["23:onMode = off",
                                "23:onIfMatch = 5 5"]
