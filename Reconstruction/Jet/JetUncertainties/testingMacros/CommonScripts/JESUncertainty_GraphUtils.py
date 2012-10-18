#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

from numpy import isnan
from ROOT import Double, TH1F
from array import *

###
# Avoid low statistics points
###

def flagLowStatPoints (graph, debug) : 

  #stick all the indices of the bad points in this vector
  points = []

  #loop on all data points of the graph
  nPoints = graph.GetN()

  for iPoint in xrange(0,nPoints) :
    dataPointX = Double(0)
    dataPointY = Double(0)
    error = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
      
    graph.GetPoint(iPoint,dataPointX,dataPointY)
      
    ##eliminate and flag NaN points (better if numpy module installed) 
    if isnan(dataPointY) : 
      dataPointY = 0.000001
    #if dataPointY < 0.000001 : 
      #dataPointY = 0.000001
      points.append(iPoint)
      
    ##flag  points where the error is zero (this means that there was just one entry in the histogram)
    if errorY < 0.00000001 :
      points.append(iPoint)

    ##flag points where the error is larger than 0.01 (relative error on uncertainty will be large)
    if errorY > 0.01 :
      points.append(iPoint)
      
  if debug == True:
    print "DEBUG: List of bad points for plot", graph.GetName(), ":"
    print points
  
  return points

###
# Avoid low statistics points, but do not check on upper bound of error
###

def flagLowStatPoints_noLargeErrorCheck (graph, debug) : 

  #stick all the indices of the bad points in this vector
  points = []

  #loop on all data points of the graph
  nPoints = graph.GetN()

  for iPoint in xrange(0,nPoints) :
    dataPointX = Double(0)
    dataPointY = Double(0)
    error = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
      
    graph.GetPoint(iPoint,dataPointX,dataPointY)
      
    ##eliminate and flag NaN points (better if numpy module installed) 
    if isnan(dataPointY) : 
      dataPointY = 0.000001
    #if dataPointY < 0.000001 : 
      #dataPointY = 0.000001
      points.append(iPoint)
      
    ##flag  points where the error is zero (this means that there was just one entry in the histogram)
    if errorY < 0.00000001 :
      points.append(iPoint)

      
  if debug == True:
    print "DEBUG: List of bad points for plot", graph.GetName(), ":"
    print points
  
  return points

def eliminateLowStatPoints(graph, listOfBadPoints) :
  
  #loop on all data points of the graph
  nPoints = graph.GetN()

  for iPoint in xrange(0,nPoints) :
    dataPointX = Double(0)
    dataPointY = Double(0)
    error = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
      
    graph.GetPoint(iPoint,dataPointX,dataPointY)
    
    if iPoint in listOfBadPoints :
      
       graph.SetPoint(iPoint, dataPointX, -999)
       graph.SetPointError(iPoint, errorX, 0)

def rescaleToGeV(graph) :
  
  #loop on all data points of the graph
  nPoints = graph.GetN()

  for iPoint in xrange(0,nPoints) :
    dataPointX = Double(0)
    dataPointY = Double(0)
    error = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
      
    graph.GetPoint(iPoint,dataPointX,dataPointY)
    
    graph.SetPoint(iPoint, dataPointX/1000, dataPointY)
    graph.SetPointError(iPoint, errorX/1000, errorY)

def printGraphContent(graph) :
  
  #loop on all data points of the graph
  nPoints = graph.GetN()
  
  print graph.GetName()
  
  for iPoint in xrange(0,nPoints) :
    dataPointX = Double(0)
    dataPointY = Double(0)
    error = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
      
    graph.GetPoint(iPoint,dataPointX,dataPointY)
    
    print dataPointX, ":", dataPointY
    
def removeXErrors(graph) :
  
  #loop on all data points of the graph
  nPoints = graph.GetN()

  for iPoint in xrange(0,nPoints) :
    errorY = graph.GetErrorY(iPoint)      
    graph.SetPointError(iPoint, 0, errorY)

def removeYErrors(graph) :
  
  #loop on all data points of the graph
  nPoints = graph.GetN()

  for iPoint in xrange(0,nPoints) :
    errorX = graph.GetErrorX(iPoint)      
    graph.SetPointError(iPoint, errorX, 0.00000001)


def scaleGraph(graphToScale, scalingInputGraph, oneMinus = False) :
  
  #get number of points for the two graphs
  nPointsGraphToScale= graphToScale.GetN()
  nPointsScalingInputGraph = scalingInputGraph.GetN()

  if nPointsGraphToScale!=nPointsScalingInputGraph :
    print "ERROR in JESUncertainty_RatioUtils::scaleGraph: graphs don't have the same number of points"
    return None
   
  #prepare the final graph
  scaledGraph = graphToScale.Clone()
  scaledGraph.SetName(graphToScale.GetName()+"_Scaled")
  
  #loop on points of the graphs
  for iPoint in xrange(0,nPointsGraphToScale) :
    
    #make the new point
    maxPoint = Double(0)
    maxPointError = Double(0)
    
    #retrieve the point
    dataPointXgraphToScale = Double(0)
    dataPointYgraphToScale = Double(0)
    errorXgraphToScale= graphToScale.GetErrorX(iPoint)
    errorYgraphToScale = graphToScale.GetErrorY(iPoint)
    graphToScale.GetPoint(iPoint,dataPointXgraphToScale,dataPointYgraphToScale)
    
    dataPointXscalingInputGraph = Double(0)
    dataPointYscalingInputGraph = Double(0)
    errorXscalingInputGraph = scalingInputGraph.GetErrorX(iPoint)
    errorYscalingInputGraph = scalingInputGraph.GetErrorY(iPoint)
    scalingInputGraph.GetPoint(iPoint,dataPointXscalingInputGraph,dataPointYscalingInputGraph)

    if abs(dataPointXgraphToScale-dataPointXscalingInputGraph) > 0.0001:
      print "ERROR in JESUncertainty_RatioUtils::scaleGraph: x coordinates of data points do not match"
      return None

    #set the point/error of the output graph to the scaled point/error of the input one (protect against negative scaling)
    if abs(dataPointYscalingInputGraph) < 0:
      print "WARNING in JESUncertainty_RatioUtils::scaleGraph: negative scale factor for point: ", iPoint, ", X:", dataPointXgraphToScale
    scaledGraph.SetPoint(iPoint, dataPointXgraphToScale, dataPointYscalingInputGraph*dataPointYgraphToScale)
    scaledGraph.SetPointError(iPoint, errorXgraphToScale, abs(dataPointYscalingInputGraph)*dataPointYgraphToScale)
    
    if oneMinus : 
      scaledGraph.SetPoint(iPoint, dataPointXgraphToScale, (1-dataPointYscalingInputGraph)*dataPointYgraphToScale)
      scaledGraph.SetPointError(iPoint, errorXgraphToScale, abs(dataPointYscalingInputGraph)*dataPointYgraphToScale)
    
  return scaledGraph
  
def extendLastGoodBin_2011(graph, warn) :
  
  #loop on all data points of the graph
  nPoints = graph.GetN()
  
  #it should not happen that the first bin is bad, but just in case we keep it obvious
  lastGoodBin = -999
  lastGoodError = -999
  
  for iPoint in xrange(0,nPoints) :
    dataPointX = Double(0)
    dataPointY = Double(0)
    error = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
      
    graph.GetPoint(iPoint,dataPointX,dataPointY)
    
    #HACK: this is rather arbitrary!
    if dataPointY > 2. or errorY > 0.1:
      if warn : 
        print "WARNING in JESUncertainty_RatioUtils::extendLastGoodBin: "
        print "found bad point for graph:", graph.GetName(), "at point: ", iPoint, ", X:", dataPointX, "Y:", dataPointY
        print "extending previous point:", lastGoodBin
      newDataPointY=lastGoodBin
      newErrorY = lastGoodError
      graph.SetPoint(iPoint,dataPointX,newDataPointY)
      graph.SetPointError(iPoint, errorX, newErrorY)
    else : 
      lastGoodBin = dataPointY
      lastGoodError = errorY


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
        
    #this needs to be done at the end of the loop, for the upper limit
    x.append(dataPointX+dataErrorX)
    
    #safety checks
    for iPoint in xrange(0,nPoints) :
        if isnan(y[iPoint]) : 
          y[iPoint]=0
        if isnan(x[iPoint]) : 
          x[iPoint]=0
        if y[iPoint]<0.00000001 :
          y[iPoint]=0
          ey[iPoint]=0
        
    myHist=TH1F("Hist"+graph.GetName(),graph.GetTitle(),nPoints,x)

    for iPoint in xrange(0,nPoints) :

        myHist.SetBinContent(iPoint+1,y[iPoint])#the +1 is needed because TH1 have overflows
        #myHist.SetBinError(iPoint+1,ey[iPoint])#the +1 is needed because TH1 have overflows
        myHist.SetBinError(iPoint+1,0)#avoid poissonian errors

    return myHist

#this fcn works with TGraphs with symmetric x errors
def TGraphToTH1_withError (graph) :
  
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
        
    #this needs to be done at the end of the loop, for the upper limit
    x.append(dataPointX+dataErrorX)
    
    #safety checks
    for iPoint in xrange(0,nPoints) :
        if isnan(y[iPoint]) : 
          y[iPoint]=0
        if isnan(x[iPoint]) : 
          x[iPoint]=0
        if y[iPoint]<0.00000001 :
          y[iPoint]=0
          ey[iPoint]=0
        
    myHist=TH1F("Hist"+graph.GetName(),graph.GetTitle(),nPoints,x)

    for iPoint in xrange(0,nPoints) :

        myHist.SetBinContent(iPoint+1,y[iPoint])#the +1 is needed because TH1 have overflows
        myHist.SetBinError(iPoint+1,ey[iPoint])#the +1 is needed because TH1 have overflows
        #myHist.SetBinError(iPoint+1,0)#avoid poissonian errors

    return myHist
