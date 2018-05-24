# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

# Set up the reading of the input file:
import AthenaRootComps.ReadAthenaxAODHybrid
theApp.EvtMax = 500
testFile = os.getenv ('ASG_TEST_FILE_DATA')
svcMgr.EventSelector.InputCollections = [testFile]

# Access the main algorithm sequence of the job:
from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now
dataType = "data"
#dataType = "mc"
#dataType = "afii"

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SysListLoaderAlg( 'SysLoaderAlg' )
sysLoader.sigmaRecommended = 1
algSeq += sysLoader

# Include, and then set up the muon analysis algorithm sequence:
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence( dataType )
muonSequence.configure( inputName = 'Muons', outputName = 'AnalysisMuons' )

# Set all algorithms in the sequence to debug output:
for alg in muonSequence:
    alg.OutputLevel = 1

# Allow the histogram writer algorithm to work correctly:
muonSequence.MuonCutFlowDumperAlg.RootStreamName = "/MUONTEST"
muonSequence.MuonKinematicDumperAlg.RootStreamName = "/MUONTEST"
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [
    "MUONTEST DATAFILE='MuonAnalysisAlgorithmsTest.hist.root' OPT='RECREATE'"
    ]

# Print the job configuration for debugging:
print( str( muonSequence ) )

# Add the sequence to the job:
algSeq += muonSequence

# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
