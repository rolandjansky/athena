#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file: trigperfmonrtt.py
# @purpose: Script to collect rtt perfmon statistics
# @author: Frank Winklmeier (fwinkl)
# $Id: trigperfmonrtt.py,v 1.15 2009-04-30 09:34:55 fwinkl Exp $
#
__author__  = "Frank Winklmeier"
__version__ = "$Revision: 1.15 $"
__doc__     = "Script to collect RTT PerfMon stats"

import os
import sys
import datetime
import time
import tempfile
import re
import glob
import logging as log
from cStringIO import StringIO
import xml.etree.cElementTree as ET

# Global for command line options
opts = None

class Config:
   memImgWidth = 800
   memImgHeight = 600
   defaultPlatform = 'x86_64-slc6-gcc48-opt'
   
class Build:
   def __init__(self, name, branch, platform, project = "offline"):
      self.name = name          # unique identifier, e.g. dev(slc4)
      self.branch = branch
      self.platform = platform
      self.project = project

   def __str__(self):
      return self.name+" "+self.branch+" "+self.platform+" "+self.project

   def __hash__(self):
      return hash(self.__str__())

   def __eq__(self, other):
      return (self.name==other.name) and (self.branch==other.branch) and \
             (self.platform==other.platform) and (self.project==other.project)

   def __ne__(self, other):
      return not self.__eq__(other)
   
class Rtt:
   """Holder for Rtt globals"""
   home = "/afs/cern.ch/atlas/project/RTT/prod/Results/rtt"
   homeUrl = "https://atlas-rtt.cern.ch/prod/rtt"
   summaryHome = "/afs/cern.ch/atlas/project/RTT/prod/summaryfiles"
   
   # Never display results for these tests
   postTestRemove = ["PerfMonDozer",
                     "FileGrepper",
                     "PerfMonRunner",
                     "LogTail","TruncateLog",
                     "CheckFileRunner",
                     "chainDump"]

   # Map between RTT and ATLAS project names
   projconv = {"hlt"     : "AtlasHLT",
               "offline" : "AtlasProduction",
               "p1hlt"   : "AtlasP1HLT",
               "tier0"   : "AtlasTier0"
               }
   
   summaryFiles = []  # List of files summaryHome

   @staticmethod
   def getSummaryFiles():
      """Read (and cache) list of summary files"""
      
      if len(Rtt.summaryFiles)>0: return Rtt.summaryFiles
      Rtt.summaryFiles = glob.glob(os.path.join(Rtt.summaryHome, '*_rtt_*.xml'))
      # Remove any files older than 9 days
      now = time.time()
      Rtt.summaryFiles = filter(lambda p:(now-float(os.path.basename(p).split('_')[0]))<9*24*3600, Rtt.summaryFiles)
      log.debug('Found %d RTT summary files' % len(Rtt.summaryFiles))
      
      Rtt.summaryFiles.sort()   # sort by timestamp, i.e. 1268748026
      return Rtt.summaryFiles


   def getBuilds(self, projects = [], cmtconfig = Config.defaultPlatform):
      """Returns a list with Builds for projects and matching cmtconfig"""
      
      # Project shortcuts for display
      projshort = {'AtlasHLT' : 'hlt',
                   'AtlasProduction' : '',
                   'AtlasP1HLT'  : 'p1',
                   'AtlasCAFHLT' : 'caf'
                   }

      builds = []
      now = time.time()
      week = 3600*24*7
      
      # Example: /path/1268748026_rtt_rel_2_15.5.X.Y-VAL_build_i686-slc4-gcc34-opt_p1hlt.xml      
      filt = re.compile('.*/([0-9]+)_rtt_rel_[0-6]_(.*)_build_(%s)_(.*).xml' % (cmtconfig))
      summaryFiles = self.getSummaryFiles()
      for f in summaryFiles:
         m = filt.match(f)
         if m==None or len(m.groups())!=4: continue
         t = float(m.group(1))
         branch = m.group(2)
         platform = m.group(3)
         rttprj = m.group(4)
         prj = Rtt.projconv.get(rttprj, rttprj)
         if prj in projects and (now-t)<week:
            alias = branch
            sh = projshort.get(prj,prj)
            if len(sh)>0: alias += '/'+sh
            if platform != Config.defaultPlatform:
               try: alias += (' [%s]') % platform.split('-')[2]
               except: pass
                  
            b = Build(alias, branch, platform, rttprj)
            if not b in builds: builds.append(b)

      return builds
  
          
class RttRun(Rtt):
   """Class to store information of one RTT run"""
   
   def __init__(self, build, nightly, packages=None):
      self.tests = []             # list of test resutls (RttTest)
      self.build = build          # list of builds (Build)
      self.nightly = nightly      # nightly number (integer)
      self.packages = packages    # only store these job groups

      try:
         self.readRttResults()
      except Exception, e:
         import traceback
         log.error("Cannot parse %s" % self.rttSummaryFile())
         traceback.print_exc(file=sys.stdout)

         
      return

   
   def rttSummaryFile(self):
      """Return path of xml summary"""
      # Example: 1268748026_rtt_rel_2_15.5.X.Y-VAL_build_i686-slc4-gcc34-opt_p1hlt.xml
      
      rel = "rel_%d" % self.nightly
      sumFile = "_rtt_%s_%s_build_%s_%s.xml" % (rel, self.build.branch, self.build.platform, self.build.project)
      sumFileList = filter(lambda s:s.find(sumFile)!=-1, self.getSummaryFiles())
      
      if len(sumFileList)==0:
         log.debug("No RTT summary file found matching '%s'" % sumFile)
         return None

      return sumFileList[-1]  # use latest match
                   
   def readRttResults(self):
      """Read all tests in xml file"""

      sumFileName = self.rttSummaryFile()
      if sumFileName==None:
         return
      
      log.debug("Reading file %s" % (sumFileName))
      
      try:
         tree = ET.parse( sumFileName )
      except:
         log.debug("Cannot open "+sumFileName)
         return
      
      for pkgNode in tree.findall('package'):
         pkgName = pkgNode.findtext('packageName').strip()

         # Skip if package not in list of packages
         if self.packages!=None and (not pkgName in self.packages): continue
         
         for m in pkgNode.getiterator('minder'):

            # Skip jobs that had serious RTT issues
            if m.findtext('errors')!=None and len(m.findtext('errors').strip())>0: continue
            
            t = PerfMonRttTest()
            t.jobName = m.findtext('jobName').strip()
            try: t.jobDoc = m.findtext('jobDocString').strip()
            except: pass
            
            try: t.jobDisplayName = m.findtext('jobDisplayName').strip()
            except: pass

            t.state = m.findtext('state').strip()
            t.jobExitCode = m.findtext('jobExitCode').strip()
            
            try: t.batchKilled = m.findtext('exceptionState').strip()=='batchKilled'
            except: t.batchKilled = False
            
            t.resultsPath = m.findtext('resultsPath').strip()
            t.packageName = pkgName
            t.ident = m.findtext('identifiedName').strip()
            t.jobGroup = m.findtext('jobGroup').strip()
            # The identifiedName is not unique. Use the hash of the resultsPath instead
            t.uid = "t_%d" % hash(t.resultsPath)

            s = m.findtext('stateHistory').strip()
            t.stateHistory = eval(s)

            testResults = m.find('tests')
            if testResults:
               for test in testResults.findall('test'):
                  name = test.findtext('testname').strip().partition("_")[2]
                  if not name in Rtt.postTestRemove:
                     testresult = test.findtext('testresult')
                     if testresult!=None:
                        t.postTests[name] = testresult.strip()


            self.tests += [t]
      return

   
class RttTest:
   """Class to hold infos of a RTT test"""

   def __init__(self):
      self.packageName = ""
      self.state = None
      self.resultsPath = ""
      self.jobName = ""
      self.jobDisplayName = ""
      self.jobDoc = ""
      self.ident = ""   
      self.uid = ""
      self.stateHistory = []
      self.postTests = {}
      return
   
   def webResultsPath(self,rtt):
      """Return the URL of results derived using an Rtt instance"""
      return self.resultsPath.replace(rtt.home,rtt.homeUrl)      
   

class PerfMonRttTest(RttTest):
   """Class to hold additional infos of PerfMon results"""

   # Check these files for the perfmon results
   perfMonRootFile = ["out.ntuple.root","ntuple.perfmon.root"]
   
   def __init__(self):
      RttTest.__init__(self)
      self.vmem_init = None
      self.leak = None
      self.memHref = ""         # href to memory graph

      return
   
   def getStats(self,imgDir,imgDirUrl):
      """Get memory stats and graph from histogram"""

      # Find the perfmon output file
      filename = None
      for name in self.perfMonRootFile:         
         path = os.path.join(self.resultsPath, name)
         if os.path.isfile(path): filename = path

      if filename==None: return False
      
      from ROOT import TFile, TCanvas, TH1, TH1F, TF1, gStyle, gROOT, gPad
      from ROOT import TPaveStats
      
      gROOT.SetStyle("Plain")
      gStyle.SetOptFit(11)
      gStyle.SetStatW(0.12)
      gStyle.SetStatH(0.1)
      gStyle.SetPadTickY(1)
      gStyle.SetTitleOffset(1.3,"Y")
      gStyle.SetTitleSize(0.04,"pad")
      gStyle.SetHistLineWidth(2)
      log.debug("Reading ROOT file %s" % filename)
      # Ignore warnings for opening of files
      gROOT.ProcessLine("gErrorIgnoreLevel = 7000;")
      f = TFile(filename)
      gROOT.ProcessLine("gErrorIgnoreLevel = 0;")
      
      if f.IsZombie(): return False

      kBlue = 4
      kRed = 2

      hv = f.Get("000/vmem_PerfMonSlice.000")
      hr = f.Get("000/rss_PerfMonSlice.000")
      if hv==None or hr==None: return False
      
      try:
         can = TCanvas("can", "can", Config.memImgWidth, Config.memImgHeight)
         can.SetGridx(1)
         can.SetGridy(1)

         # Fit starting at 15% of total event range
         fitstart = 0.15
         fitmin = hv.GetXaxis().GetXmin() + fitstart*(hv.GetXaxis().GetXmax() - hv.GetXaxis().GetXmin())
         hv.Fit("pol1","QF","",fitmin,hv.GetXaxis().GetXmax())
         hr.Fit("pol1","QF","",fitmin,hv.GetXaxis().GetXmax())
         fit = hv.GetFunction("pol1")
         if fit:
            self.vmem_init = fit.GetParameter(0)
            self.leak = fit.GetParameter(1)

            # If the leak is very small, do not trust fit         
            if (abs(self.leak)-fit.GetParError(1)) < 0:
               bins = hv.GetNbinsX()-1
               self.leak = (hv.GetBinContent(bins) - hv.GetBinContent(int(fitstart*bins)))/((1-fitstart)*bins)            
            
         hv.UseCurrentStyle()
         hr.UseCurrentStyle()
         hv.SetTitle(self.jobName)
         hr.SetTitle(self.jobName)
         hv.SetLineColor(kBlue)
         hr.SetLineColor(kRed)

         # Set min/max for y-axis
         #the below always gives zero
         #hv.SetMinimum(0.9*min(hr.GetMinimum(),hv.GetMinimum()))
         hv.SetMinimum(0.7*min(hv.GetFunction("pol1").GetParameter(0),hr.GetFunction("pol1").GetMinimum()))
         hv.SetMaximum(1.3*max(hr.GetMaximum(),hv.GetMaximum()))
         
         hv.SetName("Virtual memory")
         hr.SetName("Real memory")
         hv.GetYaxis().SetTitle("Memory usage [MB]")
         hr.GetYaxis().SetTitle(hv.GetYaxis().GetTitle())

         hv.Draw("HIST")
         can.Update()         
         st1 = gPad.GetPrimitive("stats")
         st1.SetName("stats1")
         st1.SetLineColor(kBlue)
         x1 = st1.GetX1NDC()-0.01
                  
         hr.Draw("HISTsames")
         can.Update()
         # Move stat box
         st2 = gPad.GetPrimitive("stats")
         st2.SetName("stats2")
         w = st2.GetX2NDC()-st2.GetX1NDC()
         st2.SetX1NDC(x1-w)
         st2.SetX2NDC(x1)
         st2.SetLineColor(kRed)
         
         hv.GetFunction("pol1").Draw("sames")
         hr.GetFunction("pol1").Draw("sames")
         
         st1.Draw("same")
         st2.Draw("same")


         can.Modified()
         
         self.memHref = os.path.join(imgDirUrl,self.uid+".png")
         gROOT.ProcessLine("gErrorIgnoreLevel = 3000;")
         can.SaveAs(os.path.join(imgDir,self.uid+".png"))

      except:
         raise
         return False
      
      return True
            

                    
class PerfMonRttResults:
   """Class to hold RTT results for several builds/nightlies/jobgroups"""
   
   def __init__(self, name, builds, packages = None):
      self.name = name
      self.rttRuns = []
      self.packages = packages
      self.builds = builds
      self.nightlies = range(7)  # must be consecutive starting from 0
      self.results = {}   # Test dictionary: [testName][build][nightly] = RttTest

      # Sort builds by name
      import operator
      self.builds.sort(key=operator.attrgetter("name"), reverse=True)
      # These should always appear in this order
      fixedOrder = ["val","bugfix","dev","devval"]
      for f in reversed(fixedOrder):
         for i,b in enumerate(self.builds):
            if b.name==f:
               # pop and put at front
               self.builds.insert(0, self.builds.pop(i))
               break
      
      self.readResults()
      self.cleanupResults()
      return
   
   def readResults(self):
      """Loop over branches and nightlies and read RTT test results for job groups"""
      
      for b in self.builds:
         for n in self.nightlies:
            rttRun = RttRun(b,n,self.packages)
               
            for t in rttRun.tests:
               if self.packages==None or t.packageName in self.packages:
                  # Take either the display name or the job option name as ID
                  name = t.jobDisplayName
                  if name=="" or name=="None": name = t.jobName
                  if not self.results.has_key(name):
                     self.results[name] = {}
                  if not self.results[name].has_key(b.name):
                     self.results[name][b.name] = [PerfMonRttTest() for i in self.nightlies]

                  dDay = 0
                  if len(t.stateHistory)>0:
                     lastTime = t.stateHistory[-1][1]                     
                     dDay = (time.time()-time.mktime(time.strptime(lastTime,"%y/%m/%d %H:%M")))/(24*3600)

                  if dDay>8:
                     log.debug("Skipping test (%s,%s,%s) for %s in rel_%d because it is %d days old" %
                               (name,t.jobName,t.jobDisplayName,b.name,n,dDay))
                  else:
                     log.debug("Adding test (%s,%s,%s) for %s in rel_%d" %
                               (name,t.jobName,t.jobDisplayName,b.name,n))
                     self.results[name][b.name][n] = t

      return

   def cleanupResults(self):
      """Remove all tests that have no results at all"""

      newBuilds = []
      for i,b in enumerate(self.builds):
         used = False
         for test in self.results.keys():
            for n in self.nightlies:
               try:
                  if self.results[test][b.name][n]!=None and self.results[test][b.name][n].state!=None:
                     used = True
               except: pass
         if used:
            newBuilds.append(b)
         else:
            log.debug("Deleting build (%s) because no tests were run" % self.builds[i].name)

      self.builds = newBuilds
      return

   def findJobDoc(self, testName):
      """Find the first non-empty jobDoc for testName"""
      
      # Ignore that the jobDoc might be different for different builds
      if not testName in self.results: return ''
      for nightlies in self.results[testName].itervalues():
         for j in nightlies:
            if len(j.jobDoc)>0: return j.jobDoc
            
      return ''
   

class Rtt2Html:
   """HTML formatter"""
   
   def __init__(self,rtt,outDir,outDirUrl):
      self.rtt = rtt
      self.outDir = outDir
      self.outDirUrl = outDirUrl
      self.imgDir = os.path.join(outDir,"png")
      self.imgDirUrl = "png"  #os.path.join(outDirUrl,"png")
      
      try:
         os.makedirs(self.imgDir)
      except:
         pass
      
      if not os.path.exists(self.imgDir):
         log.error("Cannot create image output directory %s" % self.imgDir)
      return

   def printHeader(self,title):
      print '''
      <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
      "http://www.w3.org/TR/html4/loose.dtd">
      <html>
      <head>
      <!-- ****************************************************************************
      This page was automatically generated by perfmonrtt.py ''' + __version__ +'''
      **************************************************************************** -->
      <link rel="stylesheet" type="text/css" href="perfmonrtt.css">
      <script src="perfmonrtt.js" type="text/javascript"></script>
      <!--
      /***********************************************
      * AnyLink Drop Down Menu- (C) Dynamic Drive (www.dynamicdrive.com)
      * This notice MUST stay intact for legal use
      * Visit http://www.dynamicdrive.com/ for full source code
      ***********************************************/
      -->'''
      print '<base target="_parent">'
      print '<title>%s</title>' % title
      print '</head>'
      return

   def printFooter(self,time):
      print datetime.datetime.now().strftime("<address>Last update: %Y/%m/%d %H:%M:%S") + \
            " by trigperfmonrtt.py "+__version__+"<br>"\
            "Time for update: %.0f sec<br>"\
            "Responsible: Frank Winklmeier</address>" % (time)
      
      print "</body></html>"
      return
   
   def printDynamicMenu(self,test,nightly,build):
      """Create the dynamic menu"""

      o = StringIO()
      html = o.write
      
      html('<div class="dropmenu" id="%s" onMouseover="clearhide()" '\
           'onMouseout="dynamichide(event)">' \
           % (test.uid))

      webResults = test.webResultsPath(self.rtt)

      # Only if we have PerfMon stats
      if not test.vmem_init==None:
         html('<a href="%s" onclick="mem(this.href); return false">Memory graph</a>' % (test.memHref))
         html('<a href="%s">PerfMon results [txt]</a>'\
              % os.path.join(webResults,"ntuple.perfmon.summary.txt"))
         
         # Show link to PDF file if it exists
         if os.path.exists(os.path.join(test.resultsPath,"ntuple.perfmon.pdf")):
            html('<a href="%s">PerfMon results [pdf]</a>'\
                 % os.path.join(webResults,"ntuple.perfmon.pdf"))

      # Show link to Hephaestus file if it exists
      try:   # in case directory doesn't exist
         for f in os.listdir(test.resultsPath):
            if f.find("pmon.heph")!=-1 or f.find("LeakCheck.txt")!=-1:
               html('<a href="%s">Leak report</a>' % os.path.join(webResults,f))
               break
      except: pass

      # Show link to log file if it exists
      if os.path.exists(os.path.join(test.resultsPath,test.jobName+"_log")):
         html('<a href="%s">Job log file</a>'\
              % os.path.join(webResults,test.jobName+"_log"))

      # Link to RTT summary page
      # e.g.: https://atlas-rtt.cern.ch/index.php?q=%28release=rel_0;branch=17.1.X.Y-VAL2;cmtconfig=i686-slc5-gcc43-opt;project=AtlasP1HLT;packagename=TrigP1Test;jobname=AllPT_data_leakCheck_chain%29&page=job   
      html('<a href="https://atlas-rtt.cern.ch/index.php?q=(release=rel_%s;branch=%s;cmtconfig=%s;project=%s;packagename=%s;jobname=%s)&page=job" target="rttresult">RTT result page</a>'\
           % (nightly,build.branch,build.platform,Rtt.projconv.get(build.project,build.project),test.packageName,test.jobName))

      html('<a href="%s">[All Memory graphs]</a>' % self.summaryFileName(build,nightly))

      # Print status and last state with date/time
      #html('state: <span id="%s" class="status">%s</span>' % (test.state,test.state))
      if len(test.stateHistory)>0:
         cssStatus = "success" if test.jobExitCode=="0" else "failure"
         state = test.stateHistory[-1][0]
         stateTime = test.stateHistory[-1][1]
         html('<span id="%s" class="status">%s</span>: %s'% (cssStatus,state,stateTime))

      # Print any post-processing test results
      if len(test.postTests)>0 and os.path.exists(test.resultsPath):
         html('<div class="hMenu">Post-processing:</div>')
         for name,status in sorted(test.postTests.items()):

            # Find the test log file
            testlog = None
            logs = os.listdir(test.resultsPath)

            # 1) Check if there is a plain text log file 
            for logname in filter(lambda s:s.endswith('.log'), logs):
               if logname.lower().find(name.lower()) != -1:
                  testlog = logname
                  break

            # 2) If not, use the html log file
            if testlog==None:
               for logname in filter(lambda s:s.endswith('.log.html'), logs):
                  if logname.lower().find(name.lower()) != -1:
                     testlog = logname
                     break
               
            if testlog:
               link = '<a href="%s" id="%s" class="status">%s</a>' % \
                      (os.path.join(test.webResultsPath(self.rtt),testlog), status, name)
            else:
               link = name

            html('<span id="%s" class="status">%s</span>' % (status,link))
               
      html('</div>\n')
      return o.getvalue()

   def getMemLeak(self,test):
      """Return text for mem leak"""

      text = ""
      # Create and fit mem graphs"""      
      if test.getStats(self.imgDir,self.imgDirUrl):
         # Initial memory usage
         text = '%.0fM/' % test.vmem_init
         # Show leak in kB or MB
         leak = test.leak
         if abs(leak)>1: text += '%.1fM' % leak
         elif abs(leak)>(1/10240) : text += '%.1fk' % (leak*1024)
         else: text += '%.0fB' % (leak*1024*1024)
         
      return text

   
class PerfMonRtt2Html(Rtt2Html):
   """HTML formatter for PerfMon results"""
   
   def __init__(self,rtt,outDir,outDirUrl):
      Rtt2Html.__init__(self,rtt,outDir,outDirUrl)   
      self.tableCounter = 0
      # Configure post-processing status symbol
      self.postTestIgnore = []  # ignore in status symbol
      self.postTestWarning = ["RootComp","RegTest"]  # only give warning (orange)
      
      return
   
   def printHeader(self, bodyClass=None):
      Rtt2Html.printHeader(self,"Trigger RTT PerfMon results")
      if bodyClass:
         print '<body onunload="saveSelections()" class="%s">' % bodyClass
      else:
         print '<body onunload="saveSelections()">'
      return

   def printLegend(self):
      print """
      <a name="legend"><h4>Legend:</h4></a>
      <table border=0>
      <tr><td class="today">rel_x</td><td>Today's tests</td></tr>
      <tr><td><span class="menuLink" id="success">ok</span></td><td>Test successfull (no PerfMon results, problem in post-processing)</td></tr>
      <tr><td><span class="menuLink">x</span></td><td>Test failure</td></tr>
      <tr><td><span class="menuLink" id="batchKilled">kill</span></td><td>Killed by batch system (e.g. timeouts)</td></tr>
      <tr><td><span class="menuLink" id="running">...</span></td><td>RTT job is running</td></tr>
      <tr><td>?</td><td>Nothing known about test</td></tr>
      <tr><td>n/a</td><td>Test is not available in this branch</td></tr>
      <tr><td><span class="menuLink" id="old">grey</span></td><td>Old result from last week</td></tr>
      """
      print '<tr><td><img src="bulletGreen.png"></td><td>All post-processing tests successful',
      if len(self.postTestIgnore)>0:
         print '(ignored: %s)' % (", ".join(self.postTestIgnore)),
      print '</td></tr>'
      
      if len(self.postTestWarning)>0:
         print '<tr><td><img src="bulletOrange.png"></td><td>All post-processing tests successful (ignored: %s)</td></tr>' % (", ".join(self.postTestIgnore+self.postTestWarning)),
         
      print """
      <tr><td><img src="bulletRed.png"></td><td>At least one post-processing test failed</td></tr>
      </table><br>
      """
      return    

   
   def printTable(self,perfmon):
      """Print the HTML table"""

      o = StringIO()
      html = o.write
      
      if perfmon==None: return
      self.tableCounter += 1

      formname = "form%d" % self.tableCounter
      # Checkboxes to select visible builds
      html('<div style="margin-bottom:5px">')
      html('<form name="%s"><b>Builds:</b>' % formname)
      for b in perfmon.builds:
         html('<input onClick="toggleBuild(\'%d\',\'%s\');" checked="checked" type="checkbox" name="build" value="%s">%s' % (self.tableCounter,b.name,b.name,b.name))
      html('&nbsp;&nbsp;&nbsp&nbsp;<i>[Default platform: %s]</i>' % Config.defaultPlatform)
      html('</form><script type="text/javascript">loadSelections("%s")</script></div>\n' % self.tableCounter)
      
      html('<table class="main" id="table%d">' % (self.tableCounter))
      html('<tr><th align="left">Test</th><th align="left">Build</th>')

      iToday = (datetime.date.today().weekday()+1) % 7
      for n in perfmon.nightlies:
         if n==iToday: html('<th align="right" class="today">')
         else: html('<th align="right">')
         html("rel_%d</th>" % n)
         
      html("</tr>\n")

      testlist = perfmon.results.keys()
      testlist.sort()

      dynMenus = ""
      for t,test in enumerate(testlist):
         if t%2==0: color = "hi"
         else: color = "lo"

         # Test title and doc string               
         doc = perfmon.findJobDoc(test)
         if len(doc)>0: doc = ' title="%s"' % doc

         html('<tr class="%s"><td class="testName" rowspan="%d"%s>%s</td><td></td>'\
              % (color,len(perfmon.builds)+2,doc,test))
         
         # Hack to have at least one visible row at the top
         for i in perfmon.nightlies:
            if i==iToday: html('<td class="%stoday"></td>' % color)
            else: html('<td></td>')
            
         html('</tr>\n')
         
         for j,b in enumerate(perfmon.builds):

            html('<tr class="%s" id="%s">' % (color,b.name))
            html('<td title="%s">%s</td>' % (b.platform, b.name))
            # This can happen if the test is new
            if not perfmon.results[test].has_key(b.name):
               for i in perfmon.nightlies:
                  if i==iToday: html('<td align="right" class="%stoday">' % color)
                  else: html('<td align="right">')
                  html('n/a</td>')
               html('</tr>\n')
               continue

            v = perfmon.results[test][b.name]

            for i in perfmon.nightlies:
               if i==iToday: html('<td align="right" class="%stoday">' % color)
               else: html('<td align="right">')

               # This means we don't have any information about this test               
               # E.g. because summary file was not available or test is not in it
               if v[i].state==None:
                  html('?</td>')
                  continue
               elif v[i].state=='runningInBatch':
                  text = "..."
               elif v[i].state=='done' and v[i].batchKilled:
                  text = "kill"
               elif v[i].state=='done' and v[i].jobExitCode=="0":
                  if opts.nograph: text = "test"
                  else:            text = self.getMemLeak(v[i])
                  if text=="": text = "ok"
               else:
                  text = "x"

               cssStatus = "success" if v[i].jobExitCode=="0" else "failure"
               if len(v[i].stateHistory)>0:
                  lastTime = v[i].stateHistory[-1][1]                     
                  dDay = (time.time()-time.mktime(time.strptime(lastTime,"%y/%m/%d %H:%M")))/(24*3600)
                  # For today's tests figure out which results are still from last week
                  # We define any results older than 4 days as old (somewhat arbitrary but works)
                  if i==iToday and dDay>4:                     
                     cssStatus = "old"                        
               
               # dynamic menu link
               html('<nobr><span class="menuLink" id="%s" onClick="return dropdown(this, event, \'%s\')" '\
                     'onMouseout="delayhide()">%s</span>'\
                     % (cssStatus,v[i].uid,text))
               
               # Post-processing test results
               if len(v[i].postTests)>0:
                  # Remove all tests to ignore
                  postTests = v[i].postTests.copy()
                  [postTests.pop(k) for k in self.postTestIgnore if postTests.has_key(k)]                  
                  allTestsOK = reduce(lambda a,b:a&b, map(lambda x:x=="success",postTests.values()))
               
                  if allTestsOK: html('<img src="bulletGreen.png">')
                  else:
                     # Remove all warning-only tests
                     tmp = v[i].postTests.copy()
                     [tmp.pop(k) for k in self.postTestWarning if tmp.has_key(k)]
                     allTestsOK = reduce(lambda a,b:a&b, map(lambda x:x=="success",tmp.values()))
                     if allTestsOK: html('<img src="bulletOrange.png">')
                     else: html('<img src="bulletRed.png">')
                  
               html('</nobr></td>\n')
               dynMenus += self.printDynamicMenu(v[i],i,b)
               
            # end of nightly loop
            html("</tr>\n")

         # Hack to have at least one visible row at the bottom
         html('<tr class="%s"><td></td>' % color)
         for i in perfmon.nightlies:
            if i==iToday: html('<td class="%stoday"></td>' % color)
            else: html('<td></td>')
         html('</tr>')

      html(dynMenus)
      html('</table><script type="text/javascript">toggleBuild("%s")</script><br>' % self.tableCounter)
      
      print o.getvalue()
      
      # Write summary files with all graphs per branch/rel
      for b in perfmon.builds:
         for n in perfmon.nightlies:
            stdout = sys.stdout
            sys.stdout = open(os.path.join(self.outDir,self.summaryFileName(b,n)),'w')
            self.writeSummaryFile(perfmon,b.name,n)
            sys.stdout.close()
            sys.stdout = stdout
         
      return
         

   def writeSummaryFile(self,perfmon,branch,rel):
      """ Write summary file for branch/rel with all tests """

      title = "Overview %s, rel_%d" % (branch,rel)
      print "<html><head><title>%s</title></head><body>" % title
      print "<h1>%s</h1>" % title
      testlist = perfmon.results.keys() 
      testlist.sort()
      for test in testlist:
         try:
            href = perfmon.results[test][branch][rel].memHref
            if href!="": print '<img src="%s"></img>' % href
         except: pass

      print "</body></html>"
      return


   def summaryFileName(self,build,nightly):
      return 'table_%d-mem-%s-%s-rel_%d.html' \
             % (self.tableCounter,build.branch,build.platform,nightly)



class TrigPerfMonRttApp:
   
   def initialize(self):
   
      log.basicConfig(level=log.WARNING,
                      stream=sys.stdout,
                      format="%(asctime)s %(levelname)7s %(message)s",
                      datefmt="%Y-%m-%d %H:%M:%S")

      from optparse import OptionParser
      parser = OptionParser()

      parser.add_option("-o",
                        dest = "outDir",
                        default = "./",
                        help = "Output directory [./]")

      parser.add_option("-u",
                        dest = "outDirUrl",
                        help = "URL of the output directory")

      parser.add_option("--nograph", action="store_true",
                        help = "Do not create any graphs")
      
      parser.add_option("--debug", action="store_true",
                        help = "Additional debugging output on stdout")

      global opts
      (opts, args) = parser.parse_args()
      if opts.outDirUrl==None: opts.outDirUrl = opts.outDir
      if opts.debug: log.getLogger().setLevel(log.DEBUG)

      # Temporary directory where the new files will be created
      self.tmpOutDir = tempfile.mkdtemp() #os.path.join(opts.outDir,".tmp")
      try: os.makedirs(self.tmpOutDir)
      except: pass
      if not os.path.exists(self.tmpOutDir):
         log.error("Cannot create temporary directory %s" % self.tmpOutDir)
         return

      # Import ROOT
      sys.argv = [sys.argv[0]]  # Do not pass any command line args to ROOT
      from PerfMonAna import PyRootLib
      self.ROOT = PyRootLib.importRoot( batch=True )
      self.opts = opts
      
      return


   def run(self):         

      # create RTT instance
      RTT = Rtt()   

      # Get all builds from today
      offlineBuilds = RTT.getBuilds(["AtlasProduction","TrigMC","AtlasP1HLT","AtlasCAFHLT"], "i686-.*-opt")
      offlineBuilds += RTT.getBuilds(["AtlasProduction","TrigMC","AtlasP1HLT","AtlasCAFHLT"], "x86.*-opt")
      
      hltBuilds = RTT.getBuilds(["AtlasHLT","AtlasP1HLT","AtlasCAFHLT","AtlasProduction"], "i686-.*-opt")
      hltBuilds += RTT.getBuilds(["AtlasHLT"], "x86.*-opt")
      
      log.debug("Offline builds:")
      for b in offlineBuilds: log.debug(" %s" % b)
      log.debug("HLT builds:")
      for b in hltBuilds: log.debug(" %s" % b)

      tests = []
      tests += [PerfMonRttResults("TriggerTest", offlineBuilds, ["TriggerTest"])]
      tests += [PerfMonRttResults("TrigAnalysisTest", offlineBuilds, ["TrigAnalysisTest"])]
      tests += [PerfMonRttResults("TrigP1Test", hltBuilds, ["TrigP1Test"])]

      html = PerfMonRtt2Html(RTT,self.tmpOutDir,self.opts.outDirUrl)

      # Main page
      stdout = sys.stdout
      sys.stdout = open(os.path.join(self.tmpOutDir,'index.html'),'w')

      print '''
      <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
      <html>
      <head>
      <link rel="stylesheet" type="text/css" href="perfmonrtt.css">
      <script src="perfmonrtt.js" type="text/javascript"></script>
      <title>Trigger RTT Perfmon Results</title>
      </head>
      
      <frameset rows="32,*" border="0" onunload="saveSelections()">
      <frame src="navi.html" name="navi" scrolling="no">
      <frame src="intro.html" name="main">
      </frameset>
      </html>
       '''

      sys.stdout.close()

      # Navigation frame
      sys.stdout = open(os.path.join(self.tmpOutDir,'navi.html'),'w')
      html.printHeader(bodyClass="navi")
      print '<form name="form0"><b>Test package:</b>'
      for t in tests:
         print '<input type="radio" name="test" value="%s" onClick=parent["main"].location.href="%s.html">%s' % (t.name,t.name,t.name)
      print '&nbsp;'*6,'<a href="intro.html" target="main">[Help]</a>'      
      print '</form><script type="text/javascript">loadSelections("0");loadMainFrame(document.forms["form0"].elements["test"],parent["main"])</script>'
      print '</body></html>'
      sys.stdout.close()

      # Main frame
      for t in tests:
         log.debug("Creating HTML test result table for %s" % t.name)
         t0 = time.time()
         sys.stdout = open(os.path.join(self.tmpOutDir,t.name+'.html'),'w')
         html.printHeader()
         html.printTable(t)
         html.printLegend()   
         html.printFooter(time.time()-t0)

         sys.stdout.close()

      sys.stdout = stdout
      
      return
   
      
   def finalize(self):      

      log.debug('Copying files to final destination')
      
      cmd = "rsync -r --delete %s/ %s" % (self.tmpOutDir, self.opts.outDir)
      os.system(cmd)
      cmd = "rm -rf %s" % self.tmpOutDir
      os.system(cmd)
      
      from TrigValTools.Utils import findInstallDir
      installDir = findInstallDir("TrigValTools/perfmonrtt")
      if not installDir:
         log.error("Could not find installation directory 'TrigValTools/perfmonrtt'")
         return 1

      # Copy additional files (js, css, etc.) to output directory
      cmd = "cp %(dir)s/perfmonrtt.css %(dir)s/perfmonrtt.js %(dir)s/*.png %(dir)s/intro.html %(dest)s" % {"dir":installDir, "dest":self.opts.outDir}
      os.system(cmd)
      
      return

   

#
# Main program starts here
#
def main():
   app = TrigPerfMonRttApp()
   app.initialize()
   app.run()
   app.finalize()


if __name__ == "__main__":
   sys.exit(main())
