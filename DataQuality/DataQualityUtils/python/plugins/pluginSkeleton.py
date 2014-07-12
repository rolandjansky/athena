# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# +-----------------------------------------------------+
# | Vassilis D. Kazazakis, pluginSkeleton.py v1.0 	|
# | Sample plugin file for the QuerryDB.py application 	|
# +-----------------------------------------------------+

### PUBLIC INTERFACE OF PLUGIN ###

PluginName = "TestPlugin"

def usage():
	print PluginName, "options:"
	print """
	  (none)
	"""

def QuerryDB(argDict): #{start=<int>, iov=<int>}
	
	print PluginName, ": QuerryDB(",argDict,")"
	return []	# return empty list so that ExtractData does not do anything

def ParseArguments(arg):
	argDict = dict([])
	
	print PluginName, ": ParseArguments(",arg,")"

	return argDict
