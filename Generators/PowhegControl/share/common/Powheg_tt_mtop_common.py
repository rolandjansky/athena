#---------------------------------------------------------------------------------------------------
# Common generator script for b-filtered non-standard top mass samples
#---------------------------------------------------------------------------------------------------
def parseargs(runArgs):
    # function for parsing the Generate_tf.py arguments
    options = {}
    if not hasattr(runArgs,'runNumber'):
        raise RuntimeError('no runNumber supplied as argument')
    else: 
        options['runNumber'] = runArgs.runNumber

    if hasattr(runArgs,'ecmEnergy'):
        options['beamEnergy'] = runArgs.ecmEnergy / 2.
    else: 
        raise RuntimeError('No center of mass energy found.')

    if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0:
        options['nevents']=runArgs.maxEvents
    else:
        raise RuntimeError('No maxEvents provided.')

    if hasattr(runArgs,'randomSeed'):
        options['randomSeed'] = runArgs.randomSeed
    else:
        raise RuntimeError('No random seed provided.')

    if hasattr(runArgs,'inputGeneratorFile'):
        options['lheFile'] = runArgs.inputGeneratorFile
    else:
        options['lheFile'] = ''
    return options

# get arguments
options = parseargs(runArgs=runArgs)

# get showering algorithm based on DSID
herwigDSIDs = range(411125,411143)
isHerwig = options['runNumber'] in herwigDSIDs
pythiaDSIDs = range(411143,411162)
isPythia = options['runNumber'] in pythiaDSIDs
if not isHerwig and not isPythia:
  raise RuntimeError('runNumber {0:d} not recognised in these jobOptions.'.format(options['runNumber']))

# get top mass and width based on DSID
dsidModEighteen = (options['runNumber']-411125) % 18

if dsidModEighteen==0 or dsidModEighteen==1:
    mtop = 169.0
    widthtop = 1.228
elif dsidModEighteen==2 or dsidModEighteen==3:
    mtop = 171.0
    widthtop = 1.280
elif dsidModEighteen==4 or dsidModEighteen==5:
    mtop = 172.0
    widthtop = 1.306
elif dsidModEighteen==6 or dsidModEighteen==7:
    mtop = 172.25
    widthtop = 1.313
elif dsidModEighteen==8 or dsidModEighteen==9:
    mtop = 172.5
    widthtop = 1.320
elif dsidModEighteen==10 or dsidModEighteen==11:
    mtop = 172.75
    widthtop = 1.326
elif dsidModEighteen==12 or dsidModEighteen==13:
    mtop = 173.0
    widthtop = 1.333
elif dsidModEighteen==14 or dsidModEighteen==15:
    mtop = 174.0
    widthtop = 1.360
elif dsidModEighteen==16 or dsidModEighteen==17:
    mtop = 176.0
    widthtop = 1.415

# get b or anti-b based on DSID
if options['runNumber'] % 2 == 0:
    isbottom = True
else:
    isbottom = False

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.keywords    = [ 'SM', 'top', 'ttbar', 'lepton', 'Jpsi' ]
evgenConfig.contact     = [ 'derue@lpnhe.in2p3.fr', 'burton@utexas.edu' ]
evgenConfig.generators += [ 'Powheg', 'EvtGen' ]
evgenConfig.minevents = 2000
if isPythia:
    evgenConfig.generators += ['Pythia8']
    evgenConfig.description = 'POWHEG+Pythia8 ttbar production with Powheg hdamp equal 1.5*top mass, \
                               A14 tune, A14 NNPDF23 LO, ME NNPDF30 NLO, at least one lepton, \
                               Jpsi->mumu filter.'
if isHerwig:
    evgenConfig.generators += ['Herwig7']
    evgenConfig.tune        = "MMHT2014"
    evgenConfig.description = 'POWHEG+Herwig704 ttbar production with Powheg hdamp equal 1.5*top mass, \
                               H7UE tune, single lepton filter, ME NNPDF30 NLO, H7UE MMHT2014 LO at \
                               least one lepton, Jpsi->mumu filter.'

#--------------------------------------------------------------
# Powheg matrix element
#--------------------------------------------------------------
if options['lheFile'] is '':
    include('PowhegControl/PowhegControl_tt_Common.py')
    # Initial settings
    PowhegConfig.decay_mode = "t t~ > all" # inclusive decay
    PowhegConfig.mass_t     = mtop
    PowhegConfig.width_t    = widthtop
    PowhegConfig.hdamp      = 1.5*mtop
    PowhegConfig.PDF        = 260000 # NNPDF30
    PowhegConfig.nEvents   *= 10.    # compensate filter efficiency
    PowhegConfig.generate()

#--------------------------------------------------------------
# Showering
#--------------------------------------------------------------
if isPythia: # Pythia8 (A14) showering
    include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
    include("Pythia8_i/Pythia8_Powheg_Main31.py")
    genSeq.Pythia8.Commands += [ 'Powheg:pTHard = 0' ]
    genSeq.Pythia8.Commands += [ 'Powheg:NFinal = 2' ]
    genSeq.Pythia8.Commands += [ 'Powheg:pTdef = 2' ]
    genSeq.Pythia8.Commands += [ 'Powheg:veto = 1' ]
    genSeq.Pythia8.Commands += [ 'Powheg:vetoCount = 3' ]
    genSeq.Pythia8.Commands += [ 'Powheg:pTemt  = 0' ]
    genSeq.Pythia8.Commands += [ 'Powheg:emitted = 0' ]
    genSeq.Pythia8.Commands += [ 'Powheg:MPIveto = 0' ]

elif isHerwig: # Herwig7 (H7UE) showering
    include("Herwig7_i/Herwig7_LHEF.py")
    include('Herwig7_i/Herwig7_EvtGen.py')
    Herwig7Config.me_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")
    Herwig7Config.tune_commands()
    Herwig7Config.lhef_powhegbox_commands(lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")
    Herwig7Config.add_commands("""
set /Herwig/Shower/LtoLGammaSudakov:pTmin 0.000001
set /Herwig/Shower/QtoGammaQSudakov:Alpha /Herwig/Shower/AlphaQED""")
    Herwig7Config.run()

#--------------------------------------------------------------
# Special decay of anti-B->Jpsi->mumu
#--------------------------------------------------------------
from EvtGen_i.EvtGen_iConf import EvtInclusiveDecay
if isbottom:
    evgenConfig.auxfiles += ['B2Jpsimumu.DEC']
    genSeq.EvtInclusiveDecay.userDecayFile = 'B2Jpsimumu.DEC'
else:
    evgenConfig.auxfiles += ['AntiB2Jpsimumu.DEC']
    genSeq.EvtInclusiveDecay.userDecayFile = 'AntiB2Jpsimumu.DEC'

#--------------------------------------------------------------
# Event filters
#--------------------------------------------------------------
# apply a non all-hadronic decay filter
include('GeneratorFilters/TTbarWToLeptonFilter.py')
filtSeq.TTbarWToLeptonFilter.NumLeptons = -1
filtSeq.TTbarWToLeptonFilter.Ptcut = 0.

# apply a J/psi to muons filter
include('GeneratorFilters/TTbarWithJpsimumuFilter.py')
filtSeq.TTbarWithJpsimumuFilter.JpsipTMinCut = 5000.