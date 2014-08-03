#!/usr/local/pkg/Python-2.2.2/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# by Davide Costanzo with a few modification from Ketevi A. Assamagan

# You have to execute this job in python:
#lxplus> python
#python>>> import globalMerge
#python>>> Ctrl D
#lxplus>

# as output:
# merge.sh - the script to do the merging
# a number of .py which contains the lists of AOD files to merge into one

import string
import os
import sys

# sub-string to select the AOD to be merged from your PoolFileCatalog.xml
myAODfiles = "BsDsPi.AOD_Trig900GeV.slim.100evts"

#number of AOD files to be merged into a single file
NFileToMerge = 20

# get the AODs to be merged:
AodList = []
import xml.dom.minidom
root  = xml.dom.minidom.parse("PoolFileCatalog.xml")
files = root.getElementsByTagName('File')
for file in files:
   # get PFN
   physical = file.getElementsByTagName('physical')[0]
   pfnNode  = physical.getElementsByTagName('pfn')[0]
   # convert UTF8 to Raw
   pfn = str(pfnNode.getAttribute('name'))
   # check basename
   if pfn.split('/')[-1].startswith(myAODfiles):
      # append
      AodList.append(pfn)

NMergedFiles=0
fileNumber=0
AodList.sort()

# Ketevi - find out the total number of AOD files to merge 
size = 0
for name in AodList :
   size = size+1

# The script merge.sh is produced as output
# you can then submit that script to LSF batch or run interactively to merge the AOD files
script=open('merge.sh','w')

i = 0
for filename in AodList :
    AodFile = string.strip(filename)
    i = i+1
    if NMergedFiles == NFileToMerge:
        NMergedFiles = 0    
        fileNumber = fileNumber+1
        f.close()
    if NMergedFiles == 0 :
        fileToOpen=myAODfiles+'Merge_'+str(fileNumber)+'.py'
        f=open(fileToOpen,'w')
        outfile = myAODfiles+'_Merge'+str(fileNumber)
        script.write("athena.py Merge.py -c 'mergeFile="+'"'+fileToOpen+'"'+'; OUTPUT="'+outfile+'.root"'+"'\n")
        print "open file ", fileToOpen
        f.write("EventSelector.InputCollections = [ \n")
    f.write("'"+AodFile+"'")
    if NMergedFiles < NFileToMerge-1 and i < size : 
       f.write(", \n")
    else :
       f.write(']')

    NMergedFiles = NMergedFiles+1


