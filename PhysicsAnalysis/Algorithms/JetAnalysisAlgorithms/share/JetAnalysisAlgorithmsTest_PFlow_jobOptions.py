# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

# User options, which can be set from command line after a "-" character
# athena EgammaAlgorithmsTest_jobOptions.py - --myOption ...
from AthenaCommon.AthArgumentParser import AthArgumentParser
athArgsParser = AthArgumentParser()
athArgsParser.add_argument("--data-type", action = "store", dest = "data_type",
                           default = "data",
                           help = "Type of input to run over. Valid options are 'data', 'mc', 'afii'")
athArgsParser.add_argument( "--old-file", dest = "old_file",
                            action = "store_true", default = False,
                            help = "Use the old ASG test file.")
athArgs = athArgsParser.parse_args()

dataType = athArgs.data_type
useOldFile = athArgs.old_file
if not dataType in ["data", "mc", "afii"] :
    raise Exception ("invalid data type: " + dataType)

print("Running on data type: " + dataType)

if not useOldFile :
    inputfile = {"data": 'ASG_TEST_FILE_DATA',
                 "mc":   'ASG_TEST_FILE_MC',
                 "afii": 'ASG_TEST_FILE_MC_AFII'}
else :
    inputfile = {"data": 'ASG_TEST_FILE_DATA_OLD',
                 "mc":   'ASG_TEST_FILE_MC_OLD',
                 "afii": 'ASG_TEST_FILE_MC_AFII_OLD'}

jetContainer = "AntiKt4EMTopoJets"

# Set up the reading of the input file:
import AthenaRootComps.ReadAthenaxAODHybrid
theApp.EvtMax = 500
testFile = os.getenv ( inputfile[dataType] )
svcMgr.EventSelector.InputCollections = [testFile]

from JetAnalysisAlgorithms.JetAnalysisAlgorithmsTest import makeSequence
algSeq = makeSequence (dataType, jetContainer)
print algSeq # For debugging

# Add all algorithms from the sequence to the job.
athAlgSeq += algSeq

# Set up a histogram output file for the job:
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [
    "ANALYSIS DATAFILE='JetAnalysisAlgorithmsTestPFlow." + dataType + ".hist.root' OPT='RECREATE'"
    ]

# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
