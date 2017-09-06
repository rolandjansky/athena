#
# get_files -xmls LVL1config_Physics_pp_v5.xml
# ln -s /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/data15_13TeV.00266904.physics_EnhancedBias.merge.RAW._lb0452._SFO-1._0001.1 input.data
# 

## @file L1Topo_ReadBS_test.py
## @brief Example job options file to read BS file to test a converter
## $Id: $
###############################################################
#
# This Job option:
# ----------------
# 1. Read ByteStream test data file and decode the L1Topo part
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob



## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc, findFileInXMLPATH
svcMgr += LVL1ConfigSvc()
svcMgr.LVL1ConfigSvc.XMLMenuFile = findFileInXMLPATH("LVL1config_Physics_pp_v5.xml")


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them


#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI()
fakeRoI.OutputLevel=VERBOSE
fakeRoI.InputFilename="testData.dat"
topSequence += fakeRoI


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=True
print svcMgr

theApp.EvtMax = 10

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

mt = GenericMonitoringTool("CaloRoIsMonitor")
mt.OutputLevel=VERBOSE
mt.Histograms += [defineHistogram('roiCount', type='TH1F', title="RoIs count;#RoIs;events", 
                                  xbins=30, xmin=0, xmax=30)]
fakeRoI.MonTool = mt


from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]


print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################
