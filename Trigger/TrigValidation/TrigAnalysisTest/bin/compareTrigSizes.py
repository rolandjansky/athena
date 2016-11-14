#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import sys

from checkFileTrigSize_RTT import *


params = [

#     Add/replace new files to compare sizes of trigger xAOD containers in this list.
#     the format for adding new file /path/myAOD.pool.root for comparison:
#
#     ({'checkFile':'/path/myAOD.pool.root.checkFile'},'AODEDMSET', 'RELEASE_VERSION')
#
#     add ".checkFile" in the end of the file name!!!
#     AODEDMSET and RELEASE_VERISON can take any string value, but the set of variables 
#     AODEDMSET and RELEASE_VERSION must be unique for each entry in this params list!
#     Otherwise the results will be not correct (you will see this in the output,
#     some entries will be missing for some containers).
#     To launch, navigate to TrigAnalysisTest/bin and do
#
#     ./compareTrigSizes.py
#
#     the output will be printed to a screen.


   # ({'checkFile':'../../../../../../AtlasProduction210Xrel6_161023_checkTriggerSizeNODYN/run/AODFULL/myAOD.pool.root.checkFile'}, 'AODFULL'),
   # ({'checkFile':'../../../../../../AtlasProduction210Xrel6_161023_checkTriggerSizeNODYN/run/AODFULLNODYN/myAOD.pool.root.checkFile'}, 'AODFULLNODYN'),
   # ({'checkFile':'../../../../../../AtlasProduction210Xrel6_161023_checkTriggerSizeNODYN/run/AODSLIM/myAOD.pool.root.checkFile'}, 'AODSLIM'),
   # ({'checkFile':'../../../../../../AtlasProduction210Xrel6_161023_checkTriggerSizeNODYN/run/AODSLIMNODYN/myAOD.pool.root.checkFile'}, 'AODSLIMNODYN'),
   # ({'checkFile':'../../../../../../AtlasProduction207XYVAL_rel3_161005_checkTriggerSize/myAOD.pool.root.checkFile'},'20.7AODSLIM')

    ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210Xrel6_161106_checkTrigSizeRyan/run/AODFULL/myAOD.pool.root.checkFile'},'AODFULL','21.0.X'),
    ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210Xrel6_161106_checkTrigSizeRyan/run/AODFULLNODYN/myAOD.pool.root.checkFile'},'AODFULLNODYN','21.0.X'),
    ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210Xrel6_161106_checkTrigSizeRyan/run/myAOD.pool.root.checkFile'},'AODSLIM','21.0.X'),
    ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210Xrel6_161106_checkTrigSizeRyan/run/AODSLIMNODYN/myAOD.pool.root.checkFile'},'AODSLIMNODYN','21.0.X'),

#   ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210XVALrel6_161106_checkTrigSizeRyan/run/AODFULL/myAOD.pool.root.checkFile'},'AODFULL'),
#   ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210XVALrel6_161106_checkTrigSizeRyan/run/AODFULLNODYN/myAOD.pool.root.checkFile'},'AODFULLNODYN'),
#   ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210XVALrel6_161106_checkTrigSizeRyan/run/myAOD.pool.root.checkFile'},'AODSLIM'),
   ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction210XVALrel6_161106_checkTrigSizeRyan/run/AODSLIMNODYN/myAOD.pool.root.checkFile'},'AODSLIMNODYN','21.0.X-VAL'),
   ({'checkFile':'/afs/cern.ch/work/o/okuprash/public/AtlasProduction207Xrel6_161106_checkTrigSizeRyan/myAOD.pool.root.checkFile'},'AODSLIM','20.7.X')

]
#params = {}

# a delimiter to be used in office spreadsheet to separate data to columns
spreadsheetDelimiter = ";"
numberFormat = "%12s"

versions = [] #'AODFULL', 'AODFULLNODYN', 'AODSLIM', 'AODSLIMNODYN', '20.7AODSLIM'
releases = [] #'21.0.X', '21.0.X-VAL' etc

# the order of elements in these lists will be kept the same throughout the code (the same as in input "params" list)
for param in params:
    versions.append( param[1] )
    releases.append( param[2] )

trigContainerLists = []
trigSizesPerCategory = []
trigTotalFileSizes = []
trigTotalFileSizesLists = []
trigTotalFileSizesCheckFile = []

# launch checkFileTrigSize_RTT for each input file and store the output in variables
for param in params:
    r =  checkFileTrigSize_RTT(param[0])
#    print "trigger container list:"
#    print r.triggerAlgList
    print "processing ",param
    status = r.run()
    print "done processing ",param
#    print "trigger container list:"
#    print r.triggerAlgList
    trigContainerLists.append( (r.triggerAlgList, param[1], param[2]) )
    trigSizesPerCategory.append( (r.triggerAlgSize, param[1], param[2]) )
    trigTotalFileSizes.append( (r.totalAlgSize, param[1], param[2]) )
    trigTotalFileSizesLists.append( (r.totalAlgSizeInLists, param[1], param[2]) )
    trigTotalFileSizesCheckFile.append( (r.total, param[1], param[2]) )


# let's setup the full list of container names and the full list of categories:
fullListContainers = []
fullListCategories = []
for containerList in trigContainerLists:
    for item in reversed(containerList[0]):
        if item[0] not in [it[0] for it in fullListContainers]:
            fullListContainers.append( [item[0], item[1]] )
        if item[1] not in fullListCategories:
            fullListCategories.append( item[1] )

# the comparison part

print ""
print "copypaste the part below to a spredsheet and use ",spreadsheetDelimiter," as a delimiter"
print ""

# first print the summary (total size) for different categories
# table header
print "release               ",
print spreadsheetDelimiter,
for release in releases:
    print numberFormat % release,
    print spreadsheetDelimiter,
print ""

print "AOD EDM set           ",
print spreadsheetDelimiter,
for version in versions:
    print numberFormat % version,
    print spreadsheetDelimiter,
print ""
print "                      ",
print spreadsheetDelimiter,
for version in versions:
    print numberFormat % "kB/event",
    print spreadsheetDelimiter,
print ""
# categories sizes
for category in fullListCategories:
    print "trigger%-15s" % category,
    print spreadsheetDelimiter,
    for version, release in zip(versions, releases):
        for catSize in trigSizesPerCategory:
            if catSize[1] == version and catSize[2] == release:
                if category in catSize[0]:
                    # the category size is printed here:
                    print numberFormat % catSize[0][ category ],
                    print spreadsheetDelimiter,
                else:
                    # if the category does not exist for the given AODEDMSET and RELEASE_VERSION, then just print "x" instead of size
                    print numberFormat % "x",
                    print spreadsheetDelimiter,
    print ""

# total trigger and file sizes
print "triggerTotal          ",
print spreadsheetDelimiter,
for version,release in zip(versions,releases):
    for catSize in trigSizesPerCategory:
        if catSize[1] == version and catSize[2] == release:
            print numberFormat % catSize[0][ 'Total' ],
            print spreadsheetDelimiter,
print ""

print "File size:                  ",
print spreadsheetDelimiter,
for version,release in zip(versions,releases):
    for t in trigTotalFileSizes:
        if t[1] == version and t[2] == release:
            print "%12.3f" % t[0],
            print spreadsheetDelimiter,
print ""
print "File size (from lists):     ",
print spreadsheetDelimiter,
for version,release in zip(versions,releases):
    for t in trigTotalFileSizesLists:
        if t[1] == version and t[2] == release:
            print "%12.3f" % t[0],
            print spreadsheetDelimiter,
print ""
print "File size (from checkFile): ",
print spreadsheetDelimiter,
for version,release in zip(versions,releases):
    for t in trigTotalFileSizesCheckFile:
        if t[1] == version and t[2] == release:
            print "%12.3f" % t[0],
            print spreadsheetDelimiter,
print ""
print ""
            

# now let's print for each category all containers within it
for category in fullListCategories:

    print "\nSummary for ", category
    # table header
    print "%-80s" % "release",
    print spreadsheetDelimiter,
    for release in releases:
        print numberFormat % release,
        print spreadsheetDelimiter,
    print ""

    print "%-80s" % "AOD EDM set",
    print spreadsheetDelimiter,
    for version in versions:
        print numberFormat % version,
        print spreadsheetDelimiter,
    print ""
    print "%-80s" % "",
    print spreadsheetDelimiter,
    for version in versions:
        print numberFormat % "kB/event",
        print spreadsheetDelimiter,
    print ""
    # the actual numbers
    for container in fullListContainers:
        if container[1] == category:
            containerSizePerVersion = []
            for containerList in trigContainerLists:
                contList = containerList[0]
                description = containerList[1]
                release = containerList[2]

                for currentContainer in contList:
                    if currentContainer[0] == container[0]:
#                        print "size of ",currentContainer[0]," from category ",category," of type ",description," is ",currentContainer[2]
                        containerSizePerVersion.append( (currentContainer[0], category, description, currentContainer[2], release) )
            if len(container[0]) < 80:
                print "%-80s" % container[0],
                print spreadsheetDelimiter,
            elif len(container[0]) < 100:
                print "%-100s" % container[0],
                print spreadsheetDelimiter,
            else:
                print container[0],
                print spreadsheetDelimiter,
            for version,release in zip(versions,releases):
                if (version not in [it[2] for it in containerSizePerVersion]) or (release not in [it[4] for it in containerSizePerVersion]):
                    # if the container does not exist for the given AODEDMSET and RELEASE_VERSION, then just print "x" instead of its size
                    print numberFormat % "x",
                    print spreadsheetDelimiter,
                else:
                    for contSize in containerSizePerVersion:
                        if contSize[2] == version and contSize[4] == release:
                            # the container size is printed here
                            print numberFormat % contSize[3],
                            print spreadsheetDelimiter,
            print ""
    # print summary for this category again, in the bottom of the list of containers
    print "Total for %-69s:" % category,
    print spreadsheetDelimiter,
    for version,release in zip(versions,releases):
        for catSize in trigSizesPerCategory:
            if catSize[1] == version and catSize[2] == release:
                if category in catSize[0]:
                    print numberFormat % catSize[0][ category ],
                    print spreadsheetDelimiter,
                else:
                    print numberFormat % "x",
                    print spreadsheetDelimiter,
    print ""
                        
