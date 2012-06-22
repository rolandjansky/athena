###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput= ["AOD.pool.root"]
athenaCommonFlags.FilesInput= ["/tmp/roberth/AOD.324274._000001.pool.root.1"]


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'hsg5_WHlnubb_boosted.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteSMWZD3PD = True
prodFlags.WriteSMWZD3PD.FileName = tupleFileOutput
prodFlags.WriteSMWZD3PD.lock()

#prodFlags.WriteHSG5_WHlnubb_boosted_D3PD = True
#prodFlags.WriteHSG5_WHlnubb_boosted_D3PD.FileName = tupleFileOutput
#prodFlags.WriteHSG5_WHlnubb_boosted_D3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1 

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.DoTrigger = False


###################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "PhysicsD3PDMaker/HSG5_WHlnubb_boosted_D3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )

include( "RecExCommon/RecExCommon_topOptions.py" )

print "topSequence: "
print topSequence
