import AthenaCommon.AtlasUnixStandardJob

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
theApp.EventLoop = "PileUpEventLoopMgr"
theApp.EvtMax = 6

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
import AthenaPoolCnvSvc.WriteAthenaPool

# Geometry
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-CSC-02-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


from AthenaCommon.AppMgr import ServiceMgr

#=======================================================================
from PileUpComps.PileUpCompsConf import PileUpEventLoopMgr
    
from PileUpComps.PileUpCompsConf import BkgStreamsCache
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool


pileUpEventLoopMgr = PileUpEventLoopMgr()
pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream"

# Set up data input
pileUpEventLoopMgr.OrigSelector="EventSelector"
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/a/andr/scratch0/ideal2_valid1.007211.singlepart_mu10.digit.RDO.v13003002/RDO.016155._00001.pool.root.1"]
pileUpEventLoopMgr.firstXing=0
pileUpEventLoopMgr.lastXing=0
ServiceMgr.EventSelector.SkipEvents = 0

# Set up MC input
pileUpEventLoopMgr.bkgCaches += [BkgStreamsCache("mcSignalEvent"+"_Cache")]
tmpCache=pileUpEventLoopMgr.bkgCaches[0]
tmpCache.CollPerXing=1
tmpCache.CollDistribution="Fixed"
tmpCache.ReadDownscaleFactor=1

tmpEvtSel=EventSelectorAthenaPool("mcSignalEvent"+"_EventSelector")
tmpEvtSel.InputCollections = [ "/afs/cern.ch/user/a/andr/scratch0/ideal2_valid1.007211.singlepart_mu10.digit.RDO.v13003002/RDO.016155._00002.pool.root.1" ]

tmpEvtSel.KeepInputFilesOpen = True
tmpEvtSel.SkipEvents = 1

ServiceMgr += tmpEvtSel
tmpCache.EventSelector="mcSignalEvent"+"_EventSelector"

ServiceMgr += pileUpEventLoopMgr

#****************************************************************
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#================================================================
# Configure the overlaying algs 

from InDetOverlay.InDetOverlayConf import InDetOverlay
job.insert(0, InDetOverlay())

#job.InDetOverlay.OutputLevel = INFO

# McEventCollection copying
from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyMcEventCollection
job += CopyMcEventCollection()

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

from StoreGate.StoreGateConf import StoreGateSvc
ServiceMgr += StoreGateSvc("StoreGateSvc")
ServiceMgr.StoreGateSvc.Dump = True


from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream( "OverlayOutputStream" )
outStream.Store = ServiceMgr.StoreGateSvc

# Stream's output file
outStream.OutputFile  = "InDetOverlayOut.pool.root"

#FIXME: ### FIXME: this does not work because I and O stores are separate.
#FIXME: # Copy everything from the input and must force reading of all input
#FIXME: # objects
#FIXME: #outStream.ForceRead          = True
#FIXME: #outStream.TakeItemsFromInput = True  # Need to set this to False if an explicit list is to be used

# Can use an explicit list to avoid broken classes. Requires TakeItemsFromInput=False.

outStream.ItemList = []

# Define output stream
outStream.ItemList += [ "EventInfo#*" ]            #FIXME: not InDet-specific
outStream.ItemList += [ "PileUpEventInfo#*" ]      #FIXME: not InDet-specific
outStream.ItemList += [ "McEventCollection#*" ]    #FIXME: not InDet-specific

# InDet MC truth
outStream.ItemList += [ "InDetSimDataCollection#*" ]

# InDet RDO
outStream.ItemList += [ "PixelRDO_Container#*" ]
outStream.ItemList += [ "SCT_RDO_Container#*" ]
outStream.ItemList += [ "TRT_RDO_Container#*" ]

print "jobOptions-overlay: final outStream = ", outStream

#================================================================
# Logging
#--------------------------------------------------------------
#ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.OutputLevel = DEBUG
ServiceMgr.MessageSvc.Format = "% F%45W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages
# ServiceMgr.MessageSvc.debugLimit = 10000000   # all debug message etc...

#================================================================
### From RecExCommon_topOptions.py
# change traceback handling. It is in principle not needed but the observation
# is that in most case it helps recovering a meaningful trace back
# http://cern.ch/seal/workbook/sealbase.html
# USR1_DUMP_CORE        =   1
# FATAL_ON_QUIT	        =   2 x
# FATAL_ON_INT          =   4 x
# FATAL_DUMP_CORE       =   8
# FATAL_DUMP_SIG        =  16 x
# FATAL_DUMP_STACK      =  32 x
# FATAL_DUMP_LIBS       =  64 
# FATAL_DUMP_CONTEXT    = 128 x
# FATAL_AUTO_EXIT       = 256 x
gbl.AthenaServices.SetFatalHandler(438)
#pylcgdict.libPyROOT.SetSignalPolicy( pylcgdict.libPyROOT.kSignalFast )



print "jobOptions-overlay: at the end. job=\n", job
print "\n\njobOptions-overlay: at the end. ServiceMgr=\n", ServiceMgr
