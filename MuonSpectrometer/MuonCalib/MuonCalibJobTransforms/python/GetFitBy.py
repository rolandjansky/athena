# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

def GetFitBy(region_string, joblist="joblist.txt", default="CHAMBER"):
  if not os.path.exists(joblist):
    print "WARNING", joblist, "does not exist. Using defalut", default
    return default
  reg_items=region_string.replace("[", "").replace("]", "").split(",")
  if len(reg_items)!=3:
    print "WARNING Cannot interpret region string for GetFitBy. Using default", default
    return default
  try:
    station_nm=reg_items[0].strip()
    station_phi=int(reg_items[1])
    station_eta=int(reg_items[2])
  except ValueError:
    print "WARNING Failed to interpret region string for GetFitBy. Using default", default
    return default
	
  inf=file(joblist)
  inf.readline()
  for line in inf.readlines():
    items=line.split()
    if not len(items)==4:
      continue
    try:
      ln_station=items[0]
      ln_eta=int(items[1])
      ln_phi=int(items[2])
    except ValueError:
      continue
    if station_nm==ln_station and station_phi==ln_phi and station_eta==ln_eta:
      return items[3]
	
  print "WARNING", region_string, "not found in joblist. Using default ", default
  return default
