include.block ( "EventOverlayJobTransforms/ConfiguredOverlay_jobOptions.py" )

#--------------------------------------------------------------
# Load POOL support
# (modified by Piyali.Banerjee to include skip  events)
#--------------------------------------------------------------

from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
topSequence = job

if not overlayFlags.isDataOverlay():
    job += CfgGetter.getAlgorithm("CopyTimings")

#=======================================================================
from AthenaCommon.AppMgr import ServiceMgr
from PileUpComps.PileUpCompsConf import PileUpEventLoopMgr
from PileUpComps.PileUpCompsConf import BkgStreamsCache

import AthenaPoolCnvSvc.WriteAthenaPool
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool

from StoreGate.StoreGateConf import StoreGateSvc
from Digitization.DigitizationFlags import digitizationFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

pileUpEventLoopMgr = PileUpEventLoopMgr()
pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream"

print "================  DetFlags  ================ "
DetFlags.Print()

if overlayFlags.isDataOverlay():
    from InDetRecExample.InDetJobProperties import InDetFlags
    include ("InDetRecExample/InDetRecConditionsAccess.py")
    from ByteStreamCnvSvc import ReadByteStream
    include("RecExCommon/BSRead_config.py")
    ServiceMgr.ByteStreamInputSvc.FullFileName = DataInputCollections
    ServiceMgr.ByteStreamInputSvc.EventStore= "StoreGateSvc/"+overlayFlags.dataStore()
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
pileUpEventLoopMgr.IsEventOverlayJobMC=not overlayFlags.isDataOverlay()
ServiceMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

# Set up MC input
mcEvtSel=EventSelectorAthenaPool("mcSignal_EventSelector")
mcEvtSel.InputCollections = SignalInputCollections
mcEvtSel.KeepInputFilesOpen = True
mcEvtSel.SkipEvents = athenaCommonFlags.SkipEvents()

ServiceMgr += mcEvtSel
pileUpEventLoopMgr.SignalSelector="mcSignal_EventSelector"

ServiceMgr += pileUpEventLoopMgr

# Explicitly create the two extra StoreGateSvc instances used by
# Overlay jobs. This prevents warnings later in the configuration.
ServiceMgr += StoreGateSvc(overlayFlags.dataStore()) #, Dump=True, OutputLevel=DEBUG)
ServiceMgr += StoreGateSvc(overlayFlags.evtStore()) #, Dump=True, OutputLevel=DEBUG)

if not hasattr(ServiceMgr, 'PileUpMergeSvc'):
    from PileUpTools.PileUpToolsConf import PileUpMergeSvc
    ServiceMgr += PileUpMergeSvc()
    del PileUpMergeSvc

# In overlaying unlike pileup we are NOT combining different MC events.
# Make sure HepMcParticleLinks use the right offset in McEventCollection.
ServiceMgr.PileUpMergeSvc.ReturnTimedData=False

#================================================================

#synchronization of Beam flags and Digitization flags - KAA
#if not (digitizationFlags.doMinimumBias.get_Value() or digitizationFlags.doCavern.get_Value() or
#        digitizationFlags.doBeamGas.get_Value() or digitizationFlags.doBeamHalo.get_Value()) :
DetFlags.pileup.all_setOff()
digitizationFlags.numberOfCollisions=0.0

#-----------------------------------------------------------
# Check Beam and Digitization jobproperties are synchronised
#-----------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.numberOfCollisions.get_Value() != digitizationFlags.numberOfCollisions.get_Value() :
    jobproperties.Beam.numberOfCollisions = digitizationFlags.numberOfCollisions.get_Value()
    jobproperties.Beam.override = True ## just incase - default is True

if jobproperties.Beam.bunchSpacing.get_Value() != digitizationFlags.bunchSpacing.get_Value() :
    jobproperties.Beam.bunchSpacing = digitizationFlags.bunchSpacing.get_Value()
    jobproperties.Beam.override = True ## just incase - default is True

#================================================================
