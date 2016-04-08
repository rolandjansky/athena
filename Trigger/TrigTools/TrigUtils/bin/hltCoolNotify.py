#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Based on Walter Lampl's getIOVAge.py and CoolConvUtilities/AtlCoolTool.py
#
# Author: Frank Winklmeier
#
__doc__ = """\
Usage: hltCoolNotify.py [OPTIONS] [Database ...]

Show changes in COOL folders between two runs. If no run number is specified, the
tool is running in online mode. In this case the difference between the current
run and the last stable beams run will be shown. The GlobalTag for a given run
is retrieved from the TriggerDB via the SuperMasterKey, which itself is retrieved
from COOL.

If only one run is specified, all COOL folders and tags will be shown.

Database is one or more COOL connection strings (e.g. COOLONL_LAR/COMP200).
If not specified, a predefined set of COOLONL database instances is used.
"""

import sys
import time
import operator
import optparse
import signal
import cPickle

import ispy
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen
from CoolConvUtilities.AtlCoolTool import AtlCoolTool
from CoolRunQuery.AtlRunQueryIOV import IOVTime, IOVRange

from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
from TrigConfigSvc.TrigConfigSvcUtils import getPropertyFromDB

from AthenaCommon.Logging import logging
msg = logging.getLogger("hltCoolNotify")
msg.setLevel(logging.INFO)
msg.setFormat("%(asctime)s  Py:%(name)-15s %(levelname)7s %(message)s")
   

# List of DB instances used by HLT
hltCondDB = ["COOLONL_CALO/COMP200",
             "COOLONL_CSC/COMP200",
             #"COOLONL_FWD/COMP200",
             "COOLONL_GLOBAL/COMP200",
             "COOLONL_INDET/COMP200",
             "COOLONL_LAR/COMP200",
             "COOLONL_MDT/COMP200",
             "COOLONL_MUONALIGN/COMP200",
             "COOLONL_PIXEL/COMP200",
             "COOLONL_RPC/COMP200",
             "COOLONL_SCT/COMP200",
             "COOLONL_TDAQ/COMP200",
             "COOLONL_TGC/COMP200",
             "COOLONL_TILE/COMP200",
             "COOLONL_TRIGGER/COMP200",
             "COOLONL_TRT/COMP200"]

# Log output 
log = []

# Global stop flag
STOP = False

# Helpers to convert validity key
def toRun(validity):
   return (validity >> 32)
   
def toLB(validity):
   return (validity & 0xFFFFFFFF)

def toSec(validity):
   return (validity / 1000000000L)

class CoolDB:
   """Cache for COOL db conneciton objects"""
   cooldbs = {}
   
   @classmethod
   def get(cls, dbname):
      if not dbname in cls.cooldbs: 
         cls.cooldbs[dbname] = indirectOpen(dbname,readOnly=True,oracle=True)

      return cls.cooldbs[dbname]


class Run:
   def __init__(self, runno):
      self.runno = runno
      self.smk = None
      self.sor = None         # start of run time in UTC
      self.globalTag = None
      if self.runno!=None: self.update()
      
   def update(self):
      # SOR
      if self.sor==None:
         db = CoolDB.get("COOLONL_TDAQ/COMP200")
         f = db.getFolder("/TDAQ/RunCtrl/SOR_Params")
         o = f.findObject(cool.ValidityKey(self.runno<<32), 0)
         self.sor = toSec(o.get().payload()['SORTime'])

      # SMK
      if self.smk==None:
         keys = TriggerCoolUtil.getHLTConfigKeys(CoolDB.get('COOLONL_TRIGGER/COMP200'),
                                                 [[self.runno,self.runno]])

         if not self.runno in keys:
            raise RuntimeError, ("Cannot find SMK for run %d" % self.runno)
         self.smk = int(keys[self.runno]['SMK'])

      # GlobalTag
      if self.globalTag==None:
         res = getPropertyFromDB('TRIGGERDB', self.smk , 'IOVDbSvc', 'GlobalTag')
         if len(res)==0:
            raise RuntimeError, ('Cannot find GlobalTag for SMK %d' % self.smk)
         
         self.globalTag = res[0][3]


class OnlineRun(Run):
   def __init__(self):
      Run.__init__(self,None)
      self.update()
      
   def update(self):
      p = ispy.IPCPartition("ATLAS")
      rp = ispy.ISObject(p, "RunParams.RunParams")
      rp.checkout()   
      conf = ispy.ISObject(p, "RunParams.TrigConfSmKey")
      conf.checkout()

      self.runno = rp.run_number
      self.sor = rp.timeSOR.c_time()
      self.smk = conf.SuperMasterKey

      Run.update(self)


class Folder:   
   def __init__(self, folder, tag, obj, istime, channel=None):
      self.iov = IOVRange(obj,istime)
      self.folder = folder
      self.tag = tag
      self.insertTime = '%s' % obj.insertionTime()
      self.channel = channel
      
   def __str__(self):
       return '%-40s %s%s %s (%s)' % (self.iov, self.folder, (' (%s)' % self.channel) if self.channel!=None else '',
                                      self.tag, self.insertTime.split('.')[0])

   def __cmp__(self, other):
      return cmp(self.iov, other.iov)

   def __eq__(self, other):
      return self.folder==other.folder and self.tag==other.tag and \
             self.iov==other.iov and self.channel==self.channel

   def __hash__(self):
      return hash('%s%s%s%s%s' % (self.iov, self.folder, self.tag,
                                  self.insertTime, self.channel))
   
class HltCoolTool(AtlCoolTool):

   def __init__(self, database):
      AtlCoolTool.__init__(self, database)
   
   def listFolders(self, run, sor, tag,
                   chan = None, allChan = False):
      """
      Return folder list
      """

      folderList = []
      
      nodelist = self.db.listAllNodes()
      for node in nodelist:
         if not self.db.existsFolder(node): continue
         f = self.db.getFolder(node)

         if chan==None:
            chansel = cool.ChannelSelection.all()
         else:
            chansel = cool.ChannelSelection(chan)

         istime = ((f.description()).find('<timeStamp>time')!=-1)
         if istime:
            limmin = limmax = sor*1000000000L
         else:
            limmin = limmax = (run << 32)

         if tag=='':
            objs = f.browseObjects(limmin,limmax,chansel)
         else:
            if not tag in f.listTags():
               # tag is not defined here, try hierarchical tag
               try:
                  restag = f.resolveTag(tag)
               except Exception:
                  continue
            else:
               restag = tag

            objs = f.browseObjects(limmin,limmax,chansel,restag)

         done = False
         while objs.goToNext() and not done:
            obj = objs.currentRef()
            ch = obj.channelId()

            if not allChan: done = True
            folderList.append(Folder(node, restag, obj, istime, ch if allChan else None))
           
      return folderList
      

def getLastPhysicsRuns(N=1):
   """Return the last N runs with ReadyForPhysics"""

   db = CoolDB.get("COOLONL_TDAQ/COMP200")
   f = db.getFolder("/TDAQ/RunCtrl/DataTakingMode")
   objs = f.browseObjects(cool.ValidityKeyMin, cool.ValidityKeyMax, cool.ChannelSelection(0))
   lastrun = []
   while objs.goToNext():
      obj= objs.currentRef()
      if obj.payload()['ReadyForPhysics']==0: continue
      lastrun.append(toRun(obj.since()))

   return lastrun[len(lastrun)-N:]


def email(toaddr, subject, log):
   import smtplib
   try:
      s = smtplib.SMTP()
      s.connect()
   except Exception as e:
      msg.error('%s' % e)
      return
   
   fromaddr = 'atlas-trigger-conditions-commit@cern.ch' # Required to allow sending to this egroup
   txt = 'From: %s\r\n'\
         'To: %s\r\n'\
         'Reply-To: %s\r\n'\
         'Subject: %s\r\n\r\n' % (fromaddr, toaddr, toaddr, subject)

   txt += '\n'.join(log)
   try:
      s.sendmail(fromaddr, toaddr, txt)
   except Exception as e:
      msg.error('Failure sending email: %s' % e)
      return

   msg.info('Successfully sent email to %s' % toaddr)
   s.quit()


def myhelp(option, opt_str, value, parser):
   """Custom help callback since optparse does not preserve newlines"""

   print __doc__
   
   parser.print_help()
   sys.exit(1)


def handler(signum, f_globals):
   global STOP
   msg.warning('Signal handler called with signal %d' % signum)
   STOP = True


def main():
   signal.signal(signal.SIGTERM, handler)
   
   parser = optparse.OptionParser(usage = '',
                                  add_help_option = False)

   parser.add_option("-r", "--run", action="append", type='int',
                     help="Run number(s). If two runs are given a diff will be done.")

   parser.add_option("-c", "--channels", action="store_true", default = False,
                     help="Show all channels, otherwise only first one")

   parser.add_option("-m", "--mail", type="string",
                     help="Email non-zero differences to given mailing address(es)")

   parser.add_option("-i", "--ignore", type="string",
                     help="File name with list of folders to ignore in diff")

   parser.add_option("-o","--oldDiff", type="string",
                     help="File name with already reported diffs")
   
   parser.add_option("-h", "--help", action="callback", callback=myhelp)
     
   (opt, args) = parser.parse_args()

   oldDiff = set()  # pairs of folder diffs already reported
   if opt.oldDiff:
      try:
         oldDiff = cPickle.load(open(opt.oldDiff))
      except:
         pass
      
   if opt.run!=None and len(opt.run)>2:
      parser.print_help()
      parser.error('Cannot specify more than two run numbers')

   if opt.run==None:
      r = OnlineRun()
      runs = [r]
      lastruns = sorted(getLastPhysicsRuns(2))
      if lastruns[1]==r.runno: runs.append(Run(int(lastruns[0])))
      else: runs.append(Run(int(lastruns[1])))
   else:
      runs = [Run(r) for r in opt.run]

   # Order by run number
   runs = sorted(runs, key=operator.attrgetter('runno'))
   msg.info('Reading conditions for %s' % [r.runno for r in runs])
   
   cooldb = args if len(args)>0 else hltCondDB

   for r in runs:
      log.append('=== Run: %d %s (%s)' %
                 (r.runno,time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(r.sor)),r.globalTag))

   log.append('=== DBs: %s' % cooldb)
   
   folders = {}
   for db in cooldb:
      msg.info('Reading %s' % db)
      try:
         coolTool = HltCoolTool(db)
      except:
         msg.error('Cannot connect to %s' % db)
         continue

      for r in runs:
         if STOP:
            msg.warning('Aborting...')
            return 0

         if not r.runno in folders: folders[r.runno] = []
         folders[r.runno] += coolTool.listFolders(r.runno, r.sor, r.globalTag,
                                                  allChan = opt.channels)

   # Diff or printout
   diff = []
   ignored = []
   if len(runs)==2:

      ignore = []
      if opt.ignore!=None:
         try:
            ignore = [l.strip() for l in open(opt.ignore).readlines() if not l.startswith('#')]
         except:
            msg.warning('Cannot open file %s' % opt.ignore)
         
      folders_r0 = dict([(f.folder,f) for f in folders[runs[0].runno]])
      for f in folders[runs[1].runno]:
         if not f in folders[runs[0].runno]:
            pair = (folders_r0[f.folder],f)
            if (f.folder in ignore or pair in oldDiff):
               l = ignored
            else:
               l = diff
               oldDiff.add(pair)
               
            l.append("    %s" % folders_r0[f.folder])
            l.append("--> %s" % f)
            
      if len(diff)>0:
         log.append('\nConditions differences found:')
         log.extend(diff)
      else:
         log.append('\nNo conditions differences found')
         
      if len(ignored)>0:
         log.append('\nConditions differences ignored:')
         log.extend(ignored)

   else:
      for f in folders[runs[0].runno]:
         log.append(f)

   print '='*80
   for l in log: print l
   print '='*80
   
   if opt.mail!=None and len(diff)>0:
      email(opt.mail, ('HLT conditions changes for run %d' % runs[1].runno),log)

   if opt.oldDiff:
      msg.info('Writing already reported differences to %s' % opt.oldDiff)
      cPickle.dump(oldDiff, open(opt.oldDiff,'w'))
      
   msg.info('Exiting')
         
if __name__ == "__main__":
   sys.exit(main())
      
