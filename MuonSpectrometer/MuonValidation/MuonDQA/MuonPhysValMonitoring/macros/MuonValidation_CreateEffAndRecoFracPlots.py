# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Written by Dan Mori
#Create efficiency plots for each author by dividing matched/author pt by truth/all pt

#Usage: python CreateEffAndRecoFracPlots.py filename [doAverage]

#Added flag doAverage to add TF1 of line showing overall efficiency/reco fraction

import ROOT
import os
import sys
import itertools
from array import array
ROOT.gROOT.SetBatch( True )

doProjX=1
doProjY=0

#--------------------------------------------------------------------------
def SetBinomialError( ratio, den ):
    if 'TH2' in ratio.IsA().GetName():
        for j in range(1,ratio.GetNbinsY()+1):
            for i in range(1,ratio.GetNbinsX()+1):
                n = den.Integral(i,i,j,j)
                p = ratio.GetBinContent(i,j)
                if n <= 0 or p>=1:
                    ratio.SetBinError( i, j, 0 )
                    continue
                else:
                    ratio.SetBinError( i, j, (p*(1-p)/n)**0.5 )
    elif 'TH1' in ratio.IsA().GetName():
        for i in range(1,ratio.GetNbinsX()+1):
            n = den.Integral(i,i)
            p = ratio.GetBinContent(i)
            if n <= 0 or p>=1:
                ratio.SetBinError( i, 0 )
                continue
            else:
                ratio.SetBinError( i, (p*(1.-p)/n)**0.5 )
    else:
        print( 'WARNING ' + ratio.GetName() + 'is not a TH1 or TH2' )

#--------------------------------------------------------------------------
#create histograms that require dividing one histogram by another
#works for both efficiency and reco fraction plots, set by plottype variable

def CreateRatioPlot( infile, numHist, denHist, var, projXorY=doProjY, x1=None, x2=None, xtitle = '', plottype = '', doAverage = False ):        
    #require plottype variable for setting plot title, etc
    #plottype = 'eff' or 'RecoFrac' to create efficiency or reco fraction histogram
    if plottype == 'eff':
        typeName = 'efficiency'
    elif plottype == 'RecoFrac':
        typeName = 'Reco Fraction'
    elif plottype == 'purity':
        typeName = 'purity'
    else:
        print('plottype must be one of the following: \'eff\', \'RecoFrac\', \'purity\'' )
        return
    
    PlotNamePrefix = numHist.GetName()[:numHist.GetName().find('_kinematics')] + '_' + typeName.replace(' ','')
    
    #create ratio hist for given eta range
    if (x1 != None and x2 != None):
        if '_' in var:
            if projXorY==doProjX:
                xvar=var.split('_')[1]
                yvar=var.split('_')[0]
            else:
                xvar=var.split('_')[0]
                yvar=var.split('_')[1]
        #PlotName = PlotNamePrefix + '_' + var.split('_')[1] + '_etaRange_{0}_{1}'.format(x1,x2).replace('-','m').replace('.','p')
        #PlotTitle = var.split('_',1)[1].capitalize() + ' ' + typeName
        PlotName = PlotNamePrefix + '_' + yvar + '_'+xvar+'Range_{0}_{1}'.format(x1,x2).replace('-','m').replace('.','p')
        PlotTitle = yvar.capitalize() + ' ' + typeName
                                       
        if x1>=0 and x2>0:
            if projXorY==doProjX:
                ibin1 = numHist.GetYaxis().FindBin(x1)
                ibin2 = numHist.GetYaxis().FindBin(x2)
                num = numHist.ProjectionX('num_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
                den = denHist.ProjectionX('den_'+xvar+"_"+yvar+"Range",ibin1,ibin2)                
            else:
                ibin1 = numHist.GetXaxis().FindBin(x1)
                ibin2 = numHist.GetXaxis().FindBin(x2)
                num = numHist.ProjectionY('num_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
                den = denHist.ProjectionY('den_'+xvar+"_"+yvar+"Range",ibin1,ibin2)                

                if yvar=='eta':
                    if projXorY==doProjX:
                        ibin1 = numHist.GetYaxis().FindBin(-1*x2)
                        ibin2 = numHist.GetYaxis().FindBin(-1*x1)
                        num2 = numHist.ProjectionX('num2_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
                        den2 = denHist.ProjectionX('den2_'+xvar+"_"+yvar+"Range",ibin1,ibin2)                
                    else:
                        ibin1 = numHist.GetXaxis().FindBin(-1*x2)
                        ibin2 = numHist.GetXaxis().FindBin(-1*x1)
                        num2 = numHist.ProjectionY('num2_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
                        den2 = denHist.ProjectionY('den2_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
                    num.Add(num2)
                    den.Add(den2)
                    PlotTitle = PlotTitle + ' ({0}<|eta|<{1})'.format(x1,x2)
                    del num2, den2
                else:
                    if yvar=='pt':
                        PlotTitle = PlotTitle + ' ({0}<pt<{1})'.format(x1,x2)                    
        # else:
        #     if projXorY==doProjX:
        #         ibin1 = numHist.GetYaxis().FindBin(x1)
        #         ibin2 = numHist.GetYaxis().FindBin(x2)
        #         num = numHist.ProjectionX('num_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
        #         den = denHist.ProjectionX('den_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
        #         #PlotTitle = PlotTitle + ' (pt>{0})'.format(eta1)
        #         PlotTitle = PlotTitle + ' ({0}<{2}<{1})'.format(x1,x2,yvar)
        #         PlotName = PlotNamePrefix + '_' + var.split('_')[1] + '_ptMin{0}'.format(x1).replace('-','m').replace('.','p')
        #     else:
        #         ibin1 = numHist.GetXaxis().FindBin(x1)
        #         ibin2 = numHist.GetXaxis().FindBin(x2)
        #         num = numHist.ProjectionY('num_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
        #         den = denHist.ProjectionY('den_'+xvar+"_"+yvar+"Range",ibin1,ibin2)
        #         PlotTitle = PlotTitle + ' ({0}<{2}<{1})'.format(x1,x2,yvar)
    else:
        PlotTitle = var.capitalize() +  ' ' + typeName
        PlotName = PlotNamePrefix + '_' + var
        num = numHist.Clone()
        den = denHist.Clone()
        
    #create variable bin histogram for pt to group high bins together
    if ('_pt' in var and projXorY==doProjY) or var=='pt':
        #rebin the initial pt hist that is in bins of 1 GeV
        ptBins = [ 0, 5, 10, 15, 20, 25, 30, 40, 50, 65, 80, 100, 200 ]
        #ptBins = [ 0, 2,3,4,5,6, 8, 15 ]
        numRebinned = ROOT.TH1D( 'num_'+var, '', len(ptBins)-1, array( 'f', ptBins ) )
        denRebinned = ROOT.TH1D( 'den_'+var, '', len(ptBins)-1, array( 'f', ptBins ) )
        for i in range( 1, numHist.GetNbinsX() + 1 ):
            numRebinned.Fill( num.GetXaxis().GetBinCenter(i), num.GetBinContent(i) )
            denRebinned.Fill( den.GetXaxis().GetBinCenter(i), den.GetBinContent(i) )
    #rebin eta and phi
    else:
        if 'TH1' in den.IsA().GetName():
            numRebinned = num.Clone()
            denRebinned = den.Clone()
            numRebinned.Rebin(2)
            denRebinned.Rebin(2)
        else:
            return

    ## @@@ for eff turn-on curve:
    # num.GetXaxis().SetRangeUser(0,15)
    # den.GetXaxis().SetRangeUser(0,15)

    ratio = numRebinned.Clone( PlotName )
    ratio.Divide( numRebinned, denRebinned, 1, 1 )
    ratio.SetTitle( PlotTitle )
    SetBinomialError( ratio, denRebinned ) #root binomial error is different - custom done

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
        
        if doAverage and den.Integral() > 0 and num.Integral() > 0:
            tot_eff = ROOT.TF1( 'aveline', "[0]", ratio.GetXaxis().GetBinLowEdge(1), ratio.GetXaxis().GetBinUpEdge( ratio.GetNbinsX() ) )
            ratio.GetListOfFunctions().Add( tot_eff )
            f1 = ratio.GetFunction( "aveline" )
            f1.SetParameter( 0, num.Integral()/den.Integral() )
            f1.SetLineColor( ROOT.kRed )
            #f1.SetBit(ROOT.TF1.kNotDraw)
            
    PlotDirName = PlotNamePrefix.replace('__','_').split('_')
    if not infile.GetDirectory( '/'.join( PlotDirName ) ):
        PlotDir = infile.Get( '/'.join( PlotDirName[:-1] ) ).mkdir( PlotDirName[-1] )
    else:
        PlotDir = infile.Get( '/'.join( PlotDirName ) )
    PlotDirName = '/'.join( PlotDirName )
    
    if not infile.GetDirectory(PlotDirName).WriteTObject( ratio, PlotName, "Overwrite" ):
        print('WARNING failed to write histogram to file: ' + PlotDirName + '/' + PlotName )
    del ratio, num, den

#--------------------------------------------------------------------------
def main( argv ):
    """
    Main function to be executed when starting the code.
    """

    if len( argv ) < 2:
        print( 'No filename given' )
        print( 'Usage: python '+argv[0]+' physval_filename [doAverage]' )
        exit(1)

    filename = argv[1]
    if not os.path.exists( filename ):
        print ( 'File not found: ' + filename )
        exit(1)

    if len(argv) > 2 and argv[2] == 'doAverage':
        doAverage = True
    else:
        doAverage = False

    infile = ROOT.TFile.Open( filename, 'update' )

    muonTypesEff  = [ 'All', 'Prompt', 'InFlight', 'NonIsolated' ]
    muonTypesReco = [ 'Prompt', 'InFlight', 'NonIsolated', 'Rest' ]
    Variables = [ 'pt', 'eta', 'phi', 'eta_phi', 'eta_pt' ]

    Xtitles = {
      'pt' : 'p_{T} [GeV]',
      'eta' : '|#eta|',
      'phi' : '#phi',
      'eta_phi' : '#phi',
      'eta_pt' : 'p_{T} [GeV]' }

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
                CreateRatioPlot( infile, matchHist, truthHist, var, xtitle = muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                if var == 'eta_phi' or var == 'eta_pt':
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjY, 0, 2.5, muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjY, 0, 0.1, muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjY, 0.1, 1.05, muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjY, 1.05, 2.0, muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjY, 2.0, 2.5, muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjY, 2.5, 2.7, muType+' Muon '+Xtitles[var], plottype = 'eff', doAverage = doAverage )
                if var == 'eta_pt' and author == 'CaloTag' :
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjX, 10, 1000, muType+' Muon '+Xtitles['eta'], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjX, 15, 1000, muType+' Muon '+Xtitles['eta'], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjX, 20, 1000, muType+' Muon '+Xtitles['eta'], plottype = 'eff', doAverage = doAverage )
                    CreateRatioPlot( infile, matchHist, truthHist, var, doProjX, 25, 1000, muType+' Muon '+Xtitles['eta'], plottype = 'eff', doAverage = doAverage )
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
                CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, xtitle = muType + ' Muon ' + Xtitles[var], plottype = 'RecoFrac', doAverage = doAverage )

    #Purity plots (matched/reco)
    ### for muType in muonTypesReco:
    muType = 'All'
    if not infile.Get( 'Muons/' + muType ):
        print( 'INFO TDirectory not found: Muons/' + muType )
    else:
        #get list of authors from matched dir
        AuthDir = infile.Get( 'Muons/{0}/matched'.format( muType ) )
        if Authors == []:
            Authors = [ i.GetName() for i in AuthDir.GetListOfKeys() if AuthDir.Get( i.GetName() ).InheritsFrom( 'TDirectory' ) ]            
        for author in Authors:            
            typedir = 'Muons/{0}/matched/{1}/kinematicsReco'.format( muType, author )
            alldir = 'Muons/{0}/reco/{1}/kinematics'.format( muType, author )
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
                CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, xtitle = muType + ' Muon ' + Xtitles[var], plottype = 'purity', doAverage = doAverage )
                if var == 'eta_pt': ## purity vs eta in pt ranges
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjX, 4, 500, muType+' Muon '+Xtitles['pt'], plottype = 'purity', doAverage = doAverage )
                if var == 'eta_pt' and author == 'CaloTag' : ## purity vs eta in pt ranges
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjX, 10, 500, muType+' Muon '+Xtitles['pt'], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjX, 15, 500, muType+' Muon '+Xtitles['pt'], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjX, 20, 500, muType+' Muon '+Xtitles['pt'], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjX, 25, 500, muType+' Muon '+Xtitles['pt'], plottype = 'purity', doAverage = doAverage )
                if (var == 'eta_phi' or var == 'eta_pt') and author is not 'CaloTag' :
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjY, 0, 0.1, muType+' Muon '+Xtitles[var], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjY, 0.1, 1.05, muType+' Muon '+Xtitles[var], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjY, 1.05, 2.0, muType+' Muon '+Xtitles[var], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjY, 2.0, 2.5, muType+' Muon '+Xtitles[var], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjY, 0, 2.5, muType+' Muon '+Xtitles[var], plottype = 'purity', doAverage = doAverage )
                    CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, doProjY, 2.5, 2.7, muType+' Muon '+Xtitles[var], plottype = 'purity', doAverage = doAverage )
                                     
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
                CreateRatioPlot( infile, typeRecoHist, allRecoHist, var, xtitle = 'Unmatched Reco Muon '+Xtitles[var], plottype = 'RecoFrac', doAverage = doAverage )
    infile.Close()

#===============================================================================

if __name__ == "__main__":
    """
    Here the code should appear that is executed when running the plotter directly
    (and not import it in another python file via 'import Plotter')
    """
    # start main program
    main( sys.argv )
