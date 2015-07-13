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

if __name__ == "__main__":
    
    s = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch')
    run = sys.argv[1]
    print run
    run_spec = {'stream': 'physics_IDCosmic', 'source': 'reproc', 'low_run': 92226, }
    test_number = s.get_latest_run(run_spec)
    
    print test_number
