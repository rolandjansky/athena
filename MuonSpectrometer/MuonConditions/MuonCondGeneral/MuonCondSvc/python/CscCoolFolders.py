# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Define all csc calibration cool constant folders for easy reference.

Usage:
  from MuonCondSvc import CscCoolFolders

  print CscCoolFolders.ped
  >> "PED"
  print CscCoolFolders.dict("ped")
  >> "PED"

  folderKeys = ["ped","f001","rms"]
  folders = folderList(folderKeys)
  print folders
  >> ["/CSC/PED","/CSC/FTHOLD","/CSC/RMS"]
  
  folders = folderList(folderKeys,True)
  print folders
  >> ["/CSC/ONL/PED","/CSC/ONL/FTHOLD","/CSC/ONL/RMS"]

"""
pslope  = "PSLOPE"   #Pulser run gain
ped 	  = "PED"      #Pedestals
noise	  = "NOISE"    #Noise (Sigma from pedestal gaussian fit)
rms     = "RMS"      #Rms of pedestal histograms (not from fit)
f001    = "FTHOLD"   #F001 threshold values
status    = "STAT"     #Status Bits
peakt   = "PEAKT"      #Peaking time 
t0base  = "T0BASE"
t0phase  = "T0PHASE"
#thold   = "/CSC/THOLD"   #Theshold (unused)
#gain = "/CSC/GAIN"       #gain (unused)
#width = "/CSC/WIDTH"     #width (unused)


dict = {
    "pslope" : pslope,
    "ped" : ped,
    "noise" : noise,
    "rms" : rms,
    "f001" : f001,
    "status" : status,
    "peakt" : peakt,
    "t0base" : t0base,
    "t0phase" : t0phase,
    } 


#retrieve a list of folder names
def folderList(folderKeys, getOnline = False):
  """Get a list of folders based upon an input list of keys"""
  folders = []
  for folderKey in folderKeys:
    if not (folderKey in dict):
      print "WARNING folder key " + folderKey + " not recognized! All keys and folder pairs are: "
      print dict
    else:
      if(getOnline):
        folders += ['/CSC/ONL/'+ dict[folderKey]]
      else:
        folders += ['/CSC/'+ dict[folderKey]]
  return folders
