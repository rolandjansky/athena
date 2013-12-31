#################################
## D3PD stuff
#################################
# Main algorithm import
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a few global variables
if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'qcd.root'
from QcdD3PDMaker.QcdD3PD import QcdD3PD

if not globals().get('useGRL'):
    useGRL = False

theGRLFile = ''
if useGRL:
    if not globals().get('GRLFile') or globals().get('GRLFile')=='':
        print "WARNING: GRLFile not set - GRL usage turned off!"
        useGRL = False
    else:
        theGRLFile = globals().get('GRLFile')

if useGRL:
    # GRL setup
    ## Configure the goodrunslist selector tool
    from GoodRunsLists.GoodRunsListsConf import *
    ToolSvc += GoodRunsListSelectorTool()
    GoodRunsListSelectorTool.GoodRunsListVec  = [ theGRLFile ]
    #GoodRunsListSelectorTool.BlackRunsListVec = [ 'blacklist.xml' ]
    #GoodRunsListSelectorTool.OutputLevel = DEBUG

    from GoodRunsListsUser.GoodRunsListsUserConf import *
    from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
    job = AlgSequence()
    job += AthSequencer("ModSequence1")
    job.ModSequence1 += GRLTriggerSelectorAlg('GRLTriggerAlg1')

    # Here we add the D3PDMaker to the grl sequence
    job.ModSequence1 += QcdD3PD (tupleFileOutput,10,'qcd',job.ModSequence1,'D3PD::RootD3PDSvc')

    job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray  = [theGRLFile[:-4]] # Assuming a file name of the form file.xml
    #job.ModSequence1.GRLTriggerAlg1.BlackRunsListArray = ['blacklist']
else:
    alg = QcdD3PD (tupleFileOutput,10,'qcd',topSequence,'D3PD::RootD3PDSvc')

# Check/set some flags, e.g. trigger
if D3PDMakerFlags.DoTrigger():
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    # ds seems to read config information from in-file metadata.
    # But the trigger configuration then falls over if no keys are actually
    # present in metadata.  Fallback to xml configuration in that case.
    TriggerFlags.configurationSourceList=['xml']
    for k in inputFileSummary['metadata'].keys():
        if k.startswith ('/TRIGGER/'):
            TriggerFlags.configurationSourceList=['ds']
            break
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()

#    # Add the trigger configuration metadata:
#    from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
#    addTrigConfMetadata( alg )
#



#################################
## D3PD stuff done
#################################

