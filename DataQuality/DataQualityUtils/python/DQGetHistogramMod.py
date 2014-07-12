# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# +---------------------------------------------+
# | Vassilis D. Kazazakis, GetHistogram.py v1.0	|
# | Extracts histograms from ROOT files		|
# |  Interface used by QuerryDB.py application	|
# +---------------------------------------------+

import sys
import getopt
import os

import ROOT
startDir = os.getcwd()
from ROOT import TFile, TBrowser, TKey ## This breaks up argument parsing and current directory
os.chdir(startDir)

def getFullName(dir):
        motherDir = dir.GetMotherDir()
        if not motherDir:
                return ""       # it means that dir = TFile and we don't want the name of the file in the full name
        else:
                return getFullName(motherDir) + dir.GetName() + "/"


# getMetaDict(TDirectory dir)
def getMetadataDict(dir):
	retDict = dict([])

	metaKey = dir.FindKey("metadata")
	if metaKey:
		retDict[getFullName(dir)] = metaKey.ReadObj()

	iter = dir.GetListOfKeys().MakeIterator()
	keyNext = iter.Next()
	while keyNext:
		if keyNext.GetClassName() == "TDirectory":
			retDict.update(getMetadataDict(keyNext.ReadObj()));
		keyNext = iter.Next()

	return retDict

def createBrowser():
	browserWin = TBrowser()
	wait = raw_input("Enter any 11-digit prime number to continue:")

def GetObjectList(tFile, path):

	dirList = path.split("/")
	ret = [tFile]
	for dir in dirList:
		if dir == '':
			continue
		key = ret[0].GetKey(dir)
		if not key:
			return []
		ret[0] = key.ReadObj()

	# if path ends with a '/' that means we must return a list of
	# all histograms (class name begins with TH) in the last directory
	if path.endswith('/'):
		iter = ret[0].GetListOfKeys().MakeIterator()
		ret = []
		keyNext = iter.Next()
		while keyNext:
			if keyNext.GetClassName()[:2] == "TH":
				objNext = keyNext.ReadObj()
				ret.append(objNext)
			keyNext = iter.Next()
	return ret

def SetObjectDir(tFile, path, obj, clearDict = False, dirDict = dict([])):
	if clearDict:
		for key,elem in dict(dirDict).iteritems():
			del dirDict[key]

	if not obj:
		return


	dirList = path.split("/")
	if not path.endswith('/'):
		dirList.pop()
	tmpf = tFile
	partialPath = ''

	for dir in dirList:
		if dir =='':
			continue
		partialPath += dir + '/'
		try:
			tmpf = dirDict[partialPath]
		except KeyError:
#			print "Creating directory '" + dir + "' for directory list:", dirList
			tmpf = tmpf.mkdir(dir)
			dirDict[partialPath] = tmpf
	obj.SetDirectory(tmpf)

# GetHistogram(string:ROOT_InputFileName, string:ROOT_OutputFileName, list:NameOfHistogramsToExtract)
def GetHistogram(inp, out, nameOptionList):

	# clear the new dir dictionary (necessary if GetHistogram is to be used more than once)
	SetObjectDir("", "", None, True)	# took me 30 mins to find this f***ing bug...

	changeCount = 0
	inpFile = TFile(inp)

	################### check for shift/expert/etc flags and construct the approprate list of histograms to get ####################
	nameList = []
	metaDict = getMetadataDict(inpFile)
	for option in nameOptionList:
		optionList = option.split(":")			# optionList[0] = histogram name, optionList[1] = flag
		if len(optionList) == 1:
			if not optionList[0] in nameList:
				nameList.append(optionList[0])
		else:	# we have a flag
			if (not optionList[0].endswith("/")) and (not optionList[0] == ""):		# flags go only with directories (i.e. name ends with "/")
				print >>sys.stderr, "Warning: Directory names should always end with \"/\" OR flag defined for histogram."
			if optionList[0] == "":					# we have to get all flagged histograms
				for key in metaDict.keys():
					for event in metaDict[key]:
						if event.LevelOfDetail == optionList[1]:
							histName = key+event.Name
							if not histName in nameList:
								changeCount += 1
								nameList.append(histName)
				if changeCount == 0:
					print >>sys.stderr, "Warning: No histogram flagged", optionList[1], "found."

			else:							# we have to get flagged histograms only from the specified directory
				try:
					for event in metaDict[optionList[0]]:
						if event.LevelOfDetail == optionList[1]:
							histName = optionList[0]+event.Name
							if not histName in nameList:
								changeCount += 1
								nameList.append(histName)
					if changeCount == 0:
						print >>sys.stderr, "Warning: No histogram flagged", optionList[1], "found in", optionList[0],"."
				except KeyError:
					print >>sys.stderr, "Warning:", optionList[0], "directory/metadata object does not exist."
	##################################################################################################################################

	outFile = TFile(out, "RECREATE")
	changeCount = 0
	for path in nameList:
		objList = GetObjectList(inpFile, path)
		for obj in objList:
		#	print "Setting path for '" + obj.GetName() + "' to '" + path + "'"
			changeCount = changeCount + 1
			SetObjectDir(outFile, path, obj)
	outFile.Write()
	outFile.Flush()
	inpFile.Close()
	outFile.Close()
	if changeCount == 0:
		os.remove(out)
		return False
	else:
		return True

