#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

from ROOT import TGraphErrors,TGraph, TGraphAsymmErrors, Double, TCanvas, gPad
#import numpy
from math import fabs, sqrt


############Division function############

def getPlotRatio (plotNum, plotDen, doAverage) :

    DataPointsForAverage=[]
    ErrorsForAverage=[]
    averageDelta = 0
    maxDelta = 0
    nPointsNum = plotNum.GetN()
    nPointsDen = plotDen.GetN()
    
    if nPointsNum!=nPointsDen :
        print "Plots don't have the same number of points!"
        return(0,0,0)

    plotRatio = TGraphErrors()

    plotRatio.SetName(plotNum.GetName())

    x1=Double(0)
    y1=Double(0)
    x2=Double(0)
    y2=Double(0)
    dx1=Double(0)
    dx2=Double(0)
    dy1=Double(0)
    dy2=Double(0)
    iv=int(0)
    dx=Double(0)
    for pointNum in xrange(0,nPointsNum) :
        pointNum = int(pointNum)
        matchcount = 0
        for pointDen in xrange(0,nPointsDen) :
            pointDen = int(pointDen)
            plotNum.GetPoint(pointNum, x1, y1)
            plotDen.GetPoint(pointDen, x2, y2)
            dx1 = plotNum.GetErrorX(pointNum)
            dx2 = plotNum.GetErrorX(pointDen)
            emean = (dx1+dx2)/2.        
            if fabs(x1-x2)>=emean and fabs(x1-x2)>dx :
                pass
                #print "no idea what's going on here" 
            else :
                matchcount=matchcount+1
                dx1 = plotNum.GetErrorX(pointNum)
                if y1 != 0 : dy1 = plotNum.GetErrorY(pointNum)/y1
                if y2 != 0 : dy2 = plotNum.GetErrorY(pointDen)/y2

                if y2 != 0 :
                    plotRatio.SetPoint(iv, x1, y1/y2)
                    if doAverage : DataPointsForAverage.append(y1/y2)
                    
                else :
                    plotRatio.SetPoint(iv, x1, y2)
                    if doAverage : DataPointsForAverage.append(y2)

                e = Double(0)

                if y1 != 0 and y2 != 0 : e=sqrt(dy1*dy1+dy2*dy2)*(y1/y2)
                plotRatio.SetPointError(iv, dx1, e)
                if doAverage : ErrorsForAverage.append(e)

                iv = iv+1

        if (matchcount>1) :
            print "too many x points matched!"
            return(0)
        
    if doAverage :
        
        averageDelta = getAverageDelta(DataPointsForAverage, ErrorsForAverage)
        maxDelta = getMaxDelta(DataPointsForAverage, ErrorsForAverage)
        
    
    return plotRatio, averageDelta-1, maxDelta
                    

def getAverageDelta (DataPoints, ErrorsOnDataPoints) :
    if len(DataPoints) != len(ErrorsOnDataPoints) :
        "Data points and errors do not match"
        return 0

    average = 0
    skippedDataPoints = 0

    for datapoint,error in zip(DataPoints, ErrorsOnDataPoints) :
        #don't care about the data point if the error is 0
        if error<0.00000001 or  error>0.03 :
            skippedDataPoints = skippedDataPoints+1
            pass
            
        else :
            average += datapoint

    average = average/(len(DataPoints)-skippedDataPoints)
    return average

def getMaxDelta (DataPoints, ErrorsOnDataPoints) :
    if len(DataPoints) != len(ErrorsOnDataPoints) :
        "Data points and errors do not match"
        return 0

    maxDelta = 0

    for datapoint,error in zip(DataPoints, ErrorsOnDataPoints) :
        #don't care about the data point if the error is 0
        if error<0.00000001 :
            pass
        #or if the error is > 3% (most likely due to a bad fit)
        elif error>0.03 :
            pass
        else :
            if fabs(datapoint-1) > fabs(maxDelta) : maxDelta = datapoint-1

    return maxDelta


def numericalInvertPlot (graph) :
    #pick up point by point pl

    #loop on all data points
    nPoints = graph.GetN()

    newGraph=TGraphAsymmErrors(nPoints)

    #x=array('f')
    #y=array('f')
    #ey=array('f')

    for iPoint in xrange(0,nPoints) :
        #get info on data point
        dataPointX = Double(0)
        dataPointY = Double(0)
        graph.GetPoint(iPoint,dataPointX,dataPointY)
        dataErrorX = graph.GetErrorX(iPoint)
        dataErrorY = graph.GetErrorY(iPoint)

        #numerical-invert the X 
        newDataPointX = dataPointX * dataPointY

        newErrorXLeft = fabs(newDataPointX - (dataPointX-dataErrorX))
        newErrorXRight = fabs(newDataPointX - (dataPointX+dataErrorX))
        
        #setting the new graph (with asymm errors this time)
        newGraph.SetPoint(iPoint, newDataPointX, dataPointY)    
        newGraph.SetPointEXhigh(iPoint, newErrorXRight)    
        newGraph.SetPointEXlow(iPoint, newErrorXLeft)    
        newGraph.SetPointEYhigh(iPoint, dataErrorY)    
        newGraph.SetPointEYlow(iPoint, dataErrorY)    

        print dataPointY, newDataPointX 
        #check that the numerical-inverted X is still in the previous bin - otherwise, ERROR!
        binRangeLow = dataPointX-dataErrorX
        binRangeHigh = dataPointX+dataErrorX
        if newDataPointX<binRangeLow or newDataPointX>binRangeHigh : 
            print "Warning! Data point should not be here!"
            print "Old data point: ", dataPointX
            print "New NI data point:" , newDataPointX
            print "XLow: ", binRangeLow
            print "XHigh: ", binRangeHigh
            newGraph.SetPoint(iPoint, dataPointX, 0.0000000000001)
            newGraph.SetPointEXhigh(iPoint, dataErrorX)    
            newGraph.SetPointEXlow(iPoint, dataErrorX)    
            newGraph.SetPointEYhigh(iPoint, dataErrorY)    
            newGraph.SetPointEYlow(iPoint, dataErrorY)    

    return newGraph
