#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

def getWikiFile():
	WikiFile = "svn+ssh://svn.cern.ch/reps/atlasphys/Physics/Top/Software/Production/TopMC12twiki/status/"
	com = "svn co "+WikiFile
	print com
	os.system(com)


def producePDFfile(fileout_name='PDF-MC12-8TeV.data'):
	filein = open('status/output/PDFInfo/generators_and_PDFs_info.csv', 'r')
	fileout = open(fileout_name, 'w')
	
	dico = {}
	
	for line in filein.readlines():
		l = line.split(' ')
		DSID = l[0][:-1]
		PDF= l[-1][:-1].replace(' ', '') # temparory variable
		
		#print DSID, PDF
		
		# format: (PDFid;PDFname)
		if PDF[0]!="(" or PDF[-1]!=")" or PDF.find(";")==-1:
			
			# for the Z', do it by hand
			if DSID>="110899" and DSID<="110910":
				PDF = "21000"
			else:
				print "wrong format for",line[:-1]
				print "-->",PDF,"===="
				print "\n"
				continue
				
		else : # good format
			PDF = PDF.replace('(', '').replace(')', '')
			PDF = PDF.split(";")[0]
		#print DSID, PDF
		
		if dico.has_key(DSID):
			if dico[DSID] != PDF:
				print DSID,"already exists"
				print "\told:",dico[DSID]
				print "\tnew:",PDF
		else :
			dico[DSID]=PDF
			out = DSID+"\t"+PDF
			fileout.write(out+"\n")
	
	for sample in ["110906", "110909"]:
		if not dico.has_key(sample):
			print "adding missing sample",sample,"by hand"
			out = sample+"\t21000"
			fileout.write(out+"\n")
		
	for sample in ["203307", "203308", "203309", "203310", "203311", "203312", "203313", "203314", "203315", "203316", "203317", "203318"]:
		if not dico.has_key(sample):
			print "adding missing sample",sample,"by hand"
			out = sample+"\t10042"
			fileout.write(out+"\n")
		
	filein.close()
	fileout.close()
	
getWikiFile()
producePDFfile()
