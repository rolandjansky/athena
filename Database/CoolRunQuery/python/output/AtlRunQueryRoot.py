#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryRoot.py
# Project: AtlRunQuery
# Purpose: ROOT interactions
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Dec 13, 2008
# ----------------------------------------------------------------
#
# ---------------------------------------------------------------------------------------------------
# ROOT TTree making and plotting
# ---------------------------------------------------------------------------------------------------

from __future__ import print_function
import datetime, sys, os
from array import array
from CoolRunQuery.utils.AtlRunQueryUtils  import importroot
importroot() # needed before any root import to switch to batch mode
from ROOT import TFile, TTree, TCanvas, gPad, TGraph, TColor, TStyle, TGaxis, gROOT, gStyle, TText, TH1F, TLegend, TF1

# ---------------------------------------------------------------------------------------------------
# Creation of ROOT output file
# ---------------------------------------------------------------------------------------------------

# general style
def SetStyle( whiteCanvas = False ):

    # colors
    c_Canvas = TColor.GetColor( "#f0f0f0" ) # TColor.GetColor( "#e9e6da" )
    if whiteCanvas:
        c_Canvas = TColor.GetColor( 10 )
    c_FrameFill      = 10 
    c_TitleBox       = TColor.GetColor( "#dae1e6" )

    myStyle = TStyle( gROOT.GetStyle("Plain") )
    myStyle.SetName("myStyle")

    myStyle.SetLineStyleString( 5, "[52 12]" )
    myStyle.SetLineStyleString( 6, "[22 12]" )
    myStyle.SetLineStyleString( 7, "[22 10 7 10]" )

    # pretty colors
    myStyle.SetPalette(1)

    # use plain black on white colors
    myStyle.SetFrameBorderMode(0)
    myStyle.SetCanvasBorderMode(0)
    myStyle.SetPadBorderMode(0)
    myStyle.SetPadColor(0)
    myStyle.SetFillStyle(0)

    myStyle.SetLegendBorderSize(0)

    myStyle.SetTitleFillColor( c_TitleBox )
    myStyle.SetFrameFillColor( c_FrameFill )
    myStyle.SetCanvasColor( c_Canvas )

    # set the paper & margin sizes
    myStyle.SetPaperSize(20,26)
    myStyle.SetPadTopMargin(0.11)
    myStyle.SetPadRightMargin(0.05)
    myStyle.SetPadBottomMargin(0.11)
    myStyle.SetPadLeftMargin(0.12)
    
    # use bold lines and markers
    myStyle.SetMarkerStyle(21)
    myStyle.SetMarkerSize(0.3)
    myStyle.SetHistLineWidth(2)
    myStyle.SetLineStyleString(2,"[12 12]") # postscript dashes
    
    # do not display any of the standard histogram decorations
    myStyle.SetOptTitle(1)
    myStyle.SetTitleH(0.052)

    myStyle.SetOptStat(0)
    myStyle.SetOptFit(0)
    
    # put tick marks on top and RHS of plots
    myStyle.SetPadTickX(1)
    myStyle.SetPadTickY(1)

    # use this style
    gROOT.GetListOfStyles().Add(myStyle)
    gROOT.SetStyle("myStyle")

def SaveGraphsToFile( filename, varnames, xvecs, yvecs, addparamName = None, addparamVal = None ):
    f = TFile.Open( filename, 'RECREATE' )
    if len(xvecs) != len(yvecs) or len(varnames) != len(xvecs):
        print ('ERROR: wrong dimensions in "AtlRunQueryRoot.SaveGraphsToFile"')
        return
    for ik in range(len(xvecs)):
        xv = xvecs[ik]
        yv = yvecs[ik]
        g = TGraph( len(xv) )
        g.SetName ( "graph%i" % (ik+1) )
        g.SetTitle( varnames[ik] )
        for i in range(len(xv)):
            g.SetPoint( i, xv[i], yv[i] )
        g.Write()

    # save additional parameter
    if addparamName is not None:
        addparam = TF1( addparamName, "[0]", 0, 1 )
        addparam.SetParameter( 0, addparamVal )
        addparam.Write()
        
    f.Close()

def MakePlots( tree, datapath ):

    # batch mode -> no windows

    gROOT.SetBatch( 1 )
    if tree is None:
        print ('ERROR: input tree is None')
        return

    # style --------------------------------------------
    #steelBlue  = TColor.GetColor( "#4863A0" )
    #slateBlue  = TColor.GetColor( "#574EC7" )
    dogerBlue  = TColor.GetColor( "#1569C7" )
    dogerBlue3 = TColor.GetColor( "#1569C7" )
    graphCol  = dogerBlue
    markerCol = dogerBlue3

    SetStyle()
    # --------------------------------------------------

    # reference 
    reflist = [ 'Run' ]

    # types that can be plotted
    validtypes = [ 'int', 'float', 'double', 'short', 'long' ]

    # get list of leaves for plotting
    leafList = tree.GetListOfLeaves()
    varlist = []
    
    for leaf in leafList:
        if leaf is not None:
            # check type, only values are plotted
            typename = leaf.GetTypeName().lower()
            valid = False
            for vt in validtypes:
                if vt in typename:
                    valid = True
            if valid:
                varlist.append( leaf.GetName() )

    # plot
    TGaxis.SetMaxDigits(6)
    c = []
    g = []
    fnames = []
    hnames = []
    for ref in reflist:
        # sanity check
        if ref not in varlist:
            print ('Big troubles in "MakePlots" --> reference variable "%s" not in TTree' % ref)
            sys.exit(1)

        for var in varlist:
            if var == ref:
                continue

            vreg = var.replace('-','_')
            rreg = ref.replace('-','_')
            
            hname = "h_%s_%s" % (ref, var)
            c.append( TCanvas( "c_%s" % hname, "ATLAS Run Query: %s vs. %s" % (var, ref), 0, 0, 600, 500 ) )
            tree.Draw( "%s:%s" % (vreg, rreg), "", "goff" )
            g.append( TGraph( tree.GetSelectedRows(), tree.GetV2(), tree.GetV1() ) )
            g[-1].SetTitle( "ATLAS Run Query: %s vs. %s" % (var, ref) )
            g[-1].SetMarkerStyle( 20 )
            g[-1].SetMarkerColor( markerCol )
            g[-1].SetMarkerSize( 1.2 )
            g[-1].SetLineWidth( 2 )
            g[-1].SetLineColor( graphCol )
            g[-1].GetXaxis().SetTitleOffset( 1.25 )
            g[-1].GetYaxis().SetTitleOffset( 1.75 )
            g[-1].GetXaxis().SetTitle( ref )
            g[-1].GetYaxis().SetTitle( var )
            g[-1].GetXaxis().SetLabelOffset( 0.012 )
            g[-1].GetYaxis().SetLabelOffset( 0.012 )
            g[-1].GetXaxis().SetNdivisions( 506 )

            # some global style settings
            scale = 1
            gPad.SetTicks()
            gPad.SetLeftMargin  ( 0.138*scale )
            gPad.SetRightMargin ( 0.050*scale )
            gPad.SetBottomMargin( 0.120*scale  )
            
            g[-1].Draw("alp")
                      
            c[-1].Update()
            fnames.append( '%s/atlrunquery_%s.png' % (datapath, hname) )
            hnames.append( '%s versus %s' % (var, ref ) )            
            c[-1].Print( fnames[-1] )

    return hnames, fnames
    
# ---------------------------------------------------------------------------------------------------
# Creation of ROOT output file
# ---------------------------------------------------------------------------------------------------

def makeLBPlotSummaryForLHC( lbrange, xvecStb, yvec, runNr, datapath, printText = '' ):

    plotDevelop = False
    if plotDevelop:
        # pickle input
        import json
        store = (list(lbrange), xvecStb, yvec, runNr, datapath)
        pf = open( '%s/plotinput.json' % datapath, 'w' )
        try:
            json.dump(store, pf)
        except Exception as ex:
            print ('ERROR: could not store plotinput data')
            print("Reason: %s" % ex)
            sys.exit(1)
        pf.close()

    # sanity check
    if not lbrange or len(yvec)==0 or not yvec[0]:
        return "None"

    ylegend = ['Intensity Beam-1', 'Intensity Beam-2', 'Beam energy', 'Online inst. luminosity' ]

    SetStyle()    
    name  = 'LHCsummary_vs_lb_run_%i' % (runNr)
    #print ("Attempt printing", name)
    title = 'LHC summary vs. LB for run_%i' % (runNr)
    szescale = 1.2
    c = TCanvas( name, title, 0, 0, int(530*szescale), int(400*szescale) )
    c.GetPad(0).SetTopMargin(0.13)
    c.GetPad(0).SetLeftMargin(0.09)
    c.GetPad(0).SetRightMargin(1.3)
    # c.GetPad(0).SetGrid()
    x1 = lbrange[0]
    x2 = lbrange[-1]

    h  = TH1F( name, title, x2 - x1 + 1, x1, x2 + 1 ) # for stable beams
    h.GetXaxis().SetTitle( 'Luminosity block number' )
    h.GetXaxis().SetTitleOffset( 1.3 )
    h.GetYaxis().SetTitle( 'Beam intensity (10^{11} protons)' )
    h.GetYaxis().SetTitleOffset( 1.0 )
    h.SetTitle( title )    

    hg = []
    for iy,y in enumerate(yvec):
        hg.append( TH1F( name + 'g%i' % iy, title, x2 - x1 + 1, x1, x2 + 1 ) )
        for lb in lbrange:
            hg[iy].SetBinContent( lb, y[lb] )
    
    # first draw histogram        
    ymax = max(hg[0].GetMaximum(),hg[1].GetMaximum())
    y2 = ymax*1.3
    if y2 <= 0:
        y2 = 1
    h.SetMinimum(0)
    h.SetMaximum(y2) 
    h.Draw("0")
    gPad.SetTicks(1, 0)

    # beam energy (rescale)
    ebcol = TColor.GetColor( "#306754" )
    heb = hg[2]
    heb.Scale( 1./1000.0, "nosw2" ) # in TeV
    if heb.GetMaximum()==0:
        heb.SetMaximum(1)
    ebmax = heb.GetMaximum()*1.3

    # one more sanity check
    if ebmax == 0:
        return "None"
    
    heb.SetLineColor( ebcol )
    heb.SetLineWidth( 1 )
    scale = h.GetMaximum()/ebmax
    heb.SetFillColor( TColor.GetColor("#C3FDB8") )
    heb.Scale( scale, "nosw2" )
    heb.Draw("same")

    if xvecStb:
        for lb in lbrange:
            if lbrange[lb] in xvecStb:
                h.SetBinContent( lbrange[lb], yvec[2][lb]*scale/1000.0 ) # draw for beam energy (in TeV)
        h.SetFillColor( TColor.GetColor( "#63bD58" ) )
        h.SetFillStyle( 3007 )
        h.SetLineColor( TColor.GetColor( "#C3FDB8" ) )
        h.SetLineWidth( 1 )
        h.SetLineStyle( 1 )
        h.Draw("same")
        hebp = TH1F( heb )
        hebp.SetFillColor( 0 )
        hebp.Draw("same")
        

    colorList = [TColor.GetColor( "#255EC7" ), TColor.GetColor( "#E42217" ), TColor.GetColor( "#2212EE" ), TColor.GetColor( "#22EE33" )]

    for ig in range(2):
        if ig < len(colorList):
            hg[ig].SetLineColor( colorList[ig] )
        hg[ig].SetLineWidth( 2 )
        hg[ig].Draw("same")

    # logo
    now = str(datetime.datetime.today())
    t = TText( h.GetXaxis().GetXmax(), y2*1.01, 'ATLAS Run Query %s' % now[:now.find('.')] )
    t.SetTextAlign( 31 )
    t.SetTextSize( 0.035 )
    t.SetTextSize( 0.030 )
    t.SetTextColor( TColor.GetColor( "#888888" ) )
    t.Draw()

    drawLegend = False
    dx = 1 - c.GetRightMargin() - c.GetLeftMargin() - 0.05
    dy = 0.1
    legend = TLegend( 1 - c.GetRightMargin() - dx, 1 - c.GetTopMargin() - dy - 0.035,
                      1 - c.GetRightMargin(), 1 - c.GetTopMargin() - 0.035)
    # legend.SetFillStyle( 1001 )
    # legend.SetFillColor( TColor.GetColor( "#FFFFFF" ) )
    legend.SetNColumns(2)
    legend.SetBorderSize( 1 )
    legend.SetMargin( 0.15 )
    legend.SetLineColor( 0 )
    legend.SetTextColor( 1 )
    for ig,hgg in enumerate(hg):
        if ylegend[ig]:
            opt = "l"
            if ig == 2:
                opt = "FL"
            legend.AddEntry( hgg, ylegend[ig], opt )
            drawLegend = True
        
    if xvecStb:
        legend.AddEntry( h,  "LBs with stable beams","F" )
        drawLegend = True
    if drawLegend:
        legend.Draw("same")

    # redraw axis
    h.Draw( "sameaxis" )
    
    # draw beam energy axis on the right side
    if h.GetMaximum() > 0:
        axis = TGaxis( x2 + 1, 0, x2 + 1, h.GetMaximum(), 0, ebmax, 510, "+L" )
        axis.SetLabelOffset( 0.007 )
        axis.SetTitleOffset( 1.2 )
        axis.SetTitleSize( h.GetXaxis().GetTitleSize() )
        axis.SetLabelSize( 0.04 )
        axis.SetLineColor( ebcol )
        axis.SetTitleColor( ebcol )
        axis.SetLabelColor( ebcol )
        axis.SetTitle( "Beam energy (TeV)" )
        axis.Draw()

    c.Update()
    fnames = '%s/atlrunquery_%s.png' % (datapath, name) 
    c.Print( fnames )
    #print ("Printing",fnames)
    
    return fnames

def makeTimePlotList( xvec, xveclb, yvec, toffset, dt, ymin, ymax,
                      xtit, ytit, ylegend, name, title, datapath, printText = '' ):

    # sanity check
    if not xvec or len(yvec)==0 or not yvec[0]:
        return "None"

    SetStyle()

    gStyle.SetPadTickX(0)
    gStyle.SetPadTickY(1)
    
    c = TCanvas( name, title, 0, 0, 530, 430 )
    c.GetPad(0).SetTopMargin(0.22)
    c.GetPad(0).SetGrid()

    # create the graphs
    graphs = []
    y1 = 1e30
    y2 = -1e30
    for ig in range(len(yvec)):
        n = len(yvec[ig])
        g = TGraph( n )
        for i in range(n):
            g.SetPoint( i, xvec[ig][i], yvec[ig][i] )        
            if ymin == ymax:
                y1 = min(y1,yvec[ig][i])
                y2 = max(y2,yvec[ig][i])
        graphs.append( g )

    if ymin != ymax:
        y1 = ymin
        y2 = ymax

    # create the frame
    frame  = TH1F( name, title, int(dt/2), 0, dt )

    # x-axis is time format
    frame.GetXaxis().SetTimeDisplay(1)   
    # use rather default: frame.GetXaxis().SetTimeFormat("%d-%Hh")

    # =================================
    # toffset += 7*3600 # BUG in ROOT ???
    # =================================

    frame.GetXaxis().SetTimeOffset(toffset, "gmt") # assumes time given was local

    frame.GetXaxis().SetTitle( xtit )
    frame.GetXaxis().SetTitleOffset( 1.3 )
    frame.GetYaxis().SetTitle( ytit )
    frame.GetYaxis().SetTitleOffset( 1.4 )
    frame.SetTitle( title )    
    frame.SetMinimum(y1 - (y2-y1)*0.1)
    frame.SetMaximum(y2 + (y2-y1)*0.3)
    frame.Draw()
    
    colorList = [TColor.GetColor( "#154EB7" ), TColor.GetColor( "#82CAFF" ), TColor.GetColor( "#D11B17" ), TColor.GetColor( "#FF9A4D" )]
    styleList = [1,1,1,1]

    for ig,g in enumerate(graphs):
        if ig < len(colorList):
            g.SetLineColor( colorList[ig] )
            g.SetLineStyle( styleList[ig] )
        g.SetLineWidth( 2 )
        g.Draw("same")

    if printText:
        t = TText( frame.GetXaxis().GetXmax(), y2*1.01, printText )
        t.SetTextAlign( 31 )
        t.SetTextSize( 0.035 )
        t.Draw()

    # logo
    now = str(datetime.datetime.today())
    tlogo = TText( frame.GetXaxis().GetXmax()*1.025 - frame.GetXaxis().GetXmin()*0.025, y1,
                   'ATLAS Run Query %s' % now[:now.find('.')] )
    tlogo.SetTextSize( 0.030 )
    tlogo.SetTextColor( TColor.GetColor( "#aaaaaa" ) )
    tlogo.SetTextAngle(90)
    tlogo.Draw()

    # axis for LBs
    lbcol = TColor.GetColor( "#777777" )
    axis = TGaxis( frame.GetXaxis().GetXmin(), frame.GetMaximum(),
                   frame.GetXaxis().GetXmax(), frame.GetMaximum(),
                   1, len(xveclb), 505, "-L" )
    axis.SetLabelOffset( frame.GetXaxis().GetLabelOffset() )
    axis.SetTitleOffset( 1.4 )
    axis.SetTitleSize( frame.GetXaxis().GetTitleSize() )
    axis.SetLabelSize( frame.GetXaxis().GetLabelSize() )
    axis.SetLineColor( lbcol )
    axis.SetTitleColor( lbcol )
    axis.SetLabelColor( lbcol )
    axis.SetTitle( "Luminosity block number (indicative)" )
    axis.Draw()

    # legend
    drawLegend = False
    dx = 1 - c.GetRightMargin() - c.GetLeftMargin() - 0.05
    dy = 0.1
    legend = TLegend( 1 - c.GetRightMargin() - dx, 1 - c.GetTopMargin() - dy - 0.035,
                      1 - c.GetRightMargin(), 1 - c.GetTopMargin() - 0.035)
    # legend.SetFillStyle( 1001 )
    # legend.SetFillColor( TColor.GetColor( "#FFFFFF" ) )
    legend.SetNColumns(2)
    legend.SetBorderSize( 1 )
    legend.SetMargin( 0.15 )
    legend.SetLineColor( 0 )
    legend.SetTextColor( 1 )
    legend.SetTextSize( 0.037 )
    for ig,g in enumerate(graphs):
        if ylegend[ig]:
            h = g.GetHistogram()
            h.SetLineColor( colorList[ig] )
            h.SetLineStyle( styleList[ig] )
            h.SetLineWidth( 2 )
            legend.AddEntry( h, ylegend[ig], "l" )
            drawLegend = True        
    if drawLegend:
        legend.Draw("same")

    # redraw axis
    frame.Draw( "sameaxis" )

    c.Update()
    fnames = '%s/atlrunquery_%s.png' % (datapath, name) 
    c.Print( fnames )
    
    return fnames


def makeLBPlotList( xvec, xvecStb, yvec, xtitle, ytitle, ylegend, histname, histtitle, datapath, printText = '', ymin = None , ymax = None ): 

    # sanity check
    if not xvec or len(yvec)==0 or not yvec[0]:
        return "None"

    SetStyle()    
    c = TCanvas( histname, histtitle, 0, 0, 530, 400 )
    c.GetPad(0).SetTopMargin(0.13)
    c.GetPad(0).SetGrid()
    x1 = xvec[0]
    x2 = xvec[-1]

    h  = TH1F( histname, histtitle, x2 - x1 + 1, x1, x2 + 1 ) # for stable beams
    hg = []
    for iy in range(len(yvec)):
        hg.append( TH1F( histname + 'g%i' % iy, histtitle, x2 - x1 + 1, x1, x2 + 1 ) )
    h.GetXaxis().SetTitle( xtitle )
    h.GetXaxis().SetTitleOffset( 1.3 )
    h.GetYaxis().SetTitle( ytitle )
    h.GetYaxis().SetTitleOffset( 1.4 )
    h.SetTitle( histtitle )


    for i in range(len(xvec)):
        for iy,y in enumerate(yvec):
            if type(y[i]) == tuple:
                val,valerr = y[i]
            else:
                val,valerr = y[i],0
            hg[iy].SetBinContent( xvec[i], val )
            hg[iy].SetBinError( xvec[i], valerr )
        if xvec[i] in xvecStb:
            h.SetBinContent( xvec[i], yvec[0][i] )

    if not ymax:
        ymax = max([h.GetMaximum() for h in hg])
        if ymax <= 0:
            ymax = 1
        ymax *= 1.2

    if not ymin:
        ymin=0

    # first draw histogram
    h.SetMinimum(ymin)
    h.SetMaximum(ymax)
    if xvecStb:
        h.SetFillColor( TColor.GetColor( "#98AFC7" ) )
        h.SetLineColor( TColor.GetColor( "#788FA7" ) )
        h.SetLineWidth( 1 )
        h.SetLineStyle( 1 )
        h.Draw("")
    else:
        h.Draw("0")

    colorList = [TColor.GetColor( "#255EC7" ), TColor.GetColor( "#E42217" ), TColor.GetColor( "#2212EE" ), TColor.GetColor( "#22EE33" )]

    for ig,hgg in enumerate(hg):
        if ig < len(colorList):
            hgg.SetLineColor( colorList[ig] )
        hgg.SetLineWidth( 2 )
        hgg.Draw("e2same")

    if printText:
        t = TText( h.GetXaxis().GetXmax(), (ymax-ymin)*1.01 + ymin, printText )
        t.SetTextAlign( 31 )
        t.SetTextSize( 0.035 )
        t.Draw()

    # logo
    now = str(datetime.datetime.today())
    tlogo = TText( h.GetXaxis().GetXmax()*1.025, (ymax-ymin)*0.0+ymin, 'ATLAS Run Query %s' % now[:now.find('.')] )
    tlogo.SetTextSize( 0.030 )
    tlogo.SetTextColor( TColor.GetColor( "#888888" ) )
    tlogo.SetTextAngle(90)
    tlogo.Draw()

    drawLegend = False
    dx = 0.4
    dy = 0.10 + 0.03*(len(hg)-1)
    legend = TLegend( 1 - c.GetRightMargin() - dx, 1 - c.GetTopMargin() - dy - 0.015,
                      1 - c.GetRightMargin(), 1 - c.GetTopMargin() - 0.015)
    # legend.SetFillStyle( 1001 )
    # legend.SetFillColor( TColor.GetColor( "#FFFFFF" ) )
    legend.SetBorderSize( 1 )
    legend.SetMargin( 0.15 )
    legend.SetLineColor( 0 )
    legend.SetTextColor( 1 )
    for ig,hgg in enumerate(hg):
        if ylegend[ig]:
            legend.AddEntry( hgg, ylegend[ig], "l" )
            drawLegend = True
        
    if xvecStb:
        legend.AddEntry( h,  "LBs with stable beams","F" )
        drawLegend = True
    if drawLegend:
        legend.Draw("same")

    # redraw axis
    h.Draw( "sameaxis" )

    c.Update()
    fnames = '%s/atlrunquery_%s.png' % (datapath, histname) 
    c.Print( fnames )
    
    return fnames


def makeBSPlots( xvec, yvec, xtitle, ytitle, histname, histtitle, datapath, ymin, ymax, printText = '' ): 

    # sanity check
    if not xvec or len(yvec)==0 or not yvec[0]:
        return None

    ROOT = importroot()

    SetStyle()
    c = TCanvas( histname, histtitle, 0, 0, 530, 400 )
    c.GetPad(0).SetTopMargin(0.13)
    c.GetPad(0).SetGrid()

    bounds = [1]
    for nlb in xvec:
        bounds += [bounds[-1]+nlb]

    from array import array
    h = TH1F( histname + 'g', histtitle, len(bounds)-1, array('f',bounds) )
    h.GetXaxis().SetTitle( xtitle )
    h.GetXaxis().SetTitleOffset( 1.3 )
    h.GetYaxis().SetTitle( ytitle )
    h.GetYaxis().SetTitleOffset( 1.4 )
    h.SetTitle( histtitle )
    h.SetMinimum(ymin)
    h.SetMaximum(ymax)
    h.SetFillColor( TColor.GetColor( "#56A5EC" ) )
    for bin,y in enumerate(yvec):
        val,valerr = y
        h.SetBinContent( bin+1, val )
        h.SetBinError  ( bin+1, valerr )
    h.Draw("e2")

    hc = h.Clone()
    hc.SetLineColor( ROOT.kBlack )
    hc.SetLineWidth(2)
    hc.Draw("e0same")

    if printText:
        t = TText( h.GetXaxis().GetXmax(), (ymax-ymin)*1.01 + ymin, printText )
        t.SetTextAlign( 31 )
        t.SetTextSize( 0.035 )
        t.Draw()

    # logo
    now = str(datetime.datetime.today())
    tlogo = TText( h.GetXaxis().GetXmax()*1.025, (ymax-ymin)*0.0+ymin, 'ATLAS Run Query %s' % now[:now.find('.')] )
    tlogo.SetTextSize( 0.030 )
    tlogo.SetTextColor( TColor.GetColor( "#888888" ) )
    tlogo.SetTextAngle(90)
    tlogo.Draw()

    c.Update()
    fnames = '%s/atlrunquery_%s.png' % (datapath, histname) 
    c.Print( fnames )
    
    return fnames


def makeLBPlot( xvec, xvecStb, yvecScalar,
                xtitle, ytitle, ylegendScalar,
                histname, histtitle,
                datapath, printText = '', ymin = None, ymax = None ):
    yvec    = [yvecScalar]
    ylegend = [ylegendScalar]
    return makeLBPlotList( xvec, xvecStb, yvec, xtitle, ytitle, ylegend, histname, histtitle, datapath, printText, ymin, ymax )

def InttypeTrf( tree, var, vlist, value, kcoord ):
    # initialisation
    if kcoord < 0:
        vlist.append( array( 'i', [ 0 ] ) )
        vreg = var.replace('-','_')
        tree.Branch( vreg, vlist[-1], "%s/I" % vreg )
        return
    # fill tree
    try:
        vlist[kcoord][0] = int(value)
    except (ValueError, TypeError):
        vlist[kcoord][0] = -1
        
def FloattypeTrf( tree, var, vlist, value, kcoord ):
    # initialisation
    if kcoord < 0:
        vlist.append( array( 'f', [ 0 ] ) )
        vreg = var.replace('-','_')
        tree.Branch( vreg, vlist[-1], "%s/F" % vreg )
        return
    # fill tree
    try:
        vlist[kcoord][0] = float(value)
    except (ValueError, TypeError):
        vlist[kcoord][0] = -1

def StringtypeTrf( tree, var, vlist, value, kcoord ):
    # initialisation
    if kcoord < 0:
        vlist.append( array( 'b', 'ab\0'.encode() ) )
        tree.Branch( var, vlist[-1], "%s/C" % var )
        return
    # fill tree
    try:
        vlist[kcoord] = array( 'b', (value + '\0').encode() ) 
        tree.SetBranchAddress( var, vlist[kcoord] )
    except (ValueError, TypeError):                    
        vlist[kcoord] = array( 'b', 'unknown\0'.encode() ) 
        tree.SetBranchAddress( var, vlist[kcoord] )

def TimetypeTrf( tree, var, vlist, value, kcoord ):
    return StringtypeTrf( tree, var, vlist, value, kcoord )

def DurationtypeTrf( tree, var, vlist, value, kcoord ):
    # original format: '[2d] 13h 15m 45s' --> transform in seconds
    if kcoord >= 0:
        try:
            dur = value.split()
            if len(dur) == 3:
                h, m, s = dur
                d = '0d'
            elif len(dur) == 4:
                d, h, m, s = dur
            else:
                print ('Unknown format in "DurationtypeTrf:"')
                print (dur)
                sys.exit(1)
                
            value = str( ( (int(d.replace('d','').strip())*24 + int(h.replace('h','').strip()))*60 + 
                           int(m.replace('m','').strip()) )*60 + int(s.replace('s','').strip()) )
        except (ValueError, TypeError):
            print (value)
            sys.exit(1)
            value = 0
    InttypeTrf( tree, var, vlist, value, kcoord )            


def MakeHtml( hnames, fnames, uselarge = False, iconwidth = 70, nbreakline = 10 ):
    windowstyle = 'openWindow'
    if uselarge:
        windowstyle = 'openLargeWindow'
        iconwidth   = 120
    s = ''
    s += '<table><tr>'
    for i in range(0,len(fnames)):
        f = fnames[i]
        h = hnames[i]
        s += """<td valign="top"><a STYLE="text-decoration: none" href=" """
        s += """javascript:%s('Plot','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;><html xmlns:my><head><title>%s</title><LINK href=&quot;atlas-runquery.css&quot; rel=&quot;stylesheet&quot; type=&quot;text/css&quot;><body><table style=&quot;font-family: sans-serif; font-size: 85%%&quot;><tr><td valign=&quot;top&quot;>""" % (windowstyle,h)
        s += """<img src=&quot;%s&quot;>""" % f
        s += """<hr color=&quot;red&quot; size=1><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type=&quot;text/javascript&quot;></script></body></html>""" % str(datetime.datetime.now())
        s += """')"""
        s += """ "><img vspace=0 src="%s" title="Plot showing query result for %s" width="%i"></a><br><font size="-2">%s</font></td>""" % (f,h,iconwidth,h)
        s += """<td>&nbsp;</td>"""
        if (i+1)%nbreakline == 0:
            s += '</tr><tr>'

    s += '</tr></table>'
    return s

def GetActions():
    # define all actions (choose small letters)
    # format: 'column header' 'include' 'type' '...'
    actions = { "run":               ( True,  InttypeTrf      ),
                "links":             ( False, StringtypeTrf   ),
                "#lb":               ( True,  InttypeTrf      ),
                "#events":           ( True,  InttypeTrf      ),
                "smk":               ( True,  InttypeTrf      ),
                "start and endtime": ( True,  TimetypeTrf     ),
                "duration":          ( True,  DurationtypeTrf ),
                "torcurrent":        ( True,  FloattypeTrf    ), 
                "solcurrent":        ( True,  FloattypeTrf    ), 
                "detector systems":  ( False, StringtypeTrf   ),
                "stream":            ( True,  InttypeTrf      ),
                "lumi":              ( True,  FloattypeTrf    ),
                "other":             ( True,  StringtypeTrf   )
                }
    return actions

def CreateRootFile( dic ):

    from CoolRunQuery.selector.AtlRunQuerySelectorBase import DataKey
    from CoolRunQuery.AtlRunQueryQueryConfig import QC
    datapath = QC.datapath

    actions = GetActions()

    if 'data' not in os.listdir('.'):
        os.mkdir('data')

    f = open( '%s/dummyroot.txt' % datapath, 'w' )

    froot = TFile( '%s/atlrunquery.root' % datapath,  'RECREATE' )

    tree = TTree( 'RunQuery', 'ATLAS Run Query' )

    varlist  = []

    # create branches
    keylist = []
    for data_key in dic:

        #print ("DATA_KEY",type(data_key),data_key)
        f.write( 'key: %s \n' % (data_key.ResultKey))

        # the actual variable name used in the tree
        var = data_key.ResultKey.replace('#',' ').strip().replace(' ','_').replace('STR:','').replace('-','_').replace(':','_')

        k = data_key.ResultKey.lower()
        if data_key.Type==DataKey.STREAM:
            k = 'stream'
        elif 'olc_' == data_key.ResultKey[0:4]:
            k = 'lumi'

        # is key included in 'actions' dictionary ?
        if k not in actions:
            k = 'other'

        useit, function = actions[k]
        if useit:
            function( tree, var, varlist, -1, -1 )  # create branch
            keylist.append( (data_key,var, function) )

    # loop over runs
    for ev in range(len(dic[DataKey('Run')])):
        for k,(data_key,var,function) in enumerate(keylist):

            val = dic[data_key][ev]
            if data_key.Type==DataKey.STREAM:
                val = val[0]

            function( tree, var, varlist, val, k )
            
        tree.Fill()

    # make plots ?
    hnames, fnames = MakePlots( tree, datapath )
    htmlstr        = MakeHtml( hnames, fnames )

    f.close()
    tree.Write()
    froot.Close()

    return froot.GetName(), htmlstr



def makeRatePlot( v, lbduration, plottriggers, averrate, xtit, ytit, name, title, datapath, printText = '' ):

    ## # pickle input
    ## import pickle
    ## store = (v, lbduration, plottriggers, averrate, xtit, ytit, name, title, datapath, printText)
    ## 
    ## pf = open( '%s/rates.pickle' % datapath, 'w' )
    ## try: pickle.dump(store, pf)
    ## except:
    ##     print ('ERROR: could not pickle rates')
    ##     sys.exit(1)
    ## pf.close()
    
    ROOT = importroot()
    
    SetStyle()
    c = TCanvas( name, title, 0, 0, 530, 400 )
    c.GetPad(0).SetTopMargin(0.13)
    c.GetPad(0).SetGrid()

    firstlb = min([lbinfo[0][0] for lbinfo in v.values()])
    lastlb = max([lbinfo[-1][0] for lbinfo in v.values()])

    frame = TH1F( name, title, lastlb-firstlb+1, firstlb, lastlb+1 )
    frame.GetXaxis().SetTitle( xtit )
    frame.GetXaxis().SetTitleOffset( 1.3 )
    frame.GetYaxis().SetTitle( ytit )
    frame.GetYaxis().SetTitleOffset( 1.4 )


    hTAP = {} # trigger after prescale (not realy, it contains (tap+tav)/2 with and error of (tap-tav)/2)
    hTAV = {} # trigger after veto
    ymax = -1

    # see http://root.cern.ch/root/html526/TColorWheel.html
    colorList = [ (ROOT.kBlue,ROOT.kAzure+1),
                  (ROOT.kGreen+2,ROOT.kGreen+2),
                  (ROOT.kRed+1,ROOT.kRed),
                  (ROOT.kTeal-1,ROOT.kTeal),
                  (ROOT.kOrange+8,ROOT.kOrange),
                  (ROOT.kYellow+1,ROOT.kYellow),
                  (ROOT.kOrange+3,ROOT.kOrange+5),
                  (ROOT.kPink+8,ROOT.kPink+9) ]

    clrIdx = 0
    for trname,avrate in plottriggers:
        hTAP[trname] = TH1F( '%s_ap_%s' % (name,trname), title, lastlb-firstlb+1, firstlb, lastlb+1 )
        hTAV[trname] = TH1F( '%s_av_%s' % (name,trname), title, lastlb-firstlb+1, firstlb, lastlb+1 )
        hap = hTAP[trname]
        hav = hTAV[trname]
        hav.SetLineWidth( 2 )
        if clrIdx < len(colorList):
            dark,bright = colorList[clrIdx]
            hav.SetLineColor( dark )
            hap.SetFillColor( bright )
            hap.SetMarkerColor( bright )
            clrIdx += 1

        counts = v[trname]
        for (lb,bp,ap,av),(lb2,lbstart,dt) in zip(counts,lbduration):
            _apr = float(ap)/dt
            _avr = float(av)/dt
            hap.SetBinContent( lb-firstlb+1, (_apr+_avr)/2 )
            hap.SetBinError( lb-firstlb+1, (_apr-_avr)/2 )
            hav.SetBinContent( lb-firstlb+1, _avr )
            ymax = max(ymax,_apr)


    if ymax <= 0:
        ymax = 1
    
    # first draw histogram        
    y2 = ymax*1.2
    y1 = 0.01
    frame.SetMinimum(y1)
    frame.SetMaximum(y2)
    frame.Draw("0")    

    for h in hTAP.values():
        h.Draw("same e2")
    for h in hTAV.values():
        h.Draw("same")

    if printText:
        t = TText( frame.GetXaxis().GetXmax(), y2*1.01, printText )
        t.SetTextAlign( 31 )
        t.SetTextSize( 0.035 )
        t.Draw()

    # logo
    now = str(datetime.datetime.today())
    tlogo = TText( frame.GetXaxis().GetXmax()*1.025, y1, 'ATLAS Run QUERY %s' % now[:now.find('.')] )
    tlogo.SetTextSize( 0.030 )
    tlogo.SetTextColor( TColor.GetColor( "#888888" ) )
    tlogo.SetTextAngle(90)
    tlogo.Draw()

    dx = 0.4
    dy = 0.10 + 0.01*(len(hTAV)-1)
    legend = TLegend( 1 - c.GetRightMargin() - dx, 1 - c.GetTopMargin() - dy - 0.015,
                      1 - c.GetRightMargin(), 1 - c.GetTopMargin() - 0.015)
    legend.SetNColumns(2)
    legend.SetBorderSize( 0 )
    legend.SetMargin( 0.15 )
    legend.SetLineColor( 0 )
    legend.SetTextColor( 1 )
    # fill legend - highest average rate first
    for trname,avtrrate in averrate:
        if trname not in hTAV:
            continue
        legend.AddEntry( hTAV[trname], trname, "l" )
    legend.Draw("same")

    # redraw axis
    frame.Draw( "sameaxis" )

    c.Update()
    fnames = '%s/atlrunquery_%s.png' % (datapath, name) 
    c.Print( fnames )
    frame.SetMaximum(ymax*5)
    c.SetLogy(1)
    c.Update()
    fnames2 = '%s/atlrunquery_%s_log.png' % (datapath, name) 
    c.Print( fnames2 )

    return fnames
    




# command line driver for convenience
if __name__=='__main__':

    import json
    datapath = "."
    pf = open( '%s/plotinput.json' % datapath, 'r' )
    (lbrange, xvecStb, yvec, runNr, datapath) = json.load(pf)
    lbrange = range(len(lbrange))
    datapath = "test"
    makeLBPlotSummaryForLHC( lbrange, xvecStb, yvec, runNr, datapath, printText = '' )
