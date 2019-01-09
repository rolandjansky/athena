from MadGraphControl.MadGraphUtils import *

# General settings
nevents = 5500
gridpack_mode = True
gridpack_dir = 'madevent/'

# filter settings
useFilter = False
minMjj    = 0


# Nominal VBF + photon
if runArgs.runNumber in [344177]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > h a j j"
    process     = "ppTOHbbajj_SM"
    description = "VBF and VH Higgs production with an additional photon"
    keywords    = ['Higgs', 'bottom', 'VBFHiggs', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.344177.Hbbajj_SM.TXT.mc15_v2.tar.gz'

# VBF + photon with Herwig parton shower
elif runArgs.runNumber in [345425]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > h a j j"
    process     = "ppTOHbbajj_SM"
    description = "VBF and VH Higgs production with an additional photon"
    keywords    = ['Higgs', 'bottom', 'VBFHiggs', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.344177.Hbbajj_SM.TXT.mc15_v2.tar.gz'


# Z + photon (EWK)
elif runArgs.runNumber in [344178]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > z a j j QCD=0, z > b b~"
    process     = "ppTOZbbajj_EWK"
    description = "Z+Gamma production with two additional jets and no QCD couplings"
    keywords    = ['Z', 'bottom', 'VBF', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.344178.Zbbajj_EWK.TXT.mc15_v2.tar.gz'
    #nevents     = 10000

# Z + photon (QCD) 
elif runArgs.runNumber in [344179]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > z a j j QED=2 QCD=10, z > b b~"
    process     = "ppTOZbbajj_QCD"
    description = "Z+Gamma production with two additional jets and at most two QED couplings"
    keywords    = ['Z', 'bottom', 'QCD', 'photon']
    gridpack    = 'group.phys-gener.MGPy8EG.344179.Zbbajj_QCD.TXT.mc15_v2.tar.gz'
    nevents     = 20000
    useFilter   = True
    minMjj      = 500

# background QCD 
elif runArgs.runNumber in [344180]:
    model       = "sm-no_b_mass"
    mg_proc     = "generate p p > b b~ j j a $ z h"
    process     = "ppTObbajj_nonRes"
    description = "Non-resonant production of two b-jets two additional jets and a photon"
    keywords    = ['bottom', 'photon', 'QCD']
    gridpack    = 'group.phys-gener.MGPy8EG.344180.bbajj_nonRes.TXT.mc15_v2.tar.gz'
    nevents     = 20000
    useFilter   = True
    minMjj      = 500

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

extras = { 'lhe_version' : '2.0',
           'cut_decays'  : 'F',
           'pdlabel' : 'lhapdf',
           'lhaid'   : '90500',
           'maxjetflavor' : '5', 
           'ptj'     : '15.0',
           'ptb'     : '15.0',
           'pta'     : '19.0',
           'etaj'    : '-1.0',
           'etaa'    : '3.0',
           'drjj'    : '0.2',
           'drbb'    : '0.2',
           'drbj'    : '0.2',
           'drab'    : '0.2',
           'draj'    : '0.2'
           }

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), 
               run_card_new='run_card.dat',
               xqcut=0,
               nevts=nevents,
               rand_seed=runArgs.randomSeed,
               beamEnergy=beamEnergy,
               extras=extras)

print_cards()

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
evgenConfig.inputfilecheck = stringy
evgenConfig.contact = ["prose@ucsc.edu", "liaoshan.shi@cern.ch"]
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

if runArgs.runNumber in [345425]:
    evgenConfig.generators += [ 'MadGraph', 'Herwig7' ]
    runArgs.inputGeneratorFile='tmp_'+stringy+'._00001.events.tar.gz'
    #--------------------------------------------------------------          
    # Herwig7 showering                                                      
    #--------------------------------------------------------------          
    include("Herwig7_i/Herwig7_701_H7UE_MMHT2014lo68cl_NNPDF3ME_LHEF_EvtGen_Common.py")
    include("Herwig7_i/Herwig7_701_StripWeights.py")
    
    from Herwig7_i import config as hw
    
    genSeq.Herwig7.Commands += hw.powhegbox_cmds().splitlines()
    
    ## only consider H->bb decays                                            
    genSeq.Herwig7.Commands += [
      '## force H->bb decays',
      'do /Herwig/Particles/h0:SelectDecayModes h0->b,bbar;',
      'do /Herwig/Particles/h0:PrintDecayModes' # print out decays modes and branching ratios to the terminal/log.generate
    ]

else:
    evgenConfig.generators = ['MadGraph', 'Pythia8']
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
    include("Pythia8_i/Pythia8_MadGraph.py")


if runArgs.runNumber in [344177]:
    include("Pythia8_i/Pythia8_SMHiggs125_inc.py")
    genSeq.Pythia8.Commands += ["25:onMode = off",
                                "25:onIfMatch = 5 5"]


if useFilter:
    include( 'GeneratorFilters/AntiKt4TruthWZJets.py')
    from GeneratorFilters.GeneratorFiltersConf import VBFForwardJetsFilter
    filtSeq += VBFForwardJetsFilter()
    filtSeq.VBFForwardJetsFilter.JetMinPt = 0.
    filtSeq.VBFForwardJetsFilter.Jet1MinPt = 0.
    filtSeq.VBFForwardJetsFilter.Jet2MinPt = 0.
    filtSeq.VBFForwardJetsFilter.TruthJetContainer = "AntiKt4TruthWZJets"
    filtSeq.VBFForwardJetsFilter.MassJJ = minMjj*1000.
    filtSeq.VBFForwardJetsFilter.DeltaEtaJJ = 0.1
