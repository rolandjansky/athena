#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import glob
from Logger import Logger

class MooRTT_makePage:
   def __init__(self,argDict={}):

       # access my args from the dictionary passed in, and convert any basic types
       # self.myArg = argDict.get('MyArg') # = the Python string type '3'
       # self.myArg = int(self.myArg) # = the Python integer type 3  

       # access the RTT descriptor instance
       rttDescriptor       = argDict.get('JobDescriptor')
     
       # grab some stuff from it
       self.branch         = str(rttDescriptor.paths.branch)
       self.jobDisplayName = str(rttDescriptor.jobDisplayName)
       self.jobGroup       = str(rttDescriptor.jobGroup)
       self.name           = str(rttDescriptor.name)
       self.package        = str(rttDescriptor.package)
       self.release        = str(rttDescriptor.paths.release)
       self.runPath        = str(rttDescriptor.runPath)
       
       self.logger         = Logger()

   def run(self):
      logfile = open(self.runPath + "/makePage.log",'a')
      logfile.write("Starting makePage.py run method\n")
      jobinfolist = { "Package" : self.package ,
                      "Name" : self.name ,
                      "Branch" : self.branch ,
                      "Release" : self.release }
      logfile.write("jobinfolist: \n")
      
      output = open("ALL_MooPerformanceResults.html","w")
      output.write("<h2><TABLE BORDER=6 CELLSPACING=4 CELLPADDING=4>\n")
      for key in jobinfolist.keys():
         logfile.write("  " + key + ": " + jobinfolist[key] +"\n")
         output.write("  <TR><TD><b>" + key + "</b></TD><TD>" + jobinfolist[key] + "</TD></TR>\n")
      output.write("  <TR><TD><b>" + "Date" + "</b></TD><TD>" + os.popen("date").readline().strip() + "</TD></TR>\n")
      output.write("</TABLE>\n\n")
      output.write("</h2>")
      
      # output the relevant text files
      txtFiles = ["trkPerformance_CombinedMuonTracks.txt",
                  "trkPerformance_ExtrapolatedMuonTracks.txt",
                  "trkSummary.txt"]
      for fileName in txtFiles:
         strArray = glob.glob(fileName)
         if (len(strArray) > 0):
            fIn = open(strArray[0])
            lines = fIn.readlines()
            output.write("<h3>" + str(fileName).strip() + "</h3>")
            output.write("<pre>")
            for i in range(len(lines)):
               output.write(str(lines[i].strip()) + "\n")
            output.write("</pre>")

      # add the gif plots
      gifFiles  = ["MS_Bdl",     "MS_material", "match_quality",         "fit_chi2", 
                   "qOverP_res", "qOverP_pull", "qOverP_CB_resByRegion", "qOverP_SA_resByRegion", "qOverP_CB_pullByRegion", "qOverP_SA_pullByRegion",
                   "Pt_res",     "Pt_pull",     "Pt_CB_resByRegion",     "Pt_SA_resByRegion",     "Pt_CB_pullByRegion",     "Pt_SA_pullByRegion",     "Pt_MG_pullByRegion",
                   "D0_res",     "D0_resLow",   "D0_pull",               "D0_SA_resByRegion",     "D0_SA_resByRegionLow",   "D0_SA_pullByRegion",
                   "Z0_res",     "Z0_resLow",   "Z0_pull",               "Z0_SA_resByRegion",     "Z0_SA_resByRegionLow",   "Z0_SA_pullByRegion",
                   "Phi_res",    "Phi_pull",    "Phi_SA_resByRegion",    "Phi_SA_pullByRegion",
                   "Theta_res",  "Theta_pull",  "Theta_SA_resByRegion",  "Theta_SA_pullByRegion", "Theta_CB_pullByRegion",
                   "MG-CB",      "CMF-CB",      "ID-CB",                 "ID-CMF",                "qOverP_ratio"]
      gifTitle  = ["MS field integral", "MS material", "ID+MS match quality",  "fit chi2/DOF",
                   "qOverP resolution", "pull qOverP", "CB qOverP resolution", "SA qOverP resolution", "CB qOverP pull",    "SA qOverP pull",
                   "Pt resolution",     "pull Pt",     "CB Pt resolution",     "SA Pt resolution",     "CB Pt pull",        "SA pull Pt",     "MG pull Pt",
                   "D0 resolution",     "D0 resolution",     "pull D0",        "SA D0 resolution",     "SA D0 resolution",  "SA D0 pull",
                   "Z0 resolution",     "Z0 resolution",     "pull Z0",        "SA Z0 resolution",     "SA Z0 resolution",  "SA Z0 pull",
                   "Phi resolution",    "pull Phi",    "SA Phi resolution",    "SA Phi pull",
                   "Theta resolution",  "pull Theta",  "SA Theta resolution",  "SA Theta pull",        "CB Theta pull",
                   "MuGirl - combined", "CombinedMuonFit - combined",  "Indet - combined",  "Indet - CombinedMuonFit",   "qOverP ratio"]
      
      #for i in range(len(gifFiles)):
      #   # print i
      #   path = str(gifFiles[i]).strip('\n') + ".gif"
      #   if os.path.isfile(str(path)):
      #      # print "File exists at path ",path
      #      output.write("<h3>" + str(gifTitle[i]).strip() + "</h3>")
      #      line = "<img src=\""+ path + "\">"
      #      output.write(line+'\n')

      output.close()
      return 0

