#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string, os

##---------------------------------------------
##  Configuration
##---------------------------------------------

## list OUTPUT datasets from your job here

datasets = []

datasets += [ "user10.DanielScheirich.12.data10.00152166.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152214.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152220.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152221.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152345.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152409.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152441.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152508.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152777.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152845.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152878.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152933.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.12.data10.00152994.JpsiTrigger" ]
datasets += [ "user10.DanielScheirich.13.data10.00152844.JpsiTrigger" ]

##---------------------------------------------
##  Script itself
##---------------------------------------------

## loop over datasets

bigmerge = []

i = 0
for dataset in datasets :

  print "Merging ", dataset

  files = os.listdir("./" + dataset)
  filesToMerge = []

  for file in files :

    parts = file.split(".")

    if( len(parts) >= 10 and parts[9] == "root" ) :

      print "    ", dataset+"/"+file

      filesToMerge += [ dataset+"/"+file ]

  outfile = dataset+".bphys.root"

  print "  => " + outfile

  ## command

  command = "hadd " + outfile + " " + " ".join(filesToMerge)

  print command

  os.system( command )

  bigmerge += [ outfile ]

print "Big merge"

command = "hadd all.root " + " ".join(bigmerge)

print command

os.system( command )


## End of script
