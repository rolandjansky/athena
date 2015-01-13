#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

psname = "ConstantsEvolution.pdf"

def drawCorr(detector):
	NullCanvas = initPsFile()
	Canvases = []
	Histos = []
	
	tmpCan = d_utils.drawAllCorr(detector)
	Canvases.append(tmpCan)

	tmpCan, tmpGraph = d_utils.drawCorrVsHits(detector)
	Canvases.append(tmpCan)
	Histos.append(tmpGraph)
	
	tmpCan = d_utils.drawCorrEvolution(detector)
	Canvases.append(tmpCan)
    
	if len(detector[0].ReturnPixelBarrelModules())>0:
		tmpCan, tmpGraph = d_utils.drawPixBarrelCorrDistributions(detector)
		Canvases.append(tmpCan)
		Histos.append(tmpGraph)
		
	if len(detector[0].ReturnSctBarrelModules()):
		tmpCan, tmpGraph = d_utils.drawSctBarrelCorrDistributions(detector)
		Canvases.append(tmpCan)
		Histos.append(tmpGraph)
		
#	level = detector[0].Level()
#	
#	if level != 3:
#		tmpCan, tmpGraph = drawCor
#		Canvases.append(tmpCan)
#		Histos.append(tmpGraph)
#	
#	if level == 3:
#		#Canvases.append(drawStaves(detector))
#		tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,None)
#		Canvases.append(tmpCan)
#		Histos.append(tmpGraph)
#		#tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,0)
#		#Canvases.append(tmpCan)
#		#Histos.append(tmpGraph)
#		#if detector[0].HasEndcaps():
#		#	tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,-1)
#		#	Canvases.append(tmpCan)
#		#	Histos.append(tmpGraph)
#		#	tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,1)
#		#	Canvases.append(tmpCan)
#		#	Histos.append(tmpGraph)
#	
#		#tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,None)
#		#Canvases.append(tmpCan)
#		#Histos.append(tmpGraph)
#		tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,0)
#		Canvases.append(tmpCan)
#		Histos.append(tmpGraph)
#		#if detector[0].HasEndcaps():
#		#	tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,-1)
#		#	Canvases.append(tmpCan)
#		#	Histos.append(tmpGraph)
#		#	tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,1)
#		#	Canvases.append(tmpCan)
#		#	Histos.append(tmpGraph)
	
	for canvas in Canvases:
		canvas.Print(psname)


	closePsFile(NullCanvas)
	
	return Canvases, Histos
	
def initPsFile():
	NullCanvas = TCanvas()
	NullCanvas.Print(psname+"[")
	return NullCanvas
	
def closePsFile(NullCanvas):
	NullCanvas.Print(psname+"]")	
	
def wait():
	rep = ''
	while not rep in [ 'q', 'Q' ]:
		rep = raw_input( 'enter "q" to quit: ' )
		if 1 < len(rep):
			rep = rep[0]

if __name__ == '__main__':
	import sys
	import os	
	import imp
	from ROOT import *
	f_utils = imp.load_source('readConstants', 'include/fileutils.py')
	#from drawutils import *
	d_utils = imp.load_source('rootSetup', 'include/drawutils.py')

	d_utils.rootSetup()	
	if len(sys.argv) == 1:
		detector = {}
		detector[0] = f_utils.readConstants("alignlogfile.txt")				
	else:
		filelist = sys.argv[1:]
		detector = {}
		i = 0
		for file in filelist:
			detector[i] = f_utils.readConstants(file)
			i = i+1
	#writeCorr("alignment.txt",detector)
	C,H = drawCorr(detector)
	
	wait()

	


	
