from AthenaCommon.Logging import logging
overlaylog = logging.getLogger('overlay')
overlaylog.info( '****************** STARTING OVERLAY *****************' )

import AthenaCommon.AtlasUnixStandardJob

for cf in runArgs.jobConfig:
    include(cf)

#==============================================================
# Job definition parameters:
#==============================================================
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.GlobalFlags  import globalflags

globalflags.isOverlay.set_Value_and_Lock(True)

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax = runArgs.maxEvents
jobproperties.AthenaCommonFlags.SkipEvents= runArgs.skipEvents

jobproperties.AthenaCommonFlags.PoolHitsInput=runArgs.inputHitsFile
jobproperties.AthenaCommonFlags.PoolRDOOutput=runArgs.outputRDOFile

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=runArgs.geometryVersion

from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.rndmSeedOffset1=int(runArgs.digiSeedOffset1)
jobproperties.Digitization.rndmSeedOffset2=int(runArgs.digiSeedOffset2)
jobproperties.Digitization.physicsList=runArgs.samplingFractionDbTag
jobproperties.Digitization.rndmSvc=runArgs.digiRndmSvc

SignalInputCollections = runArgs.inputHitsFile
jobproperties.AthenaCommonFlags.PoolRDOOutput = runArgs.outputRDOFile
OverlayCollection = runArgs.outputRDOFile

from OverlayCommonAlgs.OverlayFlags import OverlayFlags
SignalCollection = runArgs.signalRDOFile
if runArgs.signalRDOFile=="NONE":
   OverlayFlags.set_SignalOff()

OverlayFlags.set_BCMOff()
OverlayFlags.set_LUCIDOff()

readBS = runArgs.ReadByteStream
isRealData = False

if readBS:
   globalflags.InputFormat.set_Value_and_Lock('bytestream')
   DataInputCollections=runArgs.inputBSFile
else:
   DataInputCollections=runArgs.pileupBSFile
   jobproperties.AthenaCommonFlags.PoolRDOInput=runArgs.pileupBSFile

if runArgs.conditionsTag!='NONE' and runArgs.conditionsTag!='':
   globalflags.ConditionsTag=runArgs.conditionsTag
   if len(globalflags.ConditionsTag())!=0:
      from IOVDbSvc.CondDB import conddb
      conddb.setGlobalTag(globalflags.ConditionsTag())

OverlayFlags.Print()

# LVL1 Trigger Menu
if runArgs.triggerConfig!="NONE":
    # LVL1 Trigger Menu
    # PJB 9/2/2009 Setup the new triggerConfig flags here
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    triggerArg = runArgs.triggerConfig
    #if not prefixed with LVL1: add it here
    Args = triggerArg.split(":")
    if Args[0] != "LVL1":
        TriggerFlags.triggerConfig ="LVL1:"+triggerArg
    else:
        TriggerFlags.triggerConfig =triggerArg
    overlaylog.info( 'triggerConfig argument is: %s ', TriggerFlags.triggerConfig.get_Value() )
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter("HIT2RDO")

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Muon_setOn()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
if runArgs.triggerConfig=="NONE":
  DetFlags.LVL1_setOff()
else:
  DetFlags.LVL1_setOn()

DetFlags.BCM_setOn()
DetFlags.Lucid_on()

DetFlags.simulateLVL1.Lucid_setOff()

print "================ DetFlags ================ "
DetFlags.Print()

# Geometry, controlled by DetFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_geant4()
globalflags.DataSource.set_Value_and_Lock('geant4')

#from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit

include ( "RecExCond/AllDet_detDescr.py" )

#from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit
#from AtlasGeoModel import SetupRecoGeometry

#include( "BFieldAth/BFieldAth_jobOptions.py" )

#--------
from AthenaCommon.AppMgr import theApp
theApp.EventLoop = "PileUpEventLoopMgr"
theApp.EvtMax = runArgs.maxEvents

include ( "EventOverlayJobTransforms/ConfiguredOverlay_jobOptions.py" )

if OverlayFlags.doTruth():
   include ( "EventOverlayJobTransforms/TruthOverlay_jobOptions.py" )

if OverlayFlags.doBCM() or OverlayFlags.doLUCID():
   include ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

if OverlayFlags.doPixel() or OverlayFlags.doSCT() or OverlayFlags.doTRT():
   include ( "EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py" )

if OverlayFlags.doLAr() or OverlayFlags.doTile():
   include ( "EventOverlayJobTransforms/CaloOverlay_jobOptions.py" )

if OverlayFlags.doCSC() or OverlayFlags.doMDT() or OverlayFlags.doRPC() or OverlayFlags.doTGC():
   include ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

if OverlayFlags.doLVL1():
   include ( "EventOverlayJobTransforms/Level1Overlay_jobOptions.py" )

# save the overlay output first
include ( "EventOverlayJobTransforms/OverlayOutputItemList_jobOptions.py" )

# now save the signal information in the same job
if OverlayFlags.doSignal():
   include ( "EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py" )

# For random number initialization
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable(jobproperties.Digitization.rndmSvc.get_Value())()
jobproperties.Digitization.rndmSeedList.addtoService()
jobproperties.Digitization.rndmSeedList.printSeeds()

# To not overwrite the BCID
#from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.PileUpEventLoopMgr.IsEventOverlayJob=True

#================================================================
print "overlay_trf: final outStream = ", outStream

#================================================================
# Logging
#--------------------------------------------------------------
#ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%45W%S%7W%R%T %0W%M"

print "overlay_trf: at the end. job=\n", job
print "\n\noverlay_trf: at the end. ServiceMgr=\n", ServiceMgr

# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)
