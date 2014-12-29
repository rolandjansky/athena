# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#author: Dan Mori < dmori@cern.ch >
#including code by Felix Socher <Felix.Socher@cern.ch>
import ROOT
import math
import sys
import os
import argparse
from commands import getstatusoutput
from config import *

#############################################################################

def CreateEmptyPlot( filename ):
    can = ROOT.TCanvas("","",860,900)
    can.SaveAs( filename )

#############################################################################

def SetBounds( refHist, testHist ):
    maximum = max(refHist.GetMaximum(), testHist.GetMaximum())
    maximum = maximum + 0.1*abs(maximum)
    minimum = min(refHist.GetMinimum(), testHist.GetMinimum())
    minimum = minimum - 0.1*abs(minimum)
    refHist.SetMinimum(minimum)
    refHist.SetMaximum(maximum)
    return refHist

#############################################################################

def MakeComparisonPlot( refHist, testHist, plotName, path, doNorm = False, doRatio = True ):
    labelsize = 0.14
    titlesize = 0.18

    #check that test histogram exist
    if not refHist:
        print('WARNING Ref Histogram not found: '+plotName)

    if not testHist:
        print('WARNING Test histogram not found: '+plotName)
        print('        Creating empty plot')
        CreateEmptyPlot( path+'/'+plotName+'.pdf' )
        return

    #if doNorm set, scale hists to area = 1
    if refHist:
        refint = refHist.Integral()
    else:
        refint = 0
    testint = testHist.Integral()

    if doNorm:
        if refint != 0:
            refHist.Scale( 1./refint )
            for i in range( 1, refHist.GetNbinsX() + 1 ):
                refHist.SetBinError( i, refHist.GetBinError( i )/refint )
        elif refHist:
            print( 'WARNING empty ref plot ' + plotName )

        if testint != 0:
            testHist.Scale( 1./testint )
            for i in range( 1, refHist.GetNbinsX() + 1 ):
                testHist.SetBinError( i, testHist.GetBinError( i )/testint )
        else:
            print( 'WARNING empty test plot ' + plotName )

    canvas = ROOT.TCanvas( "", "", 860, 900 )
    canvas.SetLeftMargin( 0 )
    padMain = ROOT.TPad( 'padMain', 'padMain', 0, 0.3, 1, 1 )
    padMain.SetBottomMargin( 0 )
    padMain.Draw()

    if doRatio:
        padRatio = ROOT.TPad( 'padRatio', 'padRatio', 0, 0, 1, 0.3 )
        padRatio.SetTopMargin( 0 )
        padRatio.SetBottomMargin( 0.4 )
        padRatio.Draw()
    ROOT.TLine()
    padMain.cd()
    if 'cone' in plotName:
        padMain.SetLogy()

    leg = ROOT.TLegend( 0.82, 0.77, 0.96, .96 )
    leg.SetFillColor( ROOT.kWhite )
    leg.SetTextSizePixels(20)

    testHist.SetMarkerColor(ROOT.kRed)
    testHist.SetLineColor(ROOT.kRed)
    leg.AddEntry(testHist, "test",'P')

    testHist.GetYaxis().SetTitleOffset( 1.4 )
    testHist.GetYaxis().SetTitleSize( labelsize*0.4 )
    testHist.SetLabelSize( labelsize*0.4, 'Y' )

    testHist.Draw("P")
    if refint>0:
        refHist.SetMarkerColor(ROOT.kBlack)
        refHist.SetLineColor(ROOT.kBlack)
        leg.AddEntry(refHist, "ref", 'P')
        if 'cone' not in plotName:
            refHist = SetBounds(refHist, testHist)

        refHist.Draw("PSAME")
        testHist.Draw("PSAME")
    leg.Draw()

    #ks & chi^2 values
#    if refint > 0 and testint > 0:
#        ks = testHist.KolmogorovTest( refHist, 'WW' )
#        kstext = ROOT.TLatex( 0.18, .955, 'KS = %0.3f'%(ks))
#        kstext.SetNDC(ROOT.kTRUE)
#        kstext.SetTextSize(0.055)
#        kstext.Draw()
#        chi = testHist.Chi2Test( refHist, 'WW' )
#        chitext = ROOT.TLatex( 0.35, .955, '#chi^{2} = %0.3f'%(chi))
#        chitext.SetNDC(ROOT.kTRUE)
#        chitext.SetTextSize(0.055)
#        chitext.Draw()

    if refint == 0 or testint == 0:
        print( 'WARNING No ratio plot available: '+ plotName )
    elif not doRatio:
        print( 'INFO skipping ratio plot for '+ plotName )
    else:
        padRatio.cd()
        ratioHist = testHist.Clone()
        ratioHist.Divide( refHist )
        ratioHist.SetMarkerColor( ROOT.kBlack )
        ratioHist = SetBounds( ratioHist, ratioHist )
        for i in range( ratioHist.GetNbinsX() ):
            nref = refHist.GetBinContent(i)
            ntest = testHist.GetBinContent(i)
            if nref == 0 or ntest == 0:
                ratioHist.SetBinError( i, 0 )
            else:
                error = nref/ntest*math.sqrt((refHist.GetBinError(i)/nref)**2 + (testHist.GetBinError(i)/ntest)**2)
                ratioHist.SetBinError( i, error )
        ratioHist.SetLineColor( ROOT.kBlack )
        ratioHist.GetYaxis().SetTitle( "test / ref" )
        xtitle = ratioHist.GetXaxis().GetTitle()
        if 'Transverse Momentum' in xtitle:
            ratioHist.GetXaxis().SetTitle(xtitle.replace('Transverse Momentum', 'pT'))

        ratioHist.GetXaxis().SetTitleSize( titlesize )
        ratioHist.GetYaxis().SetTitleSize( labelsize )
        ratioHist.SetLabelSize( labelsize, 'XY' )
        ratioHist.GetXaxis().SetTitleOffset( 1.0 )
        ratioHist.GetYaxis().SetTitleOffset( .4 )
        ratioHist.Draw("E")

        lineRatio = ROOT.TLine( ratioHist.GetXaxis().GetXmin(), 1,
                                ratioHist.GetXaxis().GetXmax(), 1 )
        lineRatio.SetLineColor( ROOT.kRed )
        lineRatio.SetLineWidth( 2 )
        lineRatio.Draw("same")

    canvas.cd()
    canvas.SaveAs( path + '/' + plotName + '.pdf' )
    canvas.Close()

#############################################################################

def main():
    #parse options
    parser = argparse.ArgumentParser( description = 'Create Muon Validation Slides in LaTeX Beamer' )
    parser.add_argument( '-r', '--reference', help = 'reference sample ROOT file' )
    parser.add_argument( '-t', '--test', required=True, help = 'test sample ROOT file' )
    parser.add_argument( '-d', '--directory', default = 'beamer', help = 'directory to put new files' )
    parser.add_argument( '-n', '--notitlepage', action = 'store_true', help = 'set to remove title page' )
    parser.add_argument( '-c', '--compile', action= 'store_true', help = 'compile beamer file' )
    args = parser.parse_args()
    args.directory = os.path.dirname( args.directory + '/' )
    outdir = args.directory + '/plots'
    if not os.path.isdir( outdir ):
        os.makedirs( outdir )

    #read files
    testfile = ROOT.TFile.Open( args.test, 'read' )
    reffile  = ROOT.TFile.Open( args.reference, 'read' )

	##### save histograms #####

    specialTypes = [ 'RecoFrac', 'PtRes', 'Res_pT_vs', 'PtScale', 'Eff' ]
    for PlotList in PlotPages.values():
        for PlotPath in PlotList:
            HistDir, HistName = os.path.split( PlotPath )
            if reffile.GetDirectory( HistDir ):
                refhist = reffile.GetDirectory( HistDir ).Get( HistName )
            else:
                refhist = False

            if testfile.GetDirectory( HistDir ):
                testhist = testfile.GetDirectory( HistDir ).Get( HistName )
            else:
                testhist = False

            doNorm = not sum([ HistName.__contains__(i) for i in specialTypes ])
            #doRatio = not (plot.__contains__('PtRes') or plot.__contains__('PtScale'))
            MakeComparisonPlot( refhist, testhist, HistName, outdir, doNorm = doNorm, doRatio = True )


##### create Beamer file #####

    endl = '\n'
    output  = r'\documentclass{beamer}' + endl
    output += r'\definecolor{links}{HTML}{0000FF}' + endl
    output += r'\hypersetup{colorlinks=true,urlcolor=links}' + endl
    output += r'\usepackage[latin1]{inputenc}' + endl
    output += r'\usetheme{Warsaw}' + endl*2
    output += r'\definecolor{LightBlue}{cmyk}{0.248,0.0609,0,0.098}' + endl
    output += r'\setbeamercolor{author in head/foot}{fg=white, bg=blue}'+endl
    output += r'\setbeamercolor{title in head/foot}{fg=white, bg=LightBlue}'+endl
    output += r'\makeatother' + endl
    output += r'\setbeamertemplate{footline}{' + endl
    output += r'\leavevmode' + endl
    output += r'\hbox{%' + endl

    ht = 2.4
    dp = 1.1
    output += r'\begin{beamercolorbox}[wd=0.4\paperwidth,ht=%0.3fex,dp=%0.3fex,center]{author in head/foot}'%( ht, dp ) + r'%' + endl
    output += r'  \usebeamerfont{author in head/foot}\insertshortauthor' + endl

    output += r'\end{beamercolorbox}%' + endl
    output += r'\begin{beamercolorbox}[wd=0.5\paperwidth,ht=%0.3fex,dp=%0.3fex,center]{title in head/foot}'%( ht, dp ) + r'%' + endl
    output += r'  \usebeamerfont{author in head/foot}\insertshorttitle' + endl
    output += r'\end{beamercolorbox}%' + endl
    output += r'\begin{beamercolorbox}[wd=0.1\paperwidth,ht=%0.3fex,dp=%0.3fex,center]{author in head/foot}'%( ht, dp ) + r'%' + endl
    output += r'  \insertframenumber{}/\inserttotalframenumber' + endl
    output += r'\end{beamercolorbox}}%' + endl
    output += r'\vskip0pt}' + endl*2

    output += r'\makeatletter' + endl
    output += r'\setbeamertemplate{navigation symbols}{}' + endl*2
    output += r'\setbeamerfont{footline}{size=\fontsize{10}{11}\selectfont}' + endl

    output += r'\setbeamersize{text margin left=0.5cm}' + endl
    output += r'\setbeamersize{text margin right=0.5cm}' + endl

    output += r'\title[%s]{%s}'%(ShortTitle, Title) + endl
    output += r'\subtitle{%s}'%(Subtitle) + endl
    output += r'\author[%s]{%s}'%(ShortAuthor, Author) + endl
    output += r'\institute[%s]{%s}'%(ShortInstitute, Institute) + endl
    output += r'\date{%s}'%(Date) + endl
    output += r'\begin{document}' + endl*2
    output += r'%%%%%%%%%%%%%%%%%%%% BEGIN DOCUMENT %%%%%%%%%%%%%%%%%%%%' + endl*2
    if not args.notitlepage:
        output += r'{\setbeamertemplate{footline}{}' + endl
        output += r'\begin{frame} \titlepage \end{frame} }' + endl
        output += r'\addtocounter{framenumber}{-1}' + endl*2

    #intro slide
    output += r'\begin{frame}' + endl
    output += r'\frametitle{Introduction}' + endl
    output += 'Reference:' + endl*2
    output += '{0}: {1}'.format(RefVersion, RefDescription) + endl*2
    output += RefDataset + endl*2
    output += r'\vspace{12pt}' + endl
    output += 'Test:' + endl*2
    output += '{0}: {1}'.format(TestVersion, TestDescription) + endl*2
    output += TestDataset + endl*2
    #output += r'\begin{itemize}' + endl
    #output += r'\item '+ Intro + endl
    #output += r'\end{itemize}' + endl
    output += r'\vspace{12pt}' + Intro + endl
    output += r'\end{frame}' + endl*2

    for page in PlotPages:
        plotlist = PlotPages[page]
        output += r'\begin{frame}' + endl
        output += r'\frametitle{%s}'%(page) + endl
        output += r'\hspace*{-4pt}\makebox[\linewidth][c]{' + endl
        output += r'\begin{tabular}{c%s}'%(r'@{\hspace{1pt}}c'*(len(plotlist)-1)) + endl
        for n, plotpath in enumerate(plotlist):
            output += r'\includegraphics[width=%0.2f\textwidth]{%s/%s.pdf}'%( 0.35, outdir.replace( args.directory+'/', '' ), os.path.split(plotpath)[1] )
            if n+1 < len(plotlist):
                output += ' &'
            output += endl
        output += r'\end{tabular} }' + endl
        if page in PlotComments:
             output += PlotComments[page] + endl
        output += r'\end{frame}' + endl*2

    #summary slide
    if len(Summary) > 0:
        output += r'\begin{frame}' + endl
        output += 'Summary' + endl
        output += r'\begin{itemize}' + endl

        for item in Summary:
            output += r'\item %s'%(item) + endl

        output += r'\end{itemize}' + endl
        output += r'\end{frame}' + endl
    output += r'\end{document}'

    with open( args.directory + '/' + texfile, 'w' ) as f:
        f.write( output )

    if args.compile:
        stat, out = getstatusoutput( 'cd %s; pdflatex -halt-on-error %s'%( args.directory, texfile ) )
        if stat != 0:
            print out


######################### Executable Area #########################

if __name__ == "__main__":
    if sys.version_info < (2,7):
        print( 'INFO This program requires Python version 2.7 or greater' )
        exit(1)

    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetBatch()
    main()
