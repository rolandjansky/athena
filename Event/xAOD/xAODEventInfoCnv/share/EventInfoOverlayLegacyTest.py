# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#########################################################################
#
## @file EventInfoOverlayLegacyTest.py
##
## @brief xAOD::EventInfo overlay event loop test:
## read an RDO and a HITS file, output a RDO file
## Test legacy HITS with the old EventInfo
##
## @author Tadej Novak <tadej.novak@cern.ch>
#
#==============================================================

# basic job configuration
import AthenaCommon.AtlasUnixStandardJob

# get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags      import rec
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# Set that we are running MC+MC overlay in MT mode
globalflags.isOverlay.set_Value_and_Lock(True)
overlayFlags.isDataOverlay.set_Value_and_Lock(False)
overlayFlags.isOverlayMT.set_Value_and_Lock(True)

#--------------------------------------------------------------
# Load POOL support for DoubleEventSelector
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble

# For general flags
rec.doAOD       = False
rec.doTrigger   = False
rec.doWriteTAG  = False
DetDescrVersion = "ATLAS-R2-2016-01-00-01"

# the correct tag should be specified
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SDR-BS7T-04-00")

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------
svcMgr.DoubleEventSelector.InputCollections = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup-MT.100events.pool.root" ]
svcMgr.SecondaryEventSelector.InputCollections = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000765.pool.root.1" ]

#--------------------------------------------------------------
# Remapping Service
#--------------------------------------------------------------
from SGComps import AddressRemappingSvc
AddressRemappingSvc.addInputRename("EventInfo", "McEventInfo", "Sig_McEventInfo")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#--------------------------------------------------------------
# Algorithms
#--------------------------------------------------------------
# Make the signal xAOD::EventInfo
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
EventInfoCnvAlg = xAODMaker__EventInfoCnvAlg()
EventInfoCnvAlg.AODKey = "Sig_McEventInfo"
EventInfoCnvAlg.xAODKey = "Sig_EventInfo"
topSequence += EventInfoCnvAlg

# Run the overlay
from AthenaCommon import CfgGetter
EventInfoOverlay = CfgGetter.getAlgorithm("EventInfoOverlay")
EventInfoOverlay.OutputLevel = DEBUG
topSequence += EventInfoOverlay

#--------------------------------------------------------------
# EventLoop
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads > 0:
    EventLoop = Service("AthenaHiveEventLoopMgr")
else:
    EventLoop = Service("AthenaEventLoopMgr")
EventLoop.UseSecondaryEventNumber = True
svcMgr += EventLoop

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream = AthenaPoolOutputStream( "Stream" )
Stream.OutputFile  = "OutputRDO.root"
Stream.ItemList =  [ 'xAOD::EventInfo#EventInfo', 'xAOD::EventAuxInfo#EventInfoAux.' ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc = Service( "MessageSvc" )
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )
