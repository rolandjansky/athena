#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

import sys
sys.argv = [sys.argv[0], '-b']

from ROOT import TH1, TGraphErrors, TGraphAsymmErrors, Double, TCanvas, TH1F
#import numpy
from math import fabs
from array import array

def formatPlotNoStat (plot) :
    #plot.SetTitleFont(42)
    plot.GetXaxis().SetLabelFont(42)
    plot.GetXaxis().SetTitleFont(42)
    plot.GetYaxis().SetLabelFont(42)
    plot.GetYaxis().SetTitleFont(42)
    #plot.ResetStats()
    #plot.SetStats(False)

def formatJetPlot (plot, i) :
#    colorlist = [214,100,617,417,221,869,603,635,807,413,616,215]
    #colorlist = [413,603,633,807,100,869,2,616,215,214,617,417]
    #markerlist = [20,24,22,26,23,21,27,25,29,28,5]
    colorlist = [214,100,221,417,869,617,603,635,807,413,616,215]
    markerlist = [20,25,22,26,21,20,24,27,29,28,20,30]


    plot.SetMarkerStyle(markerlist[i])
    plot.SetMarkerColor(colorlist[i])
    plot.SetMarkerSize(1)
    plot.SetLineColor(colorlist[i])

def rescaleJetPlot (plot) :

    #loop on all data points
    nPoints = plot.GetN()
#    print "old:",plot.GetName(), nPoints
    #create another TGraph
    newPlot = TGraphErrors()
    
    for i in xrange(0,nPoints) :

        dataPointX = Double(0)
        dataPointY = Double(0)
        error = Double(0)
        plot.GetPoint(i,dataPointX,dataPointY)
        #if numpy.isnan(dataPointY) : dataPointY = 0.000001
        if dataPointY < 0.000001 : dataPointY = 0.000001
        errorX = plot.GetErrorX(i)
        errorY = plot.GetErrorY(i)
        newPlot.SetPoint(i, dataPointX/1000., dataPointY)
        newPlot.SetPointError(i, errorX/1000., errorY)        

#    print "new:", newPlot.GetName(), newPlot.GetN()

    return  newPlot        
    
    ## plot.SetTitle(jetCollectionTitle)
    ## plot.SetName(jetCollectionName)



def fixPlots (middleGraph) :

    thisBandsGraph = TGraphAsymmErrors()
    thisBandsGraph.SetName(middleGraph.GetName())
    thisBandsGraph.SetMarkerStyle(middleGraph.GetMarkerStyle());
    thisBandsGraph.SetMarkerSize(middleGraph.GetMarkerSize());
    thisBandsGraph.SetMarkerColor(middleGraph.GetMarkerColor());
    thisBandsGraph.SetLineColor(middleGraph.GetLineColor());  
    
    for iPoint in xrange(0, middleGraph.GetN()) :
        # Retrieve middle graph for (x,y) coordinates
        dataPointX = Double(0)
        dataPointY = Double(0)
        dataErrorX = Double(0)

        middleGraph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = middleGraph.GetErrorX(iPoint)
        dataPointY = fabs(dataPointY-1)
    
        if(dataPointY != 0 and iPoint < middleGraph.GetN()):
            thisBandsGraph.SetPoint(iPoint, dataPointX, dataPointY)
            thisBandsGraph.SetPointError(iPoint, dataErrorX, dataErrorX, 0, 0)        

    return thisBandsGraph

def fixPlots_withErrors (middleGraph) :

    thisBandsGraph = TGraphAsymmErrors()
    thisBandsGraph.SetName(middleGraph.GetName())
    thisBandsGraph.SetMarkerStyle(middleGraph.GetMarkerStyle());
    thisBandsGraph.SetMarkerSize(middleGraph.GetMarkerSize());
    thisBandsGraph.SetMarkerColor(middleGraph.GetMarkerColor());
    thisBandsGraph.SetLineColor(middleGraph.GetLineColor());  
    
    for iPoint in xrange(0, middleGraph.GetN()) :
        # Retrieve middle graph for (x,y) coordinates
        dataPointX = Double(0)
        dataPointY = Double(0)
        dataErrorX = Double(0)

        middleGraph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = middleGraph.GetErrorX(iPoint)
        dataErrorY = middleGraph.GetErrorY(iPoint)
        
        dataPointY = fabs(dataPointY-1)
    
        if(iPoint < middleGraph.GetN()):
        #if(dataPointY != 0 and iPoint < middleGraph.GetN()):
          
            print "fixPlots:" , dataPointX
  
            thisBandsGraph.SetPoint(iPoint, dataPointX, dataPointY)
            thisBandsGraph.SetPointError(iPoint, dataErrorX, dataErrorX, dataErrorY, dataErrorY)        
            
            #if (dataPointY == 0) :
              
                #thisBandsGraph.SetPoint(iPoint, dataPointX, dataPointY)
                #thisBandsGraph.SetPointError(iPoint, dataErrorX, dataErrorX, dataErrorY, dataErrorY)                     
            
    return thisBandsGraph

def removeLargeErrorPoints(nomGraph, graphs) :

    newGraphsIntermediate = []
    newGraphsFinal = []
    badPoints = []

    for i, graph in enumerate(graphs+[nomGraph]):
        #print graph.GetName()

        for iPoint in xrange(0, graph.GetN()) :

            dataPointX = Double(0)
            dataPointY = Double(0)

            graph.GetPoint(iPoint,dataPointX,dataPointY)
            dataErrorX = graph.GetErrorX(iPoint)
            dataErrorY = graph.GetErrorY(iPoint)
            print "graph no:", i
            print graph.GetName()

            print "removeLargeErrorPoints:" , dataPointX, dataPointY, dataErrorX, dataErrorY

            if dataErrorY > 0.011 or dataErrorY < 0.00000001 :
                if not (iPoint in badPoints) : badPoints.append(iPoint)

    #print badPoints

    for i, graph in enumerate(graphs):
        
        newGraph = graph.Clone()
        newGraphsFinal.append(newGraph)
        
        for iPoint in xrange(0, graph.GetN()) :

            dataPointX = Double(0)
            dataPointY = Double(0)

            dataErrorX = graph.GetErrorX(iPoint)
            dataErrorY = graph.GetErrorY(iPoint)

            graph.GetPoint(iPoint,dataPointX,dataPointY)

            if not (iPoint in badPoints) : 
                newGraph.SetPoint(iPoint, dataPointX, dataPointY)
                newGraph.SetPointError(iPoint, dataErrorX, dataErrorY)

            else : 
                newGraph.SetPoint(iPoint, dataPointX, 1)
                newGraph.SetPointError(iPoint, dataErrorX, 0)


    nomGraphFinal = nomGraph.Clone()
        
    for iPoint in xrange(0, nomGraph.GetN()) :

        dataPointX = Double(0)
        dataPointY = Double(0)
        nomGraph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = nomGraph.GetErrorX(iPoint)
        dataErrorY = nomGraph.GetErrorY(iPoint)

        if not (iPoint in badPoints) : 
            nomGraphFinal.SetPoint(iPoint, dataPointX, dataPointY)
            nomGraphFinal.SetPointError(iPoint, dataErrorX, dataErrorY)

        else : 
            nomGraphFinal.SetPoint(iPoint, dataPointX, 1)
            nomGraphFinal.SetPointError(iPoint, dataErrorX, 0)

    c = TCanvas()

    #nomGraphFinal.Draw("AP")
    #for graph in newGraphsFinal :
        #newgraph.FixPlot(graph)
        #newgraph.Draw("Psame")

    return nomGraphFinal, newGraphsFinal

        #dataPointY = fabs(dataPointY-1)

        #if(dataPointY != 0 and iPoint < graph.GetN()):
            #thisBandsGraph.SetPoint(iPoint, dataPointX, dataPointY)
            #thisBandsGraph.SetPointError(iPoint, dataErrorX, dataErrorX, 0, 0)        

    #return thisBandsGraph

def removeLargeErrorPoints_noFirstBin(nomGraph, graphs) :

    newGraphsIntermediate = []
    newGraphsFinal = []
    badPoints = []

    for i, graph in enumerate(graphs+[nomGraph]):
        #print graph.GetName()

        for iPoint in xrange(0, graph.GetN()) :

            #no check if it's the first point (it sucks anyways)
            if iPoint == 0 : continue
            dataPointX = Double(0)
            dataPointY = Double(0)

            graph.GetPoint(iPoint,dataPointX,dataPointY)
            dataErrorX = graph.GetErrorX(iPoint)
            dataErrorY = graph.GetErrorY(iPoint)
            print graph.GetName()

            if dataErrorY > 0.011 or dataErrorY < 0.00000001 :
                if not (iPoint in badPoints) : badPoints.append(iPoint)

    #print badPoints

    for i, graph in enumerate(graphs):
        
        newGraph = graph.Clone()
        newGraphsFinal.append(newGraph)
        
        for iPoint in xrange(0, graph.GetN()) :

            dataPointX = Double(0)
            dataPointY = Double(0)

            dataErrorX = graph.GetErrorX(iPoint)
            dataErrorY = graph.GetErrorY(iPoint)

            graph.GetPoint(iPoint,dataPointX,dataPointY)

            if not (iPoint in badPoints) : 
                newGraph.SetPoint(iPoint, dataPointX, dataPointY)
                newGraph.SetPointError(iPoint, dataErrorX, dataErrorY)

            else : 
                newGraph.SetPoint(iPoint, dataPointX, 1)
                newGraph.SetPointError(iPoint, dataErrorX, 0)


    nomGraphFinal = nomGraph.Clone()
        
    for iPoint in xrange(0, nomGraph.GetN()) :

        dataPointX = Double(0)
        dataPointY = Double(0)
        nomGraph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = nomGraph.GetErrorX(iPoint)
        dataErrorY = nomGraph.GetErrorY(iPoint)

        if not (iPoint in badPoints) : 
            nomGraphFinal.SetPoint(iPoint, dataPointX, dataPointY)
            nomGraphFinal.SetPointError(iPoint, dataErrorX, dataErrorY)

        else : 
            nomGraphFinal.SetPoint(iPoint, dataPointX, 1)
            nomGraphFinal.SetPointError(iPoint, dataErrorX, 0)


    #nomGraphFinal.Draw("AP")
    #for graph in newGraphsFinal :
        #newgraph.FixPlot(graph)
        #newgraph.Draw("Psame")

    return nomGraphFinal, newGraphsFinal

        #dataPointY = fabs(dataPointY-1)

        #if(dataPointY != 0 and iPoint < graph.GetN()):
            #thisBandsGraph.SetPoint(iPoint, dataPointX, dataPointY)
            #thisBandsGraph.SetPointError(iPoint, dataErrorX, dataErrorX, 0, 0)        

    #return thisBandsGraph

def removeLargeErrorPoints_noCheck(nomGraph, graphs) :

    newGraphsIntermediate = []
    newGraphsFinal = []
    badPoints = []

    for i, graph in enumerate(graphs+[nomGraph]):
        #print graph.GetName()

        for iPoint in xrange(0, graph.GetN()) :

            dataPointX = Double(0)
            dataPointY = Double(0)

            graph.GetPoint(iPoint,dataPointX,dataPointY)
            dataErrorX = graph.GetErrorX(iPoint)
            dataErrorY = graph.GetErrorY(iPoint)

            if dataErrorY > 5 or dataErrorY < 0.00000001 :
                if not (iPoint in badPoints) : badPoints.append(iPoint)

    #print badPoints

    for i, graph in enumerate(graphs):
        
        newGraph = graph.Clone()
        newGraphsFinal.append(newGraph)
        
        for iPoint in xrange(0, graph.GetN()) :

            dataPointX = Double(0)
            dataPointY = Double(0)

            dataErrorX = graph.GetErrorX(iPoint)
            dataErrorY = graph.GetErrorY(iPoint)

            graph.GetPoint(iPoint,dataPointX,dataPointY)

            if not (iPoint in badPoints) : 
                newGraph.SetPoint(iPoint, dataPointX, dataPointY)
                newGraph.SetPointError(iPoint, dataErrorX, dataErrorY)

            else : 
                newGraph.SetPoint(iPoint, dataPointX, 1)
                newGraph.SetPointError(iPoint, dataErrorX, 0)


    nomGraphFinal = nomGraph.Clone()
        
    for iPoint in xrange(0, nomGraph.GetN()) :

        dataPointX = Double(0)
        dataPointY = Double(0)
        nomGraph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = nomGraph.GetErrorX(iPoint)
        dataErrorY = nomGraph.GetErrorY(iPoint)

        if not (iPoint in badPoints) : 
            nomGraphFinal.SetPoint(iPoint, dataPointX, dataPointY)
            nomGraphFinal.SetPointError(iPoint, dataErrorX, dataErrorY)

        else : 
            nomGraphFinal.SetPoint(iPoint, dataPointX, 1)
            nomGraphFinal.SetPointError(iPoint, dataErrorX, 0)

    #c = TCanvas()

    #nomGraphFinal.Draw("AP")
    #for graph in newGraphsFinal :
        #newgraph.FixPlot(graph)
        #newgraph.Draw("Psame")

    return nomGraphFinal, newGraphsFinal
        
#this fcn works with TGraphs with symmetric x errors
def TGraphToTH1 (graph) :
  
    #loop on all data points
    nPoints = graph.GetN()
    #print "nPoints", nPoints
    
    x=array('f')
    y=array('f')
    ey=array('f')

    dataPointX = None
    dataErrorX = None
    
    for iPoint in xrange(0,nPoints) :
        #get info on data point
        dataPointX = Double(0)
        dataPointY = Double(0)
        graph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = graph.GetErrorX(iPoint)
        dataErrorY = graph.GetErrorY(iPoint)

        x.append(dataPointX-dataErrorX)
        y.append(dataPointY)
        ey.append(dataErrorY)
        
    #this needs to be done at the END for the upper limit
    x.append(dataPointX+dataErrorX)
    
#    print x, y, ey
    
    #safety checks...regardless of malmsteen
    for iPoint in xrange(0,nPoints) :
            #if numpy.isnan(y[iPoint]) : 
          #y[iPoint]=0
        #if numpy.isnan(x[iPoint]) : 
          #x[iPoint]=0
    
        if y[iPoint]<0.00000001 :
          y[iPoint]=0
          ey[iPoint]=0
        
    myHist=TH1F("Hist"+graph.GetName(),graph.GetTitle(),nPoints,x)

    for iPoint in xrange(0,nPoints) :

        myHist.SetBinContent(iPoint+1,y[iPoint])#the +1 is needed because TH1 have overflows
#        myHist.SetBinError(iPoint+1,ey[iPoint])#the +1 is needed because TH1 have overflows
        myHist.SetBinError(iPoint+1,0)#avoid poissonian errors, pick up errors from graph anyways

    print x, y, ey

    return myHist


#this fcn works with TGraphs with symmetric x errors
#it assumes that the data points of the TGraph are all around zero
#it translates a TGraphErrors into a TH1 where the bin content = error
def TGraphToTH1_error (graph) :
  
    #loop on all data points
    nPoints = graph.GetN()

    x=array('f')
    y=array('f')
    ey=array('f')

    for iPoint in xrange(0,nPoints) :
        #get info on data point
        dataPointX = Double(0)
        dataPointY = Double(0)
        graph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = graph.GetErrorX(iPoint)
        dataErrorY = graph.GetErrorYhigh(iPoint)

        x.append(dataPointX-dataErrorX)
        y.append(dataPointY)
        ey.append(dataErrorY)

    x.append(dataPointX+dataErrorX)
    
    #print x, y, ey

    if y[0]!=0 :
        print "This TGraph is not centered at zero!"
        return 0
    
    myHist=TH1F("Hist","Hist",nPoints,x)

    for iPoint in xrange(0,nPoints) :
        if x[iPoint]<=x[iPoint+1] : "HELP!" 
 
        #print ey[iPoint]
        myHist.SetBinContent(iPoint+1,ey[iPoint])#the +1 is needed because TH1 have overflows
        myHist.SetBinError(iPoint+1,0)#set the error to zero otherwise a meaningless poisson error will be stuck there
        
    return myHist

def removeLargeErrorPointsSingle(graph) :

    badPoints = []

    for iPoint in xrange(0, graph.GetN()) :

        dataPointX = Double(0)
        dataPointY = Double(0)

        graph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = graph.GetErrorX(iPoint)
        dataErrorY = graph.GetErrorY(iPoint)

        if dataErrorY > 0.011 or dataErrorY < 0.00000001 or iPoint>graph.GetN()-3 : 
            if not (iPoint in badPoints) : badPoints.append(iPoint)

    newGraph = graph.Clone()
    
    for iPoint in xrange(0, graph.GetN()) :

        dataPointX = Double(0)
        dataPointY = Double(0)

        dataErrorX = graph.GetErrorX(iPoint)
        dataErrorY = graph.GetErrorY(iPoint)

        graph.GetPoint(iPoint,dataPointX,dataPointY)

        if not (iPoint in badPoints) : 
            newGraph.SetPoint(iPoint, dataPointX, dataPointY)
            newGraph.SetPointError(iPoint, dataErrorX, dataErrorY)

        else : 
            newGraph.SetPoint(iPoint, dataPointX, 0.00000001)
            newGraph.SetPointError(iPoint, dataErrorX, 0)

    return newGraph
