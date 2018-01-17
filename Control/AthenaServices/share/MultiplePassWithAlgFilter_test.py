## @file MultiplePassWithAlgFilter_test.py
# @brief Job options file to test the MultipleEventLoopMgr. Uses an AlgTool
# rather than an algorithm as filter
#
# $Id: MultiplePassWithAlgFilter_test.py,v 1.9 2007-12-15 19:05:46 calaf Exp $

#==============================================================
# Make sure that this has been read.
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# 
theApp.EventLoop = "MultipleEventLoopMgr"
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#run without file catalog
ServiceMgr.PoolSvc.AttemptCatalogPatch = True

#
# configure the ELM and the next pass filter Alg
from AthenaServices.AthenaServicesConf import MultipleEventLoopMgr
ServiceMgr += MultipleEventLoopMgr(NextPassFilter = "ToyNextPassFilterAlg")

from AthenaCommon.AlgSequence import AlgSequence
from AthenaServices.AthenaServicesConf import ToyNextPassFilterAlg
topSeq = AlgSequence()
topSeq += ToyNextPassFilterAlg(NPasses=3)

#services that need to be reinit
ServiceMgr.MultipleEventLoopMgr.ToBeReinitialized = ["StoreGateSvc", "DetectorStore"]

#
# I/O stuff
#
ServiceMgr.EventSelector.InputCollections = [
    "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000011.pool.root.1" ]
# Set up for seeking.
ServiceMgr.EventSelector.CollectionType = "SeekableROOT"

include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "MultiplePassFile.root"; # ** mandatory parameter ** // The output file name
Stream1.ForceRead=TRUE  #force read of output data objs
Stream1.ItemList=["EventInfo#*", "McEventCollection#*"]

#--------------------------------------------------------------
#--- Monitoring and Debug printouts
#--------------------------------------------------------------

MessageSvc.OutputLevel      = INFO
Stream1.OutputLevel = DEBUG
#StoreGateSvc.Dump=1
#StoreGateSvc.OutputLevel=DEBUG

from AthenaCommon.AppMgr import theAuditorSvc
from GaudiAud.GaudiAudConf import ChronoAuditor
theAuditorSvc += ChronoAuditor()
