#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# +-----------------------------------------------------------------------------+
# | Vassilis D. Kazazakis, QuerryDB.py v1.0					|
# | Uses plugins to querry the cool database for the retrieval of ROOT files.	|
# |	Then, it extracts the requested histograms.				|
# +-----------------------------------------------------------------------------+

import sys
import getopt
import os

from PyCool import cool, coral
import DataQualityUtils.DQGetHistogramMod as mod

CurrentPlugin = "DefaultPlugin"
PluginDict = dict([])

def usage():
	print "Usage:", sys.argv[0], "[options] -- [plugin-options]"
	print """
	Options:
	  --about					print this help
	  -d/--data {histogram, directory[:flag]}	path name of histogram or directory in ROOT file (flag should only be used in case of directory)
	  -f/--flag flag				all histograms with specified flag will be retrieved

	  -o/--outputdir dir				directory to store output files (default value = "./")
	  -p/--pattern pattern				output filename pattern		(default value = "querryOutput")
			Final output form: outputdir/pattern<fileNum>.root

	  -u/--useplugin plugin				name of the plugin to use
	  -s/--plugindir directory			directory path in which to search for plugins
	  -l/--list					list all available plugins
	"""
	try:
		PluginDict[CurrentPlugin].usage()
	except KeyError:
		pass

def loadPlugins(pluginDirList):
	sys.path.extend(pluginDirList)

	pluginFileList = []
	for pluginDir in pluginDirList:
		pluginFileList.extend(os.listdir(pluginDir))

	for file in pluginFileList[:]:
		if not file.endswith(".py"):
			pluginFileList.remove(file)

	for file in pluginFileList:
		name = file[:len(file)-3]
		try:			
			plugin = __import__(name)
			PluginDict[plugin.PluginName] = plugin
			#print plugin.PluginName, "loaded."
		except:
			#print "Loading of", name, "failed."
			#raise
			pass
		
def listPlugins():	
	print "Loaded plugins:"
	for name, plugin in PluginDict.iteritems():
		print "\t  ", name

def ExtractData(fileList, dataList, pattern):
#	print "ExtractData(",fileList,",",dataList,",",pattern,")"

	fileNum = 0
	for file in fileList:
		print "Getting histograms from", file, "...",
		outputFile = pattern + str(fileNum) + ".root"
		if mod.DQGetHistogramMod(file, outputFile, dataList):	# DQGetHistogramMod returns True when
			fileNum = fileNum + 1						# new file created
			print "OK"
		else:
			print "Not found"

	return fileNum

if __name__ == "__main__":
	try:
		optlist, unparsed = getopt.getopt(sys.argv[1:],
					"d:o:p:s:u:lf:",
					["data=", "outputdir=", "pattern=", "useplugin=", "plugindir=", "about", "list", "flag="])
	except getopt.GetoptError:
		usage()
		sys.exit(-1)

	dataList = []
	outputdir = "./"
	pattern = "querryOutput" #.root
	pluginDirList = [ os.path.realpath(os.path.dirname(sys.argv[0]))+"/plugins" ]
	
	for opt, arg in optlist:
		if opt in ("-o", "--outputdir"):
			outputdir = arg
		elif opt in ("-p", "--pattern"):
			pattern = arg
		elif opt in ("-d", "--data"):
			dataList.append(arg)
		elif opt in ("-f", "--flag"):
			dataList.append(":"+arg)
		elif opt in ("-s", "--plugindir"):
			pluginDirList.append(arg)
		elif opt in ("-u", "--useplugin"):
			CurrentPlugin = arg
		elif opt in ("--about"):
			usage()
			sys.exit(0)
		elif opt in ("-l", "--list"):
			loadPlugins(pluginDirList)
			listPlugins()
			sys.exit(0)

	loadPlugins(pluginDirList)

	if len(sys.argv[1:]) == 0:
		usage()
		sys.exit(0)

	if len(dataList) == 0:
		print >>sys.stderr, "WARNING: No histogram/directory name given" 

	if pattern == "":
		print >>sys.stderr, "ERROR: Output filename pattern must be non-empty."
		sys.exit(-1)

	if pattern.count('/') > 0:
		print >>sys.stderr, "ERROR: Output filename pattern must not contain '/' characters"
		sys.exit(-1)

	if not os.access(outputdir, os.F_OK):
		print >>sys.stderr, "ERROR: Output directory", outputdir, "does not exist."
		sys.exit(-1)
	if not os.access(outputdir, os.R_OK):
		print >>sys.stderr, "ERROR: Output directory", outputdir, "is not readable."
		sys.exit(-1)

	if not outputdir.endswith('/'):
		outputdir = outputdir + '/'
	pattern = outputdir + pattern

	try:
		pluginArgs = PluginDict[CurrentPlugin].ParseArguments(unparsed)
		querryList = PluginDict[CurrentPlugin].QuerryDB(pluginArgs)
		ExtractData(querryList, dataList, pattern)
	except KeyError:
		print >>sys.stderr, "ERROR:", CurrentPlugin, "not found."
