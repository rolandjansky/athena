####
# Setup VP1 jobOptions for running at P1
# --> do not rely on RecExCommon options (doVP1),
#     but setup things ourselves
#####

### Make sure a StreamESD stream has been setup before
if not 'StreamESD' in dir():
  print "\n\n"
  print "\t WARNING: StreamESD is not available - can not run VP1 event producers!"
  print "\t          Please use \"rec.doWriteESD\" to activate StreamESD.\n\n"
  import sys
  sys.exit(3);


### Add the algorithm producing VP1 events
from VP1Algs.VP1AlgsConf import VP1EventProd
VP1EventProducer = VP1EventProd(InputPoolFile = StreamESD.OutputFile)

#Write out files in the directory given by the stream name
VP1EventProducer.DestinationDirectory = OutputDirectory
#Set number of files large so deleting is doen by prune script
VP1EventProducer.MaxNumberOfFiles = 10000

#Set the output level
if not 'VP1MsgLvl' in dir():
  VP1MsgLvl=WARNING
VP1EventProducer.OutputLevel=VP1MsgLvl

### Finally add this event producer to the main sequencer
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += VP1EventProducer

### Finally print setup in debug mode
if VP1MsgLvl <= DEBUG:
  print "\n\n\t VP1 setup\n",VP1EventProducer,"\n\n"
