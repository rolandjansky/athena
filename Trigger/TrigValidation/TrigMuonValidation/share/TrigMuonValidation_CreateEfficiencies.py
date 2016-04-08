#!/usr/bin/env python

#Written by Dan Mori
#Create efficiency plots for each author by dividing matched/author pt by truth/all pt


#--------------------------------------------------------------------------
import ROOT
import sys
import itertools
ROOT.gROOT.SetBatch( True )
#--------------------------------------------------------------------------

def CreateEffPlot( matchDir, truthDir ):
    Variables = [ 'pt', 'eta', 'phi' ]
    Xtitles = {
    'pt' : 'Transverse Momentum [GeV]',
    'eta' : 'eta',
    'phi' : 'phi'
    }
    for var in Variables:
        matchNamePrefix = matchDir.GetPath()[matchDir.GetPath().index(':')+2:].replace('/','_')
        truthNamePrefix = truthDir.GetPath()[truthDir.GetPath().index(':')+2:].replace('/','_')
        match = matchDir.Get( matchNamePrefix+'_'+var ).Clone()
        truth = truthDir.Get( truthNamePrefix+'_'+var ).Clone()
        PlotName = matchNamePrefix+'_Eff_{0}'.format( var )

        match.Rebin(2)
        truth.Rebin(2)
        
        ratio = match.Clone()
        ratio.Divide(truth)
        ratio.SetTitle( var.capitalize() + ' Efficiency' )
        ratio.GetXaxis().SetTitle( Xtitles[var] )
        ratio.GetYaxis().SetTitle( 'efficiency' )
        #Set error as binomial error
        for i in range(1,ratio.GetNbinsX()+1):
            if truth.Integral(i,i) <= 0:
                ratio.SetBinError( i, 0 )
                continue
            
            bin = ratio.GetBinContent(i)
            ratio.SetBinError( i, (bin*(1-bin)/truth.Integral(i,i))**0.5 )
        
        matchDir.WriteTObject( ratio, PlotName, "Overwrite" )
        del ratio, match, truth

#--------------------------------------------------------------------------

def main( argv ):
 if len( argv ) < 2:
     print( 'No filename given' )
     print( 'Usage: python '+argv[0]+' physval_filename' )
     exit(1)
	
 filename = argv[1]
 if not os.path.exists( filename ):
     print ( 'File not found: ' + filename )
     exit(1)
 try:
     infile = ROOT.TFile.Open(filename,'update')
 except:
     print( 'Cannot open file: {0}'.format( filename ) )

 muonTypes = [ 'All']# 'Prompt', 'InFlight', 'Rest' ]
 Authors = [ 'AllMuons' ]#[ 'MuidCombined', 'MuTagIMO', 'MuidStandalone', 'MuGirl', 'CaloTag', 'CaloLikelihood', 'AllAuthors' ]

 for muType, author in itertools.product( muonTypes, Authors ):
     truthDir = infile.GetDirectory( 'Muons/{0}/truth/all'.format( muType ) )
     matchDir = infile.GetDirectory( 'Muons/{0}/matched/{1}'.format( muType, author ) )
     CreateEffPlot( matchDir, truthDir )

 infile.Close()


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
	
