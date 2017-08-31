include.block ( "EventOverlayJobTransforms/ConfiguredOverlay_jobOptions.py" )

#--------------------------------------------------------------
# Load POOL support
# (modified by Piyali.Banerjee to include skip  events) 
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
topSequence = job

if not isRealData:
    from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyTimings
    job += CopyTimings()

#=======================================================================
from AthenaCommon.AppMgr import ServiceMgr
from PileUpComps.PileUpCompsConf import PileUpEventLoopMgr
    
from PileUpComps.PileUpCompsConf import BkgStreamsCache

import AthenaPoolCnvSvc.WriteAthenaPool
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool

from StoreGate.StoreGateConf import StoreGateSvc

from Digitization.DigitizationFlags import jobproperties
digitization = jobproperties.Digitization

pileUpEventLoopMgr = PileUpEventLoopMgr()

pupStream = "PileUpCollXingStream"
jobproperties.Digitization.rndmSeedList.addSeed( pupStream, 123, 345 )

pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream"

#include ("EventOverlayJobTransforms/synchFlags_jobOptions.py")
print "================  DetFlags  ================ "
DetFlags.Print()

#if globalflags.InputFormat()=='bytestream':
if readBS:
    #if isRealData:
    #   include ("RecExCommission/RecExCommission_BSRead_config_hack.py")
    #else:
    from InDetRecExample.InDetJobProperties import InDetFlags
    include ("InDetRecExample/InDetRecConditionsAccess.py")
    from ByteStreamCnvSvc import ReadByteStream
    include("RecExCommon/BSRead_config.py")
    ServiceMgr.ByteStreamInputSvc.FullFileName = DataInputCollections
    ServiceMgr.ByteStreamInputSvc.EventStore= "StoreGateSvc/OriginalEvent_SG"
    from AthenaKernel import StoreID
    ServiceMgr.ByteStreamAddressProviderSvc.StoreID=StoreID.UNKNOWN
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import AthenaPoolAddressProviderSvc
    ServiceMgr += AthenaPoolAddressProviderSvc("AthenaPoolAddressProviderSvc")
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "AthenaPoolAddressProviderSvc" ]
else:
    import AthenaPoolCnvSvc.ReadAthenaPool
    ServiceMgr.EventSelector.InputCollections = DataInputCollections

# Set up data input
pileUpEventLoopMgr.OrigSelector="EventSelector"
pileUpEventLoopMgr.firstXing=0
pileUpEventLoopMgr.lastXing=0
pileUpEventLoopMgr.IsEventOverlayJob=True
pileUpEventLoopMgr.IsEventOverlayJobMC=not isRealData
ServiceMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

# Set up MC input
pileUpEventLoopMgr.bkgCaches += [BkgStreamsCache("mcSignalEvent"+"_Cache")]
tmpCache=pileUpEventLoopMgr.bkgCaches[0]
tmpCache.CollPerXing=1
tmpCache.CollDistribution="Fixed"
tmpCache.ReadDownscaleFactor=1
tmpCache.RndmGenSvc=jobproperties.Digitization.rndmSvc()
tmpCache.RndmStreamName=pupStream

tmpEvtSel=EventSelectorAthenaPool("mcSignalEvent"+"_EventSelector")
tmpEvtSel.InputCollections = SignalInputCollections

tmpEvtSel.KeepInputFilesOpen = True
tmpEvtSel.SkipEvents = athenaCommonFlags.SkipEvents()

ServiceMgr += tmpEvtSel
tmpCache.EventSelector="mcSignalEvent"+"_EventSelector"

ServiceMgr += pileUpEventLoopMgr

if not hasattr(ServiceMgr, 'PileUpMergeSvc'):
    from PileUpTools.PileUpToolsConf import PileUpMergeSvc
    ServiceMgr += PileUpMergeSvc()
    del PileUpMergeSvc

# In overlaying unlike pileup we are NOT combining different MC events.
# Make sure HepMcParticleLinks use the right offset in McEventCollection.
ServiceMgr.PileUpMergeSvc.ReturnTimedData=False

#================================================================

#synchronization of Beam flags and Digitization flags - KAA
#if not (digitization.doMinimumBias.get_Value() or digitization.doCavern.get_Value() or
#        digitization.doBeamGas.get_Value() or digitization.doBeamHalo.get_Value()) :
DetFlags.pileup.all_setOff()
digitization.numberOfCollisions=0.0

#-----------------------------------------------------------
# Check Beam and Digitization jobproperties are synchronised
#-----------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.numberOfCollisions.get_Value() != digitization.numberOfCollisions.get_Value() :
   jobproperties.Beam.numberOfCollisions = digitization.numberOfCollisions.get_Value()
   jobproperties.Beam.override = True ## just incase - default is True

if jobproperties.Beam.bunchSpacing.get_Value() != digitization.bunchSpacing.get_Value() :
   jobproperties.Beam.bunchSpacing = digitization.bunchSpacing.get_Value()
   jobproperties.Beam.override = True ## just incase - default is True

#================================================================

