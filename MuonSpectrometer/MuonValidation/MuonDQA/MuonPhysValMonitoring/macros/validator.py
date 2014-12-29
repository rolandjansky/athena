# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""This script compares two given root files and saves plots where they differ
as pdfs.

author: Felix Socher  <Felix.Socher@cern.ch>"""


import ROOT
from ROOT import TFile, TH1
import sys
import argparse
import re
import os
import math


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
      self.excludedStrings = exclude.split("|")
      print self.excludedStrings
      self.CompareFunc = compareFunc
      self.DoNormalization = normalization
  
  def CompareFiles(self,reference, test, dirName):
    refFile  = TFile.Open(reference)
    testFile = TFile.Open(test)
    self.CompareDirectories(refFile, testFile, dirName)
  
  def CompareDirectories(self, refDir, testDir, dirName):
    if (self.structDirs): GoToDirectory(dirName)
    newRefDir = refDir.GetDirectory(dirName)
    newTestDir = testDir.GetDirectory(dirName)
  
    for key in newRefDir.GetListOfKeys():
     
      obj = key.ReadObj()

      #2D histograms cannot be compared; skip
      if obj.IsA().InheritsFrom(ROOT.TH2.Class()):
        continue
      
      if obj.IsA().InheritsFrom(ROOT.TH1.Class()):
        testHist = newTestDir.Get(key.GetName())
        self.CompareHistograms(obj, testHist, newRefDir.GetPath())
      elif obj.IsA().InheritsFrom(ROOT.TDirectory.Class()):
        self.CompareDirectories(newRefDir, newTestDir, obj.GetName())
    if (self.structDirs): os.chdir("..")
  
  def CompareHistograms(self, refHist, testHist, path):
    print self.excludedStrings, refHist.GetName()
    if any(exString in refHist.GetName() for exString in self.excludedStrings): 
      print "Skipped " + path + "/" + refHist.GetName()
      return

    if self.CompareFunc(refHist, testHist): print path + refHist.GetName() + " looks ok"
    else: self.MakeComparisonPlot(refHist, testHist, path)
  
  def MakeComparisonPlot(self,refHist, testHist, path):
    def SetBounds(refHist, testHist, setMinToZero):

      if setMinToZero:        
        maximum = max(refHist.GetMaximum(), testHist.GetMaximum())
        maximum = maximum + 0.1*abs(maximum)
        minimum = min(refHist.GetMinimum(), testHist.GetMinimum())
        minimum = minimum - 0.1*abs(minimum)

        if minimum<0:
            refHist.SetMinimum(minimum)
            maximum = -minimum
        else:
            refHist.SetMinimum(0)
        refHist.SetMaximum(maximum)
      else:
        refHist.SetMinimum(0.35)
        refHist.SetMaximum(1.65)
      return refHist
    
    
    # try:
    #   refHist.Scale(1.0/refHist.Integral())
    #   testHist.Scale(1.0/testHist.Integral())
    # except:
    #   return
  
    canvas = ROOT.TCanvas( "", "", 900, 900 )
    padMain = ROOT.TPad( 'padMain', 'padMain', 0, 0.3, 1, 1 )
    padMain.SetBottomMargin( 0.02 )
    padMain.Draw()
    padRatio = ROOT.TPad( 'padRatio', 'padRatio', 0, 0, 1, 0.3 )
    padRatio.SetTopMargin( 0.01 )
    padRatio.SetBottomMargin( 0.25 )
    padRatio.Draw()
    ROOT.TLine()
    padMain.cd()

    leg = ROOT.TLegend(0.82,0.78,0.96,0.94)
    #leg.SetFillColor(ROOT.kWhite)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetTextFont(43)
    leg.SetTextSizePixels(24)
    leg.AddEntry(refHist, "ref", 'lf')
    leg.AddEntry(testHist, "test",'lp')
  
    refHist.SetLineColor(17)
    refHist.SetFillColor(17)

    if self.DoNormalization and not "_Eff_" in refHist.GetName():
      i1 = 1.*refHist.Integral()
      i2 = 1.*testHist.Integral()
      if i1>i2:
        refHist.Scale(i2/i1)
      elif i2>0:
        testHist.Scale(i1/i2)
        
    refHist = SetBounds(refHist, testHist, True)

    ref_textsize = 24./(padMain.GetWh()*padMain.GetAbsHNDC())
    refHist.GetYaxis().SetLabelSize( ref_textsize )
    refHist.GetXaxis().SetLabelSize( 0 )
    refHist.GetXaxis().SetTitleSize( 0 )
    refHist.GetYaxis().SetTitleSize( ref_textsize )

    # testHist.GetYaxis().SetTextFont(43)
    # testHist.GetYaxis().SetTextSizePixels(20)

    refHist.Draw("hist")
    testHist.Draw("sameE")


    
    leg.Draw()
    padRatio.cd()
    ratioHist = testHist.Clone()
    ratioHist.Divide(refHist)
    ratioHist = SetBounds(ratioHist, ratioHist, False)
    for i in range(ratioHist.GetNbinsX()):
      nref = refHist.GetBinContent(i)
      ntest = testHist.GetBinContent(i)
      if nref == 0 or ntest == 0:
        ratioHist.SetBinError(i, 0)
      else:
        error = nref/ntest*math.sqrt((refHist.GetBinError(i)/nref)**2 + (testHist.GetBinError(i)/ntest)**2)
        ratioHist.SetBinError(i, error)

    ratioHist_textsize = 24./(padRatio.GetWh()*padRatio.GetAbsHNDC())
    ratioHist.GetYaxis().SetLabelSize( ratioHist_textsize )
    ratioHist.GetXaxis().SetLabelSize( ratioHist_textsize )
    ratioHist.GetXaxis().SetTitleSize( ratioHist_textsize )
    ratioHist.GetYaxis().SetTitleSize( ratioHist_textsize )
    ratioHist.GetYaxis().SetTitleOffset(0.6)

    
    ratioHist.SetLineColor(ROOT.kBlack)
    ratioHist.SetMarkerStyle(24)
    ratioHist.SetYTitle("test / ref")
    ratioHist.Draw("p")
    
    lineRatio = ROOT.TLine( ratioHist.GetXaxis().GetXmin(), 1, 
                            ratioHist.GetXaxis().GetXmax(), 1 ) 
    lineRatio.SetLineColor( ROOT.kRed )
    lineRatio.SetLineWidth( 2 )
    lineRatio.Draw("same")

    npath = ""
    if not self.structDirs:
      npath = path[path.find(":/")+2:] + "/"
      npath = re.sub(r"[:,./]", "_", npath+"/")

    canvas.cd()
    canvas.SaveAs(npath + refHist.GetName() + ".pdf")
    canvas.Close()

#======================================================================
def main( argv ):
  """
  Main function to be executed when starting the code.
  """

  parser = argparse.ArgumentParser( description = 'Distribution Plotter' )
  parser.add_argument( '-s', '--structDirs', default = True, action = "store_true", help = ' if true, it creates directories following the same structure as in the root file to store the pdf plots')
  parser.add_argument( '-r', '--reference', help = 'bla' )
  parser.add_argument( '-t', '--test', help = 'Print cutflow' )
  parser.add_argument( '-d', '--directory', default = "/", help = 'Print cutflow fror systematic variations' )
  parser.add_argument( '-e', '--exclude', default = "_bin_", help = 'histograms whose names contain the provided strings are not examined')
  parser.add_argument( '-n', '--normalize', default = False, action = "store_true", help = 'normalize histograms with larger stats for better comparison')
  args = parser.parse_args()

  ROOT.gROOT.Macro("rootlogon.C")
  ROOT.gROOT.SetBatch()

  validator = Validator( args.structDirs, args.exclude, SameHist, args.normalize )
  validator.CompareFiles(os.path.abspath(args.reference), os.path.abspath(args.test), args.directory)
 #======================================================================

if __name__ == "__main__":
  """
  Here the code should appear that is executed when running the plotter directly
  (and not import it in another python file via 'import Plotter')
  """

  # start main program    
  main( sys.argv[1:] )

