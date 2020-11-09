# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def ResolveTag(folder, tag):
	theTag=tag
	if tag.upper() in ['NEXT', 'CURRENT']:
		import sys
		if '/afs/cern.ch/user/a/atlcond/utils/python/' not in sys.path:
			sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
		from AtlCoolBKLib import resolveAlias
		if tag.upper()=="NEXT":
			theTag=resolveAlias.getNext()
		else:
			theTag=resolveAlias.getCurrent()
		theTag=theTag.replace('*', 'ST')
		print (tag, "resolves to", theTag)
	folder_tag=folder.resolveTag(theTag)
	if folder_tag!=theTag:
		print (theTag, "resolves to", folder_tag)
	return folder_tag
	
