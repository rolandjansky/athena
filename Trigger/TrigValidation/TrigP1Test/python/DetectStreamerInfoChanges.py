#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
import sys
import logging
import re
from shutil import copyfile
import ROOT


log = logging.getLogger(__name__)

# Needed for generating TStreamerInfo of xAOD::TrigConfKeys
ROOT.gInterpreter.ProcessLine('#include "xAODTrigger/TrigConfKeys.h"')

doTrigEDMOnly = True
bs_filename = 'bs-streamerinfos.root'

from TrigSerializeResult.dictwrite import update_streamerinfos


def append_versions(objects, cname):
    if "CPMTowerAuxContainer" in cname or "CMMEtSumsAuxContainer" in cname or "JEMHitsAuxContainer" in cname or "JEMEtSumsAuxContainer" in cname or "JetElementAuxContainer" in cname or "AFPDataAuxContainer" in cname or "_Alloc_hide" in cname or cname == "string" or "CMXJetTobAuxContainer" in cname or "RODHeaderAuxContainer" in cname or "CMMCPHitsAuxContainer" in cname or "CMXCPHitsAuxContainer" in cname or "TruthVertexAuxContainer" in cname or "CMXEtSumsAuxContainer" in cname or "CMXJetHitsAuxContainer" in cname or "CMMJetHitsAuxContainer" in cname or "CPMHitsAuxContain" in cname:
        return
    isxAOD = re.search('_v[0-9]$', class_name)
    isxAODDV = re.search('_v[0-9]>$', class_name)
    version = "1"
    if isxAOD is not None:
        version = [int(s) for s in cname.split("_v") if s.isdigit()][-1]
        for i in range (1, version+1):
            objects.append(cname.replace("_v"+str(version), "_v"+str(i)))
    elif isxAODDV is not None:
        version = [int(s) for s in re.split('_v|>',cname) if s.isdigit()][-1]
        for i in range (1, version+1):
            objects.append(cname.replace("_v"+str(version)+">", "_v"+str(i)+">"))
    else:
        pass
        #objects.append(cname)

if __name__ == '__main__':
    objects = []
    if doTrigEDMOnly:
        from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3
        from TrigEDMConfig.DataScoutingInfo import DataScoutingIdentifiers
        BS_destinations = ["BS"] + list(DataScoutingIdentifiers.keys())
        log.warning("BS_destinations = {}".format(BS_destinations))
        for item in TriggerHLTListRun3:
            if any(bs in item[1].split() for bs in BS_destinations):
                objects.append(item[0].split("#")[0])
    else:
        # try to process all classes from the build
        os.system("get_files -data clid.db")
        classid = open("clid.db", "r")
        for cl in classid:
            items = cl.split("; ")
            class_name = items[3].rstrip()
            isxAOD = re.search('_v[0-9].*$', class_name)
            if isxAOD is not None:
                log.info (class_name + " is versioned xAOD class")
                append_versions(objects, class_name)
            else:
                log.info (class_name + " is not an xAOD class")

    objects = list(set(objects))

    log.warning("Will extract TStreamerInfo for the following types:")
    log.warning(objects)

    copyfile(bs_filename, "original_" + bs_filename)
    
    updated_objects = []
    update_streamerinfos(objects, updated_objects)

    # ignore messages about these types - apparently they will always be displayed
    black_list = ["string::_Alloc_hider", "string"]
    new_objects = [item for item in updated_objects if item[0] not in black_list]

    if len(new_objects) > 0:
        log.warning("The following {} new objects were detected:".format(len(new_objects)))
        for item in new_objects:
            log.warning("{}: 0x{:x}".format(item[0], item[1]))
        log.warning("Please ask the Trigger EDM coordinator to add them to bs-streamerinfos.root")
    else:
        log.warning("No new objects detected")

    exit_code_err = len(new_objects)
    if exit_code_err:
        log.error("New TStreamerInfos detected")
    else:
        log.info("No new TStreamerInfos detected")

    log.info("DONE")

    sys.exit(exit_code_err)
