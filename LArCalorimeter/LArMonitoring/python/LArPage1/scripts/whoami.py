#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, getopt
import os
from os import open
import string
import time
import urllib2,sys
import xmlrpclib
import subprocess as sp

import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import os,fileinput
## import test
import xmlrpclib
global server
from threading import Thread
## from time import *
## import os

server = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch') 

cmd = "whoami"
cout = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
output = cout.communicate()[0]

cmd2 = "ls /afs/cern.ch/user/l/larmon/public/LArPage1/scripts/DBs/config.db"
cout2 = sp.Popen(cmd2,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
output2 = cout2.communicate()[0]

#cmd3 = "ls /afs/cern.ch/user/l/larcomm/w0/digitMon/DB/operations.db"
cmd3 = "ls /afs/cern.ch/user/l/larcomm/w0/digitMon/DB/operations.db"
cout3 = sp.Popen(cmd3,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
output3 = cout3.communicate()[0]

print '''
 <html>
 <body>
 '''+str(output)+'''<br />pinging larmon: '''+str(output2)+'''<br />pinging larcomm: '''+str(output3)+'''
 </body>
 </html>'''
