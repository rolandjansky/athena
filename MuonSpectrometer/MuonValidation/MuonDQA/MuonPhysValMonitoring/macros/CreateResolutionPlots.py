#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Ioannis.Nomidis@cern.ch
##
## Description: Retrieves Pt-Resolution histograms in bins of pt, eta, phi, fits to extract the mean and sigma (pt scale and resolution) and writes output in pt-resolution/scale -vs- pt/eta/phi histogram, stored in the same file.
##
#--------------------------------------------------------------------------
import ROOT
from ROOT import TFile, TH1F, TMath, TF1, TCanvas, TLatex
import sys
import argparse
import re
import os
import math
import itertools
from array import array
ROOT.gROOT.SetBatch( True )
ROOT.gROOT.Macro("rootlogon.C")

#--------------------------------------------------------------------------

def createResolutionPlotsFromProfile( directory ):
    keys = directory.GetListOfKeys()
    for key in keys:
        obj=key.ReadObj()
        if not obj.IsA().InheritsFrom(ROOT.TProfile.Class()):
            continue
        #print obj.GetName()
        namePrefix = directory.GetPath()[directory.GetPath().index(':')+2:].replace('/','_')
        #print '   ',namePrefix

        binVar=''
        if '_vs_eta' in obj.GetName() :
            binVar='eta'
        elif '_vs_phi' in obj.GetName() :
            binVar='phi'
        elif '_vs_pT' in obj.GetName() :
            binVar='pT'

        if not binVar:
            print 'Cannot find x-axis variable in profile histogram ',obj.GetName()


        hScale = obj.ProjectionX(namePrefix+'_PtScale_vs_'+binVar,"E")
        hScale.SetYTitle("pT scale");
        hResol = obj.ProjectionX(namePrefix+'_PtResol_vs_'+binVar,"C=E")
        hResol.SetYTitle("pT resolution");
                
        nbins = obj.GetNbinsX()+1
        for i in range(1,nbins):
            #print i,' ',obj.GetBinContent(i),' ',obj.GetBinEffectiveEntries(i),' ',obj.GetBinError(i)
            mean = hScale.GetBinContent(i)
            sigma = hResol.GetBinContent(i)
            n = obj.GetBinEffectiveEntries(i)
            meanError = 0
            sigmaError = 0
            if n>0:
                meanError = mean/math.sqrt(n)
                sigmaError = sigma/math.sqrt(n)

            hScale.SetBinError(i, meanError )
            hResol.SetBinError(i, sigmaError )
            
        directory.WriteTObject( hScale, hScale.GetName(), "Overwrite" )
        directory.WriteTObject( hResol, hResol.GetName(), "Overwrite" )        


    
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
        #print binDirName
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

        t = TLatex()
        t.SetNDC(); t.SetTextColor(1);
        tit = binDirName.replace('m','-').replace('bin_','').replace('_','<'+binVar+'<')

        if Res_pT_inBin.Integral() < 20:
            print 'WARNING Not enough entries in histogram to do fit in ', binDirName
            means+= [ 0 ]
            meansErr+= [ 0 ]
            sigmas+= [ 0 ]
            sigmasErr+= [ 0 ]

            Res_pT_inBin.DrawCopy()
            t.DrawLatex(0.2,0.96,tit);
            t.DrawLatex(0.2,0.85,'n/a');
            continue
        
        range = 2*Res_pT_inBin.GetRMS()
        Res_pT_inBin.Fit("gaus","q","",-range,range)
        func = Res_pT_inBin.GetFunction("gaus")
        func.SetLineColor(ROOT.kRed)
        means+= [ func.GetParameter(1) ]
        meansErr+= [ func.GetParError(1) ]
        sigmas+= [ func.GetParameter(2) ]
        sigmasErr+= [ func.GetParError(2) ]


        canv.cd(ipad);
        #        func.SetLineColor(8);
        Res_pT_inBin.SetTitle(tit)
        Res_pT_inBin.DrawCopy()
        t.DrawLatex(0.2,0.96,tit);
        resultMeanLab ='#mu = {0} #pm {1}'.format( format(func.GetParameter(1),'.2g'), format(func.GetParError(1),'.2g') )
        resultSigmaLab ='#sigma = {0} #pm {1}'.format( format(func.GetParameter(2),'.2g'),format(func.GetParError(2),'.2g') )
        t.DrawLatex(0.2,0.85,'#splitline{'+resultMeanLab+'}{'+resultSigmaLab+'}')
        
    bins+=[ float(binDirName.split( '_',2)[2].replace('m','-')) ]
    nbins = len(bins)-1

    xtitle = ''
    if binVar=='pt':
        xtitle = 'pT [GeV]'
    else :
        xtitle = binVar
        
    hPtScale_vs_var = TH1F('PtScaleFitted_vs_'+binVar,'PtScaleFitted_vs_'+binVar+';'+xtitle+';p_{T} scale',nbins,array('f',bins))
    hPtRes_vs_var = TH1F('PtResFitted_vs_'+binVar,'PtResFitted_vs_'+binVar+';'+xtitle+';p_{T} resolution',nbins,array('f',bins))

    for i in xrange(0,nbins):
        hPtScale_vs_var.SetBinContent(i+1,means[i])
        hPtScale_vs_var.SetBinError(i+1,meansErr[i])
        hPtRes_vs_var.SetBinContent(i+1,sigmas[i])
        hPtRes_vs_var.SetBinError(i+1,sigmasErr[i])

    print 'Updating file with ' + namePrefix+"_PtScaleFitted_vs_"+binVar
    outputDirectory.WriteTObject( hPtScale_vs_var, namePrefix+"_PtScaleFitted_vs_"+binVar, "Overwrite" )
    print 'Updating file with ' + namePrefix+"_PtResFitted_vs_"+binVar
    outputDirectory.WriteTObject( hPtRes_vs_var, namePrefix+"_PtResFitted_vs_"+binVar, "Overwrite" )

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
  resolutionTypes = [ 'MS','ID','' ]
  resolutionTypesStr = [ 'MS','ID','CB' ]
  colors = [ ROOT.kRed, 8 , ROOT.kBlack ]

  momentumParameters = ['PtResol','PtScale']
  binVariables = [ 'pT', 'eta', 'phi' ]

  for muType, author, resolType in itertools.product( muonTypes, Authors, resolutionTypes):
      resolutionHistDirName = 'Muons/{0}/matched/{1}/resolution{2}'.format( muType,author,resolType )
      resolutionHistDir = infile.GetDirectory( resolutionHistDirName )
      createResolutionPlotsFromProfile( resolutionHistDir )
      for binVar in binVariables:
          outputDir = resolutionHistDir
          resolutionHistDirVsVarName = resolutionHistDirName+'/Resolution_vs_'+binVar.lower()
          resolutionHistDirVsVar = infile.GetDirectory( resolutionHistDirVsVarName )
          if (resolutionHistDirVsVar):
              createResolutionPlots( resolutionHistDirVsVar, binVar, outputDir )
          else:
              print 'Warning: ' + resolutionHistDirVsVarName + ' does not exist! (ok if you do not want the per-bin resolution fits'
  c1 = TCanvas()
      
  for muType, author, momPar, binVar in itertools.product( muonTypes, Authors, momentumParameters, binVariables ):
      c1.Clear()
      
      leg = ROOT.TLegend(0.2,0.76,0.45,0.92)
      if not binVar=='pT':
          leg = ROOT.TLegend(0.7,0.76,0.96,0.92)
      leg.SetFillStyle(0)
      leg.SetBorderSize(0)
      leg.SetTextFont(43)
      leg.SetTextSizePixels(24)

      for i in xrange(0,3):
          h = infile.Get('Muons/{0}/matched/{1}/resolution{2}/Muons_{0}_matched_{1}_resolution{2}_{3}_vs_{4}'.format( muType,author,resolutionTypes[i],momPar,binVar ) )
          h.SetLineColor(colors[i])
          h.SetMarkerColor(colors[i])
          leg.AddEntry(h, resolutionTypesStr[i], 'lp' )
          if i==0:
              h.DrawCopy("e")
          else:
              h.DrawCopy("esame")

      leg.Draw()
      c1.Print('{0}_{1}_{2}_vs_{3}.pdf'.format(momPar,muType,author,binVar) )
              

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
