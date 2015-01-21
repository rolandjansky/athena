#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import getopt,fileinput
import xmlrpclib
import GetNumberFromDB as DB
#config reading for default url
import ConfigParser 
config = ConfigParser.RawConfigParser()
config.read('config.ini')
dqmpass = config.get('dqm', 'pass')
baseurl = config.get('url', 'base')


########################################################################
print '''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LADIeS Iframe</title>
<meta name="author" content="'''+config.get('dev', 'email')+''' ['''+config.get('dev', 'email')+''']"/> 
<link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
<link rel="Stylesheet" href="'''+config.get('url','stylescss')+'''" type="text/css" />

<base target="_blank">'''
print '''</head>'''
print '''<body onload="">
'''

########################################################################
f = cgi.FieldStorage()
run = f["run"].value

s = xmlrpclib.ServerProxy('https://'+dqmpass+'@atlasdqm.cern.ch')
run_spec = {'stream': 'physics_IDCosmic', 'source': 'reproc', 'low_run': int(run), 'high_run': int(run)}
# runinfo structure:
# 0 Run type (string)
# 1 Project tag (string)
# 2 Partition name (string)
# 3 Number of events passing Event Filter (integer)
# 4 Run start (seconds after 1970) (integer)
# 5 Run end (seconds after 1970, 0 if run ongoing) (integer)
# 6 Number of luminosity blocks (-1 if run ongoing) (string)
# 7 Data source (string) - this is only kept for backward
# compatibility and should be IGNORED
# 8 Detector mask (string)
# 9 Recording enabled (integer)
# 10 Number of events in physics streams (integer) 
runinfo = s.get_run_information(run_spec)
run_start_time_str=time.asctime(time.localtime(runinfo[run][4]))
run_stop_time_str=time.asctime(time.localtime(runinfo[run][5]))
run_duration_insec=int(runinfo[run][5])-int(runinfo[run][4])
number_of_lb=runinfo[run][6]

# beam lum info structure:
# 0 Max beam energy during run (float)
# 1 Stable beam flag enabled during run (boolean)
# 2 ATLAS ready flag enabled during run (boolean)
# 3 Total integrated luminosity (/nb) - online best estimate (float)
# 4 ATLAS ready luminosity (/nb) - online best estimate (float) 
beamluminfo=s.get_run_beamluminfo(run_spec)
maxenergy=float(beamluminfo[run][0]/1000.)
stablebeamflag=beamluminfo[run][1]
lumint=beamluminfo[run][3]
lumavg=(float(lumint)/run_duration_insec)*1.E27
lumATLAS=beamluminfo[run][4]
# endoflooptime structure:
# 0 the scheduled end of the Tier-0 calibration period in seconds
# past January 1 1970,
# 1 is whether the loop has actually ended or not (1=ended, 0=still open) 
endoflooptime=s.get_end_of_calibration_period(run_spec) 
endofloop_time_str=time.asctime(time.localtime(endoflooptime[run][0]))

number_of_events=DB.GetNumberOfCosmicCaloEvents(run)


print '''<b>More information about run: '''+str(run)+'''</b>'''
print "<br/>Run start: "+run_start_time_str
print "<br/>Run stop: "+run_stop_time_str
print "<br/>End of calib loop: "+endofloop_time_str
curenttime_src=time.asctime(time.localtime(time.time()))
print "<br/>Current time: "+curenttime_src

print "<br/>Number of LB: "+str(number_of_lb)
print "<br/>Number of Events: "+str(number_of_events)

if stablebeamflag:
    print "<br/>Max beam Energy: %.2f TeV" % (maxenergy)
    print "<br/>Luminosity (est.): "
    print "<br/>&nbsp;&nbsp;&nbsp;&nbsp;Average: %.1e cm-2s-1" % (lumavg)
    print "<br/>&nbsp;&nbsp;&nbsp;&nbsp;Total: "+str(int(float(lumint)/1000.+0.5))+" pb-1"
    print "<br/>&nbsp;&nbsp;&nbsp;&nbsp;ATLAS ready: "+str(int(float(lumATLAS)/1000.+0.5))+" pb-1"
else:
    print "<br/>No stable beam "

print'''</body></html>'''
