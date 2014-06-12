#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# GenerateAllPlots.py

import os, sys

dataDir = 'HVHistoryDat'  # No '/' here!
pictDir = 'HVHistoryPic'

# Clearing data and picture directories

try:
	if not os.path.exists(dataDir):
		print 'No directory with dat files found, exiting'
		sys.exit()

	if not os.path.exists(pictDir):
		os.mkdir(pictDir)
	else:
		for the_file in os.listdir(pictDir):
	   		os.unlink(os.path.join(pictDir, the_file))
		
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

cmd = 'java -cp ' + classpath + ' DataVisualizer '
print 'Starting: ' + cmd 
os.system(cmd)

print 'Done!'
os.chdir(curDir)