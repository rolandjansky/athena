#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TriggerEDMRun3 import TriggerHLTListRun3, EDMDetailsRun3, AllowedOutputFormats
from AthenaCommon.Logging import logging
log = logging.getLogger('testEDMRun3')

from CLIDComps.clidGenerator import clidGenerator
cgen = clidGenerator("", False)


def isCLIDDefined(typename):
  c = cgen.genClidFromName(typename)
  return (cgen.getNameFromClid(c) is not None)

def main():
  import re
  serializable_names = []
  serializable_names_no_label = []
  #Check for duplicates
  if len(set(TriggerHLTListRun3))!=len(TriggerHLTListRun3):
    log.error("Duplicates in TriggerHLTListRun3")
    import collections.abc
    for item, count in collections.Counter(TriggerHLTListRun3).items():
        if count > 1:
          log.error(str(count) + "x: " + str(item))
  for TriggerSerializable in TriggerHLTListRun3:
    serializable_name = TriggerSerializable[0]
    serializable_name_no_label = re.sub(r"\#.*", "", serializable_name)
    if '#' not in serializable_name:
      log.error("no label for " + serializable_name)
      return 1
    #Check container has a CLID
    if not isCLIDDefined(serializable_name_no_label):
      log.error("no CLID for " + serializable_name)

    #check for Aux "."
    if "Aux" in serializable_name and "Aux." not in serializable_name:
      log.error("no final Aux. in label for " + serializable_name)

    file_types = TriggerSerializable[1].split(" ")

    for file_type in file_types:
      if file_type not in AllowedOutputFormats:
        log.error("unknown file type " + file_type + " for " + serializable_name)
        return 1

    serializable_names.append(serializable_name)
    serializable_names_no_label.append(serializable_name_no_label)

  #check EDMDetails
  for EDMDetail in EDMDetailsRun3.keys():
    if EDMDetail not in serializable_names_no_label:
      log.warning("EDMDetail for " + EDMDetail + " does not correspond to any name in TriggerList")

if __name__ == "__main__":
  import sys
  sys.exit(main())
