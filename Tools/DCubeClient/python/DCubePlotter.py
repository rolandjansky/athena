#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DCubeClient/python/DCubePlotter.py
# @author Krzysztof Daniel Ciba (Krzyszotf.Ciba@NOSPAgmail.com)
# @brief implementation of DCubePlotter, DCubeStyle and test_DCubePlotter classes
#
# Bez wzgledu na to jak idiotoodporny jest twoj program, zawsze znajdzie
# sie wiekszy idiota.
#

import os
import uuid
import ROOT
from DCubeUtils import DCubeObject, DCubeException
import unittest

##
# @class DCubePlotter 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief root plotter for DCubeClient package
class DCubePlotter( DCubeObject ):

    ## DCubePlotter TCanvas
    canvas = None

    ## TPaveText with configuration
    configPave = None

    ## additional plot options
    plotOpts = []
    
    ## unzoom factor for yspread
    unzoom = 1.4

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param xmldoc DOM XML Document instance
    # @param parsed tuple with args and opts from option parser
    def __init__( self, xmldoc, parsed):
        super( DCubePlotter, self).__init__( self )
        self.debug( "constructing DCubePlotter...")

        self.xmldoc = xmldoc
        self.opts, self.args = parsed

        self.outputDir = os.path.join( os.path.dirname( self.opts.output ), "plots" )
        self.info("will store all plots in directory %s" % self.outputDir )

        try:
            if (not os.path.exists( self.outputDir ) ):
                os.mkdir( self.outputDir )
             
        except:
            self.error("cannot create output plot directory, plot creation will be disabled")
            self.opts._update_careful( { "makeplots":  False } )


        self.__applyTStyle()

    ## prepare/return ROOT.TCanvas instance
    # @param self "Me, myself and Irene" 
    def __getCanvas( self ):
        
        # if ( self.canvas ): del self.canvas
        # suggested by Time Adye: 1/05/2011
        if ( self.canvas ):
            self.canvas.IsA().Destructor(self.canvas)

        self.canvas = ROOT.TCanvas( "dcube", "dcube" , 600, 600 )
        if ( "logx" in self.plotOpts ):
            self.canvas.SetLogx(1)
        if ( "logy" in self.plotOpts ):
            self.canvas.SetLogy(1)
        if ( "logz" in self.plotOpts ):
            self.canvas.SetLogz(1)
        return self.canvas

    ## set DCube plot dtaw style 
    # @param self "Me, myself and Irene"
    def __applyTStyle( self, name="DCube" ):

        self.debug("setting up DCube plot style...")

        ROOT.gStyle.SetOptTitle(0)
        ROOT.gStyle.SetOptStat(0)
        ROOT.gStyle.SetOptFit(0)
  
        ROOT.gStyle.SetFillStyle(1001)

        ROOT.gStyle.SetMarkerSize( 0.8 )
        ROOT.gStyle.SetMarkerStyle( ROOT.kFullCircle )
        
        ROOT.gStyle.SetLabelFont(42, "xyz")  
        ROOT.gStyle.SetLabelSize(0.03, "xyz")
        
        ROOT.gStyle.SetTitleFont(42, "xyz")
        ROOT.gStyle.SetTitleFontSize(0.04)
        
        ROOT.gStyle.SetTitleFont(42, "a")   

        ROOT.gStyle.SetFuncStyle(1)
        ROOT.gStyle.SetFuncWidth(2)
        
        ROOT.gStyle.SetHistLineColor( ROOT.kBlack )
        ROOT.gStyle.SetHistFillColor( ROOT.kRed )
        ROOT.gStyle.SetHistLineWidth(1)

        ROOT.gStyle.SetPadBorderSize(1)
        ROOT.gStyle.SetPadBorderMode(0)
                
        ROOT.gStyle.SetPadLeftMargin( 0.1 )
        ROOT.gStyle.SetPadBottomMargin( 0.1 )
        ROOT.gStyle.SetPadRightMargin( 0.1 )
        ROOT.gStyle.SetPadTopMargin( 0.1 )
            
        ROOT.gStyle.SetCanvasBorderSize(1)
        ROOT.gStyle.SetCanvasBorderMode(0)
  
        ROOT.gStyle.SetGridStyle(3)
        ROOT.gStyle.SetGridWidth(1)
        
        ROOT.gStyle.SetOptDate(21)
        ROOT.gStyle.GetAttDate().SetTextFont(42)
        ROOT.gStyle.GetAttDate().SetTextSize(0.025)
           
        ROOT.gStyle.SetLegendBorderSize(1)
 
        ROOT.gStyle.SetTextColor( ROOT.kBlack )
        ROOT.gStyle.GetAttDate().SetTextColor( ROOT.kBlack )
        ROOT.gStyle.SetLabelColor(ROOT.kBlack, "xyz")
        ROOT.gStyle.SetTitleColor(ROOT.kBlack, "xyz")
        
        ROOT.gStyle.SetFillColor( ROOT.kWhite )
        ROOT.gStyle.SetMarkerColor( ROOT.kBlue )
        ROOT.gStyle.SetCanvasColor( ROOT.kWhite )    

        ROOT.gStyle.SetFrameFillColor(ROOT.kWhite)
        ROOT.gStyle.SetPadColor( ROOT.kGray )
        ROOT.gStyle.SetTitleColor(390, "a")
        ROOT.gStyle.SetFuncColor(ROOT.kOrange)
        
        ROOT.gStyle.SetGridColor(1)

        ROOT.gStyle.SetCanvasDefH( 800 )
        ROOT.gStyle.SetCanvasDefW( 800 )

        ROOT.gROOT.ForceStyle(1)
        self.debug("done!")
        

    ## get unique plot name
    # @param self "Me, myself and Irene"
    def __plotName( self, what="" ):
        return  what + "_" + str(uuid.uuid4()) + ".png"

    ## plot dispatcher
    # @param self "Me, myself and Irene"
    # @param node DOM XML element
    # @param mon monitored object
    # @param ref reference object
    # @param opts additional plot options
    def plot( self, node, mon=None, ref=None, opts=None ):

        self.mon = self.ref = self.node = None
        
        self.debug("plot called for object of name " + node.getAttribute("name") )

        self.node = node
        self.mon = mon
        self.ref = ref

        
        self.plotOpts = []
        if ( opts != "" ):
            plotOpts = [ opt.strip().lower() for opt in opts.split(";") ] 
            #self.debug("additional plot options are: '%s'" % str(plotOpts) )
           
            for opt in plotOpts:
                if ( opt not in [ "logx", "logy", "logz", "norm" ] ):
                    self.error("ignoring unknown additional plot option '%s'" % opt )
                else:
                    self.plotOpts.append(opt)
            self.debug("additional plot options are: '%s'" % str(plotOpts) )
            

        if ( not self.mon ):
            self.error("monitored object not found, creation of plots skipped!")
            return "FAIL"

        self.name = self.mon.GetName()
        self.title = self.mon.GetTitle()

        if ( not self.ref ):
            self.warn("reference object not found, not all plots will be produced")

        self.className = mon.Class().GetName()

        status = "OK" 
        if ( "TGraph" in self.className ):
            status = self.__plotGraph()
        elif ( "TH1" in self.className ):
            if ( "norm" in self.plotOpts ):
                self.__normTH( self.mon )
                self.__normTH( self.ref )
            status = self.__plotTH1()
        elif ( "TH2" in self.className ):
            if ( "norm" in self.plotOpts ):
                self.__normTH( self.mon )
                self.__normTH( self.ref )
            status = self.__plotTH2()
        elif ( "TProfile" == self.className ):
            if ( "norm" in self.plotOpts ):
                self.__normTH( self.mon )
                self.__normTH( self.ref )
            status = self.__plotProf1D()
        elif ( "TProfile2D" == self.className ):
            if ( "norm" in self.plotOpts ):
                self.__normTH( self.mon )
                self.__normTH( self.ref )
            status = self.__plotProf2D()
        else:
            self.error( "unsupported object name=%s class=%s" % ( self.name, self.className ) )
            status = "FAIL"

        return status

    ## save TCanvas to png file
    # @param self "Me, myself and Irene"
    # @param type string with type attribute
    # @param plot plot file naem prefix
    # @return status string 
    def __saveAs( self, canvas, type, plot ):
        try:
            plotName = self.__plotName( plot )
            absPath = os.path.join( self.outputDir, plotName )
            canvas.Update()
            canvas.SaveAs( absPath )
            imgNode = self.xmldoc.createElement( "img" )
            imgNode.setAttribute( "src" , os.path.join( "plots", plotName ) )
            imgNode.setAttribute( "type", type )
            if ( os.path.exists(absPath) ):
                self.debug("plot '%s' has been created" % absPath ) 
                self.node.appendChild( imgNode )
                return "OK"
            else:
                self.warn("problem when saving plot '%s' to file" % absPath )
                return "WARN"
        except:
            self.warn("unknown error when plotting %s for %s" % ( type, self.name )  )
            
        return "WARN"
        

    ## plotter for TGraph, TGraphError, TGraphAsymmError etc.
    # @param self "Me, myself and Irene"
    def __plotGraph( self ):

        status = [ "OK" ]
        self.debug("plotting %s of type %s" % ( self.name, self.className ) )

        graph = ROOT.TMultiGraph()

        canvas = self.__getCanvas()
        canvas.cd()

        xmin, xmax, ymin, ymax = self.__getMinMaxTGraph( self.mon, self.ref )

        dx = abs(xmax - xmin) * 0.15
        dy = abs(ymax - ymin) 
        
        ROOT.gPad.DrawFrame( xmin - dx, ymin - dy * 0.4, xmax + dx, ymax + dy*self.unzoom)

        ROOT.gStyle.SetPadBottomMargin(0.2)
        
     
        self.mon.SetMarkerColor( ROOT.kRed )
        self.mon.SetFillColor( ROOT.kRed )
        self.mon.SetFillStyle( 3005 )
        self.mon.SetLineColor( ROOT.kRed )
            
                            
        if ( self.ref ):
           
            self.ref.SetMarkerStyle( ROOT.kOpenCircle )
            self.ref.SetMarkerColor( ROOT.kBlue )
            self.ref.SetMarkerSize( 1.0 )        
            self.ref.SetFillColor( ROOT.kBlue )
            self.ref.SetFillStyle( 3004 )
            self.ref.SetLineColor( ROOT.kBlue )
            
            graph.Add( self.ref, "P" ) 

        graph.Add( self.mon, "P") 

        graph.Draw("P" )        

        legend = self.__legend()
        legend.Draw()

        titlePave = self.__titlePave()
        titlePave.Draw()
        
        configPave = self.__configPave()
        configPave.Draw()

        status.append( self.__saveAs( canvas, "reg", "gr" ) )
        
            
        if ( None not in ( self.ref, self.mon ) ):

            canvas.Clear()
            
            diffGraph = ROOT.TGraph()

            diffGraph.SetMarkerColor( ROOT.kRed )
            
            monN = self.mon.GetN()
            refN = self.ref.GetN()

            monXraw = self.mon.GetX()
            monYraw = self.mon.GetY()

            refXraw = self.ref.GetX()
            refYraw = self.ref.GetY()
            
            points = {}
            for i in range( monN ):
                if ( monXraw[i] not in points ):
                    points [ monXraw[i] ] = monYraw[i]        
            for i in range( refN ):
                if ( refXraw[i] not in points ):
                    points[ refXraw[i] ] = refYraw[i]
                else:
                    points[ refXraw[i] ] = points[ refXraw[i] ] - refYraw[i]
            i = 0
            for x in sorted( points.keys() ):
                diffGraph.SetPoint(i, x, points[x] )
                i += 1

            xl = xmin - dx
            xr = xmax + dx

            xmin, xmax, ymin, ymax = self.__getMinMaxTGraph( diffGraph )
            
            dy = abs(ymax - ymin) 

            if ( ymin > ymax ):
                a = ymin
                ymin = ymax
                ymax = ymin
            
            ROOT.gPad.DrawFrame( xl, ymin - (dy * 0.4), xr, ymax + (dy*self.unzoom) )
            
            diffGraph.SetTitle( self.mon.GetTitle() )
            diffGraph.SetName( self.mon.GetName() )

            diffGraph.Draw("P")
                       
            titlePave = self.__titlePave("diff (mon - ref)")
            titlePave.Draw()
            
            configPave = self.__configPave()
            configPave.Draw()

            status.append( self.__saveAs( canvas, "dif", "gd" ) )
            
        return self.__getStatus( status )

    ## get xmin, xmax, ymin, ymax for monitored and reference  TGraphs  
    # @param self "Me, myself and Irene"
    def __getMinMaxTGraph( self, mon=None, ref=None ):

        xmin = xmax = ymin=  ymax = None
        if ( mon ):
            x = ROOT.Double(0)
            y = ROOT.Double(0)

            exl = ROOT.Double(0)
            exr = ROOT.Double(0)
            eyl = ROOT.Double(0)
            eyh = ROOT.Double(0) 

            if ( self.className == "TGraph" ):
                
                for i in range( mon.GetN()  ):
                    
                    mon.GetPoint(i, x, y)
                    
                    if ( xmin == None ): xmin = ROOT.Double(x)
                    if ( xmax == None ): xmax = ROOT.Double(x)
                    if ( ymin == None ): ymin = ROOT.Double(y)
                    if ( ymax == None ): ymax = ROOT.Double(y)
                    
                    if ( x < xmin ): xmin = x
                    if ( x > xmax ): xmax = x
                    if ( y < ymin ): ymin = y
                    if ( y > ymax ): ymax = y
                    
                if ( ref ):
                    for i in range( ref.GetN() ):

                        ref.GetPoint(i, x, y)
                        
                        if ( xmin == None ): xmin = ROOT.Double(x)
                        if ( xmax == None ): xmax = ROOT.Double(x)
                        if ( ymin == None ): ymin = ROOT.Double(y)
                        if ( ymax == None ): ymax = ROOT.Double(y)
                        
                        if ( x < xmin ): xmin = x
                        if ( x > xmax ): xmax = x
                        if ( y < ymin ): ymin = y
                        if ( y > ymax ): ymax = y
            else:

                for i in range( mon.GetN() ):

                    mon.GetPoint(i, x, y)

                    exl = mon.GetErrorXhigh( i )
                    exr = mon.GetErrorXlow( i )
                    eyh = mon.GetErrorYhigh( i )
                    eyl = mon.GetErrorYlow( i )

                    xl = x - exr
                    xr = x + exl
                    yl = y - eyl
                    yh = y + eyh

                    if ( xmin == None ): xmin = ROOT.Double(xl)
                    if ( xmax == None ): xmax = ROOT.Double(xr)
                    if ( ymin == None ): ymin = ROOT.Double(yl)
                    if ( ymax == None ): ymax = ROOT.Double(yh)

                    if ( xl < xmin ): xmin = xl
                    if ( xr > xmax ): xmax = xr
                    if ( yl < ymin ): ymin = yl
                    if ( yh > ymax ): ymax = yh
                    
                if ( ref ):

                    for i in range( ref.GetN() ):

                        ref.GetPoint(i, x, y)
                        
                        exl = ref.GetErrorXhigh( i )
                        exr = ref.GetErrorXlow( i )
                        eyh = ref.GetErrorYhigh( i )
                        eyl = ref.GetErrorYlow( i )
                        
                        xl = x - exr
                        xr = x + exl
                        yl = y - eyl
                        yh = y + eyh

                        if ( xmin == None ): xmin = ROOT.Double(xl)
                        if ( xmax == None ): xmax = ROOT.Double(xr)
                        if ( ymin == None ): ymin = ROOT.Double(yl)
                        if ( ymax == None ): ymax = ROOT.Double(yh)

                        if ( xl < xmin ): xmin = xl
                        if ( xr > xmax ): xmax = xr
                        if ( yl < ymin ): ymin = yl
                        if ( yh > ymax ): ymax = yh


            return ( xmin, xmax, ymin, ymax)
            

    def __normTH( self, obj=None ):
        if ( obj ):
            self.debug( "will normalize histogram '%s'" % obj.GetName())
            integral = obj.Integral()
            if ( integral != 0.0 ):
                scale = 1.0 / integral
                self.debug("integral = %f, scale = %f" % (integral, scale ) )
                obj.Scale( scale )
            else:
                self.warn("cannot normalize histogram '%s', Integral = 0.0!" % obj.GetName())
        
    ## plotter for TH1 histograms
    # @param self "Me, myself and Irene"
    def __plotTH1( self ):

        status = [ "OK" ]
        self.debug("plotting %s of type %s" % ( self.name, self.className ) )


        canvas = self.__getCanvas()
        titlePave = self.__titlePave( )
        title = "%s;%s;%s" % ( self.mon.GetTitle(),
                               self.mon.GetXaxis().GetTitle(),
                               self.mon.GetYaxis().GetTitle() )
        stack = ROOT.THStack( "dcube stack", title )


        
        self.mon.SetMarkerColor( ROOT.kRed )
        
        self.mon.SetLineColor( ROOT.kRed )
        self.mon.SetFillStyle( 3004 )
        self.mon.SetLineColor( ROOT.kRed )

        ymax = self.mon.GetMaximum()
        ymin = self.mon.GetMinimum()
 
        if ( self.ref ):

            self.ref.SetMarkerStyle( ROOT.kOpenCircle )
            self.ref.SetMarkerColor( ROOT.kBlue )
            self.ref.SetMarkerSize( 1.0 )        
    
            self.ref.SetFillColor( ROOT.kBlue )
            self.ref.SetLineColor( ROOT.kBlue )
            self.ref.SetFillStyle( 3005 )
        

            stack.Add( self.ref )

            if ( ymax < self.ref.GetMaximum() ): ymax = self.ref.GetMaximum()
            if ( ymin > self.ref.GetMinimum() ): ymin = self.ref.GetMinimum()

        stack.Add( self.mon )

        #yspread = abs( ymax - ymin )
        #self.panic( "ymin, ymax, yspread %f, %f, %f" % ( ymin, ymax, yspread) )
        #ymax = ymin + ( self.unzoom * yspread )
        #self.panic( "ymax %f" % ymax )

        ymax = ymax * self.unzoom
        if ( ymax == 0.0 ): ymax = ymin + ( abs(ymin) * self.unzoom )
        stack.SetMaximum( ymax  )

        
        config = self.__configPave()
        legend = self.__legend()

        canvas.cd()
        stack.Draw( "NOSTACK"  )
        titlePave.Draw()
        config.Draw()
        legend.Draw()
        pvaluePave = self.__pvaluePave()
        if ( pvaluePave ): pvaluePave.Draw()

        status.append( self.__saveAs( canvas, "reg", "h1r" ) )
        

        canvas = self.__getCanvas()
        diffHist = self.mon.Clone()
        diffHist.Reset()
        diffHist.SetTitle( "diff %s" % title )
        diffHist.SetLineColor( ROOT.kRed )

        if ( self.ref ):
            diffHist.Add( self.ref, self.mon, 1.0, -1.0 )
        else:
            diffHist.Add( self.mon, self.mon, 1.0, -1.0 )

        canvas.Clear()
        canvas.cd()
        
        titlePave = self.__titlePave( "diff" )
        configPave = self.__configPave()
        
        ymax = diffHist.GetMaximum()
        ymin = diffHist.GetMinimum() 

        ymax = ymax * self.unzoom
        if ( ymax == 0.0 ): ymax = ymin + ( abs(ymin) * self.unzoom )
 

        if ( ymax ) : diffHist.SetMaximum( ymax )

        diffHist.Draw( )
        titlePave.Draw()
        configPave.Draw()
        
        status.append( self.__saveAs( canvas, "dif", "h1d" ) )

        return self.__getStatus( status )

    ## plotter for TH2 histograms
    # @param self "Me, myself and Irene"
    def __plotTH2( self ):

        status = [ "OK" ]
        self.debug( "plotting %s of type %s" % ( self.name, self.className) )

        canvas = self.__getCanvas()
        title = "%s;%s;%s;%s" % ( self.mon.GetTitle(),
                                  self.mon.GetXaxis().GetTitle(),
                                  self.mon.GetYaxis().GetTitle(),
                                  self.mon.GetZaxis().GetTitle() )

        configPave = self.__configPave( )
        titlePave = self.__titlePave() 
        pvaluePave = self.__pvaluePave()

        if ( self.ref ):

            bottom = self.mon.Clone()
            top = self.mon.Clone()
            same = self.mon.Clone()

            bottom.Reset()
            bottom.SetLineColor( ROOT.kBlack )
            bottom.SetFillColor( ROOT.kBlue )
            bottom.SetLineWidth( 2 )
            top.Reset()
            top.SetLineColor( ROOT.kBlack )
            top.SetFillColor( ROOT.kRed )
            top.SetLineWidth(2)
            same.Reset()
            same.SetLineColor( ROOT.kBlack )
            same.SetFillColor( ROOT.kGreen )
            same.SetLineWidth( 2)

            for i in range(self.ref.GetNbinsX()+1): 
                for j in range(self.ref.GetNbinsY()+1 ) :             

                    refBin = self.ref.GetBinContent(i, j)
                    monBin = self.mon.GetBinContent(i, j)

                    if ( refBin < monBin ):
                        top.SetBinContent(i, j, monBin - refBin)
                        bottom.SetBinContent( i, j, refBin )
                    elif ( refBin > monBin ):
                        bottom.SetBinContent( i, j, refBin - monBin)
                        top.SetBinContent(i, j, monBin)
                    else:
                        same.SetBinContent(i, j, refBin )
              
            stack = ROOT.THStack( "dcube stack", title )
            stack.Add( same )
            stack.Add( bottom )
            stack.Add( top )
            #stack.UseCurrentStyle()

            legend = ROOT.TLegend(0.1, 0.80, 0.45, 0.72, "", "NDC")
            legend.SetTextFont(102)
            legend.SetTextSize(0.02)
            legend.SetTextColor(1)
            legend.SetBorderSize(1)

            legend.AddEntry( top, "monitored", "F" )
            legend.AddEntry( bottom, "reference", "F" )
            legend.AddEntry( same, "same", "F")

            canvas.cd()
            stack.Draw( "lego1 nostack" )
            titlePave.Draw()
            configPave.Draw()
            legend.Draw()

            if ( pvaluePave ): pvaluePave.Draw()

            canvas.Draw()

        else:
            canvas.cd()
            self.mon.Draw( )
            
            configPave.Draw()
            titlePave.Draw()
            if ( pvaluePave ): pvaluePave.Draw()
            canvas.Draw()
            
        status.append( self.__saveAs( canvas, "reg", "h2r" ) )

        # make diff plot
        canvas = self.__getCanvas()
        diffHist = self.mon.Clone()
        diffHist.Reset()
        diffHist.SetFillColor( ROOT.kRed )
        diffHist.SetLineColor( ROOT.kBlack )

        if ( self.ref ):
            diffHist.Add( self.ref, self.mon, 1.0, -1.0 )
        else:
            diffHist.Add( self.mon, self.mon, 1.0, -1.0 )

        canvas.Clear()           
        diffHist.Draw("LEGO1 0" )
        titlePave = self.__titlePave( "diff" )
        titlePave.Draw()
        configPave.Draw()
        if ( pvaluePave ): pvaluePave.Draw()
        status.append( self.__saveAs( canvas, "dif", "h2d" ) )


        # make projection x plot
        canvas = self.__getCanvas()
        canvas.Clear()
        projX = ROOT.THStack("projectionX", "projectionX")
       
        monPX = self.mon.ProjectionX( "mon px " + str(self.name) )

        if ( "norm" in self.plotOpts ): self.__normTH( monPX )

        monPX.SetFillStyle( 3004 )
        monPX.SetLineColor( ROOT.kRed )
        projX.Add( monPX )

        ymax = monPX.GetMaximum()

        if ( self.ref ):
            refPX = self.ref.ProjectionX( "ref px " + str(self.name) )
            if ( "norm" in self.plotOpts ): self.__normTH( refPX )
            refPX.SetFillStyle( 3005 )
            refPX.SetFillColor( ROOT.kBlue )
            refPX.SetLineColor( ROOT.kBlue )
            if ( ymax < refPX.GetMaximum() ):
                ymax = refPX.GetMaximum()
                
            projX.Add( refPX )

        projX.SetMaximum( ymax * self.unzoom )
        
        canvas.cd()    
        if ( "logx" in self.plotOpts ):
            ROOT.gPad.SetLogx(1)
        if ( "logy" in self.plotOpts ):
            ROOT.gPad.SetLogy(1)
        if ( "logz" in self.plotOpts ):
            ROOT.gPad.SetLogz(1)

        projX.Draw( "NOSTACK"  )

        titlePave = self.__titlePave( "proj X")
        titlePave.Draw()
        if ( pvaluePave ): pvaluePave.Draw()
        configPave.Draw()

        legend = ROOT.TLegend(0.1, 0.80, 0.45, 0.74, "", "NDC")
        legend.SetTextFont(102)
        legend.SetTextSize(0.02)
        legend.SetTextColor(1)
        legend.SetBorderSize(1)

        legend.AddEntry( monPX, "mon projection X", "F" )
        # if self.ref:
        legend.AddEntry( refPX, "ref projection X", "F" )
        legend.Draw()

        status.append( self.__saveAs( canvas, "prx", "h2x" ) )
        
        # make projection y plot
        canvas = self.__getCanvas()
        canvas.cd()    
        if ( "logx" in self.plotOpts ):
            ROOT.gPad.SetLogx(1)
        if ( "logy" in self.plotOpts ):
            ROOT.gPad.SetLogy(1)
        if ( "logz" in self.plotOpts ):
            ROOT.gPad.SetLogz(1)
        canvas.Clear()

        projY = ROOT.THStack("projectionY", "projectionY")
        monPY = self.mon.ProjectionY( "mon py " + str(self.name) )
        if ( "norm" in self.plotOpts ): self.__normTH( monPY )
        monPY.SetFillStyle( 3004 )
        monPY.SetLineColor( ROOT.kRed )
        projY.Add( monPY )

        ymax = monPY.GetMaximum()

        if ( self.ref ):
            refPY = self.ref.ProjectionY( "ref py " + str(self.name) )
            if ( "norm" in self.plotOpts ): self.__normTH( refPY )
            refPY.SetFillStyle( 3005 )
            refPY.SetFillColor( ROOT.kBlue )
            refPY.SetLineColor( ROOT.kBlue )
            if ( ymax < refPY.GetMaximum() ):
                ymax = refPY.GetMaximum()
            
            projY.Add( refPY )

        projY.SetMaximum( ymax * self.unzoom )
            
        canvas.cd()    
         
        projY.Draw("NOSTACK" )

        titlePave = self.__titlePave( "proj Y")
        titlePave.Draw()
        configPave.Draw()
        if ( pvaluePave ): pvaluePave.Draw()


        legend = ROOT.TLegend(0.1, 0.80, 0.45, 0.74, "", "NDC")
        legend.SetTextFont(102)
        legend.SetTextSize(0.02)
        legend.SetTextColor(1)
        legend.SetBorderSize(1)

        legend.AddEntry( monPY, "mon projection Y", "F" )

        # if self.ref:
        legend.AddEntry( refPY, "ref projection Y", "F" )
        legend.Draw()


        status.append( self.__saveAs( canvas, "pry", "h2y" ) )
        
        return self.__getStatus( status )

    ## plotter for 1D TProfile
    # @param self "Me, myself and Irene"
    def __plotProf1D( self ):
        status = [ ]
        self.debug("plotting %s of type %s" % ( self.name, self.className ) )

        canvas = self.__getCanvas()

        titlePave = self.__titlePave()
        configPave = self.__configPave()
        legend = self.__legend()

        self.mon.SetMarkerColor( ROOT.kRed )
        self.mon.SetMarkerSize( 0.9 )        
        self.mon.SetFillColor( ROOT.kRed )
        self.mon.SetFillStyle( 3005 )
        self.mon.SetLineColor( ROOT.kRed )
        
        ymax = self.mon.GetMaximum()
        ymin = self.mon.GetMinimum()
        if ( self.ref ):
            self.ref.SetMarkerStyle( ROOT.kOpenCircle )
            self.ref.SetMarkerColor( ROOT.kBlue )
            self.ref.SetMarkerSize( 1.0 )        
            self.ref.SetFillColor( ROOT.kBlue )
            self.ref.SetFillStyle( 3004 )
            self.ref.SetLineColor( ROOT.kBlue )

            if ( ymax < self.ref.GetMaximum() ):
                ymax = self.ref.GetMaximum()
            if ( ymin > self.ref.GetMinimum() ):
                ymin = self.ref.GetMinimum()

    
        #yspread = abs( ymax)  + abs( ymin )
        #ymax = ymin + ( self.unzoom * yspread )
        ymax = ymax * self.unzoom
        if ( ymax == 0.0 ): ymax = ymin + ( abs(ymin) * self.unzoom )
 

        self.mon.SetMaximum( ymax  )
        if ( self.ref ): self.ref.SetMaximum( ymax  )

        if ( self.ref ): self.ref.Draw( )
        self.mon.Draw( "SAME" )
       
        titlePave.Draw()
        configPave.Draw()
        legend.Draw()
            
        status.append( self.__saveAs( canvas, "reg", "p1r" ) )

        canvas = self.__getCanvas()
        canvas.Clear()
        canvas.cd()
        
        diffProfile = self.mon.Clone()
        diffProfile.Reset()
        if ( self.ref ):
            diffProfile.Add( self.ref, self.mon, 1.0, -1.0 )
        else:
            diffProfile.Add( self.mon, self.mon, 1.0, -1.0 )

        titlePave = self.__titlePave( "diff" )

        ymin = diffProfile.GetMinimum()
        ymax = diffProfile.GetMaximum()

        ymax = ymax * self.unzoom
        if ( ymax == 0.0 ): ymax = ymin + ( abs(ymin) * self.unzoom )
 
        diffProfile.SetMaximum( ymax )
        

        diffProfile.Draw( )
        titlePave.Draw()
        configPave.Draw()
        
        status.append( self.__saveAs( canvas, "dif", "p1d" ) )
        
        return self.__getStatus( status ) 

    ## plotter for TProfile2D 
    # @param self "Me, myself and Irene"
    def __plotProf2D( self ):
        status = [ ]
        self.debug("plotting %s of type %s" % ( self.name, self.className ) )

        canvas = self.__getCanvas()

        titlePave = self.__titlePave()
        configPave = self.__configPave()
        legend = self.__legend()
     
        self.mon.SetMarkerColor( ROOT.kRed )
        self.mon.SetMarkerSize( 0.9 )        
        self.mon.SetFillColor( ROOT.kRed )
        self.mon.SetFillStyle( 3005 )
        self.mon.SetLineColor( ROOT.kBlack )

        self.mon = ROOT.TH2D( self.mon )
        
        title = "%s;%s;%s;%s" % ( self.mon.GetTitle(),
                                  self.mon.GetXaxis().GetTitle(),
                                  self.mon.GetYaxis().GetTitle(),
                                  self.mon.GetZaxis().GetTitle() )
        
        
        ymax = self.mon.GetMaximum()
        if ( self.ref ):
            
            self.ref = ROOT.TH2D( self.ref )
            
            bottom = self.ref.Clone()
            top = self.ref.Clone()
            same = self.ref.Clone()

            bottom.Reset()
            bottom.SetLineColor( ROOT.kBlack )
            bottom.SetFillColor( ROOT.kBlue )
            bottom.SetLineWidth( 2 )
            top.Reset()
            top.SetLineColor( ROOT.kBlack )
            top.SetFillColor( ROOT.kRed )
            top.SetLineWidth(2)
            same.Reset()
            same.SetLineColor( ROOT.kBlack )
            same.SetFillColor( ROOT.kGreen )
            same.SetLineWidth( 2 )

            for i in range(self.ref.GetNbinsX()+1): 
                for j in range(self.ref.GetNbinsY()+1 ) :             

                    refBin = self.ref.GetBinContent(i, j)
                    monBin = self.mon.GetBinContent(i, j)
            
                    if ( refBin < monBin ):
                        top.SetBinContent(i, j, monBin - refBin)
                        bottom.SetBinContent( i, j, refBin )
                    elif ( refBin > monBin ):
                        bottom.SetBinContent( i, j, refBin - monBin)
                        top.SetBinContent(i, j, monBin)
                    else:
                        same.SetBinContent(i, j, refBin )
              
            stack = ROOT.THStack( "dcube stack", title )
            stack.Add( same )
            stack.Add( bottom )
            stack.Add( top )
            
            legend = ROOT.TLegend(0.1, 0.80, 0.45, 0.72, "", "NDC")
            legend.SetTextFont(102)
            legend.SetTextSize(0.02)
            legend.SetTextColor(1)
            legend.SetBorderSize(1)

            legend.AddEntry( top, "monitored", "F" )
            legend.AddEntry( bottom, "reference", "F" )
            legend.AddEntry( same, "same", "F")

            canvas.cd()
            stack.Draw( "LEGO1 0 NOSTACK"  )
            titlePave.Draw()
            configPave.Draw()
            legend.Draw()

            pvaluePave = self.__pvaluePave()
            if ( pvaluePave ): pvaluePave.Draw()
            canvas.Draw()

        else:            
            canvas.cd()
            self.mon.Draw( )
            configPave.Draw()
            titlePave.Draw()
            canvas.Draw()
            
        status.append( self.__saveAs( canvas, "reg", "p2r" ) )

        canvas = self.__getCanvas()
        canvas.Clear()
        canvas.cd()
        
        diffProfile = self.mon.Clone()
        diffProfile.Reset()
        diffProfile.SetFillColor( ROOT.kRed )
        diffProfile.SetLineColor( ROOT.kBlack )

        if ( self.ref ):
            diffProfile.Add( self.ref, self.mon, 1.0, -1.0 )
        else:
            diffProfile.Add( self.mon, self.mon, 1.0, -1.0 )

        titlePave = self.__titlePave( "diff" )
        diffProfile.Draw( "LEGO1 0" )
        titlePave.Draw()
        configPave.Draw()
        
      
        status.append( self.__saveAs( canvas, "dif", "p2d" ) )
        
        return self.__getStatus( status ) 
        
       


    ## get status string from a list of strings
    # @param self "Me, myself and Irene"
    # @param sl list of strings
    def __getStatus( self, sl ):
        if ( "FAIL" in sl ): return "FAIL"
        if ( "WARN" in sl ): return "WARN"
        return "OK"

    ## runtime configuration pave
    # @param self "Me, myself and Irene"
    def __configPave( self ):
        if ( not self.configPave ):
            self.configPave = ROOT.TPaveText( 0.1, 0.9, 0.45, 0.8, "NDC" )
            self.configPave.SetBorderSize( 1 )
            self.configPave.SetTextColor( 1 )
            self.configPave.SetTextSize( 0.02 )
            self.configPave.SetTextFont( 102 )
            self.configPave.SetTextAlign( 12 ) 
            self.configPave.AddText( "branch:  " + self.opts.branch  )
            self.configPave.AddText( "install: " + self.opts.install   )
            self.configPave.AddText( "cmt:     " + self.opts.cmtconfig )
            self.configPave.AddText( "project: " + self.opts.project   )
            self.configPave.AddText( "jobId:   " + self.opts.jobId   )
        return self.configPave

    ## p-value pave
    # @param self "Me, myself and Irene"
    def __pvaluePave( self ):
        pvaluePave = ROOT.TPaveText( )
        pvaluePave.SetBorderSize( 1 )
        pvaluePave.SetTextColor( 1 )
        pvaluePave.SetTextSize( 0.02 )
        pvaluePave.SetTextFont( 42 )
        pvaluePave.SetTextAlign( 22 )

        lines = 0
        pvalues = self.node.getElementsByTagName( "pvalue" )
        for pvalue in pvalues:
            test = pvalue.getAttribute( "test" )
            status = pvalue.getAttribute( "status" )
            pval = self.__getCData( pvalue.childNodes )
            text = "p-value = %s" % pval
            
           
            if test == "chi2": text = "#chi^{2} %s" % text
            else:
                text = "%-3s  %s" % (test, text)
            
            
            text = pvaluePave.AddText( text  )
            if ( status == "FAIL" ):
                text.SetTextColor( ROOT.kRed )
            if ( status == "WARN" ):
                text.SetTextColor( ROOT.kOrange )
            lines += 1

        if ( lines ):
            Y2NDC = 0.9 - (lines*0.021)
            pvaluePave.SetX1NDC( 0.69 )
            pvaluePave.SetX2NDC( 0.9  )
            pvaluePave.SetY1NDC( 0.9  )
            pvaluePave.SetY2NDC( 0.9 - (lines*0.021) )
            return pvaluePave
            
        
    ## title pave
    # @param self "Me, myself and Irene"
    def __titlePave( self, what="" ):
        titlePave = ROOT.TPaveText( 0.0, 1.0, 1.0, 0.93 , "NDC")
        titlePave.SetBorderSize( 0 )
        titlePave.SetTextColor( ROOT.kBlack )
        titlePave.SetTextSize( 0.028 )
        titlePave.SetTextFont( 42 )
        titlePave.SetFillColor( ROOT.kGray )
        titlePave.SetTextAlign( 12 )
        titlePave.AddText( "title: %s" % self.title  ) 
        #titlePave.AddText( "name: %s" % self.name ) 
        if ( what == "" ): what = "normal"
        titlePave.AddText( "%s plot" % what  ) 
        return titlePave
    
    ## plot legend
    # @param self "Me, myself and Irene"
    def __legend( self ):
        legend = ROOT.TLegend(0.1, 0.80, 0.45, 0.75, "", "NDC")
        legend.SetTextFont(42)
        legend.SetTextSize(0.02)
        legend.SetTextColor( ROOT.kBlack )
        legend.SetBorderSize(1)

        if ( "TH" in self.className ):
            legend.AddEntry( self.mon, "monitored" )#"f" )
            if ( self.ref ): legend.AddEntry( self.ref, "reference" )#, "f" )
        else:
            legend.AddEntry( self.mon, "monitored", "pe")
            if ( self.ref ): legend.AddEntry( self.ref, "reference", "pe")
            
        return legend

    ## CDATA getter 
    # @param self "Me, myself and Irene"
    # @param nodeList list of DOM nodes 
    def __getCData( self, nodeList ):
        cdata = ""
        for node in nodeList:
            if ( node.nodeType == node.TEXT_NODE ):
                cdata += node.data
        return cdata
##
# @class test_DCubePlotter
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DCubePlotter
class test_DCubePlotter( unittest.TestCase ):

    ## test case setup
    # @param self "Me, myself and Irene"
    def setUp( self ):        
        pass

    ## c'tor
    # @param self "Me, myself and Irene"
    def test_01_ctor( self ):
        try:
            self.plotter =  DCubePlotter()
        except:
            pass
        self.assertEqual( isinstance( self.plotter, DCubePlotter), True )
            
    ## plot diff objects
    # @param self "Me, myself and Irene"
    def test_02_plot( self ):
        pass
    

# test suite execution
if __name__ == "__main__":

    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DCubePlotter)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    
