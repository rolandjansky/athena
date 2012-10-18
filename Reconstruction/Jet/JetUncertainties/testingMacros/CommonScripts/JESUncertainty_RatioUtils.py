#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

from numpy import isnan
from ROOT import Double, TGraphErrors
from math import sqrt

###
# Do the ratio of two TGraphs
###

def divideGraphs (numeratorGraph, denominatorGraph, warn) :

  #get number of points for the two graphs
  nPointsNum = numeratorGraph.GetN()
  nPointsDen = denominatorGraph.GetN()

  if nPointsNum!=nPointsDen :
    print "ERROR in JESUncertainty_RatioUtils:: graphs don't have the same number of points"
    return None
   
  #prepare the final graph
  graphRatio = TGraphErrors()
  graphRatio.SetName(numeratorGraph.GetName())
  
  #loop on points of the graphs
  for iPoint in xrange(0,nPointsNum) :
      
    #retrieve the point
    dataPointXNum = Double(0)
    dataPointYNum = Double(0)
    errorXNum = numeratorGraph.GetErrorX(iPoint)
    errorYNum = numeratorGraph.GetErrorY(iPoint)
    numeratorGraph.GetPoint(iPoint,dataPointXNum,dataPointYNum)
    
    dataPointXDen = Double(0)
    dataPointYDen = Double(0)
    errorXDen = denominatorGraph.GetErrorX(iPoint)
    errorYDen = denominatorGraph.GetErrorY(iPoint)
    denominatorGraph.GetPoint(iPoint,dataPointXDen,dataPointYDen)
     
    if abs(dataPointXNum-dataPointXDen) > 0.0001:
      print "ERROR in JESUncertainty_RatioUtils::divideGraphs: x coordinates of data points do not match"
      return None

    #do the ratio
    dataPointXRatio = dataPointXNum
    dataPointYRatio = None
    if dataPointYDen != 0 : 
      dataPointYRatio = dataPointYNum/dataPointYDen
    else : 
      dataPointYRatio = 0
      if warn == True :
        print "WARNING in JESUncertainty_RatioUtils::divideGraphs: division by zero for x point: ", dataPointXNum 
        print "numerator graph:", numeratorGraph.GetName()
        print "denominator graph:", denominatorGraph.GetName()

    errorXRatio = errorXNum
    
    
    #calculate the error (simple error propagation)
    if errorYNum != 0 and errorYDen != 0 :

      #no correlation:
      errorYRatioNoCorr = sqrt(
                            pow(errorYNum,2)/pow(dataPointYDen,2)+
                            pow(errorYDen,2)*pow(dataPointYNum,2)/pow(dataPointYDen,4)
                            )
          
      #with correlation:
      #relativeErrorYRatioSquared =  (pow((errorYNum/dataPointYNum),2)+
                                    #pow((errorYDen/dataPointYDen),2)-
                                    #2*(errorYNum*errorYDen)/(dataPointYNum*dataPointYDen))
                                    
      #errorYRatioCorr = sqrt(relativeErrorYRatioSquared/(pow(dataPointYRatio,2)))
      
      #go for no correlation:
      errorYRatio = errorYRatioNoCorr
            
    #case in which the numerator is zero (should not happen), zero the error as well
    #this is not the best approach but it should be good enough for what we are doing (we always discard points with zero response)
    elif dataPointYNum == 0 or dataPointYDen == 0 :
      errorYRatio = 0
    
    graphRatio.SetPoint(iPoint, dataPointXRatio, dataPointYRatio)
    graphRatio.SetPointError(iPoint, errorXRatio, errorYRatio)

  return graphRatio
  
  
###
# Calculate point by point |1-graph| and put it into a TGraph
###

def getDeltaVarGraph (graph) :

  #get number of points for the graph
  nPoints = graph.GetN()
   
  #prepare the final graph
  deltaVarGraph = TGraphErrors()
  deltaVarGraph.SetName(graph.GetName())
  
  #loop on points of the graphs
  for iPoint in xrange(0,nPoints) :
      
    #retrieve the point
    dataPointX = Double(0)
    dataPointY = Double(0)
    errorX = graph.GetErrorX(iPoint)
    errorY = graph.GetErrorY(iPoint)
    graph.GetPoint(iPoint,dataPointX,dataPointY)
         
    #subtract unity and do the absolute value
    dataPointXDeltaVar = dataPointX
    dataPointYDeltaVar = abs(1-dataPointY)
    errorXDeltaVar = errorX

    deltaVarGraph.SetPoint(iPoint, dataPointXDeltaVar, dataPointYDeltaVar)
    
    #being lazy (we will truncate the error if it goes below zero anyways)
    errorYDeltaVar = errorY
    
    #to calculate the error correctly    
    #if dataPointY != 0 and (dataPointY-1) > 0 :
      #errorYDeltaVar = sqrt((dataPointY-1)/abs(dataPointY-1) * errorY) #this guarantees one does not go negative spitting an exception
    #else 
      #one here would need to do a TGraphAsymmErrors with only the positive error in full
    
    deltaVarGraph.SetPointError(iPoint, errorXDeltaVar, errorYDeltaVar)
    

  return deltaVarGraph

###
# Get maximum variation out of two plots
###

def getMaxVariationGraph (graph1, graph2) :
  
  #get number of points for the two graphs
  nPointsGraph1= graph1.GetN()
  nPointsGraph2 = graph2.GetN()

  if nPointsGraph1!=nPointsGraph2 :
    print "ERROR in JESUncertainty_RatioUtils::getMaxVariationGraph: graphs don't have the same number of points"
    return None
   
  #prepare the final graph
  maxVarGraph = graph1.Clone()
  maxVarGraph.SetName(graph1.GetName()+"_Max")
  
  #loop on points of the graphs
  for iPoint in xrange(0,nPointsGraph1) :
    
    #make the new point
    maxPoint = Double(0)
    maxPointError = Double(0)
    
    #retrieve the point
    dataPointXGraph1 = Double(0)
    dataPointYGraph1 = Double(0)
    errorXGraph1= graph1.GetErrorX(iPoint)
    errorYGraph1 = graph1.GetErrorY(iPoint)
    graph1.GetPoint(iPoint,dataPointXGraph1,dataPointYGraph1)
    
    dataPointXGraph2 = Double(0)
    dataPointYGraph2 = Double(0)
    errorXGraph2 = graph2.GetErrorX(iPoint)
    errorYGraph2 = graph2.GetErrorY(iPoint)
    graph2.GetPoint(iPoint,dataPointXGraph2,dataPointYGraph2)

    if abs(dataPointXGraph1-dataPointXGraph2) > 0.0001:
      print "ERROR in JESUncertainty_RatioUtils::getMaxVariationGraph: x coordinates of data points do not match"
      return None

    #check for the largest (the convention of -999 for useless bins should work here)
    if dataPointYGraph1 > dataPointYGraph2 :
      maxPoint = dataPointYGraph1
      maxPointError = errorYGraph1

    else :
      maxPoint = dataPointYGraph2
      maxPointError = errorYGraph2
    #print dataPointXGraph1, maxPoint, maxPointError
    maxVarGraph.SetPoint(iPoint, dataPointXGraph1, maxPoint)
    maxVarGraph.SetPointError(iPoint, errorXGraph1, maxPointError)

  return maxVarGraph
    #TODO: is there anything to do for the last good bin?


#Debugging material to see if it actually works
#c=TCanvas() 
#deadMaterialMaxVarGraph.Draw("AP")
#deadMaterialMaxVarGraph.SetMarkerStyle(25)
#deadMaterialMaxVarGraph.SetMarkerSize(2)
#deadMaterialMaxVarGraph.SetMarkerColor(4)
#deadMaterialMaxVarGraph.SetLineStyle(2)
#deadMaterialMaxVarGraph.SetLineWidth(5)
#deadMaterialMaxVarGraph.SetLineColor(4)
#gPad.SetLogx()
#deadMaterialGraphE.SetMarkerStyle(20)
#deadMaterialGraphE.Draw("Psame")
#deadMaterialGraphPt.SetMarkerStyle(22)
#deadMaterialGraphPt.Draw("Psame")
#c.SaveAs("max.png")

###
# Get a graph that is the sum in quadrature of all others (they all have to look like dummygraph)
###

def getQuadratureGraph (dummyGraph, listOfGraphs) :
  
  ##preliminary checks to see if all graphs are ok
  for graphToSum in listOfGraphs :
    #print graphToSum
    nPointsDummyGraph = dummyGraph.GetN()
    nPointsGraphToSum = graphToSum.GetN()
    
    if nPointsDummyGraph!=nPointsGraphToSum :
      print "ERROR in JESUncertainty_RatioUtils::getQuadratureGraph: graphs don't have the same number of points"
      return None
      
    #loop on points of the graphs
    for iPoint in xrange(0,nPointsDummyGraph) :
            
      #retrieve the point
      dataPointXDummyGraph = Double(0)
      dataPointYDummyGraph = Double(0)
      dummyGraph.GetPoint(iPoint,dataPointXDummyGraph,dataPointYDummyGraph)
      
      dataPointXGraphToSum = Double(0)
      dataPointYGraphToSum = Double(0)
      errorXGraphToSum = graphToSum.GetErrorX(iPoint)
      errorYGraphToSum = graphToSum.GetErrorY(iPoint)
      graphToSum.GetPoint(iPoint,dataPointXGraphToSum,dataPointYGraphToSum)

      if abs(dataPointXDummyGraph-dataPointXGraphToSum) > 0.0001:
        print "ERROR in JESUncertainty_RatioUtils::getQuadratureGraph: x coordinates of data points do not match"
        return None
        
  #end of preliminary checks
  
  #prepare everything for the propagation of errors
  #these are arrays that at the end of the loop on the graphs contain the points and errors
  AllSquaredSums = []
  AllErrors = []
  #and this is the final graph that contains the quadrature of the others
  quadratureGraph = dummyGraph.Clone()
  quadratureGraph.SetName(listOfGraphs[0].GetName()+"_Quadrature")
  
  for iPoint in xrange(0,nPointsDummyGraph) :
        
    squaredSum = 0
    error = 0
    badPoint = False
    
    for graphToSum in listOfGraphs :

      dataPointXGraphToSum = Double(0)
      dataPointYGraphToSum = Double(0)
      errorXGraphToSum = graphToSum.GetErrorX(iPoint)
      errorYGraphToSum = graphToSum.GetErrorY(iPoint)
      graphToSum.GetPoint(iPoint,dataPointXGraphToSum,dataPointYGraphToSum)
      
      #check for bad points 
      if dataPointYGraphToSum < -998 :
        badPoint = True
      
      #this will be the denominator of the error propagation as well
      squaredSum += pow(dataPointYGraphToSum,2)
      
      #if iPoint == 0: 
        #print "toSum: ", dataPointYGraphToSum
        #print "toSumSquared: ", pow(dataPointYGraphToSum,2)
      #this is the numerator of the error propagation
      error += pow(dataPointYGraphToSum*errorYGraphToSum,2)
      #print "addingToErrorNum: ", pow(dataPointYGraphToSum*errorYGraphToSum,2)
      
    try :
      sqrtSquaredSum = sqrt(squaredSum)
      sqrtError = sqrt(error/squaredSum)
    except :
      sqrtSquaredSum = 0
      sqrtError = 0
    
    #propagate the bad points
    if badPoint : 
      sqrtSquaredSum = -999
      sqrtError = 0.000000001
    
    #now append them (for debugging reasons only)    
    AllSquaredSums.append(sqrtSquaredSum)
    AllErrors.append(sqrtError)
    
    #put them into the final plot
    quadratureGraph.SetPoint(iPoint, dataPointXGraphToSum, sqrtSquaredSum)
    quadratureGraph.SetPointError(iPoint, errorXGraphToSum, sqrtError)
        
  return quadratureGraph
  
###
# Get a graph that is the linear sum of all others (they all have to look like dummygraph)
###

def getLinearSumGraph (dummyGraph, listOfGraphs) :
  
  ##preliminary checks to see if all graphs are ok
  for graphToSum in listOfGraphs :
    #print graphToSum
    nPointsDummyGraph = dummyGraph.GetN()
    nPointsGraphToSum = graphToSum.GetN()
    
    if nPointsDummyGraph!=nPointsGraphToSum :
      print "ERROR in JESUncertainty_RatioUtils::getQuadratureGraph: graphs don't have the same number of points"
      return None
      
    #loop on points of the graphs
    for iPoint in xrange(0,nPointsDummyGraph) :
            
      #retrieve the point
      dataPointXDummyGraph = Double(0)
      dataPointYDummyGraph = Double(0)
      dummyGraph.GetPoint(iPoint,dataPointXDummyGraph,dataPointYDummyGraph)
      
      dataPointXGraphToSum = Double(0)
      dataPointYGraphToSum = Double(0)
      errorXGraphToSum = graphToSum.GetErrorX(iPoint)
      errorYGraphToSum = graphToSum.GetErrorY(iPoint)
      graphToSum.GetPoint(iPoint,dataPointXGraphToSum,dataPointYGraphToSum)

      if abs(dataPointXDummyGraph-dataPointXGraphToSum) > 0.0001:
        print "ERROR in JESUncertainty_RatioUtils::getQuadratureGraph: x coordinates of data points do not match"
        return None
        
  #end of preliminary checks
  
  #prepare everything for the propagation of errors
  #these are arrays that at the end of the loop on the graphs contain the points and errors
  AllSquaredSums = []
  AllErrors = []
  
  #this contains the errors for all data points for the propagation
  ListOfErrors = []
  
  #and this is the final graph that contains the quadrature of the others
  linearGraph = dummyGraph.Clone()
  linearGraph.SetName(listOfGraphs[0].GetName()+"_Quadrature")
  
  for iPoint in xrange(0,nPointsDummyGraph) :
    
    theSum = 0
    uncorrError = 0
    corrError = 0
    badPoint = False
    
    for graphToSum in listOfGraphs :
      
      dataPointXGraphToSum = Double(0)
      dataPointYGraphToSum = Double(0)
      errorXGraphToSum = graphToSum.GetErrorX(iPoint)
      errorYGraphToSum = graphToSum.GetErrorY(iPoint)
      graphToSum.GetPoint(iPoint,dataPointXGraphToSum,dataPointYGraphToSum)
      
      #check for bad points 
      if dataPointYGraphToSum < -998 :
        badPoint = True
      
      #this will be the denominator of the error propagation as well
      theSum += dataPointYGraphToSum
      
      #print "toSumSquared: ", pow(dataPointYGraphToSum,2)
      #this is for the error propagation
      uncorrError += pow(errorYGraphToSum,2)
      ListOfErrors.append(errorYGraphToSum)
      #print "addingToErrorNum: ", pow(dataPointYGraphToSum*errorYGraphToSum,2)

    #need to do all combination of errors
    while len(ListOfErrors) != 0 :
      for iError, theError in enumerate(ListOfErrors) :
        theError = ListOfErrors.pop(iError)
        for jError, theOtherError in enumerate(ListOfErrors) :
            corrError =+ theError * theOtherError  

    sqrtSum = theSum
    sqrtError = sqrt(uncorrError + 2 * corrError)
    
    #propagate the bad points
    if badPoint : 
      sqrtSum = -999
      sqrtError = 0.000000001
    
    #now append them (for debugging reasons only)    
    AllSquaredSums.append(sqrtSum)
    AllErrors.append(sqrtError)
    #print AllSquaredSums
    #print AllErrors
    
    #put them into the final plot
    linearGraph.SetPoint(iPoint, dataPointXGraphToSum, sqrtSum)
    linearGraph.SetPointError(iPoint, errorXGraphToSum, sqrtError)
    
  return linearGraph