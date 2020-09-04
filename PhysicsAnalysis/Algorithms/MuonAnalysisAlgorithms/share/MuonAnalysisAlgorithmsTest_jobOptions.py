# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack, Will Buttinger

#User options, which can be set from command line after a "-" character
#athena MuonAnalysisAlgorithmsTest_jobOptions.py - --myOption ...
from AthenaCommon.AthArgumentParser import AthArgumentParser
athArgsParser = AthArgumentParser()
athArgsParser.add_argument("--data-type",action="store",dest="data_type",
                           default="data",
                           help="Type of data to run over. Valid options are data, mc, afii")
athArgsParser.add_argument("--write-xaod",action="store",dest="write_xaod",
                           default=False,
                           help="Specify if you want xaod writing to happen (which means slower access mode for now)")
athArgs = athArgsParser.parse_args()


if athArgs.write_xaod:
    #currently we must use POOLAccess mode when writing an xAOD
    jps.AthenaCommonFlags.AccessMode = "POOLAccess"
else:
    #ClassAccess is much faster than POOLAccess
    jps.AthenaCommonFlags.AccessMode = "ClassAccess"

dataType = athArgs.data_type

# Set up a histogram/tree output file for the job:
jps.AthenaCommonFlags.HistOutputs = ["ANALYSIS:MuonAnalysisAlgorithmsTest." + dataType + ".hist.root"]
svcMgr.THistSvc.MaxFileSize=-1 #make job run faster by disabling file size check

#set a default file and number of events to process
#can override with standard athena command line options (--evtMax and --filesInput)
jps.AthenaCommonFlags.EvtMax = 500

if dataType=="data":
    testFile = os.getenv ('ASG_TEST_FILE_DATA')
elif dataType=="mc":
    testFile = os.getenv ('ASG_TEST_FILE_MC')
elif dataType=="afii":
    testFile = os.getenv ('ASG_TEST_FILE_MC_AFII')

jps.AthenaCommonFlags.FilesInput = [testFile]



from MuonAnalysisAlgorithms.MuonAnalysisAlgorithmsTest import makeSequence
algSeq = makeSequence (dataType)
print (algSeq) # For debugging

# Add all algorithms from the sequence to the job.
athAlgSeq += algSeq

# Write a mini-xAOD if requested:
if athArgs.write_xaod:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    minixAOD = MSMgr.NewPoolRootStream( 'AAOD_MUON',
                   FileName = 'MuonAnalysisAlgorithmsTest.AAOD_MUON.pool.root' )
    minixAOD.AddItem(
        [ 'xAOD::EventInfo#EventInfo',
     'xAOD::EventAuxInfo#EventInfoAux.-',
     'xAOD::MuonContainer#AnalysisMuons_NOSYS',
     'xAOD::AuxContainerBase#AnalysisMuons_NOSYSAux.eta.phi.pt' ] )


# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
