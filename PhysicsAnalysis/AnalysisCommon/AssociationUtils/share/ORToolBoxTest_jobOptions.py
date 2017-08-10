#
# This job options file defines an Athena testing job for the OverlapRemovalTool
#

import os
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence
from AssociationUtils.AssociationUtilsConf import ORToolBoxTestAlg

# Specify input file
input_file = os.getenv('ASG_TEST_FILE_MC')
ServiceMgr.EventSelector.InputCollections = [input_file]

# We just need to process a handful of events
theApp.EvtMax=5

# Access the algorithm sequence
theJob = AlgSequence()

# Configure the test algorithm
alg = ORToolBoxTestAlg('ORToolBoxTestAlg')

# Add to the alg sequence
theJob += alg

# I hate 'including' in python, but this serves to reduce unnecessary output
include("AthAnalysisBaseComps/SuppressLogging.py")
