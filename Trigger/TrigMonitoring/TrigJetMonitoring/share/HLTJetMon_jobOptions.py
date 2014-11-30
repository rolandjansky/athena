
###################################################
# File   : Job Options to run monitoring in stand-alone mode 
# Author : Venkat.Kaushik@cern.ch
# Date   : Jan 2010
###################################################
doLocal = True

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

AODir2011 = ['/raid01/venkat/dataset/data/csc/2011/mcpskim/data10_7TeV.*.phys*', 'data10_7TeV*DESD_ZMUMU*']
AODir2012 = ['/raid01/venkat/dataset/hlt-met/2012', 'AOD*pool*']
AODir2014 = ['/afs/cern.ch/work/s/sawyer/data','data12_8TeV*.pool.root']
TestDir   = ['/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/','valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root']

HLTJetInputFiles = []
if doLocal:
  getInputFiles(TestDir, HLTJetInputFiles)
  print HLTJetInputFiles
  if len(HLTJetInputFiles) < 1:
    print "No input files specified yet! Cannot do anything."
    sys.exit(0)
  else:
    for file in HLTJetInputFiles:
      print file



#################################
# Auto configure everything
#################################
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=HLTJetInputFiles
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
    
  rec.abortOnUncheckedStatusCode .set_Value_and_Lock(False)
    
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

##############################
# Output file
##############################
OutputFile = 'Monitoring.HLTjet.root'

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["MonitorHLTjet DATAFILE='" + OutputFile + "' OPT='RECREATE'"]

############################################################

##############################
# Add the Toplevel Alg
##############################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

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

HLTjetOutputLevel=INFO
myTrigDecisionTool = ""
if(DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())):
  myTrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
else:
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.OutputLevel=ERROR
  ToolSvc.TrigDecisionTool.Navigation.OutputLevel = FATAL

HLTjetMonMan = AthenaMonManager(name="HLTMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),
                                   Environment         = DQMonFlags.monManEnvironment(),
                                   OutputLevel         = HLTjetOutputLevel)

topSequence += HLTjetMonMan

# in order to check available detectors
from RecExConfig.RecFlags  import rec

try:
  from TrigJetMonitoring.TrigJetMonitoringConfig import TrigJetMonitoringTool
  HLTjetMonMan.AthenaMonTools += TrigJetMonitoringTool()

except:
  print "Problems with HLTJetTool, tool not enabled"

HLTjetMonMan.FileKey = "MonitorHLTjet"

print HLTjetMonMan;

##########################
# Events to run over
##########################
theApp.EvtMax = 10

