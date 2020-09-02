#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os
import glob
import cgitb

# Enable debugging output for CGI  
cgitb.enable()

# Use imp to import directly from path in cgi-bin?
import imp

if os.environ.get('SERVER_NAME', '') == 'atlas-lumicalc-dev.cern.ch':
    lcmod = imp.load_source('LumiCalc', '/var/www/lumicalc_dev/athena/LumiBlock/LumiCalc/python/LumiCalcHtml.py')
else:
    lcmod = imp.load_source('LumiCalc', '/var/www/lumicalc/athena/LumiBlock/LumiCalc/python/LumiCalcHtml.py')

lc = lcmod.LumiCalc()

lc.workdir = os.getcwd()
lc.subdir = os.path.basename(os.path.normpath(lc.workdir))
try:
    lc.grlfn = os.path.basename(glob.glob(lc.workdir+'/*.xml')[0])
except Exception:
    lc.grlfn = ''

lc.f = open(lc.workdir+'/working.html', 'a')
lc.parseOutput()
lc.printTable()
lc.printWarnings()
lc.printLinks()
lc.printFinish()
lc.cleanUp()

# Redirect
print ('Content-Type: text/html')
print ()# Blank line, end of headers
print ('<html><head>')
print ('<meta http-equiv="Refresh" content="0; url=result.html">')
print ('</head></html>')
