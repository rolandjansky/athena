
include( "AthenaCommon/AthenaCommonFlags.py" )

EvtMax     = -1
SkipEvents = 0

PoolRDOInput  = [ "DigitizationOutput.pool.root" ]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigT1CTP.TrigT1CTPConfig import LVL1CTP__CTPDataDumper
topSequence += LVL1CTP__CTPDataDumper("CTPDataDumper")
topSequence.CTPDataDumper.OutputLevel = VERBOSE

print 'config of CTPDataDumper\n', topSequence.CTPDataDumper

# use simple loop
doESD = False
doWriteESD = False
doAOD = False
doWriteAOD = False
doTAG = False
doWriteTag = False
doTrigger = False
doAODCaloCells = False
doFileMetaData = False

#doDumpTES = True
#doDumpTDS = True

include ( "RecExCommon/RecExCommon_topOptions.py" )
