# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Josiah Couch
#20 June 2014
#
#Edited on 23 June: Minor edits, replace sys.popen with glob.glob and filter(os.path.isdir ....), change recursion to simple loop

import os
import sys
import glob

#check to see if a directory has any subdirectories
def hasSubDir(Dir):
	dirList = filter(os.path.isdir, (os.path.join(Dir,f) for f in os.listdir(Dir)))
	if len(dirList)>0:
		return True
	return False

def findInputs(Dir,args):

	#Create pattern to search for
	searchStr = "*"
	for i in range(0,len(args)):
		searchStr = searchStr + args[i] +"*"

	#Search for that pattern, return the result
	return searchFor(Dir,searchStr)

def searchFor(Dir,searchStr):

	ret = None

	#Format dir
	if Dir[len(Dir)-1]=="/":
		Dir = Dir[:-1]

	#Search for pattern searchStr in directory dir, save results to a list of filenames
	fileList = glob.glob(Dir+"/"+searchStr)

	#If there is a result, return it
	if len(fileList)>0:
		return [fileList[0]]

	#Search subdirectories one by one, if you find something return it, if you don't add that subdirectory's subdirectory to the list and keep going until there are no more entries
	i = 0
	dirList = filter(os.path.isdir, (os.path.join(Dir,f) for f in os.listdir(Dir)))
	while len(dirList)>0:
		fileList = glob.glob(dirList[0]+"/"+searchStr)
		if len(fileList)>0:
			return [fileList[0]]
		dirList = dirList+filter(os.path.isdir, (os.path.join(dirList[0],f) for f in os.listdir(dirList[0])))
		dirList = dirList[1:]

	#If nothing has been found up to now, return file not found
	return []

def main():
	print findInputs(sys.argv[1],sys.argv[2:])
	return 0
	

if __name__ == '__main__':
	main()
	
