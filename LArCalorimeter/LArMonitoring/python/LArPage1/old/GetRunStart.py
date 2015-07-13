#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, getopt
import os
from os import open
import string
import time
import cgitb; cgitb.enable()
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

########################################################################
f = cgi.FieldStorage()

########################################################################
def GetRunNumber():
    run = 0
    for i in f.keys():
      print i
      run = f[i].value

    return str(run)

print GetRunNumber()

server = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch') 
run_spec = {'stream': 'physics_IDCosmic', 'source': 'reproc', 'low_run': int(GetRunNumber())-1, 'high_run' : int(GetRunNumber())+1}
test_map = server.get_run_information(run_spec)

print test_map
