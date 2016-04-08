#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# module: PostProcessIgprofWeb.py

from Logger import Logger

class PostProcessIgprofWeb:
   def __init__(self,argDict={}):
      
      self.logger    = Logger()
      descriptor = argDict.get('JobDescriptor', None)
      
      self.resPath      = descriptor.resPath
      self.logFile      = descriptor.log

   def run(self):
      
      f = open('IgprofResults.html', 'w')
      f.write('<html>\n<head>\n<title>You are being redirected to the igprof results</title>\n')
      log=self.resPath[self.resPath.find('/Results/'):]
      f.write('<META HTTP-EQUIV="refresh" CONTENT="2;URL=https://rig.web.cern.ch/rig/cgi-bin/igprof-navigator' + log + '/data/igreport_total/">')
      f.write('</head>\n<body>\n<h1>You are being redirected to the igprof results</h1>\n')
      f.write('<p>You are being forwarded to <a href="https://rig.web.cern.ch/rig/cgi-bin/igprof-navigator' + log + '/data/igreport_total/">https://rig.web.cern.ch/rig/cgi-bin/igprof-navigator' + log + '/data/igreport_total/</a>.</p>\n</body>\n</html>')
      
      return 0
