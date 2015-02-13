# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, re, sys, ROOT
from optparse import OptionParser
import AtlasStyle
import Objects
import PlottingTools
import FitResult

def main():

  ##### Parsing options #####
  parser = OptionParser()
  parser.add_option( '-i', '--input',  dest = 'input',  action = 'store', type = 'string', default = '', help = '' )
  ( options, args ) = parser.parse_args()

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000

  print GetAverage( options.input )

def GetAverage( Input ):

  ##### Opening file and read all contents #####
  InputFile = ROOT.TFile( Input )
  Keys = ROOT.gDirectory.GetListOfKeys() 
  ToBeUsed = []
  Iterations = set()
  GlobIters = []

  for Key in Keys:
    TheName = Key.GetName()
    if 'TVectorT<double>' in Key.GetClassName() and not 'PreFit' in TheName:
      FitRes = FitResult.FitResult( TheName, InputFile )
      Iterations.add( ( FitRes.Iter, FitRes.RegFile ) )
      ToBeUsed.append( FitRes )

  for Iter, File in Iterations:
    GlobIters.append( FitResult.IterOverview( Iter, File ) )

  MyDict = {}
  for GlobIter in GlobIters:
    for VariableName, AverageError, AverageErrorRMS in GlobIter.GetAverageError( ToBeUsed ):
      MyDict[ VariableName ] = ( AverageError, AverageErrorRMS )
  return MyDict

if __name__ == '__main__':
  sys.exit( main() )
