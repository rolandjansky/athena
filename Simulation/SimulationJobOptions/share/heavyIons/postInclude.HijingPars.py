#########################################################
#
# SimulationJobOptions/postOptions.HijingPars.py
# Andrzej Olszewski
#
# Mods for Hijing event parameters.
#
# This job option should be added via the postInclude
# command line argument. I.e. before theApp.initialize()
# is called.
#
#########################################################
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.DetFlags import DetFlags
if DetFlags.pileup.any_on():
   from MCTruthSimAlgs.MCTruthSimAlgsConf import MergeHijingPars
   mergeHijingPars = MergeHijingPars()
   index = 0
   for alg in topSequence:
      index += 1 #count position
      if 'MergeMcEventCollection' == alg.name():
         topSequence.insert(index, mergeHijingPars)
else:
   try:
      from TruthIO.TruthIOConf import PrintHijingPars
   except:
      from TruthExamples.TruthExamplesConf import PrintHijingPars
   topSequence += PrintHijingPars()

## configure Athena for POOL persistency
if hasattr(topSequence, 'StreamHITS'):
    outStream = topSequence.StreamHITS
elif hasattr(topSequence, 'StreamRDO'):
    outStream = topSequence.StreamRDO
else:
   ## temporary back-compatibility
   outStreams = AlgSequence( "Streams" )
   if hasattr(outStreams, 'StreamHITS'):
      outStream = outStreams.StreamHITS
   elif hasattr(outStreams, 'StreamRDO'):
      outStream = outStreams.StreamRDO
   else:
      #We should avoid creating the AthenaPoolOutputStream
      #outside the core Sim/Digi python code
      raise AttributeError("AthenaPoolOutputStream not found")

outStream.ItemList += [ "HijingEventParams#Hijing_event_params" ]
