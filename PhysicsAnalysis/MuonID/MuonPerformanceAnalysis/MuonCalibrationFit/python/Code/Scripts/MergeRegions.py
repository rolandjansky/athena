# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re, shutil, sys
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
  my_re = re.compile( '.*(___Region\d+)' )
  #==#==#==#==#
  if options.input == 'NONE':
    shutil.copyfile( options.certain, options.output )
  else:
    out = open( options.output, 'w' )
    in1 = open( options.certain, 'r' )
    regions_found = []
    for line in in1:
      out.write( line )
      match = my_re.match( line )
      if match:
        regions_found.append( match.group( 1 ) )
    in2 = open( options.input, 'r' )
    for index, line in enumerate( in2 ):
      if index == 0:
        continue
      match = my_re.match( line )
      keep = True
      if match:
        reg = match.group( 1 )
        for region_found in regions_found:
          if region_found == reg:
            keep = False
      if keep:
        out.write( line )
  #==#==#==#==#


if __name__ == '__main__':
  sys.exit( main() )
