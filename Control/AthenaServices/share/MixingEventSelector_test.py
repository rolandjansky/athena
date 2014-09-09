## @file MixingEventSelector_test.py
# @brief Job options file to test the Event Mixing selector
#
# $Id: MixingEventSelector_test.py,v 1.20 2007-12-15 19:05:45 calaf Exp $
#==============================================================
include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
#---   EventMixer configuration
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 300

#run without file catalog
ServiceMgr.PoolSvc.AttemptCatalogPatch = True

#ServiceMgr.ProxyProviderSvc.ProviderNames += [ "MixingEventSelector/EventMixer" ]
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable("AthenaPoolAddressProviderSvc")("AthenaPoolAddressProviderSvc" )
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "AthenaPoolAddressProviderSvc" ]
#ServiceMgr.AthenaPoolAddressProviderSvc.DataHeaderKey = "MinBiasSelector"

#why not !!! AthenaEventLoopMgr.EvtSel = "EventMixer";
theApp.EvtSel = "EventMixer";  #FIXME should decode ListItem
#
ServiceMgr += getConfigurable("MixingEventSelector")("EventMixer")
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "MixingEventSelector/EventMixer" ]
# Event mixing trigger list
# format is [frequency in %]
ServiceMgr.EventMixer.TriggerList += [ "EventSelectorAthenaPool/MinBiasSelector:0:1000" ]
ServiceMgr.EventMixer.TriggerList += [ "EventSelectorAthenaPool/CavernSelector:0:1000" ]
ServiceMgr.EventMixer.StreamStatusFileName = "streamStatus.txt"
# Pool input G4Sim  use 
ServiceMgr += getConfigurable("EventSelectorAthenaPool")("CavernSelector" )
ServiceMgr.CavernSelector.InputCollections = [ 
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000011.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000111.pool.root.1" ]
ServiceMgr += getConfigurable("EventSelectorAthenaPool")("MinBiasSelector" )

ServiceMgr.MinBiasSelector.InputCollections = [ 
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105001.pythia_minbias.merge.HITS.e577_s932_s952_tid170554_00/HITS.170554._000034.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105001.pythia_minbias.merge.HITS.e577_s932_s952_tid170554_00/HITS.170554._000043.pool.root.1" ]
ServiceMgr.EventMixer.OutputRunNumber=54321
ServiceMgr.EventMixer.EventNumbers=[1, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 47]

#--------------------------------------------------------------
#---   Output stream configuration
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "MixedFile.root"; # ** mandatory parameter ** // The output file name
Stream1.ForceRead=TRUE;  #force read of output data objs
Stream1.ItemList=["MergedEventInfo#*",
                  "EventInfo#*",
                  "McEventCollection#*",
		  "SiHitCollection#SCT_Hits",
		  "SiHitCollection/PixelHits"]; #SYNTAX ERROR: / not #

#--------------------------------------------------------------
#--- Monitoring and Debug printouts
#--------------------------------------------------------------

MessageSvc.OutputLevel      = INFO
ServiceMgr.EventMixer.OutputLevel      = VERBOSE
MessageSvc.setVerbose += [ "MixingEventSelector::TriggerList" ]
Stream1.OutputLevel = DEBUG
#StoreGateSvc.Dump=1
#StoreGateSvc.OutputLevel=DEBUG

from AthenaCommon.AppMgr import theAuditorSvc
from GaudiAud.GaudiAudConf import ChronoAuditor
theAuditorSvc += ChronoAuditor()

#print events 1 and 300, which happens to be the last. Work around problem with ATN test out parsing
ServiceMgr += getConfigurable("AthenaEventLoopMgr")(EventPrintoutInterval=299)

