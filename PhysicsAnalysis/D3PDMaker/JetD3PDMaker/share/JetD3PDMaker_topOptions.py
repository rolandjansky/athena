# $Id$
#
# @file   JetD3PDMaker/share/
# @author Haifeng Li <Haifeng.Li@cern.ch>
# @date   Feb. 11, 2009
# @brief  Top Job Option for Jet D3PD production. 
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
    tupleFileOutput = 'jetD3PD.root'


###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1


###################################################################3
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
include ("RecExCommon/RecExCommon_topOptions.py")


###################################################################3
# Make the D3PD.
#

from JetD3PDMaker.JetD3PD import JetD3PD
alg = JetD3PD ( tupleFileOutput )



