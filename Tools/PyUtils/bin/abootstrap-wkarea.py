#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @file: abootstrap-wkarea.py
# @purpose: install all needed $(CMTCONFIG) directories into a temporary area
#           so to not eat our precious AFS disk space.
#           also creates a WorkArea CMT package to ease administration
# @author: Sebastien Binet
# @date: February 2007

from __future__ import with_statement

__version__ = "$Revision: 323486 $"

import os
import os.path as osp
import glob
import sys

### basic logging and messages -----------------------------------------------
from PyCmt.Logging import logging

msg = logging.getLogger("AthBoot")

##########################
# recognized user options
##########################
import getopt

_useropts = 'i:o:hl:v'
_userlongopts = [ 'input-dir=',  'output-dir=',
                  'help',     'loglevel=',
                  'version' ]

def _usage():
   print """Accepted command line options (CLI):
   -i, --input-dir <dir>      ...  directory where the original WorkArea is
                                   sitting.
   -o, --output-dir <dir>     ...  directory where to install the bin dirs
   -h, --help                 ...  print this help message
   -l, --loglevel <level>     ...  logging level (DEBUG, INFO, WARNING, ERROR, FATAL)
   -v, --version              ...  print version number
   """
   return

## Helper class to recursively find files
import fnmatch
class _GlobDirectoryWalker:
   """a forward iterator that traverses a directory tree"""

   def __init__(self, directory, pattern="*"):
      self.stack = [directory]
      if type(pattern) != type([]):
         pattern = [ pattern ]
      self.pattern = pattern
      self.files = []
      self.index = 0
      return
    
   def __getitem__(self, index):
      while 1:
         try:
            file = self.files[self.index]
            self.index = self.index + 1
         except IndexError:
            # pop next directory from stack
            self.directory = self.stack.pop()
            self.files = os.listdir(self.directory)
            self.index = 0
         else:
            # got a filename
            fullname = osp.join(self.directory, file)
            if osp.isdir(fullname) and not osp.islink(fullname):
               self.stack.append(fullname)
            for pattern in self.pattern:
               if fnmatch.fnmatch(file, pattern):
                  msg.debug(" --> %s",fullname)
                  return fullname

         pass
      return
   
   pass # class _GlobDirectoryWalker

## Helper function to find all the bin dirs to be installed
def register_bin_dirs(top_dir = os.getcwd(),
                      pattern = os.environ['CMTCONFIG']):
   """helper function to find all 'bin' dirs to be installed in the temp space
   """
   top_dir = osp.abspath(osp.expanduser(osp.expandvars(top_dir)))
   msg.info("registering 'bin dirs' [%s]..." % str(pattern))
   msg.info("parsing [%s]..." % top_dir)
   if not osp.exists(top_dir):
      return []
   bin_dirs = [ d for d in _GlobDirectoryWalker(top_dir, pattern) 
                if osp.isdir(d) and not osp.islink(d)
                and d.count("InstallArea") <= 0 ]

   msg.info(" ==> found [%i] 'bin dirs' to process", len(bin_dirs))
   return bin_dirs

## Main entry point
import shutil, glob
def symlink_bin_dirs(input_dir, output_dir, bin_dirs):
   input_dir = osp.abspath(input_dir)
   output_dir= osp.abspath(output_dir)

   msg.info("."*50)
   msg.info("symlinking ...")
   if not osp.exists(output_dir):
      msg.warning("[%s] does NOT exists : creating it...", output_dir)
      os.mkdir(output_dir)
      pass
   
   for bin in bin_dirs:
      bin = osp.abspath(bin)
      root = osp.commonprefix([input_dir, bin]) + os.sep
      out_bin = output_dir + os.sep + bin.split(root)[1]
      msg.info(" -- %s", bin.split(root)[1])

      ## remove the linked dir if it exists
      ## (this assumes the registering of bin_dirs does not register already
      ## symlinked bin_dirs !! Worse case scenario is to rebuild...)
      if osp.exists(out_bin):
         msg.debug("... removing [%s] ...", out_bin)
         shutil.rmtree(out_bin)
         pass

      ## create all the parent path if it does not exist yet
      if not osp.exists(osp.dirname(out_bin)):
         os.makedirs(osp.dirname(out_bin))
         pass
      
      # symlink the output bin...
      shutil.move(bin, out_bin)
      os.symlink(out_bin, bin)

      # symlink the other directories so relative paths are also working
      # and g++ -o bla ../src/Bla.cxx will work
      pkg_dir = osp.dirname(bin)
      other_dirs = [d for d in glob.glob(pkg_dir + os.sep + "*") 
                    if d != bin and osp.isdir(d)]
      #msg.debug(" symlinking [%r]", other_dirs)
      for d in other_dirs:
         symlink_dest = osp.join(output_dir, d.split(root)[1])
         if osp.exists(symlink_dest):
            os.remove(symlink_dest)
         os.symlink(d, symlink_dest)

   msg.info("symlinking [DONE]")
   msg.info("."*50)
   return

class Options(object):
   """a dummy class to collect options"""
   pass

def _processOptions(useropts, userlongopts):

   # defaults
   input_dir  = None
   output_dir = None
   lvl = logging.INFO
   opts = Options()
   
   try:
      optlist,args = getopt.getopt(sys.argv[1:],
                                   useropts,
                                   userlongopts)
   except getopt.error:
      msg.error(str(sys.exc_value))
      _usage()
      sys.exit(2)
      pass
   
   for opt, arg in optlist:
      if opt in ('-h', '--help'):
         _usage()
         sys.exit()
      elif opt in ('-v', '--version'):
         print "version:",__version__
         print "By Sebastien Binet"
         sys.exit()
      elif opt in ('-i', '--input-dir'):
         input_dir = osp.expanduser(osp.expandvars(arg))
      elif opt in ('-o', '--output-dir'):
         output_dir = osp.expanduser(osp.expandvars(arg))
      elif opt in ('-l', '--loglevel'):
         lvl = string.upper(arg)
         logLevel = getattr(logging, lvl)
         msg.setLevel(logLevel)
         del lvl,logLevel
         pass
      else:
         pass
      pass

   if input_dir  is None: input_dir = os.getcwd()

   if output_dir is None:
      fname = osp.join(os.getcwd(), ".abootstrap.cfg")
      if osp.exists(fname):
         # (try) to get them from a previous run of abootstrap-wrkarea
         with open(fname, 'r') as sticky_file:
            from ConfigParser import SafeConfigParser
            cfg = SafeConfigParser()
            cfg.readfp(sticky_file)
            ath_cfg = dict(cfg.items('abootstrap'))
            hostname   = ath_cfg['hostname']
            input_dir  = ath_cfg['input-dir']
            output_dir = ath_cfg['output-dir']
            del cfg
            pass

      if output_dir is None:
         # use parent-dir...
         output_dir = osp.join('/tmp',
                               '$USER',
                               'aboot-tmp-'+osp.basename(os.getcwd()))
   opts.input_dir = osp.abspath(osp.expanduser(osp.expandvars(input_dir)))
   opts.output_dir= osp.abspath(osp.expanduser(osp.expandvars(output_dir)))

   return opts
    
if __name__ == "__main__":

   msg = logging.getLogger('AthBoot')
   msg.setLevel(logging.INFO)

   ## process user options
   opts = _processOptions(_useropts, _userlongopts)

   msg.info("#"*50)
   msg.info(" input-dir:  [%s]", opts.input_dir)
   msg.info(" output-dir: [%s]", opts.output_dir)
   msg.info("#"*50)
   msg.info("")

   import shutil
   
   # removing output dir
   if osp.exists(opts.output_dir):
      shutil.rmtree(opts.output_dir)

   # remove InstallArea if any
   if osp.exists('InstallArea'):
      shutil.rmtree('InstallArea')
      
   # create $CMTCONFIG directories...
   msg.info("creating WorkArea...")
   import commands as com
   sc,out = com.getstatusoutput('setupWorkArea.py')
   if sc:
      print out
      sys.exit(1)
   
   orig_dir = os.getcwd()
   os.chdir('WorkArea/cmt')

   ## msg.info("cmt bro cmt config...")
   ## sc,out = com.getstatusoutput('cmt bro cmt config')
   ## if sc:
   ##    print out
   ##    sys.exit(1)

   msg.info("creating $CMTCONFIG directories...")
   sc,out = com.getstatusoutput('cmt bro \"/bin/rm -rf ../$CMTCONFIG; /bin/mkdir -p ../$CMTCONFIG; echo 1\"')
   if sc:
      print out
      sys.exit(1)

   os.chdir(orig_dir)

   msg.info('registering bin dirs...')
   bin_dirs = register_bin_dirs(opts.input_dir)
   msg.info('installing symlinks...')
   symlink_bin_dirs(opts.input_dir, opts.output_dir, bin_dirs)

   for d in ('python', os.environ['CMTCONFIG']):
      d = osp.join('InstallArea', d)
      if not osp.exists(d):
         msg.info('creating [%s] (to prevent CMT bug)...', d)
         os.makedirs(d)
   

   ## to remember where we put those binary files, in case we logged on a
   ## different lxplus node...
   with open(osp.join(opts.input_dir, ".abootstrap.cfg"), 'w') as f:
      import socket
      f.writelines([
         "[abootstrap]\n",
         "hostname   = %s\n" % socket.gethostname(),
         "input-dir  = %s\n" % opts.input_dir,
         "output-dir = %s\n" % opts.output_dir,
         ])
      pass
   msg.info("## Bye.")
   
   sys.exit(0)
