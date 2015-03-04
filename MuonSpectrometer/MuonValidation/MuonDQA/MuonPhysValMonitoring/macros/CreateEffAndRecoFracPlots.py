# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Written by Dan Mori
#Create efficiency plots for each author by dividing matched/author pt by truth/all pt

import ROOT
import os
import sys
import itertools
from array import array
ROOT.gROOT.SetBatch( True )

#--------------------------------------------------------------------------
def SetBinomialError( ratio, den ):
    if 'TH2' in ratio.IsA().GetName():
        for j in range(1,ratio.GetNbinsY()+1):
            for i in range(1,ratio.GetNbinsX()+1):
                n = den.Integral(i,i,j,j)
                if n <= 0:
                    ratio.SetBinError( i, j, 0 )
                    continue
                p = ratio.GetBinContent(i,j)
                ratio.SetBinError( i, j, (p*(1-p)/n)**0.5 )
    elif 'TH1' in ratio.IsA().GetName():
        for i in range(1,ratio.GetNbinsX()+1):
            n = den.Integral(i,i)
            if n <= 0:
                ratio.SetBinError( i, 0 )
                continue
            p = ratio.GetBinContent(i)
            ratio.SetBinError( i, (p*(1-p)/n)**0.5 )
    else:
        print( 'WARNING ' + ratio.GetName() + 'is not a TH1 or TH2' )

#--------------------------------------------------------------------------
#create histograms that require dividing one histogram by another
#works for both efficiency and reco fraction plots, set by plottype variable

def CreateRatioPlot( infile, numHist, denHist, var, eta1=None, eta2=None, xtitle = '', plottype = '' ):
    #require plottype variable for setting plot title, etc
    #plottype = 'Eff' or 'RecoFrac' to create efficiency or reco fraction histogram
    if plottype == 'Eff':
        if 'InFlight' in numHist.GetName():
            typeName = 'Fake Rate'
        else:
            typeName = 'Efficiency'
    elif plottype == 'RecoFrac':
        typeName = 'Reco Fraction'
    else:
        print('plottype must be one of the following: \'Eff\', \'RecoFrac\'' )
        return

    PlotNamePrefix = numHist.GetName()[:numHist.GetName().find('_kinematics')] + '_' + typeName.replace(' ','')

    if (eta1 != None and eta2 != None):
        PlotName = PlotNamePrefix + '_' + var.split('_')[1] + '_etaRange_{0}_{1}'.format(eta1,eta2).replace('-','m').replace('.','p')
        PlotTitle = var.split('_',1)[1].capitalize() + ' ' + typeName
        if eta1>0 and eta2>0:
            ## get hist for -eta2 < eta < -eta1
            ibin1 = numHist.GetXaxis().FindBin(-1*eta2)
            ibin2 = numHist.GetXaxis().FindBin(-1*eta1)
            num = numHist.ProjectionY('num_'+var.split('_')[1]+"_etaRange1",ibin1,ibin2)
            den = denHist.ProjectionY('den_'+var.split('_')[1]+"_etaRange1",ibin1,ibin2)
            ## get hist for eta1 < eta < eta2
            ibin1 = numHist.GetXaxis().FindBin(eta1)
            ibin2 = numHist.GetXaxis().FindBin(eta2)
            num2 = numHist.ProjectionY('num_'+var.split('_')[1]+"_etaRange2",ibin1,ibin2)
            den2 = denHist.ProjectionY('den_'+var.split('_')[1]+"_etaRange2",ibin1,ibin2)
            num.Add(num2)
            den.Add(den2)
            PlotTitle = PlotTitle + ' ({0}<|eta|<{1})'.format(eta1,eta2)
            del num2, den2
        else:
            ibin1 = numHist.GetXaxis().FindBin(eta1)
            ibin2 = numHist.GetXaxis().FindBin(eta2)
            num = numHist.ProjectionY('num_'+var.split('_')[1]+"_etaRange",ibin1,ibin2)
            den = denHist.ProjectionY('den_'+var.split('_')[1]+"_etaRange",ibin1,ibin2)
            PlotTitle = PlotTitle + ' ({0}<eta<{1})'.format(eta1,eta2)
    else:
        PlotTitle = var.capitalize() +  ' ' + typeName
        PlotName = PlotNamePrefix + '_' + var
        if var == 'pt':
            #initial pt hist is in bins of 1 GeV
            ptBins = [ 5, 10, 20, 30, 40, 50, 65, 80, 100, 200, 1000 ]
            if numHist.GetNbinsX() == 200 or ( numHist.GetNbinsX() == 1000 and ( numHist.Integral(200,1000) == 0 or denHist.Integral(200,1000) == 0 ) ):
                ptBins.remove(1000)
            num = ROOT.TH1F( 'num_'+var, '', len(ptBins)-1, array( 'f', ptBins ) )
            den = ROOT.TH1F( 'den_'+var, '', len(ptBins)-1, array( 'f', ptBins ) )
            for i in range( 1, numHist.GetNbinsX() + 1 ):
                num.Fill( numHist.GetXaxis().GetBinCenter(i), numHist.GetBinContent(i) )
                den.Fill( denHist.GetXaxis().GetBinCenter(i), denHist.GetBinContent(i) )
        else:
            num = numHist.Clone()
            den = denHist.Clone()
            if 'TH1' in numHist.IsA().GetName():
                num.Rebin(4)
                den.Rebin(4)
            else:
                num.Rebin2D(4,4)
                den.Rebin2D(4,4)

    ratio = num.Clone( PlotName )
    ratio.Divide( num, den, 1, 1, "B" )
    ratio.SetTitle( PlotTitle )
    SetBinomialError( ratio, den )

    # Define x and y-axis name from histogram name (var)
    if 'TH2' in ratio.IsA().GetName():
        if '_' in var:
            ratio.GetXaxis().SetTitle( var.split('_')[0] )
            ratio.GetYaxis().SetTitle( var.split('_')[1] )
            ratio.GetZaxis().SetTitle( typeName.lower() )
        else:
            print 'WARNING Could not get axes name from histogram name'
    elif 'TH1' in ratio.IsA().GetName():
        ymax = ratio.GetBinContent( ratio.GetMaximumBin() ) + ratio.GetBinError( ratio.GetMaximumBin() )
        ratio.GetYaxis().SetRangeUser(0, ymax*1.2 )
        ratio.GetYaxis().SetTitle( typeName.lower() )
        if xtitle == '':
            xtitle = var
        ratio.GetXaxis().SetTitle( xtitle )

        if den.Integral() > 0:
            tot_eff = ROOT.TF1( 'aveline', "[0]", ratio.GetXaxis().GetBinLowEdge(1), ratio.GetXaxis().GetBinUpEdge( ratio.GetNbinsX() ) )
            ratio.GetListOfFunctions().Add( tot_eff )
            f1 = ratio.GetFunction( "aveline" )
            f1.SetParameter( 0, num.Integral()/den.Integral() )
            f1.SetLineColor( ROOT.kRed )

    PlotDirName = PlotNamePrefix.replace('__','_').split('_')
    if not infile.GetDirectory( '/'.join( PlotDirName ) ):
        PlotDir = infile.Get( '/'.join( PlotDirName[:-1] ) ).mkdir( PlotDirName[-1] )
    else:
        PlotDir = infile.Get( '/'.join( PlotDirName ) )
    PlotDirName = '/'.join( PlotDirName )
    print( 'Writing histogram to input file: ' + PlotDirName + '/' + ratio.GetName() )
    infile.GetDirectory(PlotDirName).WriteTObject( ratio, ratio.GetName(), "Overwrite" )

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

    infile = ROOT.TFile.Open( filename, 'update' )

    muonTypesEff  = [ 'All', 'Prompt', 'InFlight', 'NonIsolated' ]
    muonTypesReco = [ 'Prompt', 'InFlight', 'NonIsolated', 'Rest' ]
    Variables = [ 'pt', 'eta', 'phi', 'eta_phi', 'eta_pt' ]

    Xtitles = {
      'pt' : 'Transverse Momentum [GeV]',
      'eta' : 'eta',
      'phi' : 'phi',
      'eta_phi' : 'phi',
      'eta_pt' : 'Transverse Momentum [GeV]' }

    #Efficiency plots
    Authors = []
    for muType in muonTypesEff:
        if not infile.Get( 'Muons/' + muType ):
            print( 'INFO TDirectory not found: Muons/' + muType )
            continue
        #get list of authors from matched dir
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( muType ) )
        if Authors == []:
            Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for author in Authors:
            truthDirName = 'Muons/{0}/truth/all'.format( muType )
            matchDirName = 'Muons/{0}/matched/{1}/kinematics'.format( muType, author )
            truthDir = infile.GetDirectory( truthDirName )
            matchDir = infile.GetDirectory( matchDirName )
            if not truthDir:
                print( 'WARNING Directory not found: '+truthDirName )
                continue
            if not matchDir:
                print( 'WARNING Directory not found: '+matchDirName )
                continue
            for var in Variables:
                truthHistName = truthDirName.replace('/','_') + '_' + var
                truthHist = truthDir.Get( truthHistName )
                matchHistName = matchDirName.replace('/','_') + '_' + var
                matchHist = matchDir.Get( matchHistName )
                if not truthHist:
                    print( 'WARNING histogram not found: '+truthHistName )
                    continue
                if not matchHist:
                    print( 'WARNING histogram not found: '+matchHistName )
                    continue
                CreateRatioPlot( infile, matchHist, truthHist, var, xtitle = muType+' Muon '+Xtitles[var], plottype = 'Eff' )
                if var == 'eta_phi' or var == 'eta_pt':
                    CreateRatioPlot( infile, matchHist, truthHist, var, -0.1, 0.1, muType+' Muon '+Xtitles[var], 'Eff' )
                    CreateRatioPlot( infile, matchHist, truthHist, var, 0.1, 1.05, muType+' Muon '+Xtitles[var], 'Eff' )
                    CreateRatioPlot( infile, matchHist, truthHist, var, 1.05, 2.0, muType+' Muon '+Xtitles[var], 'Eff' )
                    CreateRatioPlot( infile, matchHist, truthHist, var,  2.0, 2.5, muType+' Muon '+Xtitles[var], 'Eff' )

    #Reco Fraction plots
    for muType in muonTypesReco:
        if not infile.Get( 'Muons/' + muType ):
            print( 'INFO TDirectory not found: Muons/' + muType )
            continue
        #get list of authors from matched dir
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( muType ) )
        if Authors == []:
            Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]
        for author in Authors:
            typedir = 'Muons/{0}/reco/{1}/kinematics'.format( muType, author )
            alldir = 'Muons/All/reco/{0}/kinematics'.format( author )
            typeRecoDir = infile.Get( typedir )
            allRecoDir = infile.Get( alldir )
            if not typeRecoDir:
                print( 'INFO TDirectory not found: '+typedir )
                continue
            if not allRecoDir:
                print( 'INFO TDirectory not found: '+alldir )
                continue
            for var in Variables:
                typeplot = typedir.replace('/','_') + '_' + var
                allplot = alldir.replace('/','_') + '_' + var
                typeRecoHist = typeRecoDir.Get( typeplot )
                allRecoHist = allRecoDir.Get( allplot )
                if not typeRecoHist:
                    print( 'WARNING plot not found: ' + typeplot )
                    continue
                if not allRecoHist:
                    print( 'WARNING plot not found: ' + allplot )
                    continue
                CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, xtitle = muType + ' Muon ' + Xtitles[var], plottype = 'RecoFrac' )

    #unmatched muon reco fraction
    muType = 'UnmatchedRecoMuons'
    typedir = 'Muons/{0}/kinematics'.format( muType )
    allnames = [ i for i in Authors if i == 'AllMuons' or i == 'AllAuthors' ]
    if len(allnames) == 0:
        return
    alldir = 'Muons/All/reco/{0}/kinematics'.format( allnames[0] )
    if not infile.GetDirectory( typedir ):
        print( 'INFO directory not found: ' + typedir )
    elif not infile.GetDirectory( alldir ):
        print( 'INFO directory not found: ' + alldir )
    else:
        for var in Variables:
            typeplot = 'Muons_{0}__kinematics_{1}'.format( muType, var )
            allplot = alldir.replace('/','_') + '_{0}'.format( var )
            #print('Working on Muons_{0}__{1}'.format(muType,var))
            typeRecoHist = infile.GetDirectory( typedir ).Get( typeplot )
            allRecoHist = infile.GetDirectory( alldir ).Get( allplot )
            if typeRecoHist and allRecoHist:
                CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, xtitle = 'Unmatched Reco Muon '+Xtitles[var], plottype = 'RecoFrac' )
    infile.Close()

#===============================================================================

if __name__ == "__main__":
    """
    Here the code should appear that is executed when running the plotter directly
    (and not import it in another python file via 'import Plotter')
    """
    # start main program
    main( sys.argv )
