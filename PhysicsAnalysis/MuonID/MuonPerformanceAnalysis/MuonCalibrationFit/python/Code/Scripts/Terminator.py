# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, shutil, sys, time
from optparse import OptionParser

def main():

  #==#==#==#==#
  parser = OptionParser()
  parser.add_option( '-i', '--input', dest = 'input', action = 'store', type = 'string', default = '', help = 'Specify the input list' )
  ( options, args ) = parser.parse_args()
  #==#==#==#==#
  print ' ---> First check at least one .out file has been written'
  begin = time.time()
  CanMoveOn = False
  while not CanMoveOn:
    time.sleep( 3 )
    for FileName in options.input.split( ',' ):
      if os.path.isfile( FileName ):
        CanMoveOn = True
  #==#==#==#==#
  print ' ---> Now check that all (available) jobs have finished' 
  UnFinishedJobs = [ True ]
  while any( UnFinishedJobs ):
    time.sleep( 3 )
    UnFinishedJobs = []
    for FileName in options.input.split( ',' ):
      if os.path.isfile( FileName ):
        if 'Job Terminated' in open( FileName ).read():
          UnFinishedJobs.append( False )
        else:
          UnFinishedJobs.append( True )
  print ' ---> Can now start !!! '
  end = time.time()
  print '      ( %d seconds passed from the start of this job )' % int( end - begin )

if __name__ == '__main__':
  sys.exit( main() )
