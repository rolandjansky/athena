#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import glob
import cgitb
import subprocess

# CGI script to display intermediate information in the lumicalc web page when iLumiCalc is processing a GRL
# This will continue to be displayed until the results.html file is available
# It is expected that this is called from the results directory

class LumiWorking:

    def __init__(self):
        self.delay = 10
        self.appearsDone = False

    def checkDone(self):

        return len(glob.glob('result.html'))>0

    def redirectDone(self):

        print ('Content-Type: text/html')
        print ()# Blank line, end of headers
        print ('<html><head>')
        print ('<meta http-equiv="Refresh" content="0; url=result.html">')
        print ('</head></html>')

    def redirectRecover(self):

        print ('Content-Type: text/html')
        print ()# Blank line, end of headers
        print ('<html><head>')
        print ('<meta http-equiv="Refresh" content="0; url=LumiCalcRecover.py">')
        print ('</head></html>')

    def printWorking(self):

        self.printHead()
        self.printParams()
        self.printDetails()
        self.printFooter()
        
    def printHead(self):    
        print ('Content-Type: text/html')
        print ()# Blank line, end of headers
        print ('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">')
        print ('<head>')
        print ('<title>ATLAS Luminosity Calculator</title>')
        print ('<meta http-equiv="content-type" content="text/html; charset=ISO-8859-1"></meta>')
        print ('<meta name="ATLAS_Luminosity_Calculator" content="ATLAS Luminosity Calculator"></meta>')
        print ('<link rel="stylesheet" type="text/css" href="/css/atlas-datasummary.css" media="screen"></link>')
        print ('<link rel="stylesheet" type="text/css" href="/css/atlas-datasummary-print.css" media="print"></link>')
        print ('<link rel="shortcut icon" href="/images/favicon.ico"></link>')
        print ('<meta http-equiv="Refresh" content="%d">' % self.delay)   # Refresh every 15 seconds
        print ('</head>')

    # Here we print what has been written into working.html after the body tag 
    def printParams(self):
        
        fhead = open("working.html", 'r')
        writeOut = False
        for line in fhead.readlines():
            if line == '<body>\n': writeOut = True
            if not writeOut: continue
            print (line, end='')

        fhead.close()

    def printDetails(self):

        print ('<h3>iLumiCalc Output</h3>')
        print ('<p>This page should automatically update approximately every %d seconds, and will show you the results when iLumiCalc is done.' % self.delay)
        print ('If for some reason the updates stop, the final results should (eventually) be available <a href="result.html">here</a>.')
        print ('<p>In the meantime, the tail of the <a href="output.txt">raw iLumiCalc output</a> is appended below.')
        print ("Please don't restart your calculation unless the process really seems to be dead!</p>")
        print ('<pre>')
        p = subprocess.Popen('tail -37 output.txt', shell=True, stdout=subprocess.PIPE).stdout

        self.appearsDone = False

        for line in p.readlines():
            print (line, end='')
            if line.strip() == "Done":
                self.appearsDone = True

        print ('</pre>')
        if self.appearsDone:
            print ('<p>lumiCalc appears to be done, but likely the original process script has timed out.  You can try finishing by hand with the link <a href="LumiCalcRecover.py">here</a>.  Note, this will likely screw things up if your process is still running, however..</p>\n')

    def printFooter(self):
        print ('</div>')
        print ('<table class="bottomtable">')
        print ('<tr style="height:45px; vertical-align: top;">')
        print ('<td><a href="http://validator.w3.org/check?uri=referer">')
        print ('<img src="/images/xhtml10.gif" alt="Valid XHTML 1.0 Transitional" /></a><br />')
        print ('<a href="http://jigsaw.w3.org/css-validator/check/referer">')
        print ('<img src="/images/css.gif" alt="Valid CSS!" /></a>')
        print ('</td><td style="text-align:right"></td></tr></table>')
        print ('</body>')
        print ('</html>')
                                                                                                                      
if __name__ == "__main__":

    # Enable debugging output for CGI
    cgitb.enable()

    lw = LumiWorking()
    if lw.checkDone():
        lw.redirectDone()

    else:
        lw.printWorking()
