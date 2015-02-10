# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Written by Dan Mori
#Create efficiency plots for each author by dividing matched/author pt by truth/all pt

import ROOT
import os
import sys
import itertools
ROOT.gROOT.SetBatch( True )

#--------------------------------------------------------------------------
def SetBinomialError( ratio, den ):
    for i in range(1,ratio.GetNbinsX()+1):
        if den.Integral(i,i) <= 0:
            ratio.SetBinError( i, 0 )
            continue
        bin = ratio.GetBinContent(i)
        ratio.SetBinError( i, (bin*(1-bin)/den.Integral(i,i))**0.5 )

#--------------------------------------------------------------------------
#create histograms that require dividing one histogram by another
#works for both efficiency and reco fraction plots, set by plottype variable

def CreateRatioPlot( numHist, denHist, var, eta1=None, eta2=None, xtitle = '', plottype = '' ):


    num = numHist.Clone()
    den = denHist.Clone()
    #require plottype variable for setting plot title, etc
    #plottype = 'Eff' or 'RecoFrac' to create efficiency or reco fraction histogram
    if plottype == '':
        print( 'plottype not set: running Efficiency plot by default\n' )
        plottype = 'Eff'

    if plottype == 'Eff':
        if 'InFlight' in num.GetName():
            typeName = 'Fake Fate'
        else:
            typeName = 'Efficiency'
    elif plottype == 'RecoFrac':
        typeName = 'Reco Fraction'
    else:
        print('plottype must be one of the following: \'Eff\', \'RecoFrac\'' )
        return

    numNamePrefix = numHist.GetDirectory().GetPath().split(':/')[1].replace('/','_')
    denNamePrefix = denHist.GetDirectory().GetPath().split(':/')[1].replace('/','_')
#    try:
#        num = numDir.Get( numNamePrefix+'_'+var ).Clone()
#    except:
#        print('WARNING histogram not found: '+numNamePrefix+'_'+var)
#        return
#    try:
#        den = denDir.Get( denNamePrefix+'_'+var ).Clone()
#    except:
#        print('WARNING histogram not found: '+numNamePrefix+'_'+var)
#        return

    PlotName = numNamePrefix+'_{0}_{1}'.format( plottype, var )
    PlotTitle = var.capitalize() +  ' ' + typeName
    #print PlotName
    if (eta1 != None and eta2 != None):
        PlotTitle = var.split('_',1)[1].capitalize() + ' ' + typeName
        if eta1>0 and eta2>0:
            ## get hist for -eta2 < eta < -eta1
            ibin1 = num.GetXaxis().FindBin(-1*eta2)
            ibin2 = num.GetXaxis().FindBin(-1*eta1)
            num1 = num.ProjectionY(numNamePrefix+'_'+var.split('_')[1]+"_etaRange1",ibin1,ibin2)
            den1 = den.ProjectionY(denNamePrefix+'_'+var.split('_')[1]+"_etaRange1",ibin1,ibin2)
            ## get hist for eta1 < eta < eta2
            ibin1 = num.GetXaxis().FindBin(eta1)
            ibin2 = num.GetXaxis().FindBin(eta2)
            num2 = num.ProjectionY(numNamePrefix+'_'+var.split('_')[1]+"_etaRange2",ibin1,ibin2)
            den2 = den.ProjectionY(denNamePrefix+'_'+var.split('_')[1]+"_etaRange2",ibin1,ibin2)
            num = num1.Clone()
            num.Add(num2)
            den = den1.Clone()
            den.Add(den2)
            PlotName = numNamePrefix+'_'+plottype+'_'+var.split('_')[1]+'_etaRange_{0}_{1}'.format(eta1,eta2).replace('-','m').replace('.','p')
            PlotTitle = PlotTitle + ' ({0}<|eta|<{1})'.format(eta1,eta2)
        else:
            ibin1 = num.GetXaxis().FindBin(eta1)
            ibin2 = num.GetXaxis().FindBin(eta2)
            num = num.ProjectionY(numNamePrefix+'_'+var.split('_')[1]+"_etaRange",ibin1,ibin2)
            den = den.ProjectionY(denNamePrefix+'_'+var.split('_')[1]+"_etaRange",ibin1,ibin2)
            PlotName = numNamePrefix+'_'+plottype+'_'+var.split('_')[1]+'_etaRange_{0}_{1}'.format(eta1,eta2).replace('-','m').replace('.','p')
            PlotTitle = PlotTitle + ' ({0}<eta<{1})'.format(eta1,eta2)

    if not num.IsA().InheritsFrom(ROOT.TH2.Class()):
        num = num.Rebin(2,"num")
        den = den.Rebin(2,"den")

    ratio = num.Clone()
    ratio.Divide(num,den,1,1,"B")
    ratio.SetTitle( PlotTitle )
    ratio.SetName( PlotName )
    # Define x and y-axis name from histogram name (var)
    if ratio.IsA().InheritsFrom(ROOT.TH2.Class()):
        if '_' in var:
            ratio.GetXaxis().SetTitle( var.split('_')[0] )
            ratio.GetYaxis().SetTitle( var.split('_')[1] )
        else:
            print 'WARNING Could not get axes name from histogram name'
    else:
        ratio.GetYaxis().SetRangeUser(0, 1.1)
        ratio.GetYaxis().SetTitle( typeName.lower() )
        if '_' in var:
            ratio.GetXaxis().SetTitle( var.split('_')[1] )
        else:
            if xtitle == '':
                xtitle = var
            ratio.GetXaxis().SetTitle( xtitle )
            SetBinomialError( ratio, den )    

    print('Writing histogram to input file: '+numHist.GetDirectory().GetPath()+'/'+ratio.GetName())
    numHist.GetDirectory().WriteTObject( ratio, PlotName, "Overwrite" )
    #can = ROOT.TCanvas()
    #ratio.Draw("E")
    #can.SaveAs('/cluster/hep/mori/public_html/muon_validation/reco_eff_plots/plots/'+PlotName+'.pdf')
    del ratio, num, den

#--------------------------------------------------------------------------
def main( argv ):
    """
    Main function to be executed when starting the code.
    """

    if len( argv ) < 2:
        print( 'No filename given' )
        print( 'Usage: python '+argv[0]+' physval_filename' )
        exit(1)

    filename = argv[1]
    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        exit(1)

    try:
        infile = ROOT.TFile.Open( filename, 'update' )
    except:
        print( 'Cannot open file: {0}'.format( filename ) )
        exit(1)

    muonTypesEff  = [ 'All', 'Prompt', 'InFlight', 'NonIsolated' ]
    muonTypesReco = [ 'Prompt', 'InFlight', 'NonIsolated' ]
    Variables = [ 'pt', 'eta', 'phi', 'eta_phi', 'eta_pt' ]

    Xtitles = {
      'pt' : 'Transverse Momentum [GeV]',
      'eta' : 'eta',
      'phi' : 'phi',
      'eta_phi' : 'eta',
      'eta_pt' : 'eta' }

    #Efficiency plots
    for muType in muonTypesEff:
        if not infile.Get( 'Muons/' + muType ):
            print( 'INFO TDirectory not found: Muons/' + muType )
            continue
        #get list of authors from matched dir
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( muType ) )
        Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for author in Authors:
            truthDirName = 'Muons/{0}/truth/all'.format( muType )
            matchDirName = 'Muons/{0}/matched/{1}'.format( muType, author )
            truthDir = infile.GetDirectory( truthDirName )
            matchDir = infile.GetDirectory( matchDirName )
            if not truthDir:
                print( 'WARNING Directory not found: '+truthDirName )
                continue
            if not matchDir:
                print( 'WARNING Directory not found: '+matchDirName )
                continue
            for var in Variables:
                truthHistName = 'Muons_{0}_truth_all_{1}'.format( muType, var )
                truthHist = truthDir.Get( truthHistName )
                matchHistName = 'Muons_{0}_matched_{1}_{2}'.format( muType, author, var )
                matchHist = matchDir.Get( matchHistName )
                if not truthHist:
                    print( 'WARNING histogram not found: '+truthHistName )
                    continue
                if not matchHist:
                    print( 'WARNING histogram not found: '+matchHistName )
                    continue
                CreateRatioPlot( matchHist, truthHist, var, xtitle = muType+' Muon '+Xtitles[var], plottype = 'Eff' )
                if var == 'eta_phi' or var == 'eta_pt':
                    CreateRatioPlot( matchHist, truthHist, var, -0.1, 0.1, muType+' Muon '+Xtitles[var], 'Eff' )
                    CreateRatioPlot( matchHist, truthHist, var, 0.1, 1.05, muType+' Muon '+Xtitles[var], 'Eff' )
                    CreateRatioPlot( matchHist, truthHist, var, 1.05, 2.0, muType+' Muon '+Xtitles[var], 'Eff' )
                    CreateRatioPlot( matchHist, truthHist, var,  2.0, 2.5, muType+' Muon '+Xtitles[var], 'Eff' )

    #Reco Fraction plots
    for muType in muonTypesReco:
        if not infile.Get( 'Muons/' + muType ):
            print( 'INFO TDirectory not found: Muons/' + muType )
            continue
        #get list of authors from matched dir
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( muType ) )
        Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for author in Authors:
            typedir = 'Muons/{0}/reco/{1}'.format( muType, author )
            alldir = 'Muons/All/reco/{0}'.format( author )
            typeRecoDir = infile.Get( typedir )
            allRecoDir = infile.Get( alldir )
            if not typeRecoDir:
                print( 'ERROR TDirectory not found: '+typedir )
                continue
            if not allRecoDir:
                print( 'ERROR TDirectory not found: '+alldir )
                continue
            for var in Variables:
                typeplot = 'Muons_{0}_reco_{1}_{2}'.format( muType, author, var )
                allplot = 'Muons_All_reco_{0}_{1}'.format( author, var )
                typeRecoHist = typeRecoDir.Get( typeplot )
                allRecoHist = allRecoDir.Get( allplot )
                if not typeRecoHist:
                    print( 'WARNING plot not found: ' + typeplot )
                    continue
                if not allRecoHist:
                    print( 'WARNING plot not found: ' + allplot )
                    continue
                CreateRatioPlot( typeRecoHist, allRecoHist, var, xtitle = muType+' Muon '+Xtitles[var], plottype = 'RecoFrac' )

    #unmatched muon reco fraction
    muType = 'UnmatchedRecoMuons'
    typedir = 'Muons/{0}'.format( muType )
    alldir = 'Muons/All/reco/AllAuthors'
    if not infile.GetDirectory( typedir ):
        print( 'WARNING directory not found: ' + typedir )
    elif not infile.GetDirectory( alldir ):
        print( 'WARNING directory not found: ' + alldir )
    else:
        for var in Variables:
            typeplot = 'Muons_{0}__{1}'.format( muType, var )
            allplot = 'Muons_All_reco_AllAuthors_{0}'.format( var )
            #print('Working on Muons_{0}__{1}'.format(muType,var))
            typeRecoHist = infile.GetDirectory( typedir ).Get( typeplot )
            allRecoHist = infile.GetDirectory( alldir ).Get( allplot )
            if typeRecoHist and allRecoHist:
                CreateRatioPlot( typeRecoHist, allRecoHist, var, xtitle = 'Unmatched Muon '+Xtitles[var], plottype = 'RecoFrac' )

    infile.Close()

#===============================================================================

if __name__ == "__main__":
    """
    Here the code should appear that is executed when running the plotter directly
    (and not import it in another python file via 'import Plotter')
    """
    # start main program
    main( sys.argv )
