#!/usr/bin/env python
#Script to run athenaHLT on the most recent run taken

from glob import glob
from fnmatch import fnmatch
from subprocess import Popen, PIPE, STDOUT, call
 
#EOS walking Code courtesy of James Robinson
def get_file_list( DATAPATH ) :
  #print "PathExpander         INFO Looking for files in {0}".format( DATAPATH )
  # Stupid hack to simulate wildcard expansion on eos
  if 'eos' in DATAPATH :
    #print "PathExpander         INFO Simulating wildcard expansion - not supported by eos ls"
    directories = [ x for x in DATAPATH.split('/') if (x is not '' and 'eosatlas' not in x) ] + [ '*' ]
    paths = [ '' ]
    for directory in directories :
      if any( character in directory for character in [ '*', '?', '[', ']' ] ) :
        new_paths = []
        for path in paths :
          subdirectories = []
          cmd = [ 'eos', 'ls', path ]
          eos_ls = Popen( args = cmd, bufsize = 1, shell = False, stdout = PIPE, stderr = STDOUT )
          while eos_ls.poll() is None :
            line = eos_ls.stdout.readline()
            if line : subdirectories.append( line.replace('\n','') )
          for subdirectory in subdirectories :
            if fnmatch( subdirectory, directory ) :
              new_paths.append( path+'/'+subdirectory )
        paths = new_paths
      else :
        new_paths = [ path+'/'+directory for path in paths ]
        paths = new_paths
    # for path in paths : print "PathExpander         INFO Found file at '{0}'".format( path )
    #return [ 'root://eosatlas.cern.ch//'+path for path in paths ]
    return paths
  return sorted( glob(DATAPATH + '*') )

 
def main():
  
  from optparse import OptionParser
  parser = OptionParser()
  parser.add_option("--nfiles", help="Number of input files",
        type='int', default=1) 
  parser.add_option("--modifiers", help="Modifiers", default='')
  parser.add_option("--dir", help="eos directory", default='/eos/atlas/atlastier0/rucio/data16_13TeV/physics_Main/')
  (opts, args) = parser.parse_args()
  
  runs = get_file_list(opts.dir)
  last_run_files = get_file_list(runs[-1] + "/*/")
  subset = []
  for i in range(0,opts.nfiles):
    subset.append('root://eosatlas.cern.ch//' + last_run_files[i])
  print subset 

  trigCmd = "athenaHLT.py -f \"" + str(subset) + "\" -c \"" + opts.modifiers + "\" TriggerRelease/runHLT_standalone.py"
  call("echo " + trigCmd, shell=True)
  call(trigCmd, shell=True)

if __name__ == "__main__":
  main()
