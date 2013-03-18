# $Id$

###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ "AOD.pool.root" ]

###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'l1calo.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteL1CALOD3PD = True
prodFlags.WriteL1CALOD3PD.FileName = tupleFileOutput
prodFlags.WriteL1CALOD3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = 100

####################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "L1CaloD3PDMaker/L1CaloUpgradeD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
include ("RecExCommon/RecExCommon_topOptions.py")
