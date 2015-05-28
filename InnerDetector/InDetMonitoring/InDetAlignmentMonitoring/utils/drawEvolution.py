#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

psname = "ConstantsEvolution.pdf"

def drawCorr(detector, labelList, drawErrors=False):
    NullCanvas = initPsFile()
    Canvases = []
    Histos = []
    
    tmpCan = d_utils.drawAllCorr(detector)
    Canvases.append(tmpCan)

    tmpCan, tmpGraph = d_utils.drawCorrVsHits(detector)
    Canvases.append(tmpCan)
    Histos.append(tmpGraph)
    
    tmpCan = d_utils.drawCorrEvolution(detector, labelList, drawErrors)
    Canvases.append(tmpCan)
    
    if len(detector[0].ReturnPixelBarrelModules())>0:
        tmpCan, tmpGraph = d_utils.drawPixBarrelCorrDistributions(detector)
        Canvases.append(tmpCan)
        Histos.append(tmpGraph)
        
    if len(detector[0].ReturnSctBarrelModules()):
        tmpCan, tmpGraph = d_utils.drawSctBarrelCorrDistributions(detector)
        Canvases.append(tmpCan)
        Histos.append(tmpGraph)
        
#   
#   if level == 3:
#       #Canvases.append(drawStaves(detector))
#       tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,None)
#       Canvases.append(tmpCan)
#       Histos.append(tmpGraph)
#       #tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,0)
#       #Canvases.append(tmpCan)
#       #Histos.append(tmpGraph)
#       #if detector[0].HasEndcaps():
#       #   tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,-1)
#       #   Canvases.append(tmpCan)
#       #   Histos.append(tmpGraph)
#       #   tmpCan, tmpGraph = drawL3CorrVsHits(detector,1,1)
#       #   Canvases.append(tmpCan)
#       #   Histos.append(tmpGraph)
#   
#       #tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,None)
#       #Canvases.append(tmpCan)
#       #Histos.append(tmpGraph)
#       tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,0)
#       Canvases.append(tmpCan)
#       Histos.append(tmpGraph)
#       #if detector[0].HasEndcaps():
#       #   tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,-1)
#       #   Canvases.append(tmpCan)
#       #   Histos.append(tmpGraph)
#       #   tmpCan, tmpGraph = drawL3CorrVsHits(detector,2,1)
#       #   Canvases.append(tmpCan)
#       #   Histos.append(tmpGraph)
    
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

##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
    print " == optparsing == start == "
    from optparse import OptionParser
    parser = OptionParser()
    #parser.add_option("--TrackSegments", dest="inputTrackSegments", help="Do track segment matching plots", action="store_true",default=False)
    parser.add_option("--inputFiles", dest="inputLogFiles", help="In the case you want to use a specific set of initial constants write here the absolute path to the alignlogfile", default="")
    parser.add_option("--fileLabels", dest="inputFileLabels", help="label to be given to each file. Defaults are: Iter0, Iter1, ...", default="")
    parser.add_option("--drawErrors", dest="inputDrawErrors", help="constants evolution plot without errors", default=False)
    
    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    print " == optparsing == completed == "
    return config

##########################################################
#             Main code                                  #
##########################################################

if __name__ == '__main__':
    import sys
    import os   
    import imp
    from ROOT import *
    gROOT.SetBatch()

    print " == drawEvolution == start == "
    config = optParsing()
    fileList = config.inputLogFiles
    fileListGiven = True
    if (len(fileList)>0): 
        #print " == drawEvolution == fileList == ",fileList, " len= ", len(fileList)
        fileList = fileList.split()
        print " == drawEvolution == filelist == ",fileList
    else:
        print " == drawEvolution == NO fileList == "
        fileListGiven = False

    labelList = config.inputFileLabels
    fileLabelsGiven = True
    if (len(labelList)>0): 
        #print " == drawEvolution == labelList == ",labelList, " len= ", len(labelList)
        labelList = labelList.split()
        print " == drawEvolution == LabelList == ",labelList
    else:
        print " == drawEvolution == NO labelList == "
        labelListGiven = False
    
        
    #import file and draw utilities    
    f_utils = imp.load_source('readConstants', 'include/fileutils.py')
    d_utils = imp.load_source('rootSetup', 'include/drawutils.py')
    d_utils.rootSetup() 
    
    if (len(sys.argv)==1 and not fileListGiven):
        detector = {}
        detector[0] = f_utils.readConstants("alignlogfile.txt")             
    else:
        if (not fileListGiven): 
            print " -- old method --" 
            fileList = sys.argv[1:]
        print " fileList = ",fileList 
        detector = {}
        i = 0
        for file in fileList:
            print " == drawEvolution == file:", i, " --> ", file
            detector[i] = f_utils.readConstants(file)
            i = i+1
    f_utils.writeCorr("alignment.txt",detector[0])
    C,H = drawCorr(detector, labelList, config.inputDrawErrors)

    f_utils.saveConstants(detector[0],"output.root")
    
    wait()
    print " == drawEvolution == completed == "



    
