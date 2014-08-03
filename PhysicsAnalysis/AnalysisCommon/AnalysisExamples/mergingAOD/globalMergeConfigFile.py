#!/usr/local/pkg/Python-2.2.2/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Original by Davide Costanzo with a few modification from Ketevi A. Assamagan
# Modified by Jack Cranshaw for rome tags 26.04.05

# You have to execute this job in python:
#lxplus> python
#python>>> import globalMergeWithTags
#python>>> Ctrl D
#lxplus>

# as output:
# mergeTags.sh - the script to do the merging
# a number of .py which contains the lists of AOD files to merge into one

import string
import os
import sys

import mergeconfig

# get parameters
try:
  myAODfiles  = mergeconfig.dir+'/'+mergeconfig.prefix+'/'+mergeconfig.prefix
  pithyprefix = mergeconfig.prefix
  postfix = mergeconfig.postfix
except:
  print "MUST define dir and prefix and postfix"
  sys.exit(1)

try:
  dataset     = mergeconfig.dsid
except:
  dataset     = 0

#number of AOD files to be merged into a single file
try:
   NFileToMerge = mergeconfig.nfiles
except:
   NFileToMerge = 20

# is it a different catalog
try:
  source = mergeconfig.catalog
except:
  source = "file:PoolFileCatalog.xml"

try:
  seed = mergeconfig.dbseed
  print "seed in conf file"
except:
  seed = dataset
  print "NO SEED in conf file"

print "**"
print "myAODfiles = ",myAODfiles
print "pithyprefix = ",pithyprefix
print "dataset = ",dataset
print " N = ",NFileToMerge
print "source = ",source

# get the AODs to be merged:
#
# if FClistPFN is not found then you should source the TestRelease setup
get = os.popen('FClistPFN -u '+source+' | grep '+myAODfiles)
AodList = get.readlines()
NMergedFiles=0
fileNumber=0
AodList.sort()

# Ketevi - find out the total number of AOD files to merge 
size = 0
for name in AodList :
   size = size+1

print "Found ", size, "files to merge"

if (size>0):
    # The script mergeTags.sh is produced as output
    # you can then submit that script to LSF batch or run interactively to merge the AOD files
    script=open('mergeTags.sh','w')

    i = 0
    for filename in AodList :
        AodFile = string.strip(filename)
        i = i+1
        if NMergedFiles == NFileToMerge:
            NMergedFiles = 0    
            fileNumber = fileNumber+1
            seed = seed+1
            f.close()
        if NMergedFiles == 0 :
            fileToOpen=postfix+'._'+str(fileNumber)+'.py'
            f=open(fileToOpen,'w')
            outfile=mergeconfig.dir+'/'+mergeconfig.prefix+'/'+postfix+"._"+str(fileNumber)+".AOD"
            script.write('athena.py MergeWithTags.py -c "DSID='+str(dataset)+'; dbseed='+str(seed)+'; mergeFile='+"'"+fileToOpen+"'"+"; OUTPUT='"+outfile+"'"+'"'+"\n")
            print "open file ", fileToOpen
            f.write("EventSelector.InputCollections = [ \n")
        f.write("'"+AodFile+"'")
        if NMergedFiles < NFileToMerge-1 and i < size : 
           f.write(", \n")
        else :
           f.write(']')

        NMergedFiles = NMergedFiles+1
else:
    print " ** No files for search string \n",myAODfiles, "\nfound in catalog"

