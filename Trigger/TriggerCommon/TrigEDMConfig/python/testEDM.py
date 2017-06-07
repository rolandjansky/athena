#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TriggerEDM import *
from CLIDComps.clidGenerator import clidGenerator
cgen = clidGenerator("", False)

def isCLIDDefined(typename):
  c = cgen.genClidFromName(typename)
  pkg = cgen.getPackageFromClid(c)     
  return bool(pkg)

def main():
  import re
  serializable_names = []
  serializable_names_no_label = []
  TriggerList = TriggerL2List + TriggerEFList + TriggerResultsList + TriggerResultsRun1List + TriggerLvl1List + TriggerIDTruth + TriggerHLTList
  for TriggerSerializable in TriggerList:
    #print TriggerSerializable 
    serializable_name = TriggerSerializable[0]
    serializable_name_no_label = re.sub(r"\#.*", "", serializable_name)
    if not '#' in serializable_name:
      print "ERROR, no label for " + serializable_name
      return 1
    #Check container has a CLID
    if not isCLIDDefined(serializable_name_no_label):
      print "ERROR, no CLID for " + serializable_name
    if not serializable_name_no_label in EDMDetails.keys():
      print "ERROR: " + serializable_name_no_label + " does not correspond to any name in EDMDetails" 

    
    #check for Aux "."
    if "Aux" in serializable_name and not "Aux." in serializable_name:
      print "ERROR, no final Aux. in label for " + serializable_name
    
    file_types = TriggerSerializable[1].split(" ")
    allowed_file_types = ("", "BS", "DS", "ESD", "AODFULL", "AODSLIM", "AODVERYSLIM", "AODBLSSLIM")
    for file_type in file_types:
      if not file_type in allowed_file_types:
        print "ERROR, unknown file type " + file_type + " for " + serializable_name
        return 1

    serializable_names.append(serializable_name)
    serializable_names_no_label.append(serializable_name_no_label)

  #check EDMDetails
  for EDMDetail in EDMDetails.keys():
    EDMDetail_values = EDMDetails[EDMDetail]
    if not EDMDetail in serializable_names_no_label:
      print "WARNING: EDMDetail for " + EDMDetail + " does not correspond to any name in TriggerList" 

if __name__ == "__main__":
  import sys
  try:
    sys.exit(main())
  except RuntimeError, e:
    print e
    sys.exit(1)
  except KeyboardInterrupt:
    sys.exit(1)


