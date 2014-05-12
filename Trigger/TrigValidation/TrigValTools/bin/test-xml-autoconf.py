#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Takes test xml file and enabled/disables tests marked with:
#
#  <!-- DISABLE-IN-PROJECT-BEGIN: AtlasTier0 AtlasProduction-->
#    <TEST name="HelloWorld" type="script" suite="TrigOnline">
#    ...
#    </TEST>
#  <!-- DISABLE-IN-PROJECT-END -->
#
#  or
#
#  <!-- ENABLE-IN-PROJECT-BEGIN: AtlasHLT -->
#    <TEST name="HelloWorld" type="script" suite="TrigOnline">
#    ...
#    </TEST>
#  <!-- ENABLE-IN-PROJECT-END -->
#
#
import sys
import os
import re

if len(sys.argv) < 2:
   print 'Syntax: text-xml-autoconf.py test.xml'
   sys.exit(1)

project = os.environ.get('AtlasProject','')
if len(project)==0:
   print 'Cannot read $AtlasProject'
   sys.exit(1)
   
reDisable = re.compile(".*<\!--\s*DISABLE-IN-PROJECT-BEGIN:\s*(.*)-->")
reEnable = re.compile(".*<\!--\s*ENABLE-IN-PROJECT-BEGIN:\s*(.*)-->")
reEnd = re.compile(".*<\!--.*-IN-PROJECT-END.*-->")
disable = False

f = open(sys.argv[1],'r')
for line in f:
   
   m = reDisable.match(line)   
   if m and (project in m.group(1).split()):
      disable = True
      continue
      
   m = reEnable.match(line)
   if m and not (project in m.group(1).split()):
      disable = True
      continue

   if disable and reEnd.match(line):      
      disable = False
      continue

   if not disable: print line,

