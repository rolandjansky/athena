# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import subprocess
'''
Text = 'Subject: Test mail 1\n'
Text += 'FROM: MuonCalibrationFit\n'
Text += 'TO: giacomo.artoni@cern.ch\n'
Text += 'Content-Type: text/html; charset=ISO-8859-1'
Text += '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"><html>\n'
Text += '<b>a</b> &nbsp; &#8592; &nbsp; <b>b</b> &nbsp; &#8592; &nbsp; <b>c</b> &nbsp; &#8592; &nbsp; <b>d</b><br>'
'''
Text  = 'Subject: Results available for MuonCalibrationFit - TEST\n'
Text += 'FROM: MuonCalibrationFit\n'
Text += 'TO: giacomo.artoni@cern.ch\n'
Text += 'Content-Type: text/html; charset=ISO-8859-1\n'
Text += '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"><html>\n'
Text += '<br><b>MuonCalibrationFit</b>: FromScratch4<br><br>Start date: Thu Nov 20 18:13:55 2014<br>End date: Thu Nov 20 18:15:49 2014<br>Total time taken: <font color="#CC0000">01 minutes, 53 seconds</font><br><br>SVN Info: Revision n.508 (<a href="https://svnweb.cern.ch/trac/gartonisvn/browser/Muons/Calibration/MuonCalibrationFit?rev=508">Link to code</a>)<br><br>Job Info: Links (<a href="https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_FromScratch4/">General</a>, <a href="https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_FromScratch4/Configuration.php">Configuration</a>, <a href="https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_FromScratch4/BackgroundFits.php">Background Fits</a>, <a href="https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_FromScratch4/TemplateFits.php">Teamplate Fits</a>)<br><br>Job History: <a href="https://gartoni.web.cern.ch/gartoni/Work/Muons/Calibration/Results_FromScratch4/Configuration.php">FromScratch4</a> &nbsp; &#8592; &nbsp; FromScratch3 &nbsp; &#8592; &nbsp; FromScratch2 &nbsp; &#8592; &nbsp; FromScratch1<br><br>'
#subprocess.call( 'echo "%s" | mailx -a \'Content-Type: text/html\' -s "$(echo -e "%s\nContent-Type: text/html")" giacomo.artoni@cern.ch' % ( Text, 'Test mail' ), shell = True )
#print subprocess.check_output( 'echo "%s" | sendmail "giacomo.artoni@cern.ch"' % Text, shell = True ).rstrip()
print 'echo "%s" | sendmail "giacomo.artoni@cern.ch"' % Text
