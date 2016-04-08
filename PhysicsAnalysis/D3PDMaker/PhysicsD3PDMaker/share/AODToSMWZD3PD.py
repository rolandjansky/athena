# $Id: AODToSMWZD3PD.py 352577 2011-03-17 18:14:25Z boonekam $
#
# @file D3PDMakerConfig/share/AODToSMWZD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Example for building an egamma D3PD from an AOD.
#


###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'smwz.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteSMWZD3PD = True
prodFlags.WriteSMWZD3PD.FileName = tupleFileOutput
prodFlags.WriteSMWZD3PD.lock()

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
rec.DPDMakerScripts.append( "PhysicsD3PDMaker/SMWZD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
include( "RecExCommon/RecExCommon_topOptions.py" )
