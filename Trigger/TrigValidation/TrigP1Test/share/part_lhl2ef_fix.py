#!/usr/bin/env python
#
# Script to fix a few objects in the partition after PM was run
#

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import sys
import os
import socket
from optparse import OptionParser

parser = OptionParser(usage = "%prog partition_file")

(opts, args) = parser.parse_args()

if len(args)!=1:
   parser.print_help()
   sys.exit(1)

## Merge partition so we can modify all objects
os.system('oks_merge -o part.merge.xml -s schema.xml ' + args[0])
os.system('mv part.merge.xml ' + args[0])

import pm.project
db = pm.project.Project(args[0])

## Set the correct architecture matching CMTCONFIG
tag = [t for t in db.getObject('Tag') if t.id==os.environ['CMTCONFIG']]
if len(tag)==0:
   print "Cannot find OKS Tag matching",os.environ['CMTCONFIG']
   sys.exit(1)
   
for part in db.getObject('Partition'):
   if part.id=='initial': continue
   part.DefaultTags = [tag[0]]
   db.updateObjects([part])

## Set the HW_Tag for the localhost
for c in db.getObject('Computer'):
   if c.id==socket.gethostname():
      c.HW_Tag = '-'.join(os.environ['CMTCONFIG'].split('-')[:2])
      db.updateObjects([c])
   
## If an L2PU/PT dies make this an error
apps = db.getObject('PTTemplateApplication')
apps += db.getObject('L2PUTemplateApplication')

for a in apps:
   a.IfDies = 'Error'
   a.IfFailed = 'Error'

## Get number of events from data file   
import robhit
import eformat
events = len(eformat.istream(robhit.filename))

## Increase LVL2 timeout, set max number of events
l2sv = db.getObject('L2SVConfiguration')
for a in l2sv:
   a.l2puTimeout_ms = 20000
   a.eventMax = events

## Increase timeout for stop transition
efd = db.getObject('EFD_TemplateApplication')
for a in efd:
   a.ActionTimeout = 600
   a.ShortTimeout = 600

l2pu = db.getObject('L2PUConfiguration')
for a in l2pu:
   a.PSC_process_warning_percentage = 10

pt = db.getObject('PTTemplateApplication')
for a in pt:
   a.PSC_process_warning_percentage = 10
   
## Remove onasic and oks2xxx applications if exists
try:
   rs1 = db.getObject('ResourceSetAND','Onasic')
   rs1.Contains = []
   db.updateObjects([rs1])
except:
   pass

try:
   rs2 = db.getObject('ResourceSetAND','ConfigArchive')
   rs2.Contains = []
   db.updateObjects([rs2])
except:
   pass

## Add temporary IS schemas (needed by runner, can be removed in tdaq-04-00-00)
onlRep = db.getObject('SW_Repository','Online')
onlRep.ISInfoDescriptionFiles += ['share/data/l2pu/schema/l2pu_temp_is.xml',
                                  'share/data/pt/schema/pt_temp_is.xml']
db.updateObjects([onlRep])
   
db.updateObjects(apps+l2sv+efd+l2pu+pt)
sys.exit(0)
