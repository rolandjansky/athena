#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# GeneratePlot.py

import os, sys
from optparse import OptionParser

dataDir = 'HVHistoryDat'  # No '/' here!
pictDir = 'HVHistoryPic'

parser=OptionParser()
(options,args)=parser.parse_args()

if len(args) == 0:
	print 'No argument provided'
	print 'Should use: python GeneratePlot.py tripID instead'
else:
	ID = args[0]



# Clearing data and picture directories

try:
	if not os.path.exists(dataDir):
		print 'No directory with dat files found, exiting'
		sys.exit(-1)

	if not os.path.exists(pictDir):
		os.mkdir(pictDir)

	elif os.path.exists(pictDir + '/trip_' + ID + '.png'):
		print 'Plot already exists, exiting'
		sys.exit(0)
	
except Exception, e:
	print e

curDir = os.getcwd()
os.chdir(curDir + '/DataVisualizer')

classpath = os.getcwd()

# Verify if we need to compile it first
if not os.path.exists(classpath + '/DataVisualizer.class') :
	cmd = 'javac -cp ' + classpath + ' DataVisualizer.java'
	print 'Compiling: ' + cmd 
	os.system(cmd)

# Starting Java

cmd = 'java -cp ' + classpath + ' DataVisualizer ' + ID
print 'Starting: ' + cmd 
os.system(cmd)

print 'Done!'
os.chdir(curDir)