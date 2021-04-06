#-------------------------------------------------------------------------------------------------
# Prophecy4f setup with input from PowHeg LHE, merge decays from Prophecy4f, output Pythia8 evtGen
#-------------------------------------------------------------------------------------------------

# need to save the value of inputGeneratorFile into powhegInputFile, since inputGeneratorFile is
# overwritten with an OTF file name in PowhegControl
powhegInputFile                          = runArgs.inputGeneratorFile    # for prod jobs

include('PowhegControl/PowhegControl_ggF_H_Common.py')


# Setup Powheg to use some variables to be the same in the Prophecy config. These
# should eventually be taken from the incoming LHE file

# Set Powheg variables, overriding defaults (RDS: defaults are ok. 2019/02)
# Note: width_H will be overwritten in case of CPS.
# PowhegConfig.mass.H  = 125.
# PowhegConfig.width.H = 0.00407

# Increase number of events requested to compensate for potential Pythia losses
# PowhegConfig.nEvents *= 1.02 

# ** RDS **
# PowhegConfig.nEvents = 100

# PowhegConfig.generateRunCard()
# PowhegConfig.generateEvents()
# PowhegConfig.generate(run_card_only=True)
PowhegConfig.generate(create_run_card_only=True)

#--------------------------------------------------------------
# configure Prophecy
#--------------------------------------------------------------
include('Prophecy4fControl/ProphecyPowhegCommon.py')

# set name of LHE file - used below in merger

print "runArgs", runArgs

# powhegInputFile                          = "TXT.10248297._000705.events"   # for interactive tests
# powhegInputFile                          = runArgs.inputGeneratorFile    # for prod jobs
# powhegInputFile                          = "XXX.TTT" # runArgs.inputGeneratorFile    # for prod jobs

#------------------------------------------------------------------------------------------------------
# configure and run ProphecyPowhegMerger to merge the prophecy4f LHE file with the incoming PowHeg file
#------------------------------------------------------------------------------------------------------

include('Prophecy4fControl/ProphecyPowhegMergeCommon.py')

# # Use when using OTF powheg: ProphecyPowhegMerger.input_powhegLHE = PowhegConfig.output_events_file_name
# ProphecyPowhegMerger.input_powheg_file_name         = powhegInputFile
# # ProphecyPowhegMerger.input_prophecy4e_file_name     = ProphecyConfig4e.output_events_file_name
# # ProphecyPowhegMerger.input_prophecy_2e2mu_file_name = ProphecyConfig2e2mu.output_events_file_name
# ProphecyPowhegMerger.random_seed                    = 431*runArgs.randomSeed # separate random seed four decay modes

# ProphecyPowhegMerger.merge()


# runArgs.inputGeneratorFile  =  ProphecyPowhegMerger.output_events_file_name


#--------------------------------------------------------------
# Pythia8 showering
# note: Main31 is set in Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py
#--------------------------------------------------------------
include('MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')

#--------------------------------------------------------------
# Pythia8 main31 update
#--------------------------------------------------------------
# genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 1' ]

if "UserHooks" in genSeq.Pythia8.__slots__.keys():

  genSeq.Pythia8.Commands  += ['Powheg:NFinal = 2']

else:

  genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 2' ]

#--------------------------------------------------------------
# Higgs at Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByL = off",
                            "TimeShower:QEDshowerByGamma = off",
                            '25:onMode = off',     # decay of Higgs
                            '23:onMode = off' ]    # decay of Z

# genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByOther = off"]

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+PROPHECY+PYTHIA8+EVTGEN, ggH H->llll+X mh=125 GeV CPS"
evgenConfig.keywords    = [ "Higgs", "SMHiggs", "ZZ" ]
evgenConfig.contact     = [ 'R.D.Schaffer@cern.ch' ]

