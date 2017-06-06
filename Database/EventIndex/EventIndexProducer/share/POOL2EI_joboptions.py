## @file: POOL2EI_joboptions.py
## @purpose: simple joboptions to convert AOD to EI
## @date Feb 2014
## @author Javier Sanchez <Javier.Sanchez@ific.uv.es>
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

__version__ = "$Revision$"
__author__  = "Javier Sanchez <Javier.Sanchez@ific.uv.es>"


from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# algorithm
from EventIndexProducer.POOL2EI_Lib import POOL2EI
pool2ei = POOL2EI('pool2ei', OutputLevel=Lvl.INFO)
job += pool2ei

# service
from EventIndexProducer.POOL2EI_Lib import  POOL2EISvc
pool2eisvc = POOL2EISvc(algo=pool2ei)
svcMgr += pool2eisvc
theApp.CreateSvc += [pool2eisvc.getFullJobOptName()]


#--------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
try:
    svcMgr.MessageSvc.OutputLevel = Level
except:
    svcMgr.MessageSvc.OutputLevel = INFO


#--------------------------------------------------------------
# Number of events to write
#--------------------------------------------------------------
try:
    theApp.EvtMax = EvtMax
except:
    theApp.EvtMax = -1


#--------------------------------------------------------------
# Input collection name
#--------------------------------------------------------------
try:
    svcMgr.EventSelector.InputCollections  = In 
except:
    svcMgr.EventSelector.InputCollections  = [ "test.root" ]


#--------------------------------------------------------------
# Output Event Index file name
#--------------------------------------------------------------
try:
    job.pool2ei.Out = Out
except:
    import os
    job.pool2ei.Out = 'pool2ei.{:08i}.pkl'.format(os.getpid())


#--------------------------------------------------------------
# Output Provenance references flag
#--------------------------------------------------------------
try: 
    job.pool2ei.DoProvenanceRef = DoProvenanceRef
except:
    job.pool2ei.DoProvenanceRef = False

#--------------------------------------------------------------
# Output Trigger Info flag
#--------------------------------------------------------------
try: 
    job.pool2ei.DoTriggerInfo = DoTriggerInfo
except:
    job.pool2ei.DoTriggerInfo = True

#--------------------------------------------------------------
# Send to Broker flag
#--------------------------------------------------------------
try: 
    job.pool2ei.SendToBroker = SendToBroker
except:
    job.pool2ei.SendToBroker = False

#--------------------------------------------------------------
# Input dataset name. Overrrides value read for job options
#--------------------------------------------------------------
try: 
    job.pool2ei.EiDsName = EiDsName
except:
    job.pool2ei.EiDsName = None

#--------------------------------------------------------------
# Test Message Broker. Overrrides value read for job options
#--------------------------------------------------------------
try: 
    job.pool2ei.TestBrk = TestBrk
except:
    job.pool2ei.TestBrk = False

#--------------------------------------------------------------
# EI Format. Overrrides value read for job options
#--------------------------------------------------------------
try: 
    job.pool2ei.EiFmt = EiFmt
except:
    job.pool2ei.EiFmt = 0

#--------------------------------------------------------------
# Tier0 job parameters
#--------------------------------------------------------------
try:
    job.pool2ei.TaskID = TaskID
except:
    job.pool2ei.TaskID = None
  
try:
    job.pool2ei.JobID = JobID
except:
    job.pool2ei.JobID = None

try:
    job.pool2ei.AttemptNumber = AttemptNumber
except:
    job.pool2ei.AttemptNumber = None


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = In

#from RecExConfig.RecFlags import rec
#rec.AutoConfiguration=['everything']
rec.readRDO=False
rec.readESD=False
rec.readAOD=True
rec.readTAG=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False


from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.GlobalFlags  import globalflags
globalflags.InputFormat = 'pool'
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'

# set projectName from inputFileSummary
try:
    # in first place try to get it from tag_info
    rec.projectName = inputFileSummary['tag_info']['project_name']
except:
    try: 
        # in last place from metadata
        rec.projectName = inputFileSummary['metadata']['/TagInfo']['project_name']
    except:
        pass

# set RUN1 flag
run_number = inputFileSummary['run_number'][0]
if run_number < 222222:
    job.pool2ei.RUN1 = True
else:
    job.pool2ei.RUN1 = False

log = logging.getLogger( "Py:pool2ei" )
# if EVNT, disable trigger processing
if  job.pool2ei.DoTriggerInfo:
    if 'StreamEVGEN' in inputFileSummary['stream_names']:
        log.info ("Disable trigger processing for EVNT files")
        job.pool2ei.DoTriggerInfo = False
        job.pool2ei.HaveHlt = False
        job.pool2ei.HaveXHlt = False

# if MC, check that trigger information is available in the metadata
if inputFileSummary['evt_type'][0] == "IS_SIMULATION":
    if 'metadata' not in inputFileSummary or '/TRIGGER/HLT/HltConfigKeys' not in inputFileSummary['metadata']:
        log.info ("Disable trigger processing for MC files with no trigger inside")
        job.pool2ei.DoTriggerInfo = False
        job.pool2ei.HaveHlt = False
        job.pool2ei.HaveXHlt = False

if job.pool2ei.DoTriggerInfo:

    # we should search for 'xAOD::TrigDecision' in eventdata_items but 'zero events' files do not
    # contain eventdata_items, so we look also for 'xAOD::TriggerMenuContainer' in metadata_items
    job.pool2ei.HaveXHlt = False
    if inputFileSummary.has_key('eventdata_items') and 'xAOD::TrigDecision' \
            in [ x[0] for x in inputFileSummary['eventdata_items'] ]:
        job.pool2ei.HaveXHlt = True
    if inputFileSummary.has_key('metadata_items') and 'xAOD::TriggerMenuContainer' \
            in [ x[0] for x in inputFileSummary['metadata_items'] ]:
        job.pool2ei.HaveXHlt = True

    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    trigcfg = TriggerConfigGetter("ReadPool")

    # ensure that /TRIGGER/HLT/PrescaleKey is always loaded
    # it is not loaded by TriggerConfigGetter when trigger metadata is missing
    # but it should have been loaded. We need LB-wise HLT prescale key
    if trigcfg.hasLBwiseHLTPrescalesAndL1ItemDef is False and inputFileSummary['evt_type'][0] == "IS_DATA":
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderWithTag("TRIGGER", "/TRIGGER/HLT/PrescaleKey", "HEAD")
        conddb.addFolderWithTag("TRIGGER", "/TRIGGER/HLT/Prescales", "HEAD")
        conddb.addFolderWithTag("TRIGGER", "/TRIGGER/LVL1/ItemDef", "HEAD")

    from AthenaCommon.AppMgr import ToolSvc
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    if job.pool2ei.HaveXHlt:
        tdt = Trig__TrigDecisionTool(name="TrigDecisionTool", 
                                     TrigConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc")
        tdt.TrigDecisionKey = 'xTrigDecision'
        tdt.UseAODDecision = False
    else:
        # use old decision
        tdt = Trig__TrigDecisionTool(name="TrigDecisionTool", 
                                     TrigConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc")
        tdt.TrigDecisionKey = 'TrigDecision'
        tdt.UseAODDecision = True
    ToolSvc += tdt

    if job.pool2ei.HaveXHlt:
        from TrigEDMConfig.TriggerEDM import EDMLibraries
        ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in EDMLibraries if 'TPCnv' not in e]
    
        from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
        hltouput = Lvl1ResultBuilderGetter()
    
        from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
        hltouput = HLTTriggerResultGetter()
    
    # To read files with trigger config stored as in-file meta-data,
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['ds']
    TriggerFlags.configForStartup = 'HLToffline'

    job.pool2ei.HaveHlt = True
else:
    job.pool2ei.HaveHlt = False
    job.pool2ei.HaveXHlt = False

