# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

# Set up the reading of the input file:
import AthenaPoolCnvSvc.ReadAthenaPool
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
muonSequence = makeMuonAnalysisSequence( dataType, deepCopyOutput = True )
muonSequence.configure( inputName = 'Muons',
                        outputName = 'AnalysisMuons_%SYS%' )
print( muonSequence ) # For debugging

# Add the sequence to the job:
algSeq += muonSequence

# Add an ntuple dumper algorithm:
ntupleMaker = CfgMgr.CP__AsgxAODNTupleMakerAlg( 'NTupleMaker' )
ntupleMaker.TreeName = 'muons'
ntupleMaker.Branches = [ 'EventInfo.runNumber     -> runNumber',
                         'EventInfo.eventNumber   -> eventNumber',
                         'AnalysisMuons_NOSYS.eta -> mu_eta',
                         'AnalysisMuons_NOSYS.phi -> mu_phi',
                         'AnalysisMuons_%SYS%.pt  -> mu_%SYS%_pt', ]
ntupleMaker.systematicsRegex = '(^MUON_.*)'
algSeq += ntupleMaker

# Set up a histogram/tree output file for the job:
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [
    "ANALYSIS DATAFILE='MuonAnalysisAlgorithmsTest.hist.root' OPT='RECREATE'"
    ]

# Write a mini-xAOD:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
minixAOD = MSMgr.NewPoolRootStream( 'AAOD_MUON',
                   FileName = 'MuonAnalysisAlgorithmsTest.AAOD_MUON.pool.root' )
minixAOD.AddItem(
   [ 'xAOD::EventInfo#EventInfo',
     'xAOD::EventAuxInfo#EventInfoAux.-',
     'xAOD::MuonContainer#AnalysisMuons_*',
     'xAOD::AuxContainerBase#AnalysisMuons_*.eta.phi.pt' ] )

# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
