# +===========================================================================+
# +                                                                           +
# + Author ........: F. Ledroit                                               +
# + Institut ......: ISN Grenoble                                             +
# + Creation date .: 01/12/2005      Version 0.01                             +
# + Subject .......: Job Option file to test LArTTL1Maker on Cosmics          +
# +===========================================================================+ 
# Top Level JobOptions to test LArTTl1Maker algorithm.
# Input is from LArHits (simulation)

# to read pool Data
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# G4 Pool input 
# make sure the file listed is in PoolFileCatalog.xml in run directory
# AND that you have a copy or soft link of this file in the run directory
# it is possible to specify a list of files to be processed consecutively
EventSelector = Service( "EventSelector" )

EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/h/hma/g4_cosmic/comm.004920/simul/comm.004920.simul.cosmic._00000.pool.root"]
#EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/f/fledroit/e-gamma_private/rome.007777.simul.piminus_pt600_eta165._00000.pool.root"]

# Number of events to be processed (default is 10)
theApp.EvtMax = 3

include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

include( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#MessageSvc.OutputLevel               = 2
#LArTTL1Maker.OutputLevel               = 2

#MessageSvc.debugLimit = 100000

#
# ............ if true, noise added in all subdetectors (default = true)
#
#LArTTL1Maker.NoiseOnOff = FALSE
#
# ............ if true, chrono monitoring is enabled (default = false)
#
#LArTTL1Maker.ChronoTest = TRUE
#
# ............ transverse energy threshold for debug printout (default = 5000 MeV)
#
#LArTTL1Maker.DebugThreshold = 1000.
#
# ..................... if true, retrieve trigger time (and subtract) (default = false)
#.......................this option is inactive (because useless) when PileUp is true
#
LArTTL1Maker.UseTriggerTime = TRUE
theApp.Dlls += ["CommissionUtils"]
