#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import cgi, os, sys
import re
import string
import random
import subprocess
import cgitb
import shutil

class LumiCalc:

    def __init__(self):

        self.verbose = False
        # Python file to make updates in working directory
        self.updateScript = 'LumiCalcWorking.py'
        self.recoverScript = 'LumiCalcRecover.py'

        # Prod area
        self.homeDir = '/var/www/lumicalc/'
        self.dev = False

        # Check if we are running as dev
        if os.environ.get('SERVER_NAME', '') == 'atlas-lumicalc-dev.cern.ch':
            self.homeDir = '/var/www/lumicalc_dev/'
            self.dev = True

        self.resultsDir = '/var/www/lumifiles/'

        self.htmlDir = self.homeDir + 'athena/LumiBlock/LumiCalc/html/'
        self.scriptDir = self.homeDir + 'athena/LumiBlock/LumiCalc/share/'
        self.workdir = os.getcwd()

        self.uselar = True
        self.subdir = '.'

    def createWorkdir(self):
        
        # Create a new subdirectory for this
        self.subdir = hex(random.randint(0,0xFFFFFF))[2:]
        self.workdir = self.resultsDir + self.subdir
        os.mkdir(self.workdir)

        # Open the output file
        self.f = open(self.workdir+'/working.html', 'w')

        # Copy the working script
        shutil.copyfile(self.updateScript, self.workdir+'/'+self.updateScript)

        # Make sure it is executable
        os.chmod(self.workdir+'/'+self.updateScript, 0o755)

        # Also the recovery script        
        shutil.copyfile(self.recoverScript, self.workdir+'/'+self.recoverScript)
        os.chmod(self.workdir+'/'+self.recoverScript, 0o755)

    def cleanUp(self):

        # Close the output file
        self.f.close()
        
        # OK, want to move working.html to result.html
        # Update script should do the rest
        shutil.move(self.workdir+'/working.html', self.workdir+'/result.html')
        
    def printHead(self):

        self.f.write( '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n' )
        self.f.write( '<head>\n' )
        self.f.write( '<title>ATLAS Luminosity Calculator</title>\n' )
        self.f.write( '<meta http-equiv="content-type" content="text/html; charset=ISO-8859-1"></meta>\n' )
        self.f.write( '<meta name="ATLAS_Luminosity_Calculator" content="ATLAS Luminosity Calculator"></meta>\n' )
        self.f.write( '<link rel="stylesheet" type="text/css" href="/css/atlas-datasummary.css" media="screen"></link>\n' )
        self.f.write( '<link rel="stylesheet" type="text/css" href="/css/atlas-datasummary-print.css" media="print"></link>\n' )
        self.f.write( '<link rel="shortcut icon" href="/images/favicon.ico"></link>\n' )
        self.f.write( '</head>\n' )

    def printTopTable(self):    
        self.f.write( '<body>\n' )
        self.f.write( '<table class="toptable">\n' )

        # Header file
        fhead = open(self.htmlDir+"header.html", 'r')
        for line in fhead.readlines():
            self.f.write( line )
        self.f.write( '</table>\n' )

    def printBottomTable(self):

        # Footer file
        ffoot = open(self.htmlDir+"footer.html", 'r')
        for line in ffoot.readlines():
            self.f.write( line )

    def printFooter(self):
        self.f.write( '<table class="bottomtable">\n')
        self.f.write( '<tr style="height:45px; vertical-align: top;">\n')
        self.f.write( '<td><a href="http://validator.w3.org/check?uri=referer">\n')
        self.f.write( '<img src="/images/xhtml10.gif" alt="Valid XHTML 1.0 Transitional" /></a><br />\n')
        self.f.write( '<a href="http://jigsaw.w3.org/css-validator/check/referer">\n')
        self.f.write( '<img src="/images/css.gif" alt="Valid CSS!" /></a>\n')
        self.f.write( '</td><td style="text-align:right"></td></tr></table>\n')

    def parseForm(self):
        
        form = cgi.FieldStorage()

        # A nested FieldStorage instance holds the file
        self.fileitem = form['file']
        self.lumitag = form['lumitag'].value
        self.livetrig = form['livetrig'].value
        self.trigger = form['trigger'].value
        self.clopts = form['clopts'].value
        self.lartag = form['lartag'].value
        for otheritem in form.getlist('other'):
            self.clopts += (' ' + otheritem)
        
        if self.lartag == 'None':
            self.lartag = ''
            
    def printForm(self):    
        self.f.write( '<div id="content">\n' )

        # Go through and check for errors
        error = False
        self.f.write( '<h3>Checking inputs</h3>\n' )
        self.f.write( '<ul>\n' )

        self.f.write( '<li>Luminosity DB tag: '+self.lumitag+'</li>\n' )

        if len(self.livetrig) == 0:
            self.f.write( '<li><font color="red">Error: Live fraction trigger not specified!</font></li>\n' )
            error = True

        else:
            self.f.write( '<li>Live fraction trigger: '+self.livetrig+'</li>\n' )

            if string.find(self.livetrig, 'MBTS') > -1 and string.find(self.lumitag, 'HI') > -1:
                self.f.write( '<li><font color="red">Error: Using MBTS to determine live fraction during Heavy Ion running is not recommended!  Try using L1_LUCID_A_C instead.</font></li>\n')
                
        if len(self.trigger) == 0:
            self.f.write( '<li><font color="red">Warning: No physics trigger specified, uncorrected luminosity will be reported!</font></li>\n' )

        else:
            self.f.write( '<li>Physics trigger: '+self.trigger+'</li>\n' )

        self.uselar = False
        if len(self.lartag) == 0:
            self.f.write( '<li><font color="red">Warning: No LAr EventVeto tag specified, uncorrected luminosity will be reported!</font></li>\n')

        else:
            self.f.write( '<li>LAr EventVeto Tag: ' + self.lartag+'</li>\n' )
            self.uselar = True
            
        # Test if the file was uploaded
        if self.fileitem.filename:

            # strip any leading C: from windows
            if self.fileitem.filename[0:2] == 'C:':
                self.fileitem.filename = self.fileitem.filename[2:]
            # strip leading path from file name to avoid directory traversal attacks
            self.grlfn = os.path.basename(self.fileitem.filename)

            self.grlfilepath = self.workdir + '/' + self.grlfn    
            open(self.grlfilepath, 'w').write(self.fileitem.file.read())
            self.f.write( '<li>GRL file: '+self.grlfn+' was uploaded successfully to '+self.workdir+'</li>\n' )
    
        else:
            self.f.write( '<li><font color="red">Error: No GRL file was uploaded</font></li>\n' )
            error = True

        if len(self.clopts) > 0:
            self.f.write( '<li>Other options: '+self.clopts+'</li>\n' )

        # Check if LAr EventVeto specified at the command line 
        if '--lar' in self.clopts:
            self.uselar = True
            
        self.f.write( '</ul>\n' )

        if error:
            self.f.write( '<p>Please fix errors before continuing!</p>\n' )

        else:
            self.f.write( '<p>Specification complete!</p>\n' )
            
        return error

    def printEnvironment(self): 
        
        self.f.write( '<h3>Environment</h3>\n' )
        f = open(self.htmlDir+'version.html', 'r')
        for line in f.readlines():
            self.f.write( line )
    
    def printCommand(self):
        
        self.f.write( '<h3>Command Details</h3>\n' )

        self.command = ''
        if self.lumitag == '--online':
            self.command += ' --online'
        elif self.lumitag == 'None':
            pass
        else:
            self.command += (' --lumitag=' + self.lumitag)

        self.command += (' --livetrigger=' + self.livetrig)
        if len(self.trigger) > 0:
            self.command += (' --trigger=' + self.trigger)
    
        self.command += (' --xml=' + self.grlfilepath)

        if len(self.lartag) > 0:
            self.command += (' --lar --lartag=' + self.lartag)
            
        if len(self.clopts) > 0:
            self.command += (' '+self.clopts)

        self.f.write( '<p>iLumiCalc '+self.command+'</p>\n' )

        #self.cmdstr = os.getcwd()+'/runLumiCalc.sh '+self.command
        if self.dev:
            self.cmdstr = self.scriptDir+'runLumiCalcDev.sh '+self.command
        else:
            self.cmdstr = self.scriptDir+'runLumiCalc.sh '+self.command

    def runCommand(self):
        p = subprocess.Popen(self.cmdstr+' > output.txt 2>&1', executable='/bin/bash', cwd=self.workdir, shell=True) 
        p.wait()

#
# Now, parse iLumiCalc output file
#
    def parseOutput(self):
        
        matchrun = re.compile(r'Run ([0-9]+) LB \[([0-9]+)-([0-9]+)\]')
        matchlumidel = re.compile(r': IntL delivered \(ub\^-1\) : ([0-9\.e+]+)')
        matchlumipre = re.compile(r': IntL after livefraction \(ub\^-1\):  ([0-9\.\e\+\-]+)')
        matchlumilar = re.compile(r': IntL after LAr fraction \(ub\^-1\):  ([0-9\.\e\+\-]+)')
        matchlumirec = re.compile(r': IntL recorded after prescale \(ub\^-1\) : ([0-9\.\e\+\-]+)')
        matchgoodlb = re.compile(r': Good LBs     : ([0-9]+)')
        matchbadlb = re.compile(r': Bad LBs      : ([0-9]+)')

        matchtotlumidel = re.compile(r': Total IntL delivered \(ub\^-1\) : ([0-9\.\e\+\-]+)')
        matchtotlumipre = re.compile(r': Total IntL after livefraction \(ub\^-1\):  ([0-9\.\e\+\-]+)')
        matchtotlumilar = re.compile(r': Total IntL after LAr fraction \(ub\^-1\):  ([0-9\.\e\+\-]+)')
        matchtotlumirec = re.compile(r': Total IntL recorded \(ub\^-1\) : ([0-9\.\e\+\-]+)')
        matchtotgoodlb = re.compile(r': Total Good LBs     : ([0-9]+)')
        matchtotbadlb = re.compile(r': Total Bad LBs     : ([0-9]+)')

        matchrealtime = re.compile(r': Real time: ([0-9\.\e\+\-]+)')
        matchcputime = re.compile(r': CPU time:  ([0-9\.\e\+\-]+)')

        self.runset = set()
        self.lumidel = dict()
        self.lumirec = dict()
        self.lumipre = dict()
        self.lumilar = dict()
        self.goodlb = dict()
        self.badlb = dict()
        
        self.realtime = 0
        self.cputime = 0
        
        currentrun = '000000'

        for line in open(self.workdir+'/output.txt').readlines():
            m=matchrun.search(line)
            if m:
                if self.verbose:
                    print ('Found run/lbstart/lbend:', m.group(1), m.group(2), m.group(3))
                currentrun = m.group(1)
                self.runset.add(currentrun)

            m=matchlumidel.search(line)
            if m:
                if self.verbose:
                    print ('Found lumiDel:', m.group(1), 'in run', currentrun)
                self.lumidel[currentrun] = float(m.group(1)) + self.lumidel.get(currentrun, 0.)
                
            m=matchlumirec.search(line)
            if m:
                if self.verbose:
                    print ('Found lumiRec:', m.group(1), 'in run', currentrun)
                self.lumirec[currentrun] = float(m.group(1)) + self.lumirec.get(currentrun, 0.)
        
            m=matchlumipre.search(line)
            if m:
                if self.verbose:
                    print ('Found lumiPre:', m.group(1), 'in run', currentrun)
                self.lumipre[currentrun] = float(m.group(1)) + self.lumipre.get(currentrun, 0.)
        
            m=matchlumilar.search(line)
            if m:
                if self.verbose:
                    print ('Found lumiLar:', m.group(1), 'in run', currentrun)
                self.lumilar[currentrun] = float(m.group(1)) + self.lumilar.get(currentrun, 0.)
        
            m = matchgoodlb.search(line)
            if m:
                if self.verbose:
                    print ('Found goodLB:', m.group(1), 'in run', currentrun)
                self.goodlb[currentrun] = int(m.group(1)) + self.goodlb.get(currentrun, 0)

            m = matchbadlb.search(line)
            if m:
                if self.verbose:
                    print ('Found badLB:', m.group(1), 'in run', currentrun)
                self.badlb[currentrun] = int(m.group(1)) + self.badlb.get(currentrun, 0)

            # Match end-of-job totals
            m=matchtotlumidel.search(line)
            if m:
                if self.verbose:
                    print ('Found Total lumiDel:', m.group(1))
                self.lumidel['Total'] = float(m.group(1)) + self.lumidel.get('Total', 0.)
        
            m=matchtotlumirec.search(line)
            if m:
                if self.verbose:
                    print ('Found Total lumiRec:', m.group(1))
                self.lumirec['Total'] = float(m.group(1)) + self.lumirec.get('Total', 0.)
        
            m=matchtotlumipre.search(line)
            if m:
                if self.verbose:
                    print ('Found Total lumiPre:', m.group(1))
                self.lumipre['Total'] = float(m.group(1)) + self.lumipre.get('Total', 0.)
        
            m=matchtotlumilar.search(line)
            if m:
                if self.verbose:
                    print ('Found Total lumiLar:', m.group(1))
                self.lumilar['Total'] = float(m.group(1)) + self.lumilar.get('Total', 0.)
        
            m = matchtotgoodlb.search(line)
            if m:
                if self.verbose:
                    print ('Found Total goodLB:', m.group(1), 'in run', currentrun)
                self.goodlb['Total'] = int(m.group(1)) + self.goodlb.get('Total', 0)

            m = matchtotbadlb.search(line)
            if m:
                if self.verbose:
                    print ('Found Total badLB:', m.group(1), 'in run', currentrun)
                self.badlb['Total'] = int(m.group(1)) + self.badlb.get('Total', 0)

            m = matchrealtime.search(line)
            if m:
                self.realtime = float(m.group(1))
                
            m = matchcputime.search(line)
            if m:
                self.cputime = float(m.group(1))

    def printTable(self):
        
        # Scale factor to apply to lumi values
        # Base value is ub-1
        totalLumi = self.lumirec.get('Total', 0.)
        if totalLumi > 1E8:
            scale = 1E-6
            unit = 'pb<sup>-1</sup>'
        elif totalLumi > 1E5:
            scale = 1E-3
            unit = 'nb<sup>-1</sup>'
        elif totalLumi > 1E2:
            scale = 1.
            unit = 'ub<sup>-1</sup>'
        else:
            scale = 1000.
            unit = 'mb<sup>-1</sup>'

        
        # Open excel file
        self.exf = open(self.workdir+'/lumitable.csv', 'w')

        self.f.write( '<p>Command complete - CPU time: '+str(round(float(self.cputime), 1))+' s, Clock time: '+str(round(float(self.realtime), 1))+' s</p>\n' )
        self.f.write( '<h3>Total Luminosity: '+str(scale*self.lumirec.get('Total', 0.))+' '+unit+'</h3>\n' )

        self.f.write( '<table class="lumitable"><tbody>\n' )
        self.f.write( '<tr><th>Run<br />Number</th><th>Number of<br />Good LBs</th><th>Number of<br />Bad LBs</th><th>Luminosity<br />Delivered ('+unit+')</th><th>Luminosity<br />Livefraction Corrected ('+unit+')</th>')
        self.exf.write('Run, Good, Bad, LDelivered, LRecorded')
        if self.uselar:
            self.f.write('<th>Luminosity<br />LAr Veto Corrected ('+unit+')</th>')
            self.exf.write(', LAr Corrected')
            
        self.f.write('<th>Luminosity<br />Prescale Corrected ('+unit+')</th><th>Lumi-Weighted<br />Live Fraction (percent)</th>')
        self.exf.write(', Prescale Corrected, Live Fraction')
        
        if self.uselar:
            self.f.write('<th>Lumi-Weighted<br />LAr Fraction (percent)</th>')
            self.exf.write(', LAr Fraction')
            
        self.f.write('<th>Lumi-Weighted<br />Prescale</th></tr>\n' )
        self.exf.write(', Prescale Fraction\n')
        
        # Sort this to get in order
        runlist = []
        for run in self.runset:
            runlist.append(run)
    
        runlist.sort()

        # Tack this on the end so we print total also
        runlist.append('Total')        

        for run in runlist:

            if run == 'Total':
                self.f.write( '<tr class="highlight">\n' )
                self.f.write( '<td>Total</td>\n')
                self.exf.write('Total, ')
                
            else:
                self.f.write( '<tr>\n' )
                self.f.write( '<td>'+run+' (<a href="http://atlas-runquery.cern.ch/query.py?q=find+run+'+run+'+%2F+show+all+and+lhc">ARQ</a>, <a href="http://atlas-trigconf.cern.ch/psevo/'+run+'/">Prescales</a>)</td>\n')
                self.exf.write((run+', '))
                
            self.f.write( '<td>'+str(self.goodlb.get(run, 0))+'</td><td>'+str(self.badlb.get(run, 0))+'</td>\n' )
            
            self.f.write( '<td>'+str(scale*self.lumidel.get(run, 0.))+'</td>\n' )
            self.f.write( '<td>'+str(scale*self.lumipre.get(run, 0.))+'</td>\n' )
            self.exf.write('%d, %d, %f, %f, ' % (self.goodlb.get(run, 0), self.badlb.get(run, 0), scale*self.lumidel.get(run, 0.), scale*self.lumipre.get(run, 0.)))

            if self.uselar:
                self.f.write( '<td>'+str(scale*self.lumilar.get(run, 0.))+'</td>\n' )
                self.exf.write('%f, ' % (scale*self.lumilar.get(run, 0.)) )
                
            self.f.write( '<td>'+str(scale*self.lumirec.get(run, 0.))+'</td>\n' )
            self.exf.write('%f, ' % (scale*self.lumirec.get(run, 0.)) )
            
            # Print live fraction
            if self.lumidel.get(run, 0.) > 0.:
                self.f.write( '<td>'+str(round(100*self.lumipre.get(run, 0.)/self.lumidel.get(run, 0.), 2))+'</td>\n' )
                self.exf.write('%.2f, ' % (100*self.lumipre.get(run, 0.)/self.lumidel.get(run, 0.)) )
            else:
                self.f.write( '<td></td>\n' )
                self.exf.write(' , ')
                
            # Print LAr fraction
            if self.uselar:
                if self.lumipre.get(run, 0.) > 0.:
                    self.f.write( '<td>'+str(round(100*self.lumilar.get(run, 0.)/self.lumipre.get(run, 0.), 2))+'</td>\n' )
                    self.exf.write('%.2f, ' % (100*self.lumilar.get(run, 0.)/self.lumipre.get(run, 0.)) )
                    
                else:
                    self.f.write( '<td></td>\n' )
                    self.exf.write(' , ')
                
                # Print prescale
                if self.lumirec.get(run, 0.) > 0.:
                    self.f.write( '<td>'+str(round(self.lumilar.get(run, 0.)/self.lumirec.get(run, 0.), 3))+'</td>\n' )
                    self.exf.write('%.2f' % (100*self.lumilar.get(run, 0.)/self.lumirec.get(run, 0.)) )
                else:
                    self.f.write( '<td></td>\n' )

            else:
                # Print prescale
                if self.lumirec.get(run, 0.) > 0.:
                    self.f.write( '<td>'+str(round(self.lumipre.get(run, 0.)/self.lumirec.get(run, 0.), 3))+'</td>\n' )
                    self.exf.write('%.2f' % (100*self.lumipre.get(run, 0.)/self.lumirec.get(run, 0.)) )
                else:
                    self.f.write( '<td></td>\n' )

            self.f.write( '</tr>\n' )
            self.exf.write('\n')


        self.exf.close()

        self.f.write( '</tbody></table>\n' )
        self.f.write( '<p>[<a href="/results/'+self.subdir+'/lumitable.csv">Luminosity table as CSV file</a>]</p>\n' )
        
    #
    # Print warnings and errors from log file
    #
    def printWarnings(self):

        matchwarn = re.compile('--- <.+>')
        warnlist = []
        
        for line in open(self.workdir+'/output.txt').readlines():
            if matchwarn.search(line):
                warnlist.append(line)

        if len(warnlist) == 0:
            return
        
        self.f.write('<h3>Warnings/Errors</h3>\n')
        self.f.write('<pre style="color: red">')
        for warnline in warnlist:
            self.f.write(warnline.replace('<', '&lsaquo;').replace('>', '&rsaquo;'))
            
        self.f.write('</pre>\n')        
        self.f.write('<p>Check the <a href="/results/'+self.subdir+'/output.txt">Raw iLumiCalc output</a> for more information.</p>\n' )
        
    #
    # Now setup full dump (hidden by JS button)
    #
    def printScript(self):
        self.f.write( '<script>' )
        self.f.write( 'function makevisible() {' )
        self.f.write( 'var visible' )
        self.f.write( 'var hidden' )
        self.f.write( 'visible = document.getElementById("visible").innerHTML;' )
        self.f.write( 'hidden = document.getElementById("results").innerHTML;' )
        self.f.write( 'document.getElementById("visible").innerHTML = hidden;' )
        self.f.write( '}' )
        self.f.write( '</script>' )

    def printDetails(self):
        self.f.write( '<div id="visible"><p><input type="button" value="Show detailed output" onclick="makevisible()"></p></div>\n' )

        self.f.write( '<div id="results" style="visibility: hidden">\n' )
        self.f.write( '<h3>Detailed Output</h3>\n' )
        self.f.write( '<pre>\n' )
        for line in open(self.workdir+'/output.txt').readlines():
            self.f.write( line )

        self.f.write( '</pre></div>\n' )

    def printLinks(self):
        self.f.write( '<h3>Links</h3>\n' )
        self.f.write( '<p>The following links provide access to the iLumiCalc output files.  These will likely remain for several weeks, but please copy any critical files to a more permanent location.</p>\n' )

        self.f.write( '<p><a href="/results/'+self.subdir+'/">iLumiCalc working directory</a> - location for all output files</p>\n' ) 
        self.f.write( '<p><a href="/results/'+self.subdir+'/result.html">Output HTML</a> - this page</p>\n' )
        self.f.write( '<p><a href="/results/'+self.subdir+'/output.txt">Raw iLumiCalc output</a></p>\n' )
        self.f.write( '<p><a href="/results/'+self.subdir+'/'+self.grlfn+'">Original GRL XML file</a></p>\n' )

    # Call iLumiCalc to print out help
    def printHelp(self):
        self.f.write( '<h3>LumiCalc Usage</h3>\n' )
        self.f.write( '<pre>\n' )
        self.f.write( '> iLumiCalc --help\n\n' )
        for line in open (self.scriptDir+'help.txt').readlines():
            self.f.write( line )

        self.f.write( '</pre>\n' )

    # All done
    def printFinish(self):
        self.f.write( '</div>\n' )
        # self.printBottomTable()
        self.printFooter()
        self.f.write( '</body></html>\n' )

    def printRedirect(self, outfile='LumiCalcWorking.py'):
        print ('Content-Type: text/html')
        print ()# Blank line, end of headers
        print ('<html><head>')
        print ('<meta http-equiv="Refresh" content="0; url=/results/'+self.subdir+'/'+outfile+'">')
        print ('</head></html>')

    # For debugging, dump all os.environ variables
    def dumpEnviron(self):

        print ('Content-Type: text/html')
        print () # Blank line, end of headers
        print ('<html>')
        for key in os.environ:
            print('<p><b>',key,':</b>',os.environ[key],'<p>')
        print ('</html>')

# Run from command line    
if __name__ == "__main__":

    # Enable debugging output for CGI
    cgitb.enable()

    # Experimental way here
    lc = LumiCalc()
    lc.createWorkdir()
    
    # Make sure we have something in working.html
    lc.parseForm() 
    lc.printHead()
    lc.printTopTable()
    if lc.printForm():
        # Error, finish here
        lc.printHelp()
        lc.printFinish()
        lc.cleanUp()
        lc.printRedirect('result.html')
        sys.exit(0)  # Exit on error

    # OK, we are going to run iLumiCalc
    # Print the redirect and close stdout to get the browser to respond
    lc.printEnvironment()
    lc.printCommand()
    lc.f.flush()
    lc.printRedirect()
    sys.stdout.flush()
    sys.stdout.close() # Trigger browser to complete
    os.close(1) # Really close it

    # Here we run the command
    lc.runCommand()

    lc.parseOutput()
    lc.printTable()
    lc.printWarnings()
    lc.printLinks()
    lc.printFinish()
    lc.cleanUp()

    # Other possibility to fork
    #sys.stdout.flush()
    #if os.fork(): return
    #fw = open('/dev/null','w')
    #os.dup2(fw.fileno(),1)
    #os.dup2(fw.fileno(),2)
    
