#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu_l1only')

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

include("TrigUpgradeTest/testL1_MT.py")


##########################################
# Some debug
##########################################
from AthenaCommon.AlgSequence import dumpSequence, AthSequencer
dumpSequence(topSequence)


##########################################
# Output configuration
##########################################

# Configure BS result only in the following three cases
# 1) Running in partition
# 2) Running with athenaHLT
# 3) Running with athena and saving BS output
import os
onlineWriteBS = os.getenv('TDAQ_PARTITION') is not None  # athenaHLT also sets this
offlineWriteBS = False # TODO: cover configuration of BS writing in athena
configureBSResult = onlineWriteBS or offlineWriteBS

if onlineWriteBS and opt.doWriteESD:
    __log.error("ESD writing configured in online mode")

from AthenaCommon.CFElements import findAlgorithm,findSubSequence
if opt.doWriteESD:
  from RecExConfig.RecFlags  import rec
  rec.doESD=True
  rec.doWriteESD=True


# this part uses parts from the NewJO configuration, it is very hacky for the moment

# try to find L1Decoder
log.info("No L1Decoder")
decObj = []

# Get the EDM list
from TrigEDMConfig.TriggerEDM import getTriggerEDMList
from TrigEDMConfig.TriggerEDMRun3 import persistent
TriggerFlags.EDMDecodingVersion = 3
TriggerFlags.ESDEDMSet = 'BS' if configureBSResult else 'ESD'
edmList = getTriggerEDMList(TriggerFlags.ESDEDMSet(), TriggerFlags.EDMDecodingVersion())

# Build the output ItemList
ItemList = []
for edmType, edmKeys in edmList.iteritems():
    edmType = persistent(edmType) if configureBSResult else edmType
    for key in edmKeys:
        ItemList.append(edmType+'#'+key)

if not configureBSResult:
    # Do not serialise EventInfo into BS, as it is already represented by the BS event header
    ItemList.append("xAOD::EventInfo#EventInfo")
    ItemList.append("xAOD::EventAuxInfo#EventInfoAux.")

for item in decObj:
    typeName = 'xAOD::TrigCompositeContainer#{}'.format(item)
    typeNameAux = 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(item)
    ItemList.append(typeName)
    # Temporary workaround for the case when item from decObj is explicitly in TriggerEDM list
    # to avoid adding it twice - with and without dynamic variables
    if typeNameAux+'-' not in ItemList:
        ItemList.append(typeNameAux)

ItemList = list(set(ItemList))

# Configure ESD writing
if opt.doWriteESD:
    import AthenaPoolCnvSvc.WriteAthenaPool
    from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
    StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
    StreamESD.ItemList = ItemList
