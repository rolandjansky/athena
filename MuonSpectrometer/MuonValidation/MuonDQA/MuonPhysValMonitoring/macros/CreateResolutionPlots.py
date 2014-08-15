# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Ioannis.Nomidis@cern.ch
##
## Description: Retrieves Pt-Resolution histograms in bins of pt, eta, phi, fits to extract the mean and sigma (pt scale and resolution) and writes output in pt-resolution/scale -vs- pt/eta/phi histogram, stored in the same file.
##
#--------------------------------------------------------------------------
import ROOT
from ROOT import TFile, TH1F, TMath, TF1, TCanvas
import sys
import argparse
import re
import os
import math
import itertools
from array import array
ROOT.gROOT.SetBatch( True )
#--------------------------------------------------------------------------

def createResolutionPlots( directory, binVar, outputDirectory ):

    keys = directory.GetListOfKeys()
    bins = [ ]
    means = [ ]
    meansErr = [ ]        
    sigmas = [ ]
    sigmasErr = [ ]

    namePrefix = outputDirectory.GetPath()[outputDirectory.GetPath().index(':')+2:].replace('/','_')
    
    
    canv = TCanvas("canv","",800,800);
    canv.Divide(3,3)

    ipad = 0
    icanv = 0
    
    for key in keys:
        binDir = key.ReadObj()
        if binDir.IsA().InheritsFrom(ROOT.TH1.Class()):
            continue
        
        binDirName =  binDir.GetName()
        bins+=[ float(binDirName.split( '_',2)[1].replace('m','-')) ]

        ipad+=1
        if ipad>9:
            canv.cd()
            canv.Print(storePlots+namePrefix+"_PtResFits_{0}_bins_{1}.pdf".format(binVar,icanv))
            canv.Clear()
            canv.Divide(3,3)
            icanv+=1
            ipad=1
        canv.cd(ipad)
        
        Res_pT_inBin = 0
        ## Find pt-resolution histogram for this bin
        histKeys = binDir.GetListOfKeys()
        for hkey in histKeys:
            h = hkey.ReadObj()
            if h.IsA().InheritsFrom(ROOT.TH1.Class()) and '_Res_pT' in h.GetName() :
                Res_pT_inBin = h.Clone()
                break
        if not Res_pT_inBin:
            print 'ERROR Res_pT hist not found in ', binDirName

        if Res_pT_inBin.Integral() < 20:
            print 'WARNING Not enough entries in histogram to do fit in ', binDirName
            means+= [ 0 ]
            meansErr+= [ 0 ]
            sigmas+= [ 0 ]
            sigmasErr+= [ 0 ]

            Res_pT_inBin.DrawCopy()
            continue
        
        range = 2*Res_pT_inBin.GetRMS()
        Res_pT_inBin.Fit("gaus","q","",-range,range)
        func = Res_pT_inBin.GetFunction("gaus")
        means+= [ func.GetParameter(1) ]
        meansErr+= [ func.GetParError(1) ]
        sigmas+= [ func.GetParameter(2) ]
        sigmasErr+= [ func.GetParError(2) ]


        canv.cd(ipad);
        #        func.SetLineColor(8);
        Res_pT_inBin.SetTitle(binDirName.replace('m','-').replace('bin_','').replace('_','<'+binVar+'<'))
        Res_pT_inBin.DrawCopy()

        
    bins+=[ float(binDirName.split( '_',2)[2].replace('m','-')) ]
    nbins = len(bins)-1

    xtitle = ''
    if binVar=='pt':
        xtitle = 'pT [GeV]'
    else :
        xtitle = binVar
        
    hPtScale_vs_var = TH1F('PtScale_vs_'+binVar,xtitle+';p_{T} scale;',nbins,array('f',bins))
    hPtRes_vs_var = TH1F('PtRes_vs_'+binVar,xtitle+';p_{T} resolution;',nbins,array('f',bins))

    for i in xrange(0,nbins):
        hPtScale_vs_var.SetBinContent(i+1,means[i])
        hPtScale_vs_var.SetBinError(i+1,meansErr[i])
        hPtRes_vs_var.SetBinContent(i+1,sigmas[i])
        hPtRes_vs_var.SetBinError(i+1,sigmasErr[i])

    outputDirectory.WriteTObject( hPtScale_vs_var, namePrefix+"_PtScale_vs_"+binVar, "Overwrite" )
    outputDirectory.WriteTObject( hPtRes_vs_var, namePrefix+"_PtRes_vs_"+binVar, "Overwrite" )

    canv.cd()
    canv.Print(storePlots+namePrefix+"_PtResFits_{0}_bins_{1}.pdf".format(binVar,icanv))
    canv.Close()
    
#======================================================================
def main( argv ):
  """
  Main function to be executed when starting the code.
  """

  try:
      filename = sys.argv[1]
  except:
      print( 'No filename given' )
      print( 'Usage: python '+sys.argv[0]+' physval_filename' )

  try:
      infile = ROOT.TFile.Open(filename,'update')
  except:
      print( 'Cannot open file: {0}'.format( filename ) )

  ## ROOT.gROOT.Macro("~/rootlogon.C")

  #muonTypes = [ 'All', 'Prompt', 'InFlight', 'Rest' ]
  #Authors = [ 'MuidCombined', 'MuTagIMO', 'MuidStandalone', 'MuGirl', 'CaloTag', 'CaloLikelihood', 'AllAuthors' ]
  muonTypes = [ 'Prompt' ]
  Authors = [ 'MuidCombined' ]
  resolutionTypes = [ '','MS','ID' ]
  binVariables = [ 'pt', 'eta', 'phi' ]
  
  for muType, author, resolType, binVar in itertools.product( muonTypes, Authors, resolutionTypes, binVariables ):
      resolutionHistDirName = 'Muons/{0}/matched/{1}/resolution{2}/Resolution_vs_{3}'.format( muType,author,resolType,binVar )
      resolutionHistDir = infile.GetDirectory( resolutionHistDirName )
      outputDir = infile.GetDirectory( 'Muons/{0}/matched/{1}/resolution{2}'.format( muType,author,resolType ) )
      createResolutionPlots( resolutionHistDir, binVar, outputDir )

  infile.Close()

 #======================================================================

if __name__ == "__main__":
  """
  Here the code should appear that is executed when running the plotter directly
  (and not import it in another python file via 'import Plotter')
  """

  # start main program
  storePlots = "resolutionFits/"
  os.system('mkdir '+storePlots)
  main( sys.argv[1:] )
