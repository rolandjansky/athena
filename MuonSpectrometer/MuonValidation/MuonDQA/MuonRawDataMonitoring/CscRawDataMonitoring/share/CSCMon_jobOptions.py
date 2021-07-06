###################################################
# File   : Job Options to run monitoring in stand-alone mode 
# Author : Venkat.Kaushik@cern.ch
# Date   : Jan 2010
###################################################

doCSCRDO = False
doCSCPRD = False
doCSCClus = False
doLocal = True
doCSCSegm = True

import sys

######################################################################################
# helper gets input collection
##############################
def getInputFiles(dir, AODHLTP):

  import fnmatch, os, glob
  if len(AODHLTP) > 0:
    del AODHLTP[:]

  if len(dir) == 2:
    all_dirs = glob.glob(dir[0])
    for thedir in all_dirs:
      print 'i am in', thedir
      for fileName in os.listdir(thedir):
        if fnmatch.fnmatch(fileName, dir[1] ):
          fullpath = os.path.join(thedir,fileName)
          print 'file ', fullpath
          AODHLTP.append(fullpath)


######################################################################################
# Input files
##############################

AODir2=['/afs/cern.ch/work/p/panagoul/CSC_data', 'data15_13TeV.00280950*']

CSCInputFiles = []
if doLocal:
  getInputFiles(AODir2, CSCInputFiles)
  print CSCInputFiles
  if len(CSCInputFiles) < 1:
    print "No input files specified yet! Cannot do anything."
    sys.exit(0)
  else:
    for file in CSCInputFiles:
      print file

######################################################################################
# Auto configure everything
#################################

import AthenaPoolCnvSvc.ReadAthenaPool
if doLocal:
  ServiceMgr.EventSelector.InputCollections= CSCInputFiles
else:
  ServiceMgr.EventSelector.InputCollections= [ "/raid01/venkat/dataset/data/hlt/2011/177531/express_express/data11_7TeV.00177531.express_express.merge.AOD.x93_m755._lb0057-lb0076._0001.1" ]

ServiceMgr.PoolSvc.AttemptCatalogPatch = True;
ServiceMgr.OutputLevel=ERROR


if 1:
  #
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

  if len(athenaCommonFlags.FilesInput()) == 0:
    athenaCommonFlags.FilesInput = ServiceMgr.EventSelector.InputCollections
    
  #
  import AthenaPoolCnvSvc.ReadAthenaPool
    
  import AthenaPoolCnvSvc.WriteAthenaPool
    
  #
  from RecExConfig.RecFlags import rec

  rec.AutoConfiguration          .set_Value_and_Lock(["everything"])
    
  rec.doCBNT                     .set_Value_and_Lock(False)
  rec.doESD                      .set_Value_and_Lock(False)
  rec.doHist                     .set_Value_and_Lock(False)
  rec.doPerfMon                  .set_Value_and_Lock(False)
    
  rec.doWriteAOD                 .set_Value_and_Lock(False)
  rec.doWriteESD                 .set_Value_and_Lock(False)
  rec.doWriteTAG                 .set_Value_and_Lock(False)
  rec.doWriteTAGCOM              .set_Value_and_Lock(False)

  #
  from AthenaCommon.Include import include
    
  include ("RecExCommon/RecExCommon_topOptions.py")

#
for i in ServiceMgr:  i.OutputLevel=INFO


######################################################################################
# Output file 
##############################
OutputFile = 'Monitoring.CSC.root'

myc = 0
if 1:
  from PyUtils.MetaReaderPeeker import metadata
  isdata = metadata['eventTypes'][0]
  isdata = isdata.replace('IS_','')
  runno = metadata['runNumbers'][0]
  streamname = metadata['processingTags'][0]
  streamname = streamname.replace('Stream','')
  events = metadata['nentries']
  OutputFile = 'Monitoring.CSC.'+isdata.lower()+'.'+str(runno)+'.'+streamname.lower()+'.'+str(events)+'.root'


from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["CSCMonitor DATAFILE='" + OutputFile + "' OPT='RECREATE'"]

######################################################################################
# Add the Toplevel Alg
##############################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

###############################
# Configure the job
###############################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.RecFlags import rec
rec.readRDO=False
rec.readAOD=True
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False


###############################
# Setup Athena Monitoring
###############################
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

if not 'DQMonFlags' in dir():
  from AthenaMonitoring.DQMonFlags import DQMonFlags
  from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags

from AthenaCommon.AppMgr import ToolSvc


###############################
# Setup CfgMgr
###############################

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone

###############################
# CSC monitoring algorithms
###############################

muonOutputLevel=INFO

#---------------------------------------------------------------
# CSC PRD Monitoring
#--------------------------------------------------------------

CalibCscStripFitter = getPublicTool("CalibCscStripFitter")
ToolSvc += CalibCscStripFitter

if doCSCPRD:
  cscprdRawMonMan = AthenaMonManager(name="CscPrdRawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)

  from CscRawDataMonitoring.CscRawDataMonitoringConf import CscPrdValAlg
  cscPrdESDValAlg = CscPrdValAlg ( name = 'cscPrdESDValAlg',
                              CSCPrepRawDataPath = 'Muon/MuonRawDataMonitoring/CSC/PRD',
                              CSCPrepRawDataKey = 'CSC_Measurements',
                              NoiseCutADC = 100,
                              CSCStripFitter = ToolSvc.CalibCscStripFitter
                            )
  ToolSvc += cscPrdESDValAlg
  cscprdRawMonMan.AthenaMonTools += [ cscPrdESDValAlg ]

  topSequence += cscprdRawMonMan
  cscprdRawMonMan.FileKey = "CSCMonitor"


#---------------------------------------------------------------
# CSC Cluster Monitoring
#---------------------------------------------------------------
if doCSCClus:
  ## trigger-aware monitoring: sample seletion triggers
  csc_evtSelectionTriggers = [ "L1_MU10", "L1_MU15", "EF_mu20_muCombTag_NoEF", "EF_mu15", "EF_mu15_mu10_EFFS", "EF_2mu10", "EF_2mu10_loose" ]

  ## retrieve TDT (this should be done already?)
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.OutputLevel=ERROR
  ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

  cscesdRawMonMan = AthenaMonManager(name="CscClusterEsdRawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)

  from CscRawDataMonitoring.CscRawDataMonitoringConf import CscClusterValAlg
  cscClusterESDValAlg = CscClusterValAlg( name = 'cscClusterESDValAlg',
                                       CSCClusterPath = 'Muon/MuonRawDataMonitoring/CSC/Clusters',
                                       CSCClusterKey = 'CSC_Clusters',
                                       CSCPrepRawDataKey = 'CSC_Measurements',
                                       CSCQmaxCutADC = 200,
                                       CSCCalibTool = getPrivateTool("CscCalibTool"),
                                       CSCStripFitter = getPrivateTool("CalibCscStripFitter"),
                                       CSCTrigDecisionTool = ToolSvc.TrigDecisionTool,
                                       CSCDoEventSelection = False,
                                       CSCEventSelTriggers = csc_evtSelectionTriggers
                                     )

  ToolSvc += cscClusterESDValAlg
  cscesdRawMonMan.AthenaMonTools += [cscClusterESDValAlg]

  topSequence += cscesdRawMonMan
  cscesdRawMonMan.FileKey = "CSCMonitor"


#---------------------------------------------------------------
# CSC Segment Monitoring
#---------------------------------------------------------------
if doCSCSegm:
  cscsegmMonMan = AthenaMonManager(name="CscSegmEsdRawMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = muonOutputLevel)

  from CscRawDataMonitoring.CscRawDataMonitoringConf import CSCSegmValAlg
  from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags
  MuonDQAFlags.doMuonSegmMon = True
  
  ## TDT instance (this should be done already?)
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.OutputLevel=ERROR
  ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

  clusStatWords = [ "Unspoiled", "Simple", "Edge", "MultiPeak", "Narrow",
                        "Wide", "Skewed", "QRatInc", "StripFitFailed",
                        "SplitUnspoiled", "SplitSimple", "SplitEdge", "SplitMultiPeak",
                        "SplitNarrow", "SplitWide", "SplitSkewed", "SplitQRatInc",
                        "SplitStripFitFailed", "Undefined" ]
  ## trigger-aware monitoring: sample seletion triggers (express stream menu physics_pp_v2)
  evtSelectionTriggers = [  "L1_MU10", "L1_MU15", "EF_mu20_muCombTag_NoEF", "EF_mu15", "EF_mu15_mu10_EFFS", "EF_2mu10", "EF_2mu10_loose" ]

  CSCSegmValAlg = CSCSegmValAlg ( name = "CSCSegmValAlg",
    TrigDecisionTool = ToolSvc.TrigDecisionTool, DoEventSelection = False, EventSelTriggers = evtSelectionTriggers,
    SegmentSlopeCut = 0.07, ClusterStatus = clusStatWords)

  ToolSvc += CSCSegmValAlg
  cscsegmMonMan.AthenaMonTools += [ CSCSegmValAlg ]

  topSequence += cscsegmMonMan
  cscsegmMonMan.FileKey = "CSCMonitor"


# in order to check available detectors
from RecExConfig.RecFlags  import rec


##########################
# Events to run over
##########################

theApp.EvtMax = 1000

