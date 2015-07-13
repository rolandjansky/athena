#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
#!/bin/env python
import sys,os,getopt,fileinput
import subprocess as sp
#import MyCOOLlib
import test
import xmlrpclib
global server
passfile = open("/afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/passfile.txt")
passwd = passfile.read().strip(); passfile.close()
server = xmlrpclib.ServerProxy('https://%s@atlasdqm.cern.ch'%(passwd))

#To work around the damn DBRelease. I *hate* DBreleases!!!!
#os.environ["CORAL_DBLOOKUP_PATH"]="/afs/cern.ch/atlas/software/builds/AtlasCore/16.6.3/InstallArea/XML/AtlasAuthentication"

from threading import Thread
from time import *
from threading import Thread
import os

def TestImport():
    print 'TestImport works'

print '''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LADIeS Iframe</title>
<meta name="author" content="Olivier Davignon [davignon@cern.ch]"/>
<link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
<link rel="Stylesheet" href="styles.css" type="text/css" />
<base target="_blank">'''+TestImport()+'''
</head>
<body onload="uncheck()"></body></html>'''


## print '''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
## <html xmlns="http://www.w3.org/1999/xhtml">
## <head>
## <title>LADIeS Iframe</title>
## <meta name="author" content="Olivier Davignon [davignon@cern.ch]"/>
## <link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
## <link rel="Stylesheet" href="styles.css" type="text/css" />
## <base target="_blank">test
## </head>
## <body onload="uncheck()"></body></html>'''
