# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# @file: POOL2EI_joboptions.py
# @brief: simple joboptions to convert AOD to EI
# @date Feb 2014
# @updated: Jan 2022
# @author Javier Sanchez <Javier.Sanchez@ific.uv.es>
#

__version__ = "$Revision$"
__author__ = "Javier Sanchez <Javier.Sanchez@ific.uv.es>"


from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# algorithm
from EventIndexProducer.POOL2EI_Lib import POOL2EI
pool2ei = POOL2EI('pool2ei', OutputLevel=Lvl.INFO)
job += pool2ei

# service
from EventIndexProducer.POOL2EI_Lib import POOL2EISvc
pool2eisvc = POOL2EISvc(algo=pool2ei)
svcMgr += pool2eisvc
theApp.CreateSvc += [pool2eisvc.getFullJobOptName()]


# --------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
# --------------------------------------------------------------
try:
    svcMgr.MessageSvc.OutputLevel = Level
except Exception:
    svcMgr.MessageSvc.OutputLevel = INFO


# --------------------------------------------------------------
# Number of events to write
# --------------------------------------------------------------
try:
    theApp.EvtMax = EvtMax
except Exception:
    theApp.EvtMax = -1


# --------------------------------------------------------------
# Input collection name
# --------------------------------------------------------------
try:
    svcMgr.EventSelector.InputCollections = In
except Exception:
    svcMgr.EventSelector.InputCollections = ["test.root"]


# --------------------------------------------------------------
# Output Event Index file name
# --------------------------------------------------------------
try:
    job.pool2ei.Out = Out
except Exception:
    import os
    job.pool2ei.Out = 'pool2ei.{:08i}.pkl'.format(os.getpid())


# --------------------------------------------------------------
# Output Provenance references flag
# --------------------------------------------------------------
try:
    job.pool2ei.DoProvenanceRef = DoProvenanceRef
except Exception:
    job.pool2ei.DoProvenanceRef = False

# --------------------------------------------------------------
# Output Trigger Info flag
# --------------------------------------------------------------
try:
    job.pool2ei.DoTriggerInfo = DoTriggerInfo
except Exception:
    job.pool2ei.DoTriggerInfo = True

# --------------------------------------------------------------
# Send to Broker flag
# --------------------------------------------------------------
try:
    job.pool2ei.SendToBroker = SendToBroker
except Exception:
    job.pool2ei.SendToBroker = False

# --------------------------------------------------------------
# Input dataset name. Overrrides value read for job options
# --------------------------------------------------------------
try:
    job.pool2ei.EiDsName = EiDsName
except Exception:
    job.pool2ei.EiDsName = None

# --------------------------------------------------------------
# Test Message Broker. Overrrides value read for job options
# --------------------------------------------------------------
try:
    job.pool2ei.TestBrk = TestBrk
except Exception:
    job.pool2ei.TestBrk = False

# --------------------------------------------------------------
# EI Format. Overrrides value read for job options
# --------------------------------------------------------------
try:
    job.pool2ei.EiFmt = EiFmt
except Exception:
    job.pool2ei.EiFmt = 0

# --------------------------------------------------------------
# Tier0 job parameters
# --------------------------------------------------------------
try:
    job.pool2ei.TaskID = TaskID
except Exception:
    job.pool2ei.TaskID = None

try:
    job.pool2ei.JobID = JobID
except Exception:
    job.pool2ei.JobID = None

try:
    job.pool2ei.AttemptNumber = AttemptNumber
except Exception:
    job.pool2ei.AttemptNumber = None


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = In
athenaCommonFlags.FilesInput = In

# from RecExConfig.RecFlags import rec
# rec.AutoConfiguration=['everything']
rec.readRDO = False
rec.readESD = False
rec.readAOD = True
rec.readTAG = False
rec.doWriteAOD = False
rec.doWriteESD = False
rec.doWriteTAG = False

log = logging.getLogger("Py:pool2ei")

from PyUtils.MetaReaderPeekerFull import metadata, convert_itemList
from AthenaCommon.GlobalFlags import globalflags

globalflags.InputFormat = 'pool'

# find key for 'EventStreamInfo'
metadata_items = metadata['metadata_items']
for k in metadata_items:
    if metadata_items[k] == 'EventStreamInfo':
        EventStreamInfo_key = k
        break
else:
    EventStreamInfo_key = None

# item_list
if EventStreamInfo_key is not None:
    item_list = [a for a, b in metadata[EventStreamInfo_key]['itemList']]
else:
    item_list = []

job.pool2ei.meta_hlt_hltconfigkeys = '/TRIGGER/HLT/HltConfigKeys' in \
                                     metadata_items
job.pool2ei.meta_hlt_prescalekey = '/TRIGGER/HLT/PrescaleKey' in metadata_items
job.pool2ei.meta_lvl1_lvl1configkey = '/TRIGGER/LVL1/Lvl1ConfigKey' in \
                                      metadata_items
job.pool2ei.meta_hlt_menu = '/TRIGGER/HLT/Menu' in metadata_items
job.pool2ei.meta_lvl1_menu = '/TRIGGER/LVL1/Menu' in metadata_items
job.pool2ei.meta_triggermenu = 'TriggerMenu' in metadata_items
job.pool2ei.meta_triggermenujson_hlt = 'TriggerMenuJson_HLT' in metadata_items
job.pool2ei.meta_triggermenujson_l1 = 'TriggerMenuJson_L1' in metadata_items

job.pool2ei.item_eventinfo = 'EventInfo' in item_list
job.pool2ei.item_xaod_eventinfo = 'xAOD::EventInfo' in item_list
job.pool2ei.item_xaod_TrigConfKeys = 'xAOD::TrigConfKeys' in item_list
job.pool2ei.item_xaod_TrigDecision = 'xAOD::TrigDecision' in item_list

if not (job.pool2ei.item_eventinfo or job.pool2ei.item_xaod_eventinfo):
    # EventInfo should exist
    job.pool2ei.item_eventinfo = True


log.info("meta_hlt_hltconfigkeys: {}".format(
    job.pool2ei.meta_hlt_hltconfigkeys))
log.info("meta_hlt_prescalekey: {}".format(job.pool2ei.meta_hlt_prescalekey))
log.info("meta_lvl1_lvl1configkey: {}".format(
    job.pool2ei.meta_lvl1_lvl1configkey))
log.info("meta_hlt_menu: {}".format(job.pool2ei.meta_hlt_menu))
log.info("meta_lvl1_menu: {}".format(job.pool2ei.meta_lvl1_menu))
log.info("meta_triggermenu: {}".format(job.pool2ei.meta_triggermenu))
log.info("meta_triggermenujson_hlt: {}".format(
    job.pool2ei.meta_triggermenujson_hlt))
log.info("meta_triggermenujson_l1: {}".format(
    job.pool2ei.meta_triggermenujson_l1))
log.info("item_eventinfo: {}".format(job.pool2ei.item_eventinfo))
log.info("item_xaod_eventinfo: {}".format(job.pool2ei.item_xaod_eventinfo))
log.info("item_xaod_TrigConfKeys: {}".format(
    job.pool2ei.item_xaod_TrigConfKeys))
log.info("item_xaod_TrigDecision: {}".format(
    job.pool2ei.item_xaod_TrigDecision))


try:
    if EventStreamInfo_key is not None:
        type0 = metadata[EventStreamInfo_key]['eventTypes'][0]['type']
        globalflags.DataSource = 'data' if "IS_DATA" in type0 else 'geant4'
    else:
        globalflags.DataSource = 'geant4'
except Exception:
    # take the risk
    globalflags.DataSource = 'data'

# set projectName from metadata
try:
    rec.projectName = metadata['/TagInfo']['project_name']
except Exception:
    pass

# if EVNT, disable trigger processing
if job.pool2ei.DoTriggerInfo:
    try:
        if (EventStreamInfo_key is not None and 'StreamEVGEN' in
                metadata[EventStreamInfo_key]['processingTags']):
            log.info("Disable trigger processing for EVNT files")
            job.pool2ei.DoTriggerInfo = False

        if (EventStreamInfo_key is not None and "IS_SIMULATION" in
                metadata[EventStreamInfo_key]['eventTypes'][0]['type']):
            if not (job.pool2ei.meta_hlt_hltconfigkeys or
                    job.pool2ei.item_xaod_TrigConfKeys):
                log.info("Disable trigger processing for MC files "
                         "with no trigger inside")
                job.pool2ei.DoTriggerInfo = False
    except Exception:
        pass
