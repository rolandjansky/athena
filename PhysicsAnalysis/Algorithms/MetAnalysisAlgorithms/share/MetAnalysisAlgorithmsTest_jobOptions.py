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

# Set up a selection alg for demonstration purposes
# Also to avoid warnings from building MET with very soft electrons
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
selalg = createAlgorithm( 'CP::AsgSelectionAlg', 'METEleSelAlg' )
addPrivateTool( selalg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
selalg.selectionTool.minPt = 10e3
selalg.selectionTool.maxEta = 2.47
selalg.selectionDecoration = 'selectPtEta'
selalg.particles = 'Electrons'
# We need to copy here, because w/o an output container, it's assumed
# that the input container is non-const
selalg.particlesOut = 'DecorElectrons_%SYS%'
algSeq += selalg
print( selalg ) # For debugging

# Now make a view container holding only the electrons for the MET calculation
viewalg = createAlgorithm( 'CP::AsgViewFromSelectionAlg','METEleViewAlg' )
viewalg.selection = [ 'selectPtEta' ]
viewalg.input = 'DecorElectrons_%SYS%'
viewalg.output = 'METElectrons_%SYS%'
algSeq += viewalg
print( viewalg ) # For debugging

# Include, and then set up the met analysis algorithm sequence:
from MetAnalysisAlgorithms.MetAnalysisSequence import makeMetAnalysisSequence
# Touch the ObjectType enum to trigger dict loading
from ROOT import xAOD
xAOD.Type.ObjectType
metSequence = makeMetAnalysisSequence( dataType, metSuffix="AntiKt4EMTopo",
                                       jetContainer="AntiKt4EMTopoJets", jetSystematics="(^$)",
                                       components=[
                                        {"containerName":"Muons", "regex":"(^$)", "type":xAOD.Type.Muon, "termName":"RefMuon"},
                                        {"containerName":"METElectrons_%SYS%", "regex":"(^$)", "type":xAOD.Type.Electron, "termName":"RefEle"}] )
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
