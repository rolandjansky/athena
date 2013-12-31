# $Id: AODToTrigBJetD3PD.py 355147 2011-03-29 16:30:16Z krasznaa $
#
# @file TrigBJetD3PDMaker/share/AODToTrigBJetD3PD.py
#
# @date March, 2011
# @brief Example for building a TrigBJet D3PD from an AOD.
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
    tupleFileOutput = 'bjet.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteTrigBJetD3PD = True
prodFlags.WriteTrigBJetD3PD.FileName = tupleFileOutput
prodFlags.WriteTrigBJetD3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = 100


###################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "TrigBJetD3PDMaker/TrigBJetD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
include( "RecExCommon/RecExCommon_topOptions.py" )
