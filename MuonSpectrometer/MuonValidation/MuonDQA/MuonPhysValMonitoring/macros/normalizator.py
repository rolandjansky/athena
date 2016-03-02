#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#This script normalizes all histograms (except TH2, efficiencies and TProfiles) in a given root file.
import ROOT
from ROOT import TFile, TH1
import sys
import argparse
import re
import os
import math


def browseDir( dir ):
    #print dir.GetPath().split(':', 1)[-1]
    path = dir.GetPath().split(':/', 1)[-1]
    pathdir = dir #.GetDirectory(path)
    #print pathdir.GetPath()
    keylist = dir.GetListOfKeys()
    
    for key in keylist:
        obj = key.ReadObj()

        #2D histograms cannot be compared, tprofiles and efficiencies should not be normalized; skip
        if obj.IsA().InheritsFrom(ROOT.TH2.Class()) or obj.IsA().InheritsFrom(ROOT.TProfile.Class()) or "fficiency" in obj.GetName() or 'RecoFraction' in obj.GetName() or 'purity' in obj.GetName() or "PtResol" in obj.GetName() or "PtScale" in obj.GetName() or "Prof" in obj.GetName() or "Fit" in obj.GetName():
            continue

        if obj.IsA().InheritsFrom(ROOT.TH1.Class()) :
            #normalize
            #print '----- ', obj.GetName()
            ii = 1.*obj.Integral()
            hname = obj.GetName()
            if ii>0:
                 obj.Scale(1./ii)
                 #print '   ',hname
                 pathdir.WriteTObject( obj , hname , "WriteDelete" )
            continue

        if obj.IsA().InheritsFrom(ROOT.TDirectory.Class()):
            #print obj.GetName()
            browseDir(obj)
        
  

#======================================================================
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

  ROOT.gROOT.SetBatch()

  infile.cd("Muons")
  topDir = infile.GetDirectory("Muons")

  browseDir(topDir)
  infile.Close()
  
#======================================================================

if __name__ == "__main__":
  """
  Here the code should appear that is executed when running the plotter directly
  (and not import it in another python file via 'import Plotter')
  """

  # start main program    
  main( sys.argv )
