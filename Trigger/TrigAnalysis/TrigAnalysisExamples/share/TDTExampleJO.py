
#
# Properties that should be modified by the user:
#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if not "InputFiles" in dir():
    #InputFiles = [ "ESD.pool.root" ]
    InputFiles = [ "/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/attila.AOD.pool.root" ]
if not "OutputFile" in dir():
    OutputFile = "TDTExample.root"
if not "athenaCommonFlags.EvtMax" in dir():
    athenaCommonFlags.EvtMax = 20

# Set up the needed RecEx flags:
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )

# Set up what the RecExCommon job should and shouldn't do:
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

#########################################################################
#                                                                       #
#                      Now set up the example job                       #
#                                                                       #
#########################################################################

#
# Configure an instance of TrigDecisionTool:
#
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.OutputLevel=ERROR
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
#ToolSvc.TrigDecisionTool.PublicChainGroups = { "EFTau": "EF_[0-9]?tau.*",
#                                               "EFPhoton": "EF_[0-9]?g*",
#                                               "EFJets":"EF_J.*" }

#
# Example Code
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import Trig__TDTExample
#from TrigAnalysisExamples.TDTExample import TriggerTree
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += Trig__TDTExample( "TDTExample" )
#topSequence += TriggerTree( "trigger" )

#
# Add Histograms
#
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [ "turnontree DATAFILE='" + OutputFile +
                                "' TYP='ROOT' OPT='RECREATE'" ]
ServiceMgr.THistSvc.OutputLevel = ERROR
