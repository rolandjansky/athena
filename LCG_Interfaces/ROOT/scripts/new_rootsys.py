# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys,shutil
from optparse import OptionParser

class new_rootsys :

  """
  assumptions : cmt is properly setup
  """
  def __init__(self,new_rs,old_rs,app,verb) :
    # intializing constructor values
    self.old_rootsys = old_rs
    self.new_rootsys = new_rs
    self.append = app
    self.verbose = verb
    # the script name
    self.argv0 = sys.argv[0]
    # check old ROOTSY
    if not self.old_rootsys :
      self.msg('INFO','current ROOTSYS not set, try to find it via $ROOTSYS')
      if os.environ.has_key('ROOTSYS') : self.old_rootsys = os.environ['ROOTSYS']
      else :
        self.msg('ERROR', 'environment variable ROOTSYS not defined and no option -o given, exiting')
        sys.exit(1)
    # the list of directories in ROOTSYS that need to be created in the new location
    self.rootsys_dirs = []
    self.msg('INFO','creating the list of directories to symlink into new location from ls $ROOTSYS')
    for d in os.listdir(self.old_rootsys) :
      if os.path.isdir(self.old_rootsys+os.sep+d) :
        self.msg('INFO','adding directory %s'%d)
        self.rootsys_dirs.append(d)
    # check location of new ROOTSYS
    if not self.new_rootsys :
      self.msg('INFO','location of new ROOTSYS not set')
      if sys.platform == 'win32' : self.new_rootsys = os.environ['USERPROFILE'] 
      else                       : self.new_rootsys = os.environ['HOME'] 
      self.new_rootsys += os.sep + 'new_rootsys' + os.sep + os.environ['CMTCONFIG'] + os.sep + 'root'
    self.msg('INFO','location for new ROOTSYS set to %s'%self.new_rootsys)
    # get the package name and package_home
    self.package = os.popen('cmt show macro_value package').read()[:-1]
    self.msg('INFO','the current package, i.e. the one to overwrite is %s'%self.package)
    self.pkgroot = os.popen('cmt show macro_value ' + self.package + '_home').read()[:-1]
    self.msg('INFO','the package_home of this package is %s'%self.pkgroot)

  def msg(self,typ,msg) :
    if typ in ('ERROR',) or self.verbose :
      print '%s: %s: %s' % (self.argv0, typ.ljust(7), msg)

  def make_link(self, old_dir, new_dir) :
    # iterate over all files in the old directory and either create symlinks
    # (on linux,mac) or copy them (on windows) into the new location
    # if a destination file exists remove it before
    for f in os.listdir(old_dir) :
      oldf = old_dir + os.sep + f
      newf = new_dir + os.sep + f
      self.msg('DEBUG','linking old file %s to %s' % (oldf, newf))
      if sys.platform == 'win32' :
        if os.path.isfile(newf) :
          self.msg('WARNING', 'destination file %s exists, will be removed' % newf)
          os.remove(newf)
        shutil.copyfile(oldf,newf)
      else                       :
        if os.path.islink(newf) :
          self.msg('WARNING', 'destination file %s exists, will be removed' % newf)
          os.remove(newf)
        os.symlink(oldf,newf)
      
  def create_new_rootsys(self) :
    # check if the new location dir already exists
    if os.path.isdir(self.new_rootsys) :
      self.msg('INFO','location of new rootsys already exists')
      # if the dir exists and append=True return, ie we only overwrite with new pkgs
      if self.append :
        self.msg('INFO','script was called with option append, not changing already exsisting rootsys')
        return
      # else remove the whole tree 
      else :
        self.msg('WARNING','removing whole directory tree of existing rootsys '+self.new_rootsys)
        shutil.rmtree(self.new_rootsys)
    # the directory shall not exist, we create it here
    self.msg('INFO','creating new rootsys directory '+self.new_rootsys)
    os.makedirs(self.new_rootsys)
    # for each sub directory in ROOTSYS link all files into the new location
    self.msg('INFO','linking from old ROOTSYS into new directory')
    for d in self.rootsys_dirs :
      nd = self.new_rootsys + os.sep + d
      self.msg('INFO','working on directory '+nd)
      os.mkdir(nd)
      self.make_link(self.old_rootsys+os.sep+d, nd)

  def overwrite_package(self) :
    # for the one package check all directories that the package contains
    # these can only be a subset of the ROOTSYS dirs (usually) and then
    # link all files in those directories into the new location
    self.msg('INFO','overwriting contents of package ' + self.package)
    for d in self.rootsys_dirs :
      od = self.pkgroot + os.sep + d
      if os.path.isdir(od) : self.make_link(od,self.new_rootsys+os.sep+d)

  def create_requirements(self) :
    # create a requirements file in the new location which will overwrite
    # basically ROOT_home with the new value, CMTUSERCONTEXT shall point
    # to this directory ie. NEWROOTSYS/cmt
    self.msg('INFO','creating requirements file with new entries')
    nd = self.new_rootsys + os.sep + 'cmt'
    os.mkdir(nd)
    f = open(nd+os.sep+'requirements','w')
    f.write('use ROOT v* LCG_Interfaces\n\n')
    f.write('macro ROOT_home "%s"\n' % self.new_rootsys)
    f.close()
  
  def create(self) :
    self.msg('INFO','creating new rootsys')
    self.create_new_rootsys()
    self.msg('INFO','overwriting package '+self.package)
    self.overwrite_package()
    self.msg('INFO','creating requirements file')
    self.create_requirements()

if __name__ == "__main__" :

  parser = OptionParser()

  parser.add_option('-n', '--new_rootsys',   action='store',       dest='new_rootsys',    help='location of new ROOTSYS (default=$HOME/new_rootsys)' )
  parser.add_option('-o', '--old_rootsys',   action='store',       dest='old_rootsys',    help='location of current ROOTSYS (default="$ROOTSYS")')
  parser.add_option('-a', '--append',        action='store_true',  dest='append',         help='append new overwrite pkgs to existing installation')
  parser.add_option('-v', '--verbose',       action='store_true',  dest='verbose',        help='verbose output')

  (opts,args) = parser.parse_args()

  if args : parser.error('The script does not take arguments')

  nr = new_rootsys(opts.old_rootsys,opts.new_rootsys,opts.append,opts.verbose)
  nr.create()
  
