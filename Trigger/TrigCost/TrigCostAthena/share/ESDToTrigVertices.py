# $Id: ESDToTrigVertices.py 347096 2011-02-22 12:41:04Z aidan $
#
# @file D3PDMakerConfig/share/ESDToPhysicsD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Example for building a physics D3PD from an ESD.
#


###################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

###################################################################
# Define the output file here.
#

if not globals().get('outputFileType'):
    inputFileType = 'ESD'

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'physics.root'

###################################################################
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

###################################################################
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


###################################################################
# Make the Ntuple.
#

setOption = 'vtxNtuple=' + runArgs.outputNTUP_ENHBIASFile
include('Trigger/TrigCost/TrigCostAthena/share/saveTrigCostData.py')



