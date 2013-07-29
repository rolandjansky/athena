#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @author $Author: cranshaw $
## @version $Revision: 1.11 $
##

#########################################################################
##
## Transformation for the Tier-0 TAG copy
##
##  - input: 
##    a. single TAG file  
##    b. TAG DB output destination (optional)
##       - default is project_physics if (a) follows nomenclature, otherwise take file name - extension  
##    c. Atlas release (optional), e.g. 14.2.10,
##       - "takeFromEnv" will cause it to assume setup was done outside script
##    d. TAG DB connect string (optional)
##       - default is oracle://int8r/atlas_tags_loading_test
##  - uploads TAGs to dedicated database
##  - no output data files
##  - creates gpickle report file
##
##  Example1: File would be loaded into table fdr08_run1_MinBias
##      % tagUploadTest.py fdr08_run1.0003053.MinBias.merge.TAG.o1_r6_t2._0001.1 
##  Example2: File would be loaded into table t0tag
##      % tagUploadTest.py fdr08_run1.0003053.MinBias.merge.TAG.o1_r6_t2._0001.1 t0tag 
## 
## Uses Atlas (version = 12.0.6 + patches)
##
## (C) Jack Cranshaw, Luc Goossens, Armin Nairz 
##     (September 2006, October 2007)
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time

from AtlasCollectionTransforms.Tool_setup import *
from AtlasCollectionTransforms.Utilities import *

class tagCopyTool:

   # map for storing config
   copymap = {}
   copymap['urls_in'] = []
   copymap['urls_out'] = []
   copymap['query']   = []
   copymap['addattr'] = []
   copymap['metaopt'] = 'NONE'
   copymap['release'] = 'Atlas,takeFromEnv'
   # general log file name
   logfilename = 'log'
   # copy code
   retc_ld = 0
   # log code
   retc_log = 0
   level = 0
   copystate = 'UNKNOWN'
   acronym = ''
   dtcmt = 0
   dttagdb = 0
   nevt = 0
   hasCommits = False

   # constructor
   def __init__(self, dict):
      print "BLARGUS",dict
      self.copymap.update(dict)
      #self.printAll()

   def printAll(self):
      print "Config: ",self.copymap
      print "Commit status: ",self.hasCommits
      print "Run status: ",self.runStatus()
      print "Log status: ",self.logStatus()

   def listArgs(self):
      return self.copymap.keys()

   def runStatus(self):
      return (self.retc_ld,self.copystate,self.dtcmt,self.dttagdb)

   def commitStatus(self):
      return self.hasCommits

   def logStatus(self):
      return (self.level,self.acronym,self.retc_log,self.nevt)

   def buildMergeCmd(self):
      # Check inputs
      if (len(self.copymap['urls_in'])<1 or len(self.copymap['urls_out'])<1):
         print 'Bad parameters'
         return ('','')

      # Check query vector
      if (len(self.copymap['query']) > 1 and len(self.copymap['query']) != len(self.copymap['urls_in'])):
         print 'Query vector ',self.copymap['query'],' not compatible with input'
         return ('','')

      # Build the input and output poolrl's
      # Input
      poolrl_inlist =[]
      for url in self.copymap['urls_in']:

         (prot,poolrl) = urlToPoolrl(url)
         if poolrl != '':
            poolrl_inlist += [poolrl]

      # Output
      poolrl_outlist =[]
      protout = ''
      for url in self.copymap['urls_out']:

         (prot,poolrl) = urlToPoolrl(url)
         if poolrl != '':
            poolrl_outlist += [(prot,poolrl)]

      # Check that inputs and outputs processing went ok
      if (len(self.copymap['urls_in'])!=len(poolrl_inlist) or len(self.copymap['urls_out'])!=len(poolrl_outlist)):
         print 'FATAL: Problem with input/output url arguments'
         return ('','')

      # set base util name
      util="CollAppend"

      # decode release and modify util accordingly
      (code,util,cmd) = parseConfig(util,self.copymap['release'])

      # check that cmd,util are ready
      if code != "OK": return ('','')

      print "##################################################################"
      print "## STEP 2: preparing the run ..." 
      print "##################################################################"
      print " "
      # assemble the run command
      executeLine = "touch log; " + util
      executeLine += " -merge "
      # Input
      executeLine += " -src "
      for poolrl in poolrl_inlist:
         executeLine += "'" + poolrl + "'" + " PHYSICAL_NAME "
      # Output
      executeLine += "-dst "
      for poolrl in poolrl_outlist:
         executeLine += "'" + poolrl[1] + "'" + " PHYSICAL_NAME " 
         if poolrl[0]=='oracle': 
            executeLine += " -nevtcached 5000"
            executeLine += " -extendprov StreamTAG_ref "
            executeLine += " -nevtpercommit 10000000"
      # Query
      if (len(self.copymap['query']) == 1 and self.copymap['query'][0] != ""):   
         executeLine += " -query '"   + self.copymap['query'][0]   +"'"
      elif len(self.copymap['query'])>1:
         executeLine += " -query "
         for query in self.copymap['query']:
            executeLine += "'" + query + "'"
      executeLine += " -metaopt " + self.copymap['metaopt']
      # Addattr for MC proc id ...
      addattr = self.copymap['addattr']
      if len(addattr) > 0 and len(addattr)%3 == 0:
         executeLine += " -addattr"
         for element in addattr:
            executeLine += " '" + element +"'"
      #if xmlover != "":
      #   executeLine += " -xmlMode OVERRIDE -xmlInput " + xmlover
      executeLine += " >> log 2>&1 "

      return (executeLine,util)

   # method which actually executes the copy
   def multiTagCopy(self) :

      #--------------------------------------------------------------------------
      # Job to upload TAG file for Tier0 testing
      # - uses rel. 12.0.6 + patches
      #--------------------------------------------------------------------------
      print " "
      print " "
      print "###################   ATLAS Tier0 TagDB Loading   ################"
      print "##"
      print "##################################################################"
      print "## setting up the run environment ..." 
      print "##################################################################"
      print " "

      # run directory
      wcmd = "echo `pwd`"
      (s,o) = commands.getstatusoutput(wcmd)
      os.environ['BASEDIR'] = o
  
      # coral connection parameters
      os.environ['POOL_RCOLL_CONN_RETRY'] = '120'
      os.environ['POOL_RCOLL_CONN_TIMEOUT'] = '3600'

      (executeLine,util) = self.buildMergeCmd()
      if executeLine == '':
         self.retc_ld = 1000
         self.copystate='INIT_ERROR'
         return self.runStatus()

      print " "
      print "##################################################################"
      print "## running the executable ..." 
      print "##################################################################"
      print " "
      print executeLine
      print " "

      cmd = "rm -f " + self.logfilename + "; touch " + self.logfilename + "; which " + util + " >> log 2>&1; "
      cmd += executeLine
      print cmd
      t1 = time.time()
      (self.retc_ld, o) = commands.getstatusoutput(cmd)
      print "cmd output ",o
      dttot = int(time.time() - t1)
      try : self.dtcmt = int(o[(o.find('CMTTIME = ') + 10):])
      except : self.dtcmt = 0
      self.dttagdb = dttot - self.dtcmt
      print

      # print logfile
      print " "
      print "##----------------------------------------------------------------"
      print "## ... Begin logfile"
      print "##----------------------------------------------------------------"
      cmd = "cat " + self.logfilename
      (s,o) = commands.getstatusoutput(cmd)
      print o
      print "##----------------------------------------------------------------"
      print "## ... End logfile"
      print "##----------------------------------------------------------------"

      print " "
      print "## ... total elapsed time:        ", dttot, "sec"
      print "## ... CMT setup time:            ", self.dtcmt, "sec"
      print "## ... TAG uploading time:        ", self.dttagdb, "sec"

      self.copystate = 'DONE'
      return self.runStatus()

   # ----
   # method which checks logfile for various error conditions of upload
   #
   def checkLog(self,logfile=''):
 
      if logfile == '': logfile = self.logfilename
      acrotxt = []

      # First check if any commits
      (retc, ocom) = commands.getstatusoutput("grep -i 'COMMIT' "+logfile)
      if ocom != '':
         if self.retc_ld != 0 : self.copystate = 'NEEDSCHECK'
         self.hasCommits = True;
         # determine number of processed (=uploaded) events
         cmd = "grep \"total events appended\" "+logfile
         (s,o) = commands.getstatusoutput(cmd)
         if s == 0 : self.nevt = int(o.split()[1])

      # Next check if execution failed
      elif self.retc_ld != 0 : 
         print "ERROR: commands.gso execution problem!"
         self.retc_log += 1
         self.copystate = 'FATAL'
         self.acronym = 'FATAL'
         self.level = 3
         acrotxt += [[self.retc_ld,'TRF_COMMANDSGSO_EXE','']]
         if ocom=='': return self.logStatus()

      # Filter known errors out of log file
      # known_errors: pairs (error text in logfile, replacement)
      known_errors = [ ('Error ORA-02391', 'ORAKNOWN-02391'),  # KNOWN ERROR 1 (Error ORA-02391: exceeded connections on server)
                       ('Error ORA-12560', 'ORAKNOWN-12560'),  # KNOWN ERROR 2 (Error ORA-12560: protocol adapter error, non-fatal)
                       ('Error ORA-12514', 'ORAKNOWN-12514'),  # KNOWN ERROR 3 (Error ORA-12514: listener service unknown, non-fatal)
                       ('Error ORA-03135', 'ORAKNOWN-03135'),  # KNOWN ERROR 4 (Error ORA-03135)
                     ] 

      (inputpath,inputfile) = os.path.split(logfile)
      newlog = inputfile + '.2'
      replace_cmd  = "rm -f " + newlog + " ; "
      replace_cmd += "cat " + logfile
      for e,r in known_errors :
         replace_cmd += " | sed \'s/%s/%s/g\'" % (e, r)
      replace_cmd += " > " + newlog
      #print 'replace_cmd :',replace_cmd 
      (retc_temp, temp) = commands.getstatusoutput(replace_cmd)

      # Bit 1
      # Check logfile for errors initializing the collection
      (retc, txt5) = commands.getstatusoutput("grep -i 'Error executing database procedure POOL_COLLECTION_INIT' "+newlog)
      retc_init = 0
      if retc ==0:
         retc_init = 2

      # Bit 2
      # check logfile for error messages
      (retc_logerr, txt2) = commands.getstatusoutput('grep Error '+logfile)
      if txt2 == '':
         (retc_logerr, txt2) = commands.getstatusoutput('grep ERROR '+logfile)
      if retc_logerr == 0 : 
         retc_logerr = 4  # "error" found
      else : 
         retc_logerr = 0      # no "error" found 

      # Bit 3
      # check logfile for connection problem
      (retc_connerr, txt3) = commands.getstatusoutput("grep -i 'cannot be established' "+logfile)
      if retc_connerr ==0:
         retc_connerr = 8
      else:
         retc_connerr = 0

      # Bit 4
      # Some oracle error or known recoverable
      (retc_ora, txtora) = commands.getstatusoutput("grep -i 'ORA-' "+logfile)
      if self.hasCommits==False and txtora != '':
         retc_ora = 1 << 4
      else:
         retc_ora = 0

      retc_timeout = 0
      if self.hasCommits == False:
         # Bit 5 (two possible routes)
         #
         # Check logfile for database timeout (recoverable) errors
         (retc_dblock, o) = commands.getstatusoutput("grep -i 'database lock detected' "+logfile)
         txt4 = "NO LOCKS"
         # condition 1: DB has locked up (shared table is unavailable)
         if retc_dblock == 0:
            print "WARNING: DB locking occurring"
            (retc_to, o) = commands.getstatusoutput("grep -i 'Wait time exceeded' "+logfile)
            if retc_to == 0:
               txt4 = "DBLOCK: " + o
               retc_timeout = 1 << 5

         # condition 2: Wait was too long, network latency(?)
         if retc_timeout ==0:
            (retc_to, o) = commands.getstatusoutput("grep -i 'No connection to the server is active' "+logfile)
            if retc_to == 0:
               print "latency problem"
               txt4 += " // CONN_TIMEOUT: " + o
               retc_timeout = 1 << 5

            # condition 3: Server was full
            (retc_to, o) = commands.getstatusoutput("grep -i 'ORA-02391' "+logfile)
            if o != '':
               print "Server full"
               retc_ora = 0;  # reset ora flag as non-lethal
               txt4 += " // CONN_TIMEOUT: " + o
               retc_timeout = 1 << 5

      # Now determine content to return
      # It should start with most severe and move to most recoverable

      if retc_init != 0 :
         print "ERROR FATAL: Problem initializing collection, loading failed!"
         self.retc_log += retc_init
         if self.level < 3: 
            self.copystate = 'FATAL'
            self.level = 3
         acrotxt += [[retc_init,'TRF_INIT_PROBLEM',txt5]]

      if retc_logerr != 0 : 
         print "ERROR: Errors detected in log file!\n"
         self.retc_log += retc_logerr
         if self.level < 2: 
            self.copystate = 'ERROR'
            self.level = 2
         acrotxt += [[retc_logerr,'TRF_EXEPROBLEM',txt2]]

      if retc_connerr != 0 :
         print "ERROR: DB connection problem, possible bad config!\n"
         self.retc_log += retc_connerr
         if self.level < 2: 
            self.copystate = 'ERROR'
            self.level = 2

      return self.logStatus()

