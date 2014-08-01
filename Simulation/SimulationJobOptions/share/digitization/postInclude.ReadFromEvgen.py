PileUpEventLoopMgr = Service( "PileUpEventLoopMgr" )

pupStream = "PileUpCollXingStream"
from PileUpComps.PileUpCompsConf import BkgStreamsCache
from Digitization import PileUpEventType
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool

from Digitization.DigitizationFlags import digitizationFlags
PileUpEventLoopMgr.OrigSelector="EventSelector"
minBiasCache = BkgStreamsCache("MinBiasCache")
if hasattr(digitizationFlags, 'numberOfNDMinBias'):
    minBiasCache.CollPerXing=digitizationFlags.numberOfNDMinBias()
elif hasattr(digitizationFlags, 'numberOfLowPtMinBias'):
    minBiasCache.CollPerXing=digitizationFlags.numberOfLowPtMinBias()
else:
    raise AttributeError("These job options require digitization jobproperties not present in this release! This job will crash.")
minBiasCache.RndmGenSvc=digitizationFlags.rndmSvc()
minBiasCache.RndmStreamName=pupStream
minBiasCache.PileUpEventType=PileUpEventType.MinimumBias
minBiasCache.ReadDownscaleFactor=10  # read one new event every event

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
minBiasEvtSel = EventSelectorAthenaPool("minBiasEventSelector")
if hasattr(digitizationFlags, 'NDMinBiasInputCols'):
    minBiasEvtSel.InputCollections = digitizationFlags.NDMinBiasInputCols()
elif hasattr(digitizationFlags, 'LowPtMinBiasInputCols'):
    minBiasEvtSel.InputCollections = digitizationFlags.LowPtMinBiasInputCols()
else:
    raise AttributeError("These job options require digitization jobproperties not present in this release! This job will crash.")
minBiasEvtSel.KeepInputFilesOpen = kfOpen
minBiasEvtSel.ProcessMetadata = False
ServiceMgr += minBiasEvtSel
minBiasCache.EventSelector = 'minBiasEventSelector'
PileUpEventLoopMgr.bkgCaches += [ minBiasCache ]

MergeMcEventCollection = Algorithm( "MergeMcEventCollection" )
if hasattr(MergeMcEventCollection, 'MergeMcEventCollTool'):
    MergeMcEventCollection.MergeMcEventCollTool.TruthCollKey = "GEN_EVENT"
else:
    MergeMcEventCollection.TruthCollKey = "GEN_EVENT"

try:
    itemlist = StreamRDO_FH.ItemList
except:
    itemlist = StreamRDO.MetaDataItemList

if itemlist.count( "IOVMetaDataContainer#*" ):
    print 'ZLM found it!'
    itemlist[ itemlist.index( "IOVMetaDataContainer#*" ) ] = "IOVMetaDataContainer#_TagInfo"
else:
    itemlist += [ "IOVMetaDataContainer#_TagInfo" ]

print 'ZLM container thingy:',itemlist

itemlist += [ "IOVMetaDataContainer#_Digitization_Parameters" ]

try:
    print StreamRDO_FH.ItemList
except:
    print StreamRDO.MetaDataItemList


