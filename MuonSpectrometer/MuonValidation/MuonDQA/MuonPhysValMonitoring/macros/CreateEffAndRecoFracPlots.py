# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Written by Dan Mori
#Create efficiency plots for each author by dividing matched/author pt by truth/all pt

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

def CreateRatioPlot( numDir, denDir, var, eta1=None, eta2=None, xtitle = '', plottype = '' ):
    #require plottype variable for setting plot title, etc
    #plottype = 'Eff' or 'RecoFrac' to create efficiency or reco fraction histogram
    if plottype == '':
        print( 'plottype not set: running Efficiency plot by default\n' )
        plottype = 'Eff'

    if plottype == 'Eff':
        typeName = 'Efficiency'
    elif plottype == 'RecoFrac':
        typeName = 'Reco Fraction'
    else:
        print('plottype must be one of the following: \'Eff\', \'RecoFrac\'' )
        return

    numNamePrefix = numDir.GetPath()[numDir.GetPath().index(':')+2:].replace('/','_')
    denNamePrefix = denDir.GetPath()[denDir.GetPath().index(':')+2:].replace('/','_')
    try:
        num = numDir.Get( numNamePrefix+'_'+var ).Clone()
    except:
        print('WARNING histogram not found: '+numNamePrefix+'_'+var)
        return
    try:
        den = denDir.Get( denNamePrefix+'_'+var ).Clone()
    except:
        print('WARNING histogram not found: '+numNamePrefix+'_'+var)
        return

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
        ratio.GetYaxis().SetTitle( typeName.lower() )
        if '_' in var:
            ratio.GetXaxis().SetTitle( var.split('_')[1] )
        else:
            if xtitle == '':
                xtitle = var
            ratio.GetXaxis().SetTitle( xtitle )
            SetBinomialError( ratio, den )    

    numDir.WriteTObject( ratio, PlotName, "Overwrite" )
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

    #muonTypes = [ 'All', 'Prompt', 'InFlight', 'Rest' ]
    #Authors = [ 'MuidCombined', 'MuTagIMO', 'MuidStandalone', 'MuGirl', 'CaloTag', 'CaloLikelihood', 'AllAuthors' ]
    muonTypesEff = [ 'Prompt' ]
    muonTypesReco = [ 'InFlight', 'Rest' ] #, 'UnmatchedReco' ]
    Authors = [ 'MuidCombined', 'MuTagIMO', 'MuidStandalone', 'CaloTag' ]
    Variables = [ 'pt', 'eta', 'phi', 'eta_phi', 'eta_pt' ]

    Xtitles = {
      'pt' : 'Transverse Momentum [GeV]',
      'eta' : 'eta',
      'phi' : 'phi',
      'eta_phi' : 'eta',
      'eta_pt' : 'eta' }

    for muType, author in itertools.product( muonTypesEff, Authors ):
        truthDir = infile.GetDirectory( 'Muons/{0}/truth/all'.format( muType ) )
        matchDir = infile.GetDirectory( 'Muons/{0}/matched/{1}'.format( muType, author ) )
        for var in Variables:
            CreateRatioPlot( matchDir, truthDir, var, xtitle = Xtitles[var], plottype = 'Eff' )
            if var == 'eta_phi' or var == 'eta_pt':
                CreateRatioPlot( matchDir, truthDir, var, -0.1, 0.1, Xtitles[var], 'Eff' )
                CreateRatioPlot( matchDir, truthDir, var, 0.1, 1.05, Xtitles[var], 'Eff' )
                CreateRatioPlot( matchDir, truthDir, var, 1.05, 2.0, Xtitles[var], 'Eff' )
                CreateRatioPlot( matchDir, truthDir, var, 2.0, 2.5, Xtitles[var], 'Eff' )

    for muType, author in itertools.product( muonTypesReco, Authors ):
#        if muType == 'UnmatchedReco':
#            typeRecoDir = infile.GetDirectory( 'Muons/{0}/' )
#        else:
        typeRecoDir = infile.GetDirectory( 'Muons/{0}/reco/{1}'.format( muType, author ) )
        allRecoDir = infile.GetDirectory( 'Muons/All/reco/{0}'.format( author ) )

        for var in Variables:
            CreateRatioPlot( typeRecoDir, allRecoDir, var, xtitle = Xtitles[var], plottype = 'RecoFrac' )

    infile.Close()

#===============================================================================

if __name__ == "__main__":
    """
    Here the code should appear that is executed when running the plotter directly
    (and not import it in another python file via 'import Plotter')
    """

    import ROOT
    import os
    import sys
    import itertools
    ROOT.gROOT.SetBatch( True )

    # start main program    
    main( sys.argv )

