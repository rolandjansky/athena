# $Id: makeTrigMETD3PD.py 295662 2010-04-27 08:24:34Z krasznaa $
#
# This is a simple top level jobO for creating "the" trigger MET D3PD from
# the specified AOD/ESD file(s).
#

# Set the input file name(s):
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( [
    "AOD.pool.root"
    ] )

# Set the number of events to process:
athenaCommonFlags.EvtMax.set_Value_and_Lock( 100 )

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

# Read some parameters from the command line:
if not "level" in dir():
    level = 10
if not "OutputName" in dir():
    OutputName = "trig_met_d3pd_level%d.root" % level

# Now add the D3PD maker to the job:
from TrigMissingETD3PDMaker.TrigMETD3PD import TrigMETD3PD
alg = TrigMETD3PD( OutputName, level )
