#!/usr/bin/env python
#Script to run athenaHLT on the most recent run taken

from glob import glob
from fnmatch import fnmatch
from subprocess import Popen, PIPE, STDOUT, check_call
from CoolConvUtilities.AtlCoolLib import indirectOpen
from PyCool import cool

class CoolDB:
   """Cache for COOL db conneciton objects"""
   cooldbs = {}
   
   @classmethod
   def get(cls, dbname):
      if not dbname in cls.cooldbs: 
         cls.cooldbs[dbname] = indirectOpen(dbname,readOnly=True,oracle=True)

      return cls.cooldbs[dbname]

# Helpers to convert validity key
def toRun(validity):
   return (validity >> 32)

def getLastPhysicsRuns(N=1):
   """Return the last N runs with ReadyForPhysics"""

   db = CoolDB.get("COOLONL_TDAQ/CONDBR2")
   f = db.getFolder("/TDAQ/RunCtrl/DataTakingMode")
   objs = f.browseObjects(cool.ValidityKeyMin, cool.ValidityKeyMax, cool.ChannelSelection(0))
   lastrun = []
   while objs.goToNext():
      obj= objs.currentRef()
      if obj.payload()['ReadyForPhysics']==0: continue
      lastrun.append(toRun(obj.since()))

   return lastrun[len(lastrun)-N:]



 
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
            if line : 
              if not 'INFO: ' in line:
                subdirectories.append( line.replace('\n','') )
          for subdirectory in subdirectories :
            if fnmatch( subdirectory, directory ) :
              new_paths.append( path+'/'+subdirectory )
        paths = new_paths
      else :
        new_paths = [ path+'/'+directory for path in paths ]
        paths = new_paths
    for i,path in enumerate(paths):
      print "PathExpander         INFO Found file at '{0}'".format( path )
      if i > 5: 
        break

    #return [ 'root://eosatlas//'+path for path in paths ]
    return paths
  return sorted( glob(DATAPATH + '*') )

 
def main():
  
  from optparse import OptionParser
  parser = OptionParser()
  parser.add_option("--nfiles", help="Number of input files",
        type='int', default=1) 
  parser.add_option("--nevents", help="Number of max events",
        type='int', default=-1)
  parser.add_option("--modifiers", help="Modifiers", default='')
  parser.add_option("--dir", help="eos directory", default='/eos/atlas/atlastier0/rucio/data16_13TeV/physics_Main/')
  (opts, args) = parser.parse_args()
  
  runs = get_file_list(opts.dir)
  physics_runs = getLastPhysicsRuns(100)
  physics_runs = [str(x) for x in physics_runs]
  good_runs = []
  for r in runs:
     good=False
     for p in physics_runs:
        if p in r:
          good=True
     if(good):
        good_runs += [r]
  runs = good_runs

  if(len(runs)==0):
     print " FATAL : no run found"
    
  else:
     last_run_files = get_file_list(runs[-1] + "/*/")
     subset = []
     for i in range(0,opts.nfiles):
        subset.append('root://eosatlas/' + last_run_files[i])
     print subset 

     trigCmd = "athenaHLT.py -n " + str(opts.nevents) + " -f \"" + str(subset) + "\" -c \"" + opts.modifiers + "\" TriggerRelease/runHLT_standalone.py"
     trigCmdEsc = trigCmd.replace("\\","\\\\").replace("\"","\\\"")#For output to echo
     check_call("echo \"" + trigCmdEsc + "\"", shell=True)#Call echo rather than print so that it completes first
     check_call(trigCmd, shell=True)

if __name__ == "__main__":
  main()
