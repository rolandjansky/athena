#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

psname = "ConstantsEvolution.pdf"

def drawCorr(detector, labelList, drawErrors=False, drawLine=True, drawingDOF=-1):
    debug = False
    NullCanvas = initPsFile()
    Canvases = []
    Histos = []

    if (debug): print " -- drawCorr -- calling d_utils.drawAllCorr(",detector,")"
    tmpCan = d_utils.drawAllCorr(detector)
    Canvases.append(tmpCan)

    tmpCan, tmpGraph = d_utils.drawCorrVsHits(detector)
    Canvases.append(tmpCan)
    Histos.append(tmpGraph)

    if (True): print " -- drawCorr -- calling d_utils.drawCorrEvolution for drawingDOF=",drawingDOF
    d_utils.drawCorrEvolution(detector, labelList, drawErrors, drawLine, drawingDOF, 2 ) # add dof number -->plot only that dof
    #tmpCan = d_utils.OLD_drawCorrEvolution(detector, labelList, drawErrors, drawLine, drawingDOF ) # add dof number -->plot only that dof
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
    parser.add_option("--fileLabels", dest="inputFileLabels", help="Label to be given to each file. Defaults are: Iter0, Iter1, ...", default="")
    parser.add_option("--drawErrors", dest="inputDrawErrors", help="Constants evolution plot without errors", action="store_true", default=False)
    parser.add_option("--dof", dest="inputDof", help="User may select a given dof (0=Tx, 1=Ty, 2=Tz, 3=Rx, 4=Ry, 5=Rz, 6=Bx. -1 = from Tx to Rz (no Bx)", default=-1)
    parser.add_option("--SaveData", dest="inputSaveData", help="Define which of the input files is saved in the ntuple and txt file. Default the accumulative one", default = -1)
    parser.add_option("--noLine", dest="inputDrawLine", help="Draw a line conecting the points of the same structure", action="store_false", default=True)
    parser.add_option("--noEndCaps", dest="inputUseEndCaps", help="remove the end caps from the plots", action="store_false", default=True)
    parser.add_option("--noBarrels", dest="inputUseBarrels", help="remove the barrel parts from the plots", action="store_false", default=True)
    
    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    print " == optparsing == completed == "
    return config

##########################################################
#             Main code                                  #
##########################################################

if __name__ == '__main__':
    import sys
    
    print " == drawEvolution == start == "
    config = optParsing()

    import os   
    import imp
    from ROOT import *
    gROOT.SetBatch()

    fileList = config.inputLogFiles
    fileListGiven = True
    if (len(fileList)>0): 
        #print " == drawEvolution == fileList == ",fileList, " len= ", len(fileList)
        fileList = fileList.split()
        # print " == drawEvolution == filelist == ",fileList
    else:
        # print " == drawEvolution == NO fileList == "
        fileListGiven = False

    labelList = config.inputFileLabels
    labelsListGiven = True
    if (len(labelList)>0): 
        #print " == drawEvolution == labelList == ",labelList, " len= ", len(labelList)
        labelList = labelList.split()
        #print " == drawEvolution == LabelList == ",labelList
    else:
        #print " == drawEvolution == NO labelList == "
        labelsListGiven = False

    userSaveData = int(config.inputSaveData)
    userDOF = int(config.inputDof)
    
    global TestUseBarrel
    TestUseBarrel = config.inputUseBarrels

    ###############################    
    #import file and draw utilities    
    #
    f_utils = imp.load_source('readConstants', 'include/fileutils.py')
    d_utils = imp.load_source('rootSetup', 'include/drawutils.py')
    d_utils.rootSetup() 
    #s_utils = imp.load_source('', 'include/settings.py')
    
    if (len(sys.argv)==1 and not fileListGiven):
        detector = {}
        detector[0] = f_utils.readConstants("alignlogfile.txt")             
    else:
        if (not fileListGiven): 
            print " -- old method --" 
            fileList = sys.argv[1:]
        #print " fileList = ",fileList 
        detector = {}
        i = 0
        for file in fileList:
            if (labelsListGiven):                
                print " == drawEvolution == file:", i, " --> ", file, "[",labelList[i],"]"
            else:
                print " == drawEvolution == file:", i, " --> ", file

            detector[i] = f_utils.readConstants(file, config.inputUseBarrels, config.inputUseEndCaps)
            i = i+1

    #
    gStyle.SetTitleYOffset(2.4)
    # create a final detector set for storing the accumulated values
    inewdet = len(detector)
    detector[inewdet] = detector[inewdet-1]
    if (userSaveData < 0): userSaveData = inewdet 
    if (True): print " == drawEvolution == new detector[",inewdet,"] created for the accumulation"

    C,H = drawCorr(detector, labelList, config.inputDrawErrors, config.inputDrawLine, userDOF)

    if (True): print " == drawEvolution == saving detector[",userSaveData,"]"
    f_utils.writeCorr("alignment.txt",detector[userSaveData])
    f_utils.saveConstants(detector[userSaveData],"output.root")
    
    wait()
    print " == drawEvolution == completed == "



    
