# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""This script compares two given root files and saves plots where they differ
as pdfs.

author: Felix Socher  <Felix.Socher@cern.ch>"""


import ROOT
from ROOT import *
import sys
import argparse
import re
import os
import math

LABELS = []

def GoToDirectory(directory):
  cwd = os.getcwd()
  print os.getcwd()
  newdir = cwd + "/" + directory 
  if not os.path.isdir( newdir ):
    print "Creating folder " + newdir
    os.makedirs( newdir )
  os.chdir(newdir)


def SameHist(refHist, testHist):
  chi2 = 0
  for x in range(refHist.GetNbinsX()):
    for y in range(refHist.GetNbinsY()):
      for z in range(refHist.GetNbinsZ()):
        if refHist.GetBinContent(x,y,z) != testHist.GetBinContent(x,y,z): return False
  return True

def SimilarHist(refHist, testHist):
  for x in range(refHist.GetNbinsX()):
    for y in range(refHist.GetNbinsY()):
      for z in range(refHist.GetNbinsZ()):
        if abs(refHist.GetBinContent(x,y,z) - testHist.GetBinContent(x,y,z)) > 0.01: return False
  return True




class Validator(object):
  """docstring for Validator"""
  def __init__(self, structDirs, exclude, compareFunc, normalization):
      super(Validator, self).__init__()
      self.structDirs = structDirs
      #self.excludedStrings = exclude.split("|")
      #print self.excludedStrings
      self.CompareFunc = compareFunc
      self.DoNormalization = normalization
  
  def CompareFiles(self, fileNames, dirName):
    openedFiles = [ TFile.Open(f) for f in fileNames ]
    self.CompareDirectories(openedFiles, dirName)
  
  def CompareDirectories(self, dirs, dirName):
    if (self.structDirs): GoToDirectory(dirName)
    refDir = dirs[0]
    newRefDir = refDir.GetDirectory(dirName)
    for testDir in dirs:
      if testDir == refDir: continue
      if not testDir:
        print 'ERROR --------- test dir not found: ', dirName, testDir
        return

    newTestDirs = [ testDir.GetDirectory(dirName) for testDir in dirs ]
    
    theDir = newRefDir
    # if not newRefDir:
    #   print 'ERROR --------- reference dir not found: ', dirName
    #   theDir = newTestDir

    for key in theDir.GetListOfKeys():
          
      obj = key.ReadObj()

      #2D histograms cannot be compared; skip
      if obj.IsA().InheritsFrom(ROOT.TH2.Class()):
        continue
      
      if obj.IsA().InheritsFrom(ROOT.TH1.Class()):
        print key.GetName()
        #testHist = newTestDir.Get(key.GetName())
        testHists = [ newTestDir.Get(key.GetName()) for newTestDir in newTestDirs ]
        self.CompareHistograms( testHists, newRefDir.GetPath())
      elif obj.IsA().InheritsFrom(ROOT.TDirectory.Class()):
        self.CompareDirectories( newTestDirs, obj.GetName())
    if (self.structDirs): os.chdir("..")
  
  def CompareHistograms(self, hists, path):
    # print self.excludedStrings, refHist.GetName()
    # if any(exString in refHist.GetName() for exString in self.excludedStrings): 
    #   print "Skipped " + path + "/" + refHist.GetName()
    #   return
    #@@@if self.CompareFunc(refHist, testHist): print path + refHist.GetName() + " looks ok"
    #else:
    #self.MakeComparisonPlot(refHist, testHist, path)
    self.MakeComparisonPlot(hists, path)
  
  #def MakeComparisonPlot(self,refHist, testHist, path):
  def MakeComparisonPlot(self, hists, path):
    def SetBounds(hists, ymin=0, ymax=0):
      if (ymin==0 and ymax==0):
        ymin = min( [ h.GetMinimum() for h in hists ] )
        ymax = min( [ h.GetMaximum() for h in hists ] )
        if ymin>0 and ymax>0:
          ymin = 0
        else:          
          ymin = ymin - 0.15*abs(ymin)
        ymax = ymax+ 0.15*ymax
        # if ymin<0 and ymax>0:
        #     if ymax>0 and abs(minimum)>ymax:
        #   ymax = -minimum
      hists[0].SetMinimum(ymin)
      hists[0].SetMaximum(ymax)
      return hists[0]
    ################################## end SetBounds
          
    # try:
    #   refHist.Scale(1.0/refHist.Integral())
    #   testHist.Scale(1.0/testHist.Integral())
    # except:
    #   return
    canvas = ROOT.TCanvas( "", "", 900, 900 )
    padMain = ROOT.TPad( 'padMain', 'padMain', 0, 0.3, 1, 1 )
    padMain.SetBottomMargin( 0.02 )
    padMain.Draw()
    #padMain.SetLogy(1) #@@@
    padRatio = ROOT.TPad( 'padRatio', 'padRatio', 0, 0, 1, 0.3 )
    padRatio.SetTopMargin( 0.01 )
    padRatio.SetBottomMargin( 0.25 )
    padRatio.Draw()
    ROOT.TLine()
    padMain.cd()
    
    leg = ROOT.TLegend(0.82,0.78,0.96,0.94)  
    #leg = ROOT.TLegend(0.52,0.76,0.96,0.94)  
    #leg.SetFillColor(ROOT.kWhite)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetTextFont(43)
    leg.SetTextSizePixels(32)    
    
    refHist = hists[0]
    refHist.SetLineColor(17)
    #refHist.SetFillColor(30)

    ## @@@ Legend!!!
    #leg.AddEntry(hists[0], 'ref', 'lp')
    #leg.AddEntry(hists[1], 'test', 'lp')
    #if len(hists)>2:
    #   for i in range(2,len(hists)):
    #       leg.AddEntry(hists[i], 'test{0}'.format(i), 'lp')
    for i in range(0,len(hists)):      
      leg.AddEntry(hists[i], LABELS[i], 'lp')
              
    if self.DoNormalization and not "_Eff_" in refHist.GetName() and not "_eff" in refHist.GetName() and not "_Eff" in refHist.GetName():
      for h in hists:
        n = h.Integral()
        if n>0:
          h.Scale(1./n)
          for i in range (1,h.GetXaxis().GetNbins()+1):
            h.SetBinError(i,h.GetBinError(i)/n);
                
    refHist = SetBounds(hists)
    
    ref_textsize = 32./(padMain.GetWh()*padMain.GetAbsHNDC())
    refHist.GetYaxis().SetLabelSize( ref_textsize )
    refHist.GetXaxis().SetLabelSize( 0 )
    refHist.GetXaxis().SetTitleSize( 0 )
    refHist.GetYaxis().SetTitleSize( 1.3*ref_textsize )
    refHist.GetYaxis().SetTitleOffset(1)
    refHist.GetYaxis().SetTitleColor( kAzure )

    # testHist.GetYaxis().SetTextFont(43)
    # testHist.GetYaxis().SetTextSizePixels(20)
    refHist.SetMarkerSize(0)
    refHist.SetLineColor(ROOT.kRed)
    #refHist.GetYaxis().SetRangeUser(0.00001,refHist.GetMaximum()*5) ##@@@

    #testHist.Rebin(2)
    #refHist.Rebin(2);
    histmax = refHist
    histmax.DrawCopy('e')
    # for h in hists:
    #   if h.GetMaximum() > refHist.GetMaximum():
    #     histmax = h
    #   if histmax == refHist:
    #     histmax.DrawCopy("e")
    #   else:
    #     histmax.DrawCopy("ehist")
    nh=0
    for h in hists:
      if h is not histmax:
        if h==refHist:
          h.DrawCopy('histsame')
        else:
          nh=nh+1
          if nh>1:
            h.SetMarkerStyle(0)
            h.SetMarkerColor(0)
            h.SetLineColor(8) # green
            h.SetLineStyle(ROOT.kDashed)
          h.DrawCopy('ehistsame')
    # if refHist.GetMaximum()>histmax.GetMaximum():
    #   refHist.DrawCopy("e")
    #   testHist.DrawCopy("ehistsame")
    #   refHist.DrawCopy("ehistsame")
    # else:
    #   testHist.DrawCopy("ehist")
    #   refHist.DrawCopy("ehistsame")
      
    leg.Draw()
    padRatio.cd()

    for h in hists:
      if h is refHist: continue
      ratioHist = h.Clone()
      ratioHist.Divide(refHist)
      ratioHist = SetBounds( [ratioHist, ratioHist], 0.84,1.16) # 0.941,1.059
      for i in range(ratioHist.GetNbinsX()):
        nref = refHist.GetBinContent(i)
        ntest = h.GetBinContent(i)
        if nref == 0 or ntest == 0:
          ratioHist.SetBinError(i, 0)
        else:
          #error = nref/ntest*math.sqrt((refHist.GetBinError(i)/nref)**2 + (testHist.GetBinError(i)/ntest)**2)
          error = nref/ntest* max(refHist.GetBinError(i)/nref, h.GetBinError(i)/ntest) 
          ratioHist.SetBinError(i, error)
          
      ratioHist_textsize = 32./(padRatio.GetWh()*padRatio.GetAbsHNDC())
      ratioHist.GetYaxis().SetLabelSize( ratioHist_textsize )
      ratioHist.GetXaxis().SetLabelSize( ratioHist_textsize )
      ratioHist.GetXaxis().SetTitleSize( 1.2*ratioHist_textsize )
      ratioHist.GetXaxis().SetTitleOffset(0.75)
      ratioHist.GetXaxis().SetTitleColor(kAzure)
      ratioHist.GetYaxis().SetTitleSize( ratioHist_textsize )
      ratioHist.GetYaxis().SetTitleOffset(0.6)

    
      ratioHist.SetLineColor(ROOT.kBlack)
      ratioHist.SetMarkerStyle(24)
      ratioHist.SetYTitle("test / ref")
      if h == hists[1]:
        ratioHist.DrawCopy("p")
        lineRatio = ROOT.TLine( ratioHist.GetXaxis().GetXmin(), 1, 
                              ratioHist.GetXaxis().GetXmax(), 1 ) 
        lineRatio.SetLineColor( ROOT.kRed )
        lineRatio.SetLineWidth( 2 )
        lineRatio.Draw("same")
      else:
        ratioHist.SetMarkerStyle(25)
        ratioHist.SetMarkerColor(8)
        ratioHist.SetLineColor(8)
        ratioHist.DrawCopy("psame")
      
    npath = ""
    if not self.structDirs:
      npath = path[path.find(":/")+2:] + "/"
      npath = re.sub(r"[:,./]", "_", npath+"/")

    canvas.cd()
    t = ROOT.TLatex();
    t.SetNDC()
    t.SetTextColor(1)
    t.SetTextSize(0.03);
    t.DrawLatex(0.,0.97,refHist.GetName()) #@@@

    canvas.SaveAs(npath + refHist.GetName() + ".pdf")
    canvas.Close()

#======================================================================
def main( argv ):
  """
  Main function to be executed when starting the code.
  """

  parser = argparse.ArgumentParser( description = 'Distribution Plotter' )
  parser.add_argument( '-s', '--structDirs', default = True, action = "store_true", help = ' if true, it creates directories following the same structure as in the root file to store the pdf plots')
  parser.add_argument( '-r', '--reference', help = 'The reference' )
  parser.add_argument( '-t', '--test', help = 'The test' )
  parser.add_argument( '-t2', '--test2', default ='', help = 'The additional test' )
  parser.add_argument( '-d', '--directory', default = "/", help = 'Print cutflow fror systematic variations' )
  parser.add_argument( '-e', '--exclude', default = "_bin_", help = 'histograms whose names contain the provided strings are not examined')
  parser.add_argument( '-n', '--normalize', default = False, action = "store_true", help = 'normalize histograms with larger stats for better comparison')
  parser.add_argument( '-l', '--labels', default ='', help = 'Add text to legend for ref/test/test2, split with commas' )

  args = parser.parse_args()


  #ROOT.gROOT.Macro("rootlogon.C")
  ROOT.gROOT.SetBatch()
  #ROOT.gROOT.LoadMacro("./AtlasUtils.C") 
  ROOT.gROOT.LoadMacro("./AtlasStyle.C")
  ROOT.TH1.SetDefaultSumw2(ROOT.kTRUE)
  
  SetAtlasStyle()

  LABELS = []
  if args.labels is '':
    LABELS = [ 'Ref','Test' ]
    if args.test2 is not '':
      LABELS.append( 'Test2' )
  else:
    LABELS = args.labels.split(",")
    #print LABELS
    
  # if you want to pass labels to legend directly from the file name...:
  #LABELS.append( args.test.split('.')[2]+' '+args.test.split('.')[3] )
  #
    
  validator = Validator( args.structDirs, args.exclude, SameHist, args.normalize )
  if args.test2 == '':
    allFiles = [ os.path.abspath(args.reference), os.path.abspath(args.test) ]
  else:
    allFiles = [ os.path.abspath(args.reference), os.path.abspath(args.test), os.path.abspath(args.test2) ]
 #======================================================================

if __name__ == "__main__":
  """
  Here the code should appear that is executed when running the plotter directly
  (and not import it in another python file via 'import Plotter')
  """

  # start main program    
  main( sys.argv[1:] )

