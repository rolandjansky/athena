#--------------------------------------------------------------
# Powheg ggH_quark_mass_effects setup
#--------------------------------------------------------------

# Must save input gen file names before it is overwritten
input_powheg_files = runArgs.inputGeneratorFile   


include('PowhegControl/PowhegControl_ggF_H_Common.py')


# Temporarily, we setup Powheg to use some variables to be the same in the Hto4l config. These
# should eventually be taken from the incoming LHE file

PowhegConfig.generate(create_run_card_only=True)

#--------------------------------------------------------------
# configure Hto4l
#--------------------------------------------------------------
include('Hto4lControl/Hto4lPowhegCommon.py')


#--------------------------------------------------------------
# configure Hto4lPowhegMerger
#--------------------------------------------------------------

include('Hto4lControl/Hto4lPowhegMergeCommon.py')

# input powheg list of files comes from the input generator file names
hto4lPowhegMerger.input_powheg_file_name = input_powheg_files

# setup input for the individual Hto4l jobs - merge multiple LHE input files, if more than one
# job on the grid submission:
hto4lPowhegMerger.setUpInput()

# Generate a file for 4e
hto4lConfig4e.channel      = "eeee"
hto4lConfig4e.lheInput     = hto4lPowhegMerger.input_powheg_to_hto4l_file_name
hto4lConfig4e.lheOutput    = hto4lPowhegMerger.output_hto4l4e_file_name
hto4lConfig4e.nEvents      = int( runArgs.maxEvents / 4 + 0.5 ) # 1/4 for this channel
hto4lConfig4e.nskip        = int(0)  # no events to skip
hto4lConfig4e.maxDiffWidth = 3.2052962671115008E-002 # from first run with value set to 0
hto4lConfig4e.random_seed  = 123*runArgs.randomSeed # separate random seed for 4e

hto4lConfig4e.generateRunCard()
hto4lConfig4e.generateEvents()

# Generate a file for 4mu
hto4lConfig4mu.channel      = "mmmm"
hto4lConfig4mu.lheInput     = hto4lPowhegMerger.input_powheg_to_hto4l_file_name
hto4lConfig4mu.lheOutput    = hto4lPowhegMerger.output_hto4l4mu_file_name
hto4lConfig4mu.nEvents      = int( runArgs.maxEvents / 4 + 0.5 ) # 1/4 for this channel
hto4lConfig4mu.nskip        = hto4lConfig4e.nEvents # skip 4e events
hto4lConfig4mu.maxDiffWidth = 1.3396654719615330E-002
hto4lConfig4mu.random_seed  = 368*runArgs.randomSeed # separate random seed for 4mu

hto4lConfig4mu.generateRunCard()
hto4lConfig4mu.generateEvents()

# Generate a file for 2e2mu
hto4lConfig2e2mu.channel      = "eemm"
hto4lConfig2e2mu.lheInput     = hto4lPowhegMerger.input_powheg_to_hto4l_file_name
hto4lConfig2e2mu.lheOutput    = hto4lPowhegMerger.output_hto4l2e2mu_file_name
hto4lConfig2e2mu.nEvents      = int( 1.1*(runArgs.maxEvents / 2) ) # 1/2 for this channel + extra 10% for events failing pythia checks
hto4lConfig2e2mu.nskip        = hto4lConfig4e.nEvents + hto4lConfig4mu.nEvents # skip 4e and 4mu events
hto4lConfig2e2mu.maxDiffWidth = 0.12975814265233268
hto4lConfig2e2mu.random_seed  = 278*runArgs.randomSeed # separate random seed for 2e2mu

hto4lConfig2e2mu.generateRunCard()
hto4lConfig2e2mu.generateEvents()


#--------------------------------------------------------------
# run final LHE file merging with Hto4lPowhegMerger
#--------------------------------------------------------------

hto4lPowhegMerger.merge()


#--------------------------------------------------------------
# Pythia8 showering
# note: Main31 is set in Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py
#--------------------------------------------------------------
# include('MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')
include('Pythia8_i/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')

#--------------------------------------------------------------
# Pythia8 main31 update
#--------------------------------------------------------------
# genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 2' ]

if "UserHooks" in genSeq.Pythia8.__slots__.keys():

  genSeq.Pythia8.Commands  += ['Powheg:NFinal = 2']

else:

  genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 2' ]

#--------------------------------------------------------------
# Higgs at Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByL = off",
                            "TimeShower:QEDshowerByGamma = off"]


# genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByOther = off"]

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+HTO4L+PYTHIA8+EVTGEN, ggH H->llll+X mh=125 GeV CPS"
evgenConfig.keywords    = [ "Higgs", "SMHiggs", "ZZ" ]
evgenConfig.contact     = [ 'R.D.Schaffer@cern.ch' ]

evgenConfig.nEventsPerJob = 10000
