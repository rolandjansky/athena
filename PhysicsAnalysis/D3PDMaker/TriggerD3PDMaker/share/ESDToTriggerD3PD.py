# $Id: ESDToTriggerD3PD.py 481363 2012-02-07 16:36:15Z krasznaa $
#
# A simple jobOption to produce a Trigger D3PD using vanilla
# athena.py. Used mostly for (performance) testing.

###################################################################3
# Define the input file here.
#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= [ "ESD.pool.root" ]


###################################################################3
# Define the output file here.
#
if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'trigger.root'
    pass

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteTriggerD3PD = True
prodFlags.WriteTriggerD3PD.FileName = tupleFileOutput
prodFlags.WriteTriggerD3PD.lock()


###################################################################3
# Define other job options here.
#
athenaCommonFlags.EvtMax = 100


###################################################################
# Configure RecExCommon.
#
from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "TriggerD3PDMaker/TriggerD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )

include( "RecExCommon/RecExCommon_topOptions.py" )
