# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import shutil, subprocess, sys
import ROOT
from optparse import OptionParser

def main():

  #==#==#==#==#
  parser = OptionParser()
  parser.add_option( '-c', '--certain',  dest = 'certain',  action = 'store', type = 'string', default = '', help = 'Specify the certain list' )
  parser.add_option( '-i', '--input',    dest = 'input',    action = 'store', type = 'string', default = '', help = 'Specify the input list' )
  parser.add_option( '-o', '--output',   dest = 'output',   action = 'store', type = 'string', default = '', help = 'Specify the output folder' )
  ( options, args ) = parser.parse_args()
  #==#==#==#==#
  if options.input == 'NONE':
    shutil.copyfile( options.certain, options.output )
  else:
    subprocess.call( 'hadd %s %s %s' % ( options.output, options.certain, options.input ), shell = True )
  #==#==#==#==#

if __name__ == '__main__':
  sys.exit( main() )
