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

# Include, and then set up the met analysis algorithm sequence:
from MetAnalysisAlgorithms.MetAnalysisSequence import makeMetAnalysisSequence
# Touch the ObjectType enum to trigger dict loading
from ROOT import xAOD
xAOD.Type.ObjectType
metSequence = makeMetAnalysisSequence( dataType, metSuffix="AntiKt4EMTopo",
                                       jetContainer="AntiKt4EMTopoJets", jetSystematics="(^$)",
                                       components=[
                                        {"containerName":"Muons", "regex":"(^$)", "type":xAOD.Type.Muon, "termName":"RefMuon"},
                                        {"containerName":"Electrons", "regex":"(^$)", "type":xAOD.Type.Electron, "termName":"RefEle"}] )
print( metSequence ) # For debugging

# Add the sequence to the job:
algSeq += metSequence

# Set up a histogram output file for the job:
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [
    "ANALYSIS DATAFILE='MetAnalysisAlgorithmsTest.hist.root' OPT='RECREATE'"
    ]

# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
