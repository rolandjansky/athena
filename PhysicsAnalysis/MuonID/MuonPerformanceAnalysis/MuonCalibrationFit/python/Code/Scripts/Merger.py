# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, shutil, sys
import ROOT
from optparse import OptionParser

def main():

  #==#==#==#==#
  parser = OptionParser()
  parser.add_option( '-i', '--input',      dest = 'input',      action = 'store',      type = 'string', default = '',    help = 'Specify the input list' )
  parser.add_option( '-o', '--output',     dest = 'output',     action = 'store',      type = 'string', default = '',    help = 'Specify the output folder' )
  parser.add_option( '-f', '--function',   dest = 'function',   action = 'store',      type = 'int',    default = 15,    help = '15 chi2, 16 LH' )
  parser.add_option( '-j', '--job',        dest = 'job',        action = 'store',      type = 'string', default = '',    help = '' )
  parser.add_option( '-r', '--region',     dest = 'region',     action = 'store',      type = 'string', default = '',    help = '' )
  parser.add_option( '-c', '--check-scan', dest = 'check_scan', action = 'store_true',                  default = False, help = '' )
  ( options, args ) = parser.parse_args()
  #==#==#==#==#
  FilesAndFunctionsWithScan = []
  FilesAndFunctions = []
  for FileName in options.input.split( ',' ):
    if not os.path.isfile( FileName ):
      print FileName, 'not found, skipping it!'
      continue
    File = ROOT.TFile( FileName )
    Vec = File.Get( 'FinalResults___%s___%s' % ( options.job, options.region ) )
    val = Vec[ options.function ]
    ScanVec = File.Get( 'PostScanFit___%s___%s' % ( options.job, options.region ) )
    if not val > 0:
      print 'Something fishy happened to this job ( found function n.%d with %f as a value), skipping it!' % ( options.function, val ) 
    else:
      print ( val, FileName )
      FilesAndFunctions.append( ( val, FileName ) )
      if options.check_scan and not ScanVec:
        print 'no succeeded scan found, skipping...'
      else:
        FilesAndFunctionsWithScan.append( ( val, FileName ) )
  BestFileName = '' 
  if options.check_scan and len( FilesAndFunctionsWithScan ) > 0:
    BestFileName = ( sorted( FilesAndFunctionsWithScan )[ 0 ] )[ 1 ]
  else:
    BestFileName = ( sorted( FilesAndFunctions )[ 0 ] )[ 1 ]
    if options.check_scan:
      print 'no job has a succeeded scan, falling back to normal selection'
  print 'the winner is: ', BestFileName
  shutil.copyfile( BestFileName, options.output )
  #==#==#==#==#


if __name__ == '__main__':
  sys.exit( main() )
