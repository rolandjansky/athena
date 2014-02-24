#--------------------------------------------------------------
# Example JobOption to run MCTester on a McEventColletion
#--------------------------------------------------------------

include( "PartPropSvc/PartPropSvc.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#use glob to get a list of files in a directory
import glob
files=glob.glob("director_with_files/*.root")
svcMgr.EventSelector.InputCollections = files

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from MCTester_i.MCTester_iConf import MCTesterAlg
job += MCTesterAlg( "MCTester_i" )

MCTesterAlg.McEventKey="GEN_EVENT" #"TruthEvent" for RDOs and ESDs
MCTesterAlg.result_path="myFileName1.root" #output filename
MCTesterAlg.decay_particle=15 #particle pdgID to be examined
MCTesterAlg.nbins=100 #number of histogram bins
MCTesterAlg.bin_min=0.0 #bin minimum (MeV)
MCTesterAlg.bin_max=2000.0 #bin maximum (MeV)
MCTesterAlg.collision_check_sum_margin=1 #margin of allowance in 4 momentum check (MeV)

#Three lines of description to go in the booklet
MCTesterAlg.generation_description_line1="Dataset: My dataset name"
MCTesterAlg.generation_description_line2="Some description of the events here"
MCTesterAlg.generation_description_line3=""

#include particle -> particle + gamma decays?
MCTesterAlg.include_decays_to_self='false' 
                            
MCTesterAlg.OutputLevel = INFO #Output level. Should be at least INFO for validation checks

theApp.EvtMax = 10 #Number of events to process
#==============================================================


