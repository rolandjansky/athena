# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, re, sys, ROOT
from optparse import OptionParser

def main():

  ##### Parsing options #####
  parser = OptionParser()
  parser.add_option( '-i', '--input',  dest = 'input',  action = 'store', type = 'string', default = '', help = '' )
  ( options, args ) = parser.parse_args()

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 3000


  s0_Dict = {}
  s1_Dict = {}
  p0_Dict = {}
  p1_Dict = {}
  p2_Dict = {}
  File = ROOT.TFile( options.input )
  for Key in File.GetListOfKeys():
    Name = Key.GetName()
    if 'FinalResult' in Name:
      Vector = File.Get( Name )
      ShortName = Name.split( '___' )[ -1 ]
      s0_Dict[ ShortName ] = Vector( 0 )
      s1_Dict[ ShortName ] = Vector( 3 )
      p0_Dict[ ShortName ] = Vector( 6 )
      p1_Dict[ ShortName ] = Vector( 9 )
      p2_Dict[ ShortName ] = Vector( 12 )

  print '.Init_s0 = ', s0_Dict
  print '.Init_s1 = ', s1_Dict
  print '.Init_p0 = ', p0_Dict
  print '.Init_p1 = ', p1_Dict
  print '.Init_p2 = ', p2_Dict

if __name__ == '__main__':
  sys.exit( main() )
