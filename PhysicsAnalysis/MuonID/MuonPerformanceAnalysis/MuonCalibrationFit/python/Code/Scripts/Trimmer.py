# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import shutil, sys
import ROOT
from optparse import OptionParser

def main():

  #==#==#==#==#
  parser = OptionParser()
  parser.add_option( '-i', '--input',      dest = 'input',      action = 'store',      type = 'string', default = '',    help = 'Specify the input list' )
  parser.add_option( '-o', '--output',     dest = 'output',     action = 'store',      type = 'string', default = '',    help = 'Specify the output folder' )
  ( options, args ) = parser.parse_args()
  #==#==#==#==#
  InputFile = ROOT.TFile( options.input )
  OutputFile = ROOT.TFile( options.output, 'recreate' )

  for Key in InputFile.GetListOfKeys():

    Name = Key.GetName()
    if 'FinalResults' in Name:
      Obj = InputFile.Get( Name )
      OutputFile.cd()
      Obj.Write( Name  )

  InputFile.Close()
  OutputFile.Close()

if __name__ == '__main__':
  sys.exit( main() )
