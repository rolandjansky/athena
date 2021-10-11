#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('testEDM')

from TriggerEDM import (TriggerL2List, TriggerEFList, TriggerResultsList, TriggerResultsRun1List,
                        TriggerLvl1List, TriggerIDTruth, TriggerHLTList)
from TriggerEDMRun2 import EDMDetails
import TriggerEDMRun1
from CLIDComps.clidGenerator import clidGenerator
cgen = clidGenerator("", False)

def isCLIDDefined(typename):
  c = cgen.genClidFromName(typename)
  return (cgen.getNameFromClid(c) is not None)

def main():
  import re
  serializable_names = []
  serializable_names_no_label = []
  TriggerList = TriggerL2List + TriggerEFList + TriggerResultsList + TriggerResultsRun1List + TriggerLvl1List + TriggerIDTruth + TriggerHLTList
  TriggerList += TriggerEDMRun1.TriggerL2List + TriggerEDMRun1.TriggerEFList + TriggerEDMRun1.TriggerResultsRun1List
  for TriggerSerializable in TriggerList:
    #print TriggerSerializable 
    serializable_name = TriggerSerializable[0]
    serializable_name_no_label = re.sub(r"\#.*", "", serializable_name)
    if '#' not in serializable_name:
      log.error("ERROR, no label for " + serializable_name)
      return 1
    #Check container has a CLID
    if not isCLIDDefined(serializable_name_no_label):
      log.error("no CLID for " + serializable_name)
      return 1
    if serializable_name_no_label not in EDMDetails.keys():
      log.error(serializable_name_no_label + " does not correspond to any name in EDMDetails")

    
    #check for Aux "."
    if "Aux" in serializable_name and "Aux." not in serializable_name:
      log.error("no final Aux. in label for " + serializable_name)
    
    file_types = TriggerSerializable[1].split(" ")

    allowed_file_types = ("", "BS", "DS", "ESD", "AODFULL", "AODSLIM", "AODVERYSLIM", "AODBLSSLIM", "AODCONV")

    for file_type in file_types:
      if file_type not in allowed_file_types:
        log.error("unknown file type " + file_type + " for " + serializable_name)
        return 1

    serializable_names.append(serializable_name)
    serializable_names_no_label.append(serializable_name_no_label)

  #check EDMDetails
  for EDMDetail in EDMDetails.keys():
    if EDMDetail not in serializable_names_no_label:
      log.warning("EDMDetail for " + EDMDetail + " does not correspond to any name in TriggerList")

if __name__ == "__main__":
  import sys
  sys.exit(main())
