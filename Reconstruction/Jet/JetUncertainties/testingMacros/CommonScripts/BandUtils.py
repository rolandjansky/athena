#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

import sys
sys.argv = [sys.argv[0], '-b']

from ROOT import TGraphAsymmErrors, Double, TGraphErrors, TCanvas
import atlasStyleMacro
from math import fabs, sqrt

def makeBand (middleGraph, lowerGraph, upperGraph, noorder) :

    bandsGraph = TGraphAsymmErrors()
    bandsGraph.SetName(middleGraph.GetName())
    bandsGraph.SetMarkerStyle(middleGraph.GetMarkerStyle());
    bandsGraph.SetMarkerColor(middleGraph.GetMarkerColor());
    bandsGraph.SetLineColor(middleGraph.GetLineColor());  

    x1 = Double(0)
    x2 = Double(0)
    x3 = Double(0)
    y = []
    y.append(Double(0))
    y.append(Double(0))
    y.append(Double(0))

    tmp = Double(0)

    if lowerGraph.GetN() != upperGraph.GetN() or lowerGraph.GetN() != middleGraph.GetN() :
        print "Plots don't have the same number of points!"
        print "Lower: ", lowerGraph.GetN() 
        print "Upper: ", upperGraph.GetN() 
        print "Middle: ", middleGraph.GetN() 
        return 0

    #again a hack to forget about the 1st point
    for iPoint in xrange(1,lowerGraph.GetN()) :
        middleGraph.GetPoint(iPoint, x1, y[0]) 
        lowerGraph.GetPoint(iPoint, x1, y[1])
        upperGraph.GetPoint(iPoint, x1, y[2])

        if (iPoint == lowerGraph.GetN()-1) : x2 = x1
        else : upperGraph.GetPoint(iPoint+1, x2, buf)
        
        if (iPoint == 0) : x3 = x1
        else : upperGraph.GetPoint(iPoint-1, x3, buf)

        tmp = Double(0)
        yup = Double(0)
        yce = Double(0)        
        ydn = Double(0)

        if noorder == 0 :
            yce = y[0]
            ydn = y[1]
            yup = y[2]

        elif noorder == 1 :
            tmp = y[2]
            if y[1] < y[2] : 
                y[2] = y[1] 
                y[1] = tmp

            yce=y[0]
            ydn=y[1]
            yup=y[2]

        else :
            for p in [0,1,2] :
                for q in [0,1,2] :
                    if y[q] > y[q+1] :
                        tmp = y[q+1]
                        y[q+1]=y[q]
                        y[q]=tmp

            ydn=y[0]
            yce=y[1]
            yup=y[2]

        bandsGraph.SetPoint(iPoint, x1, yce)

        ex0 = Double(lowerGraph.GetErrorX(iPoint))

        binwl = Double(0)
        binwh = Double(0)

        if (ex0==0) :
            binwl=(x1-x3)/2.;
            binwh=(x2-x1)/2.;
            if binwl==0. : binwl= binwh;
            if binwh==0. : binwh= binwl;
        else :
            binwl = ex0
            binwh = ex0

        dxl= Double(yce-ydn)
        dxh= Double(yup-yce)

        if noorder==0 :
            if dxl<0 : 
                tmp=-dxl
                dxl=dxh
                dxh=tmp
        
        bandsGraph.SetPointError(iPoint, binwl, binwh, dxl, dxh)
        
    return bandsGraph 

############ Make a plot with the largest data point out of two ###########

def getLargestVariationGraph (graph1, graph2) :

    # Protection
    if graph1.GetN() != graph2.GetN() :

        print "Plots don't have the same number of points!"
        print "Error: ", graph1.GetN() 
        print "Middle: ", graph2.GetN() 
        return 0

    largestGraph = graph1.Clone()

    for iPoint in xrange(0, graph1.GetN()) :
        
        # Retrieve points
        graph1PointX = Double(0)
        graph1PointY = Double(0)
        graph1ErrorX = graph1.GetErrorX(iPoint)
        graph1ErrorY = graph1.GetErrorY(iPoint)

        graph1.GetPoint(iPoint,graph1PointX,graph1PointY)
        
        graph2PointX = Double(0)
        graph2PointY = Double(0)
        graph2ErrorX = graph2.GetErrorX(iPoint)
        graph2ErrorY = graph2.GetErrorY(iPoint)

        graph2.GetPoint(iPoint,graph2PointX,graph2PointY)

        # More protection - who needs it!! we are young and we listen to malmsteen and we are obviously fearless
        if graph1PointX - graph2PointX > 0.0001 :
            print "Points are not ordered!"
            print "graph1: ", graph1PointX
            print "graph2: ", graph2PointX
            return 0

        largestPoint = Double(0)
        
        #worst case scenario: one of the two is _completely_ wrong 
        if fabs(fabs(1-graph1PointY) - fabs(1-graph2PointY)) > 1000 :
            print "found bad data point: ", graph1PointX, graph2PointY
            if fabs(1-graph1PointY) > 100 :    
              largestPoint = graph2PointY
              largestPointErrorX = graph2ErrorX
              largestPointErrorY = graph2ErrorY        
            if fabs(1-graph2PointY) > 100 :    
              largestPoint = graph1PointX
              largestPointErrorX = graph1ErrorX
              largestPointErrorY = graph1ErrorY        


        elif fabs(1-graph1PointY) > fabs(1-graph2PointY) :
            largestPoint = graph1PointY
            largestPointErrorX = graph1ErrorX
            largestPointErrorY = graph1ErrorY

        else :
            largestPoint = graph2PointY
            largestPointErrorX = graph2ErrorX
            largestPointErrorY = graph2ErrorY

        print "newpoint:",  largestPoint, largestPointErrorY
        
        #set the point in the new plot
        largestGraph.SetPoint(iPoint, graph1PointX, largestPoint)
        largestGraph.SetPointError(iPoint, largestPointErrorX, largestPointErrorY)

    return largestGraph

#this function returns a plot that contains the variation of a graph wrt 1
def getVariationGraph (graph) :

    varGraph = graph.Clone()

    for iPoint in xrange(0, graph.GetN()) :
        
        # Retrieve points
        graphPointX = Double(0)
        graphPointY = Double(0)
        graphErrorX = graph.GetErrorX(iPoint)
        graphErrorY = graph.GetErrorY(iPoint)

        graph.GetPoint(iPoint,graphPointX,graphPointY)
        
        newPoint = Double(0)

        newPoint = fabs(1-graphPointY)
        newPointErrorX = graphErrorX
        newPointErrorY = graphErrorY

        #set the point in the new plot
        varGraph.SetPoint(iPoint, graphPointX, newPoint)
        varGraph.SetPointError(iPoint, newPointErrorX, newPointErrorY)

    return varGraph


############ Make band function ###########

def makeSingleBandPlot (middleGraph, errorGraphs) :

    bandsGraph = []
    
    for iPoint in xrange(0, middleGraph.GetN()) :

        # Retrieve middle graph for (x,y) coordinates
        dataPointX = Double(0)
        dataPointY = Double(0)
        middleGraph.GetPoint(iPoint,dataPointX,dataPointY)
        #print "DATAPOINT: ", dataPointX

        # Prepare empty arrays
        errorX = Double(0)
        errorY = []

        # Loop over all ErrorGraphs
        index = 0
        for errorGraph in errorGraphs:
            #print "NEW ERRORGRAPH: ", errorGraph.GetName()
            if iPoint==0 :
                bandsGraph += [TGraphAsymmErrors()]
            #print "Point ", iPoint, " Index: ", index    
            thisBandsGraph = bandsGraph[index]
            thisBandsGraph.SetName(middleGraph.GetName())
            thisBandsGraph.SetMarkerStyle(middleGraph.GetMarkerStyle());
            thisBandsGraph.SetMarkerSize(middleGraph.GetMarkerSize());
            thisBandsGraph.SetMarkerColor(middleGraph.GetMarkerColor());
            thisBandsGraph.SetLineColor(middleGraph.GetLineColor());
            
            index+=1

            # Protection
            if errorGraph.GetN() != middleGraph.GetN() :
                print "Plots don't have the same number of points!"
                print "Error: ", errorGraph.GetN() 
                print "Middle: ", middleGraph.GetN() 
                return 0

            # Retrieve errors
            errorPointX = Double(0)
            errorPointY = Double(0)
            errorGraph.GetPoint(iPoint,errorPointX,errorPointY)

            # More protection - who needs it!!
            if dataPointX - errorPointX > 0.0001 :
                print "Points are not ordered!"
                print "dataPointX: ", dataPointX
                print "errorX: ", errorPointX
                return 0
            
            errorX = middleGraph.GetErrorX(iPoint)
            errorY += [fabs(1-errorPointY)]

            # Add things so far in quadrature
            sumErrorY = Double(0)
            for a in errorY:
                #print "adding ", a
                sumErrorY += a*a
            
            sumErrorY = sqrt(sumErrorY)    
            
            if( iPoint < middleGraph.GetN()): 
                thisBandsGraph.SetPoint(iPoint, dataPointX, 0)
                thisBandsGraph.SetPointError(iPoint, errorX, errorX, sumErrorY, sumErrorY)        
                thisBandsGraph.SetPointEYlow(iPoint, 0)

    return bandsGraph[-1:]

def makeSingleBandPlot_error (middleGraph, varGraphs) :

    bandsGraph = []
    #contains all the stuff necessary for propagating errors in each plot
    propagationVarY = []
    
    for iPoint in xrange(0, middleGraph.GetN()) :
        #print "NEW DATA POINT"
        # Retrieve middle graph for (x,y) coordinates
        dataPointX = Double(0)
        dataPointY = Double(0)
        middleGraph.GetPoint(iPoint,dataPointX,dataPointY)

        # Prepare empty arrays
        varX = Double(0)
        varY = []
        #contains all the errors 
        errorY = []

        # Loop over all ErrorGraphs
        index = 0
        for iGraph, varGraph in enumerate(varGraphs):

            #print "NEW GRAPH"

            if iPoint==0 :
                bandsGraph += [TGraphAsymmErrors()]
            #print "Point ", iPoint, " Index: ", index    
            thisBandsGraph = bandsGraph[index]
            thisBandsGraph.SetName(middleGraph.GetName())
            thisBandsGraph.SetMarkerStyle(middleGraph.GetMarkerStyle());
            thisBandsGraph.SetMarkerSize(middleGraph.GetMarkerSize());
            thisBandsGraph.SetMarkerColor(middleGraph.GetMarkerColor());
            thisBandsGraph.SetLineColor(middleGraph.GetLineColor());
            
            index+=1

            # Protection
            if varGraph.GetN() != middleGraph.GetN() :
                print "Plots don't have the same number of points!"
                print "Error: ", varGraph.GetN() 
                print "Middle: ", middleGraph.GetN() 
                return 0

            # Retrieve vars
            varPointX = Double(0)
            varPointY = Double(0)
            varGraph.GetPoint(iPoint,varPointX,varPointY)
            varErrorY = varGraph.GetErrorY(iPoint)
            #print "varErrorY:", varErrorY

            # More protection - who needs it!!
            if dataPointX - varPointX > 0.0001 :
                print "Points are not ordered!"
                print "dataPointX: ", dataPointX
                print "varX: ", varPointX
                return 0
            
            varX = middleGraph.GetErrorX(iPoint)
            varY += [fabs(1-varPointY)]
            errorY += [varErrorY]
            #print "errorY:", errorY 

            # Add things so far in quadrature
            sumVarY = Double(0)
            propagationY = Double(0)

            for var in varY:
                sumVarY += var*var

            sumVarY = sqrt(sumVarY)    
                
            #the square root of sumVarY is now the denominator of the propagation for all plots 
            #now need to multiply it by the single variation
            #and store it in a vector for further use
            for var, error in zip(varY, errorY) :
                if sumVarY > 0.0000001 : propagationY += var*var / sumVarY * error

            #print "squared data point: ", sumVarY
            #print "error: ", propagationY

            propagationY = sqrt(propagationY)    
            
            #print "data point: ", sumVarY
            #print "error: ", propagationY

            #this will happen once per graph, but only the last time will be the one with all the graphs taken into account - that is the one that will stay!
            if( iPoint < middleGraph.GetN()): 
                thisBandsGraph.SetPoint(iPoint, dataPointX, 0)
                thisBandsGraph.SetPointError(iPoint, varX, varX, sumVarY, sumVarY)        
                thisBandsGraph.SetPointEYlow(iPoint, 0)

            #at the last graph, save the error in array
            if (iGraph == len(varGraphs)-1) :
                propagationVarY += [propagationY]

    #transform the first bandsGraph into a TH1F
    #print bandsGraph[-1:]#this is a TGraph in an array

    #pick up the errors calculated previously and assign them to the TH1F
    #print "varY:", varY
    #print "errorY:", errorY
    #print "propagationVarY: ", propagationVarY
    return (bandsGraph[-1:], propagationVarY)
