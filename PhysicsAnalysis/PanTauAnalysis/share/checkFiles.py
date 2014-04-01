#!/usr/bin/env python

# @file:    checkFiles.py
# @purpose: open Pool files and check for a given number of events to identify corrupted files
#           Based on checkFile.py from Sebastien Binet
# @author:  Sebastian Fleischmann
# @date:    February 2009
#
# @example:
# @code
# checkFiles.py -d /somedirectory -o filesInMyDir
# @endcode
#

__version__ = "$Revision: 478526 $"
__author__  = "Sebastian Fleischmann"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool")
    p = parser.add_option
    p( "-f",
       "--file",
       dest = "fileName",
       help = "The path to the POOL file to analyze" )
    p( "-d",
       "--dir",
       dest    = "inputDirectory",
       help = "use directory, i.e. loop over all files in this directory" )
    #p( "--sort-fct",
       #dest    = "sortFctName",
       #default = "diskSize",
       #help = "Sorting function used to list containers (allowed are: diskSize, memSize, name)" )
    p( "-o",
       "--output",
       dest = "outFileName",
       default = None,
       help = "Name of the output file which will contain the informations gathered during checkFile processing." )
    p( "-e",
       "--events",
       type="int",
       dest = "numberOfExpectedEvents",
       default = 50,
       help = "Number of expected events in the file" )
    p( "--maxFilesPerJoFile",
       type="int",
       dest = "maxFiles",
       default = 0,
       help = "Maximum number of files to write per job options input file (0 means no limit)" )
    p( "-m",
       "--maxFiles",
       type="int",
       dest = "maxTotalFiles",
       default = 0,
       help = "Maximum number of files to process (0 means no limit)" )
    #p( "--fast",
       #dest = "fastMode",
       #default = False,
       #action  = "store_true",
       #help = "Switch to enable the fast mode of checkFile.py (memory size will not be accurate -AT ALL-)"
       #)
    
    (options, args) = parser.parse_args()

    fileNames = []
    
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0 and options.inputDirectory == None:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.fileName != None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    if options.inputDirectory != None:
        folderName = os.path.expandvars(os.path.expanduser(options.inputDirectory))
        for filename in os.listdir (folderName):
            # Ignore subfolders
            if os.path.isdir (os.path.join (folderName, filename)):
                continue
            # "filename" contains a file in FOLDER. Use os.path.join to get the full path, as above
            fileNames.append(os.path.join (folderName, filename))
    import PyUtils.PoolFile as PF
    PF.PoolOpts.FAST_MODE = True
    #fileNames = set( fileNames )
    fileNames.sort()
    sc = 0
    goodFileList = "filelist.txt"
    badFileList = "badfiles.txt"
    pyFileList = "files.py"
    if options.maxFiles > 0:
            pyFileList = "files1.py"
    if options.outFileName != None:
        goodFileList = options.outFileName+"-good.txt"
        badFileList = options.outFileName+"-bad.txt"
        pyFileList = options.outFileName+".py"
        if options.maxFiles > 0:
            pyFileList = options.outFileName+"1.py"
    out = open( goodFileList, "w" )
    out3 = open( pyFileList, "w" )
    out2 = open (badFileList, "w")
    countGoodFiles = 0
    countOutFiles = 1
    countTotalFiles = 0
    print "max total files: %i max files per jo file: %i" % (options.maxTotalFiles , options.maxFiles)
    for fileName in fileNames:
        countTotalFiles = countTotalFiles+1
        if (options.maxTotalFiles>0) and (countTotalFiles > options.maxTotalFiles):
            break
        #for subdir, dirs, files in os.walk(fileDirs):
        #for onlyFileName in files:
        try:
            #fileName = fileDirs+"/"+onlyFileName
            #import PyUtils.PoolFile as PF
            #PF.PoolOpts.FAST_MODE = options.fastMode
            poolFile = PF.PoolFile( fileName )
            print "my events %i" % poolFile.dataHeader.nEntries
            if (poolFile.dataHeader.nEntries == options.numberOfExpectedEvents):
                countGoodFiles = countGoodFiles+1
                print "file OK: [%s], good: %i" % (fileName,countGoodFiles)
                out.write(fileName + os.linesep )
                out.flush()
                if (options.maxFiles > 0) and ( countGoodFiles > options.maxFiles) :
                    out3.close()
                    countOutFiles = countOutFiles+1
                    countGoodFiles = 1
                    if options.outFileName != None:
                        pyFileList = options.outFileName+str(countOutFiles)+".py"
                    else :
                        pyFileList = "files"+str(countOutFiles)+".py"
                    out3 = open( pyFileList, "w" )
                out3.write("ServiceMgr.EventSelector.InputCollections += ['"+fileName+"']"+os.linesep)
                out3.flush()
            else:
                print "number of entries (%i) does not match in file [%s]" % (poolFile.dataHeader.nEntries,  fileName)
                out2.write(fileName + " entries: " + str(poolFile.dataHeader.nEntries) + os.linesep )
                out2.flush()
            #poolFile.checkFile( sorting = options.sortFctName )
            #if options.doDetailedDump:
                #dumpFile = os.path.basename(fileName)+ ".txt"
                #print "## dumping details into [%s]" % dumpFile
                #poolFile.detailedDump( dumpFile )
            #if options.outFileName:
                #outFileName = options.outFileName
                #print "## saving checkFile report into [%s]..." % outFileName
                #poolFile.saveReport( outFileName )
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            out2.write(fileName + " gives exception: " + str(e.__class__) + os.linesep )
            pass

        except :
            print "## Caught something !! (don't know what)"
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 10
            pass
        if len(fileNames) > 1:
            print ""
        pass # loop over fileNames
    
    #print "## Bye."
    sys.exit(sc)
