# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys

def replacePattern(dirName, old, new):

	for f in os.listdir(dirName):
		
		if "svn" in f: continue
		if f.endswith("~"): continue
		if "make" in f : continue
		if f.startswith("i686") or f.startswith("x86"): continue
		if f.startswith("cmt") or f.startswith("genConf"): continue

		nextName=dirName+"/"+f
		if os.path.isdir(nextName) and not f.startswith("."):
			newDir=dirName+"/"+f
			replacePattern(newDir,old,new)
		else:
			if not f.endswith(".cxx") and not f.endswith(".h"): continue
			fileName=dirName+"/"+f

			f=open(fileName,"r")
			text=f.read()
			f.close()

			if old in text:
				print fileName

				textNew=text.replace(old,new)
				f=open(fileName,"w")
				f.write(textNew)
				f.close()




	return




if __name__ == "__main__":


	patternOLD = sys.argv[1]	
	patternNEW = sys.argv[2]

	dirName = sys.argv[3]

	replacePattern(dirName,patternOLD,patternNEW)
