#--------------------------------------------------------------
# Powheg ggH_quark_mass_effects setup
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_ggF_H_Common.py')


# Temporarily, we setup Powheg to use some variables to be the same in the Prophecy config. These
# should eventually be taken from the incoming LHE file

# Set Powheg variables, overriding defaults
# Note: width_H will be overwritten in case of CPS.
PowhegConfig.mass_H  = 125.
PowhegConfig.width_H = 0.00407

# Increase number of events requested to compensate for potential Pythia losses
PowhegConfig.nEvents *= 1.02

# ** RDS **
PowhegConfig.nEvents = 100

# PowhegConfig.generateRunCard()
# PowhegConfig.generateEvents()
PowhegConfig.generate(run_card_only=True)

#--------------------------------------------------------------
# configure Prophecy
#--------------------------------------------------------------
include('Prophecy4fControl/ProphecyPowhegCommon.py')

# set name of LHE file - used below in merger
powhegInputFile                          = "TXT.10248297._000705.events"   # runargs.inputGeneratorFile

# runargs.maxEvents number of generated events

# Generate a file for 4e
ProphecyConfig4e.mass_t                  = PowhegConfig.mass_t
ProphecyConfig4e.mass_b                  = PowhegConfig.mass_b
ProphecyConfig4e.mass_c                  = PowhegConfig.mass_c
ProphecyConfig4e.mass_H                  = PowhegConfig.mass_H
# ProphecyConfig4e.nEvents                 = PowhegConfig.nEvents (set by --maxEvents="20")
ProphecyConfig4e.nEvents_weighted        = PowhegConfig.nEvents # shouldn't do this for production?
ProphecyConfig4e.channel                 = "e anti-e e anti-e"
ProphecyConfig4e.output_events_file_name = "ProphecyOTF4e._1.events"

ProphecyConfig4e.generateRunCard()
ProphecyConfig4e.generateEvents()

# Generate a file for 2e2mu
ProphecyConfig2e2mu.mass_t                  = PowhegConfig.mass_t
ProphecyConfig2e2mu.mass_b                  = PowhegConfig.mass_b
ProphecyConfig2e2mu.mass_c                  = PowhegConfig.mass_c
ProphecyConfig2e2mu.mass_H                  = PowhegConfig.mass_H
# ProphecyConfig2e2mu.nEvents                 = PowhegConfig.nEvents (set by --maxEvents="20")
ProphecyConfig2e2mu.nEvents_weighted        = PowhegConfig.nEvents # shouldn't do this for production?
ProphecyConfig2e2mu.channel                 = "e anti-e mu anti-mu"
ProphecyConfig2e2mu.output_events_file_name = 'ProphecyOTF2e2mu._1.events'

ProphecyConfig2e2mu.generateRunCard()
ProphecyConfig2e2mu.generateEvents()

#--------------------------------------------------------------
# configure ProphecyPowhegMerger
#--------------------------------------------------------------

include('Prophecy4fControl/ProphecyPowhegMergeCommon.py')

# Use when using OTF powheg: ProphecyPowhegMerger.input_powhegLHE = PowhegConfig.output_events_file_name
ProphecyPowhegMerger.input_powheg_file_name         = powhegInputFile
ProphecyPowhegMerger.input_prophecy4e_file_name     = ProphecyConfig4e.output_events_file_name
ProphecyPowhegMerger.input_prophecy_2e2mu_file_name = ProphecyConfig2e2mu.output_events_file_name

ProphecyPowhegMerger.merge()

#--------------------------------------------------------------
# Pythia8 showering
# note: Main31 is set in Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py
#--------------------------------------------------------------
include('MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')

#--------------------------------------------------------------
# Pythia8 main31 update
#--------------------------------------------------------------
# genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 1' ]
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 2' ]

#--------------------------------------------------------------
# Higgs at Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += [ '25:onMode = off',    # decay of Higgs
                             '23:onMode = off' ]    # decay of Z

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+PROPHECY+PYTHIA8+EVTGEN, ggH H->llll+X mh=125 GeV CPS"
evgenConfig.keywords    = [ "Higgs", "SMHiggs", "ZZ" ]
evgenConfig.contact     = [ 'jochen.meyer@cern.ch' ]

