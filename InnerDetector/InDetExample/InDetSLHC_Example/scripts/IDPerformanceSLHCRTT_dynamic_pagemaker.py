#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
from Logger import Logger

# module: IDPerformanceSLHCRTT_dynamic_pagemaker.py
		
class IDPerformanceSLHCRTT_dynamic_pagemaker:
   def __init__(self,argDict={}):

       # access my args from the dictionary passed in, and convert any basic types
       # self.myArg = argDict.get('MyArg') # = the Python string type '3'
       # self.myArg = int(self.myArg) # = the Python integer type 3  

       # access the RTT descriptor instance
       rttDescriptor = argDict.get('JobDescriptor')
     
       # grab some stuff from it
       self.release = str(rttDescriptor.paths.release)
       #self.build   = str(rttDescriptor.paths.build)
       self.jobGroup = str(rttDescriptor.jobGroup)
       #self.platform = str(rttDescriptor.paths.targetCMTCONFIG)
       self.branch = str(rttDescriptor.paths.branch)
       self.runPath = str(rttDescriptor.runPath)

       # I don't use the logger, but the RTT post-processing module runner now fails if it can't find one.  Hrmph.
       self.logger  = Logger()

   def run(self):
       # do the stuff I wanted to do...
       logfile = open(self.runPath + "/IDPerformanceSLHCRTT_dynamic_pagemaker.log",'a')
       logfile.write("Starting IDPerformanceSLHCRTT_dynamic_pagemaker.py run method\n")

       inputfile = self.runPath + "/ALL_IDPerformanceSLHCRTT_Plots.html"
       outputfile = inputfile.replace(".html","_Dynamic.html")

       datasetlist = { "AthenaMu1SLHCRTT": "Muons1GeV",
                       "AthenaMu100SLHCRTT": "Muons100GeV",
                       "AthenaZtomumuSLHCRTT": "ZtoMuMu",
                       "AthenaMu1SLHCRTTKalman": "Muons1GeVKalman",
                       "AthenaMu100SLHCRTTKalman": "Muons100GeVKalman",
                       "AthenaZtomumuSLHCRTTKalman": "ZtoMuMuKalman",
                       "AthenaPi1SLHCRTT": "Pions1GeV",
                       "AthenaPi100SLHCRTT": "Pions100GeV",
                       "AthenaEle1SLHCRTT": "Electrons1GeV",
                       "AthenaEle100SLHCRTT": "Electrons100GeV",
                       "AthenaPi1SLHCRTTKalman": "Pions1GeVKalman",
                       "AthenaPi100SLHCRTTKalman": "Pions100GeVKalman",
                       "AthenaEle1SLHCRTTKalman": "Electrons1GeVKalman",
                       "AthenaEle100SLHCRTTKalman": "Electrons100GeVKalman",
                       "AthenaTtbarSLHCRTT": "Ttbar",
                       "AthenaTtbarSLHCRTTKalman": "TtbarKalman",
                       "AthenaZtomumuPileupSLHCRTT": "ZtoMuMuPileup",
                       "AthenaZtomumuPileupSLHCRTTKalman": "ZtoMuMuPileupKalman",
                       "AthenaMinBiasSLHCRTT": "MinBias",
                       "AthenaMinBiasSLHCRTTKalman": "MinBiasKalman",
                       "AthenaEle5SLHCRTTKalmanDNA": "Electrons5GeVKalmanDNA",
                       "AthenaEle60SLHCRTTKalmanDNA": "Electrons60GeVKalmanDNA",
                       "AthenaEle5SLHCRTTGaussianSumFilter": "Electrons5GeVGaussianSumFilter",
                       "AthenaEle60SLHCRTTGaussianSumFilter": "Electrons60GeVGaussianSumFilter"}
                       
                       
       # helper function
       def szsplit(stringlist,splitchar=" "):
         if str(splitchar) == splitchar:
           try:
             return stringlist.split(splitchar)
           except:
             newlist = []
             for item in stringlist:
               newlist += item.split(splitchar)
             return newlist
         else:
           for onechar in splitchar:
             stringlist = szsplit(stringlist,onechar)
           return stringlist

       logfile.write("Input file: " + inputfile + "\n")
       logfile.write("Output file: " + outputfile + "\n")
       logfile.write("datasetlist: \n")
       for key in datasetlist.keys():
         logfile.write("  " + key + ": " + datasetlist[key] +"\n")

       logfile.write("Getting dataset... ")
       try:
          dataset = datasetlist[self.jobGroup]
          logfile.write("OK!\n")
       except:
          dataset = "Unknown (jobGroup = " + self.jobGroup + ")"
          logfile.write("Unknown!\n")

       logfile.write("  Dataset name is " + dataset + "\n")

       jobinfolist = { "Release" : self.release , "Branch" : self.branch , "Dataset" : dataset }
       #jobinfolist = { "Release" : self.release , "Branch" : self.branch , "Build" : self.build , "Platform" : self.platform  , 
       #                "Dataset" : dataset }

#http://atlas-project-rtt-results.web.cern.ch/atlas-project-rtt-results/page2.php?xml=rel_3/pcache/build/i686-slc3-gcc323-opt/offline/RTTSummary.xml&package=InDetRTT&job=InDetRTT_jobOptions&id=182
#/afs/cern.ch/atlas/project/RTT/Results/rel_1/val/build/i686-slc4-gcc34-opt/offline/InDetRTT/AthenaHighPtMu/InDetRTT_jobOptions/183
#http://atlas-project-rtt-results.web.cern.ch/atlas-project-rtt-results/page2.php?xml=rel_1/val/build/i686-slc4-gcc34-opt/offline/RTTSummary.xml&package=InDetRTT&job=InDetRTT_jobOptions&id=183

       # For testing purposes
       if self.runPath == ".":
         self.runPath = "/afs/cern.ch/atlas/project/RTT/Results/rel_1/bugfix/build/i686-slc4-gcc34-opt/offline/InDetPerformanceRTT/AthenaMu100SLHCRTT/InDetPerformanceRTT_jobOptions/334"

       logfile.write("Runpath: " + self.runPath + "\n")

       logfile.write("jobinfolist: \n")
       for key in jobinfolist.keys():
         logfile.write("  " + key + ": " + jobinfolist[key] +"\n")

       logfile.write("Opening input file: " + inputfile + "\n")
       input = open(inputfile,'r')

       logfile.write("Opening output file: " + outputfile + "\n")
       output = open(outputfile,'w')

       logfile.write("Starting copy...\n")

       for line in input:
          logfile.write("Line: " + line)
          if line.count("<h2>Contents</h2>"):
             logfile.write("  Contains <h2>Contents</h2>\n")
             output.write("<TABLE BORDER=6 CELLSPACING=4 CELLPADDING=4>\n")
             for key in jobinfolist.keys():
                logfile.write("  Going through jobinfo list and writing value of " + key + "... ")
                output.write("  <TR><TD><b>" + key + "</b></TD><TD>" + jobinfolist[key] + "</TD></TR>\n")
                logfile.write("Done.\n")
             logfile.write("Writing date and finishing table... ")
             output.write("  <TR><TD><b>" + "Date" + "</b></TD><TD>" + os.popen("date").readline().strip() + "</TD></TR>\n")
             output.write("</TABLE>\n\n")
             logfile.write("Done.\n")
          if line.count("DCube"):
            htmlline = "<li><a href=\""
            dcubedir = False
            for item in os.listdir(self.runPath):
              if item.count("DCube-"):
                dcubedir = item
                break
            if dcubedir:
              htmlline += dcubedir + "/InDetStandardPlots.root.dcube.xml.php\">DCube histogram comparison page</a></li>\n"
              logfile.write("Writing DCube line: " + htmlline)
              output.write(htmlline)
              continue
          if line.count("Log files"):
            logfile.write("  Contains log files")
            htmlline = "<li>Log files: "
            for filename in os.listdir(self.runPath):
              if filename.count("log"):
                htmlline += "<a href=\"" + filename + "\">" + filename + "</a> "
            htmlline += "</li>\n"
            output.write(htmlline)
            logfile.write("    Writing: " + htmlline)
          else:
            output.write(line)
            logfile.write("  Output original line.\n")

       logfile.write("Now closing files.\n")
       output.close()
       input.close()
       logfile.close()

       return 0

class fakePaths:
   def __init__(self,release="release",branch="branch"):
      self.release = release
      #self.build = build
      #self.targetCMTCONFIG = platform
      self.branch = branch
      self.runPath = os.popen("pwd").readline().strip()

#class fakePaths:
#   def __init__(self,release="release",build="build",platform="platform",branch="branch"):
#      self.release = release
#      self.build = build
#      self.targetCMTCONFIG = platform
#      self.branch = branch
#      self.runPath = os.popen("pwd").readline().strip()

class fakeRttDescriptor:
   def __init__(self):
      self.paths = fakePaths()
      self.jobGroup = "AthenaSingleMu"
      self.runPath = "."

# Test run
if sys.argv[0] == "./IDPerformanceSLHCRTT_dynamic_pagemaker.py":

   fakedecr = fakeRttDescriptor()
   fakeArgDict = { 'JobDescriptor' : fakedecr }
   pagemaker = IDPerformanceSLHCRTT_dynamic_pagemaker(fakeArgDict)
   pagemaker.run()   
