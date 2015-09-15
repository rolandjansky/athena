#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#################################################################
# script to dump selected results to a text file
# Upated by czodrows, 12-12-14 - adding the L1 items implemented by Joerg
#
#################################################################

import sys;
import string;
import os;
import math;
import re;
import glob;
import xml;
import time;
import datetime;

# set defaults here so they are same for RTT and command line
rfile = "expert-monitoring.root"
# directories and histograms for chain acceptance and TEs
dirs  = ["TrigSteer_HLT","CTPSimulation"]  
hists = ["ChainAcceptance","NumberOfActiveTEs","L1ItemsAV"]
#backup names
#backupName = { "ChainAcceptance":"ChainAcceptance_runsummary",
#               "NumberOfActiveTEs":"NumberOfActiveTEs_runsummary"}

# directories and histogram to be used for finding number of events that
# were run on
totDirs = dirs
totHist ="NInitialRoIsPerEvent"
# short names for files
histDict = { "TrigSteer_HLT":"HLT",
             "ChainAcceptance":"Chain",
             "NumberOfActiveTEs":"TE",
             "CTPSimulation":"L1",
             "L1ItemsAV":"AV" }
defFracTolerance  = "0.001"
defSigmaTolerance = "0.0"
defIntTolerance   = "2"
defEnhancedTag    = "Enhanced "

class chainDump:
    """
    Class to dump trigger chains
    """

    def __init__(self, paramDict={}):

        print "==chainDump.py"
        try: 
            from Logger import Logger
            self.logger    = Logger()
        except:
           print "can't import logger in test mode"
    
      
        self.success   =  0
        self.error     = -1

        # defaults are given twice -- once here and once below for interactive use
        self.checkFile         = paramDict.get('rootFile',rfile)
        self.checkDir          = paramDict.get('rootDirs', dirs)
        self.checkHist         = paramDict.get('rootHists',hists)
        self.totalEvHist       = paramDict.get('totalHist',totHist)
        self.totalEvDirs       = paramDict.get('totalDirs',totDirs)
        self.intTolerance      = paramDict.get('intTolerance',defIntTolerance) 
        self.fracTolerance     = paramDict.get('fracTolerance',defFracTolerance) 
        self.saveToFile        = paramDict.get('saveToFile',True)   
        self.verbose           = paramDict.get('verbose',False)
        self.compare           = paramDict.get('compare',False)
        self.noSum             = paramDict.get('noSum',False)
        self.enhancedBias      = paramDict.get('enhancedBias',False)
        self.enhancedTag       = paramDict.get('enhancedTag',defEnhancedTag)
        self.top               = paramDict.get('top',False)
        self.out               = paramDict.get('out',False)
        self.ignoreDates       = paramDict.get('ignoreDates',True)

        if self.verbose:
           print "verbose mode                        ", self.verbose
           print "checking file(s):                   ", self.checkFile 
           print "saveToFile:                         ", self.saveToFile
           print "compare results of multiple files:  ", self.compare
           print "integer tolerance:                  ", self.intTolerance
           print "fractional tolerance:               ", self.fracTolerance
           print "calculate sum                       ", self.noSum
           print "only look at enhancedBias RTT files ",self.enhancedBias

        print "chainDump: looking in directories:", self.checkDir
        print "chainDump: looking at Histogram:", self.checkHist

        if self.compare:
            print "chainDump: using tolerances: ",self.fracTolerance,self.intTolerance

        # now only used for total events in a container
        self.grandTotalEvents = 0

        # results is a list of [fileContinainer,fileName,nevt]  (corresponding to an expert-monitoring.root)
        # Each fileContainer is a list of   [TextFile,ChainList]  (each TextFile corresponds to EFChain, etc. ) 
        # Each Chainlist is a list of [ chain, count]      e.g.    L2_tau16, 25
        self.results=[]
  
    # extract list of elements from an exper-monitoring.root histogram
    def hstlist(self,hst,fileTitle):

        nbins = hst.GetNbinsX()
        lenMax = 10
        for j in range(nbins):
            if len(hst.GetXaxis().GetBinLabel(j+1)) > lenMax:
                lenMax =  len(hst.GetXaxis().GetBinLabel(j+1)) 

        pairedList=[]
        for j in range(nbins):
            # first bin is for underflows
            char_label = hst.GetXaxis().GetBinLabel(j+1)
            #PC - skip the ones without any label
            if char_label=="" :
                continue
            chainCounts =  hst.GetBinContent(j+1)
            pairedList +=[[char_label,chainCounts]]
            if self.verbose:
                print char_label.ljust(lenMax) + "%15.0f" % hst.GetBinContent(j+1)

        return pairedList

    # check a list of triggers to see if any are out of tolerance
    def checkList(self,list):
        okay = True
        for n,(x,xt) in enumerate(list):
            #print "DMS",n, x,xt
            for y,yt in list[n:]:
                if x > -1 and y > -1 and xt > 0 and yt > 0:
                    fx = x/xt
                    fy = y/yt
                    ft = float(self.fracTolerance)*min(fx,fy)
                    it = float(self.intTolerance)*2.0/(xt+yt)
                    #print self.fracTolerance,ft, it
                    # note yt and xt can be greater than one for TEs
                    if abs(fx - fy) > it and abs(fx - fy) > ft : 
                        #print x, y, xt, yt, fx, fy, it, ft  self.intTolerance,self.fracTolerance
                        #print 'out'
                        okay = False
                else:
                    okay = False
        return okay
    
    def saveComparison(self,goodReleaseFileList):

        textFileList=[]
        
        for fileContainer,fileName,nevt in self.results:
            for textFile,chains in fileContainer:
                #print textFile
                # make a list of files
                if not textFile in textFileList:
                    textFileList+=[textFile]
        #print 'textFileList', textFileList     
        
        # now get full list of triggers for each file type
        # e.g.  EFChain, EFTE, etc.
        
        for textFile in textFileList:
            nameList = []
            nmax = 30
            noutOfTolerance = 0
            try:
                print "storing comparison in file: ",textFile
                f = open(textFile,'w')
            except:
                print "warning, could not open comparison file"
                self.saveToFile = False 
            # get a list of all chain names and the length of the longest one
            for fileContainer,fileName,nevt in self.results:
                for thisTextFile,chains in fileContainer:
                    if thisTextFile == textFile:
                        for chainName,count in chains:
                            if not chainName in nameList:
                                nameList += [chainName]
                                if len(chainName) > nmax:
                                   nmax = len(chainName)
            # get max number of events a release
            maxReleaseEvents = 0.0
            giveDiffWarning = False
            line = "total events in hist files:".ljust(nmax+1)
            for age,release,goodFileList in goodReleaseFileList:       
                eventSum = 0
                for goodFile in goodFileList:
                    #print "DMS goodFile",goodFile
                    for fileContainer,fileName,nevt in self.results:
                        if fileName == goodFile:
                            eventSum += nevt
                            #print fileName,nevt,eventSum
                line += " %8d" % eventSum
                if eventSum > maxReleaseEvents:
                    if maxReleaseEvents > 0.0:
                        giveDiffWarning = True
                    maxReleaseEvents = eventSum
            print line
            if self.saveToFile:
                f.write(line+"\n")
            #print "maxReleaseEvents",maxReleaseEvents
            #print "DMS ",nameList
            if  giveDiffWarning and self.enhancedBias:
                print "WARNING: Releases do not all have the same number of events, rescaling to",maxReleaseEvents
            for chainName in nameList:
                line = chainName.ljust(nmax+1)
                if self.verbose: 
                    print chainName.ljust(nmax+1),
                counts = []
                for age,release,goodFileList in goodReleaseFileList:
                    sum = -1
                    releaseEvents = 0
                    for goodFile in goodFileList:
                        #print "DMS goodFile",goodFile
                        for fileContainer,fileName,nevt in self.results:
                            #print "DMS fileName,evt",fileName,nevt
                            if fileName == goodFile:
                                #print "DMS names match"
                                for thisTextFile,chains in fileContainer:
                                    if thisTextFile == textFile:
                                        chainCount = -1
                                        for thisChainName,count in chains:
                                            if thisChainName == chainName:
                                                #print "DMS", chainName, count
                                                if sum == -1:
                                                    sum = 0
                                                sum += count
                                                releaseEvents += float(nevt)
                                                #releaseCounts +=[ count ]
                                                if self.verbose:
                                                    print count,
                    # end of loop over goodFiles in a release
                    #print chainName,sum,releaseCounts
                    counts += [ [sum,releaseEvents] ]
                    if self.enhancedBias and releaseEvents > 0:
                        line += " %8d" % int(sum*maxReleaseEvents/releaseEvents)
                    else: 
                        line += " %8d" % sum
                # end of processing for a given chain
                if self.checkList(counts):
                    if not self.out: 
                        if self.saveToFile:
                           f.write(line+"\n")
                else:
                    noutOfTolerance+=1; 
                    if self.saveToFile:
                        f.write( line+"<<<<<<<<<\n" ) 
                    else:
                        print line+"<<<<<<<<<"
            if noutOfTolerance == 0:
                print "All triggers  are within tolerance in",textFile
            else:
                print "WARNING: %d triggers are out of tolerance in" % noutOfTolerance,  textFile 
                print "%s \n" % "Columns are defined as follows:"
                n = 0
                for age,release,goodFileList in goodReleaseFileList:
                     n += 1
                     print "[%d]  " % n , release, age

            if self.saveToFile:
                f.write("Number of triggers out of tolerance: %d \n" % noutOfTolerance)
                f.write("%s \n" % "Columns are defined as follows:" ) 
                n = 0
                for age,release,goodFileList in goodReleaseFileList:
                     n += 1
                     f.write( "[%d]  " % n ) 
                     f.write( "%s " %  release )
                     #f.write( age )
                     f.write( "\n")
                f.close()
 
#        print textFileList

    def saveResults(self):

       # save total entries to a file
        if self.saveToFile :  
            try:      
                f=open('TotalEventsProcessed.txt','w')
            except:
                self.saveToFile = False

            if self.saveToFile:
                print self.grandTotalEvents
                f.write("%d" % int(self.grandTotalEvents) )
                f.close()

#        if self.verbose:
#            print self.results
#
#       prints a single file
#

        for fileContainer,fileName,nevt in self.results:
            for textFile,chains in fileContainer:
                print textFile
                # store to file if asked  
                try:      
                    f=open(textFile,'w')
                except:
                    self.saveToFile = False
                if self.verbose: 
                    print chains
                for  name,count in chains:
                    if self.saveToFile:
                        f.write( name + "%15.0f" % count+"\n" )
                    else:
                        print name, count

                if self.saveToFile:
                    f.close()       
        
    def onefile(self,file):

        import ROOT
        from ROOT import gDirectory
        from ROOT import TH1
        from ROOT import TDirectory

        if self.verbose:
            print "look at file:", file
        tfile = ROOT.TFile(file)
        if tfile.IsZombie():
            print "WARNING: ", file, " is misisng or empty"
            return self.error
        
        # get list of directories
        tDir = tfile.GetDirectory("")
        bDirs = tDir.GetListOfKeys()
        baseList = []
        for bdir in bDirs:
           baseList.append(bdir.GetName()) 
     

        # first try and extract total number of events
        total_events = -1
        #print self.totalEvHist
        for dir in self.totalEvDirs: 
            # check if dir is in list of base directories
            if dir in baseList:
                #print dir
                # trap an errors
                try:
                    tfile.cd(dir)
                    hist = gDirectory.Get(self.totalEvHist)
                    if hist != 0:
                        hist_entries = int(hist.GetEntries())
                        #print "hist_entries",hist_entries, total_events
                        if total_events == -1:
                            total_events = hist_entries
                            self.grandTotalEvents += hist_entries
                            #print "grandTotalEvents", self.grandTotalEvents
                        else:
                            if hist_entries != total_events:
                                print "WARNING: total events in different directories don't match" 
                        if total_events != -1:
                            print "Found %s events" % total_events
                            break
                    tfile.cd("..")
                except:
                  print "ERROR: cound not cd to directory: ", dir
            else:
                print "WARNING: direcory ", dir," not found, could be normal if only HLT is run"
    
        found = False
        fileList=[]
        chainList=[]
        dummy=[]
        for dir in self.checkDir:
            # check if dir is in list of base directories
            if dir in baseList:
                if self.verbose:
                    print dir
                try:
                    tfile.cd(dir)
                    for histName in self.checkHist:
                        #print "trying dir,histName",dir,histName
                        hist = 0
                        if histName in gDirectory.GetListOfKeys():
                            hist = gDirectory.Get(histName)
                            # else:
                            #   print "trying backup",backupName[histName]
                            #   hist = gDirectory.Get(backupName[histName])
                        if  hist != 0:
                            if  dir in histDict  and  histName in histDict:
                                textFileName=histDict[dir]+histDict[histName]+".txt"
                            else:
                                textFileName=dir+histName+".txt"
                            histDump = self.hstlist(hist,textFileName)
                            chainList = [textFileName,histDump]
                            fileList +=[chainList]
                            found = True
                        else:
                            #not really useful to print a warning for not working dir/hist combinations  
                            #print "WARNING:  missing L2 or EF resutls (normal if only HLT is run)"
                            #print "nope combination ain't working - but fine"
                            continue
                    tfile.cd("..")
                except:
                  print "ERROR2: cound not cd to directory: ", dir
            else:
                print "WARNING: direcory ", dir," not found"
 
        self.results += [ [fileList,file,total_events] ]
        #print "DMS len:",len(self.results)

        # return error or not
        if found:
            return self.success
        else: 
            print "ERROR: no histograms found"
            return  self.error     

    def prResults(self):
#        for debuging only
#        print "DMS length of self.results", len(self.results)
        for fileContainer,file,total_events in self.results:
            print "file and total events", file, total_events
            for textFile,chains in fileContainer:
                print "textFile: ", textFile
                for chain,count in chains:
                    print chain,count

    def run(self):
 

       files = glob.glob(self.checkFile)
       if len(files) < 1:
           return self.error

       #
       #  group files into different releases -- mainly for enhanced bias
       # 
       ages       = [ ]
       releases  = [ ]
       goodFiles = [ ]
       for file in files:
 
           # first make some checks on the file
           processFile = True
 
           #(mode, ino, dev, nlink, uid, gid, size, atime, mtime, ctime) = os.stat(file)
           delta =  datetime.timedelta(days=0)
           try:
               modified = datetime.datetime.fromtimestamp(os.stat(file)[8]) 
               delta = datetime.datetime.today() - modified
               #print delta, modified, datetime.date.today()
               #print delta, datetime.timedelta(days=8)
               if delta > datetime.timedelta(days=8):
                  if  self.ignoreDates:
                      print "WARNING: old file found:",file,modified
                      print "WARNING: processing anyway"
                  else:
                      print "WARNING:  skipping stale file: ",file,modified
                      print "WARNING:  use option -i (--ignoreDates) to override and ignore file date"
                      processFile = False
 
           except:
              print "WARNING:  couldn\'t get time info skipping file ",file
              processFile = False

           try:
              bytesize = os.path.getsize(file)
              if bytesize < 10000:
                  print "WARNING:   File too small, job crashed or is not finished"
                  processFile = False
           except:
              print "WARNING: couldn't get filesize"
              processFile = False

           if self.enhancedBias:
               processFile = False
               xmlFile=file.rstrip('/expert-monitoring.root')+'/rttjobinfo.xml'
               try: 
                   from xml.etree import ElementTree as elementTree
                   e = open(xmlFile,'r')
                   #print "xmlFile: ",e
                   tree = elementTree.parse(e)
                   e.close()
                   if self.enhancedTag in  displayName:
                   #if "Enhanced" in  displayName:
                       print "got one:",file
                       processFile = True
               except:
                   #print "failed to get ElementTree, use grep instead" 
                   try:
                       e = open(xmlFile,'r')
                       if self.enhancedTag in e.read():
                       #if "Enhanced " in e.read():
                           processFile = True
                           print "got one",file    
                   except:
                       print "WARNING: could\'t open xmlFile, skipping: ",xmlFile
       
           if self.top:
               processFile = False
               xmlFile=file.rstrip('/expert-monitoring.root')+'/rttjobinfo.xml'
               try: 
                   from xml.etree import ElementTree as elementTree
                   e = open(xmlFile,'r')
                   #print "xmlFile: ",e
                   tree = elementTree.parse(e)
                   e.close()
                   if "top" in  displayName:
                       #print "got one:",file
                       processFile = True
               except:
                   #print "failed to get ElementTree, use grep instead" 
                   try:
                       e = open(xmlFile,'r')
                       if "top " in e.read():
                               processFile = True
                   except:
                       print "WARNING: could\'t open xmlFile, skipping: ",xmlFile
       

           if processFile:

               print "adding ",file," to good files list"
               goodFiles += [file]
               ages += [delta]
               # search for a pattern like /23/expert-monitoring.root
               # allow last few characters of file to be different...
               # find last / in file list
               nextra=3
               lastSlash  = file.rfind("/")
               nLastSlash = file[0:lastSlash].rfind("/")
               print nLastSlash, lastSlash
               if nLastSlash > -1:
                   try:
                       # new rtt structure not integers
                       # for the moment assume that all subdirectories are summed
                       #dummy = int(file[nLastSlash+1:lastSlash])
                       #print "integer value:",dummy
                       #print  file[0:nLastSlash]
                       found = False
                       print "releases so far: ",releases
                       for release in releases: 
                           #prresult()
                           #print "looping over releases"
                           #print "comparing ",release,file[0:nLastSlash-nextra]
                           if release == file[0:nLastSlash-nextra]:
                               found = True
                       if not found:
                           if self.verbose:
                               print "release not found in list"
                               print  file[0:nLastSlash], file
                           releases += [ file[0:nLastSlash-nextra]  ]
                   except:
                       print "WARNING:  exception could not interpret direcotry structure"
                       releases +=[file]
               else:
                   #this is not appropiate here
                   #print "WARNING: no directory and/or release sturucture found"
                   releases +=[file]

       if self.verbose: 
           print "releases",releases
#
#     now make a paired listing of fileList and releases.
#
       releaseFileList = []
       for release in releases:
           fileList = []
           oldest =  datetime.timedelta(days=0)
           for n,file in enumerate(goodFiles):
              if release in file:
                 fileList += [file]
                 if ages[n] > oldest:
                     oldest =  ages[n]
           #print 'oldest',oldest,release
           releaseFileList +=[ [oldest,release,fileList] ] 

       status  = self.success
       goodReleaseFileList = []
       for age,release,fileList in releaseFileList:
           if self.verbose:
               print "Processing release",release
               print "fileList:",fileList
#        
           goodFileList = []  
           for file in fileList:
               if self.onefile(file) == self.error:
                   print "WARNING: error in file:",file
                   status = self.error
               else:
                   goodFileList += [file]
    
         
           goodReleaseFileList+=[ [age,release,goodFileList] ]
#
#          sort list 
#
           goodReleaseFileList.sort(reverse=True)
#
#      need to switch saveComparison so that 
#      if works from a matrix of releases and goodfile
#      in the case of multiple files in the same release
#      it should just sum them....
#
       if self.verbose:
           print "printing results so far"
           print self.prResults()
#
#     only one file given 
#
       if len(files) == 1 and not self.compare:
           self.saveResults()
       else: 
           self.saveComparison(goodReleaseFileList)

       return status

# This is just for testing outside of the RTT
if __name__ == "__main__":
    #params = {'checkFile':'AOD.pool.root.checkFile'}


    from optparse import OptionParser
    parser = OptionParser(usage = "usage: %prog [options] root file",
                         description = "dumps list of triggers from histogram")

    # note: set all options as global variables above so they are same for 
    # command line and non-command line calling
    parser.add_option("--rootFile",
                      action = "store", default = rfile, type="string",
                      help = "name of root file")

    parser.add_option("--fracTolerance",
                      action = "store", default = [], type="string",
                      help = "tolerance as a fraction, flagged channels must exceed all tolerances")

    parser.add_option("--intTolerance",
                     action = "store", default = [], type="string",
                     help = "tolerance as a number of counts, flagged channels must exceed all toelrances")

    parser.add_option("--rootDirs",
                      action = "append", default = [], type="string",
                      help = "name of root direcotries to use")
 
    parser.add_option("--rootHists", 
                      action = "append", default = [], type="string",
                      help = "name of histograms to use")

    parser.add_option("--totalDirs",
                      action = "append", default = [], type="string",
                      help = "name of directories to use for total events")
 
    parser.add_option("--totalHist", 
                      action = "store", default = totHist, type="string",
                      help = "name of histogram with total events")

    parser.add_option("--enhancedTag", 
                      action = "store", default = "Enhanced ", type="string",
                      help = "string to search for to verify as minbias")

    parser.add_option("-S", "--saveToFile",
                     action = "store_true", default = False,
                     help = "save output to file")

    parser.add_option("-v","--verbose",
                      action = "store_true", default = False,
                      help = "verbose mode")

    parser.add_option("-c","--compare",
                      action = "store_true", default = False,
                      help = "make files as if comparing releases, even if only one file")

    parser.add_option("-n","--noSum",
                      action = "store_true", default = False,
                      help = "don\'t sum files, depreciated")

    parser.add_option("-e","--enhancedBias",
                      action = "store_true", default = False,
                      help = "only look at enhanced Bias tests")

    parser.add_option("-t","--top",
                      action = "store_true", default = False,
                      help = "only look at top tests")
 
    parser.add_option("-o","--out",
                      action = "store_true", default = False,
                      help = "only store out of tolerance results")
     
    parser.add_option("-i","--ignoreDates",
                      action = "store_true", default = False,
                      help = "ignore dates and process stale files")

    (opts, args) = parser.parse_args()

    print 'debug: opts.rootDirs:',opts.rootDirs

    # fill in params dictionary as it would be in RTT

    params = {'rootFile'  : opts.rootFile}

    if  not opts.rootDirs == [] :
       params['rootDirs'] = opts.rootDirs
    else:
       params['rootDirs'] = dirs

    if not opts.rootHists == [] :
       params['rootHists'] = opts.rootHists
    else:
       params['rootHists'] = hists

    if not opts.totalDirs == [] :
        params['totalDirs'] = opts.totalDirs


    if not opts.fracTolerance == [] :
        params['fracTolerance'] = opts.fracTolerance
 
    if not opts.intTolerance == [] :
        params['intTolerance'] = opts.intTolerance

    params['enhancedTag'] = opts.enhancedTag

    params['out'] = opts.out

    params['totalHist'] = opts.totalHist

    params['saveToFile'] = opts.saveToFile

    params['verbose'] = opts.verbose

    params['compare'] = opts.compare

    params['noSum'] = opts.noSum

    params['enhancedBias'] = opts.enhancedBias

    params['top'] = opts.top

    params['ignoreDates'] = opts.ignoreDates

    r =  chainDump(params)
    status = r.run()
    
    sys.exit(status)
