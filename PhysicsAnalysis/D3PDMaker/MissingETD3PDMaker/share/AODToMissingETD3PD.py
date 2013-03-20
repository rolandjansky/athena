# $Id$
#
# @file D3PDMakerConfig/share/AODToPhysicsD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Example for building a physics D3PD from an AOD.
#

if not 'evtmax' in dir(): evtmax=-1
if not 'doTruth' in dir(): doTruth = True
if not 'METPhysicsD3PDDetailLevel' in dir(): METD3PDDetailLevel=10

if not 'METPhysicsD3PDFile' in dir(): METPhysicsD3PDFile = 'MET.d3pd.root'

####################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]

####################################################################
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = METPhysicsD3PDFile


####################################################################
# Define other job options here.
#

athenaCommonFlags.EvtMax = evtmax

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.DoTrigger = False


####################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")


####################################################################
# Make the D3PD.
#

from RecExConfig.RecFlags import rec
from MissingETD3PDMaker.MissingETD3PDMakerFlags import MissingETD3PDMakerFlags

if 'IS_SIMULATION' in inputFileSummary['evt_type']:
    rec.doTruth = True

MissingETD3PDMakerFlags.doMissingETRegions = False
MissingETD3PDMakerFlags.doCellOutEflow = False
MissingETD3PDMakerFlags.doCells = False


from MissingETD3PDMaker.MissingETD3PD import MissingETD3PD
alg = MissingETD3PD( tupleFileOutput, METD3PDDetailLevel )
