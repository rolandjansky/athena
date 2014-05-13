# +===================================================================================+
# +                                                                                   +
# + Author ........: F. Ledroit                                                       +
# + Institut ......: LPSC Grenoble                                                    +
# + Creation date .: 16/12/2004      Version 0.01                                     +
# + Subject .......: Job Option file to test the writing of pool persistified LArTTL1 +
# +===================================================================================+ 

# to read pool Data (hits)
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# G4 Pool input 
# make sure the file listed is in PoolFileCatalog.xml in run directory
# AND that you have a copy or soft link of this file in the run directory
# it is possible to specify a list of files to be processed consecutively
EventSelector = Service( "EventSelector" )

#EventSelector.InputCollection = "$INFN";
#EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/datafiles/dc2/simul/dc2.003026.simul.A0_top/dc2.003026.simul.A0_top._00001.pool.root.1","rfio:/castor/cern.ch/grid/atlas/datafiles/dc2/simul/dc2.003026.simul.A0_top/dc2.003026.simul.A0_top._00002.pool.root.1"]
#EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/c/costanzo/validation/data/dc2val.e_et25_eta25.006002.g4sim/dc2val.e_et25_eta25.006002.g4sim.0000.root","rfio:/castor/cern.ch/user/c/costanzo/validation/data/dc2val.e_et25_eta25.006002.g4sim/dc2val.e_et25_eta25.006002.g4sim.0001.root"]
EventSelector.InputCollections = ["/afs/cern.ch/user/f/fledroit/scratch0/castor/dc2val.e_et25_eta25.006002.g4sim.0000.root"]

# Number of events to be processed (default is 10)
theApp.EvtMax = 3

include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

include( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )

# Pool Output
Stream1 = Algorithm( "Stream1" )
theApp.OutStream     +=["Stream1"]
theApp.OutStreamType ="AthenaOutputStream"
Stream1.EvtConversionSvc     ="AthenaPoolCnvSvc"
Stream1.OutputFile =   "TTL1PoolFile2.root"
Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["LArTTL1Container#*"]

