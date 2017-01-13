#!/usr/bin/env python
#
# Extract the HLT environment as created by 'asetup'
#
# Author: Frank Winklmeier

__doc__ = """\
Usage: make_hlt_env.py [Options] [FILENAME]

Create the HLT environment and store it in FILENAME (default: HLT-Environment.data.xml)
By default the following tags are added: setup,noTest

Examples:
    make_hlt_env.py --tags=15.5.4,opt,32,slc5
    make_hlt_env.py --tags=15.5.4,opt,32,slc5 --project=AtlasTrigger
    make_hlt_env.py --tags=15.5.4,opt,32,slc5 --prefix=MYDET    
"""

import subprocess
import os, sys, shutil
import tempfile
import cPickle
import re
import atexit

import pm.project
from pm.dal import dal

# Command line options
opt = None

class Config:
   # Possible locations for AtlasSetup
   atlasSetupHome = ['/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/AtlasSetup/current/AtlasSetup',
                     '/afs/cern.ch/atlas/software/dist/AtlasSetup',
                     '/sw/atlas/AtlasSetup',  # Point1
                     os.environ.get('AtlasSetup',None)
                     ]
   
   common_env = 'daq/segments/common-environment.data.xml'  # CommonEnvironment
   
   # Do not add these variables to the HLT environment
   ignoreVars = ['TDAQ_.*',
                 'DYLD_LIBRARY_PATH$',
                 'OKS_GUI.*',
                 'CLASSPATH$',
                 'ROOTSYS$',
                 'AtlasVersion$','AtlasProject$',   # set by HLTPatchRepository
                 'TNS_ADMIN$',                      # from CommonEnvironment
                 'SVN.*','RPM_release_package$',                 
                 'LCG_INST_PATH$',
                 'AtlasLoginRoot$',
                 'CMTBIN$','CMTEXTRATAGS$','CMTHEADVERSION$','CMTHOME$',
                 'CMTPROJECTPATH$','CMTRELEASE$','CMTROOT$','CMTSITE$','MAKEFLAGS$',
                 'LANG$','LC_ALL$','PEDANTIC$','_$','OLDPWD$'
                 ]

   # Additonal variables (with source DB file) to add to HLT-Environment
   addVars = {'ROOTSYS': 'daq/sw/dqm-common-external-repository.data.xml'}
   
   # Replace the value of these variables with the variable name
   symbolicVars = ['ATLAS_EXTERNAL',
                   'ATLAS_BASE',
                   'JAVA_SUBDIR',
                   'ATLASMKLLIBDIR_PRELOAD'
                   ]

   # Always add these to the Parameters set (in addition to symbolicVars)
   hltPars = []

   # Reduce entries in these variables to existing (non-empty) directories
   slimVars = ['PATH',
               'LD_LIBRARY_PATH',
               'PYTHONPATH',
               'DATAPATH',
               'MANPATH',
               'JOBOPTSEARCHPATH',
               'ROOT_INCLUDE_PATH',
               ]

   # Possible description for OKS variables
   doc = {}
   
def ignoreVar(var):
   """Should we ignore this variable?"""
   
   for v in Config.ignoreVars:
      if re.match(v,var)!=None: return True
   return False


def getAtlasSetup():
   """Return location of AtlasSetup home directory"""
   
   for p in Config.atlasSetupHome:
      if p!=None and os.path.exists(p): return p

   raise RuntimeError('Cannot find AtlasSetup. Please set $AtlasSetup')
      

def slimpath(value):
   """Reduce the path to non-empty directories"""

   path = []
   for p in value.split(os.path.pathsep):
      if p=='.':
         path += [p]
      elif os.access(p, os.R_OK) and len(os.listdir(p))>0:
         path += [p]
            
   return os.path.pathsep.join(path)


def symbolicReplace(hltenv):
   """Replace any occurence of a symolic variable's value with its name"""

   for name,value in hltenv.iteritems():
      for sv in Config.symbolicVars:
         if sv==name or sv not in hltenv:
            continue
         else:
            hltenv[name] = hltenv[name].replace(hltenv[sv], "${%s}" % sv)
            

def getHltEnv(asetup, asetup_opts):
   """Extract the process environment by running CMT setup"""

   sh = tempfile.mkstemp()[1]
   fenv1 = tempfile.mkstemp()[1]
   fenv2 = tempfile.mkstemp()[1]
   fasetup = open(tempfile.mkstemp()[1],'w')
   fcmd = open(tempfile.mkstemp()[1],'w')
   
   dumpEnv1 = 'import cPickle,os; cPickle.dump(os.environ,open("%s","w"))' % fenv1
   dumpEnv2 = 'import cPickle,os; cPickle.dump(os.environ,open("%s","w"))' % fenv2

   # Cleanup environment
   print >>fcmd, 'export PATH=/usr/kerberos/bin:/usr/bin:/bin:/usr/bin/X11'
   print >>fcmd, 'unset MANPATH LD_LIBRARY_PATH PYTHONPATH XMLPATH JOBOPTSEARCHPATH DATAPATH'
   # Dump environment as dictionary
   print >>fcmd, "python -c '%s'" % dumpEnv1
   # AtlasSetup
   print >>fcmd, "export AtlasSetup=%s" % asetup
   print >>fcmd, "source ${AtlasSetup}/scripts/asetup.sh %s" % asetup_opts
   
   # Dump environment as dictionary   
   print >>fcmd, "python -c '%s'" % dumpEnv2
   fcmd.close()

   if not opt.quiet: print "Creating runtime setup with 'asetup %s'" % asetup_opts
   os.system('env - HOME=$HOME bash -l %s' % fcmd.name)

   env1 = cPickle.load(open(fenv1))
   env2 = cPickle.load(open(fenv2))

   # Delete temporary files
   for f in (sh,fenv1,fenv2,fcmd.name):
      os.remove(f)
      #print f
      
   # Get difference between env2 and env1
   hltenv = {}      
   for k,v in env2.iteritems():
      if k not in env1: hltenv[k] = v
      elif env1[k]!=env2[k]: hltenv[k] = v
      
   # Slim path
   for k in hltenv:
      if k in Config.slimVars: hltenv[k] = slimpath(hltenv[k]) 
         
   return hltenv
   

def patchHltEnv(hltenv):
   """Apply patches to environment"""

   return

def writeHltEnv(hltenv, filename, prefix):

   common_vars = []
   if opt.useCommonEnv:
      try:
         db = pm.project.Project(Config.common_env)
         common_vars = db.getObject('VariableSet', 'CommonEnvironment').Contains
      except:
         print ('Could not load CommonEnvironment from', Config.common_env)
   
   hltvars = dal.VariableSet('%s-Environment' % prefix,
                             Description='Variables for HLT executables')
   hltpars = dal.VariableSet('%s-Parameters' % prefix,
                             Description='Parameters to expand for HLT executables')

   common_vars_name = [v.Name for v in common_vars]
   for k in sorted(hltenv):

      if ignoreVar(k): continue

      if opt.useCommonEnv and k in common_vars_name:
         if not opt.quiet: print '%s already provided by CommonEnvironment. Skipping it.' % k
         continue
      
      var = dal.Variable('%s-%s' % (prefix,k),
                         Name = k,
                         Value = hltenv[k],
                         Description = Config.doc.get(k,''))

      hltvars.Contains += [var]

   for var,inc in Config.addVars.iteritems():
      try:
         db = pm.project.Project(inc)
         v = db.getObject('Variable', var)
         hltvars.Contains += [v]
      except:
         print ('Cannot find variable %s in %s' % (var,inc))
         
   for v in hltvars.Contains:
      if v.Name in Config.symbolicVars+Config.hltPars: hltpars.Contains += [v]

   # Write the OKS file
   proj = pm.project.Project(filename, ['daq/schema/core.schema.xml'] +
                             list(set(Config.addVars.values())))
   proj.addObjects([hltvars, hltpars])
   if not opt.quiet: print('Created %s' % filename)


def createWrapper(hltenv, filename):

   prefix = os.path.basename(hltenv).split('-Environment.data.xml')[0]
   
   try:
      db = pm.project.Project(hltenv)
   except:
      print ('Could not open', hltenv)
      return

   try:
      rel = db.getObject('Variable','%s-AtlasArea' % prefix).Value.split('/')[-1]
   except:
      print ('Could not determine release number from AtlasArea')
      return


   hltbase = dal.Variable('HLT-AtlasBaseVersion',
                          Name='AtlasBaseVersion',
                          Value=rel,
                          Description='Base release version')
   
   hltvars = dal.VariableSet('HLT-Environment',
                             Description='Variables for HLT executables')
   hltpars = dal.VariableSet('HLT-Parameters',
                             Description='Parameters to expand for HLT executables')

   hltvars.Contains.append(db.getObject('VariableSet','%s-Environment' % prefix))
   hltpars.Contains.append(db.getObject('VariableSet','%s-Parameters' % prefix))
   hltpars.Contains.append(hltbase)
   
   # Write the OKS file
   proj = pm.project.Project(filename, ['daq/schema/core.schema.xml',hltenv])
   proj.addObjects([hltvars, hltpars, hltbase])
   if not opt.quiet:
      print('Setting AtlasBaseVersion to %s' % rel)
      print('Created %s wrapping %s' % (filename,hltenv))

      
def myhelp(option, opt_str, value, parser):
   """Custom help callback since optparse does not preserve newlines"""

   print __doc__   
   parser.print_help()
   sys.exit(1)


def main():

   import optparse
   parser = optparse.OptionParser(usage=optparse.SUPPRESS_USAGE,
                                  add_help_option = False)

   parser.add_option('--tags', action='store', type='string',
                     help='Tags for asetup (if none provided deduce from current environment)')

   parser.add_option('--asetupargs', action='store', type='string',
                     help='Extra arguments passed to asetup')   

   parser.add_option('--asetuphome', action='store', type='string',
                     help='AtlasSetup location (automatically locate if not provided)')
      
   parser.add_option('--prefix', action='store', type='string', default='HLT',
                     help='Prefix for OKS variables (default: %default)')

   parser.add_option('--useCommonEnv', action='store_true',
                     help='Skip variables that are provided by the CommonEnvironment')

   parser.add_option('--wrap', metavar='CHILD', action='store', type='string',
                     help='Create HLT-Environment wrapping CHILD (e.g. daq/segments/HLT_16.1.0-Environment.data.xml)')
   
   parser.add_option('-q', '--quiet', action='store_true', default=False,
                     help='Be quiet.')

   parser.add_option('-h', '--help', action='callback', callback=myhelp)
   
   global opt
   (opt, args) = parser.parse_args()

   if len(args)>1:
      myhelp(None,None,None,parser)

   if len(args)==0:
      filename = '%s-Environment.data.xml' % opt.prefix
   else:
      filename = args[0]

   cmtconfig = None
   lba = ''
   if not opt.tags:      
      cmtconfig = os.getenv('CMTCONFIG')
      prj = os.getenv('AtlasProject')
      ver = os.getenv('AtlasVersion')
      # Example for local build area: /build/atnight/localbuilds/nightlies/15.X.0
      lba = os.environ.get('NICOS_PROJECT_HOME','').rstrip('AtlasHLT')   # NICOS use
      if len(lba)==0: lba = os.environ.get('AtlasBaseDir','')            # interactive use
      
      if cmtconfig==None or prj==None or ver==None:
         print('Cannot deduce asetup tags from current environment')
         return 2
      opt.tags = '%s,%s' % (prj,ver)
      
   if opt.wrap:
      createWrapper(opt.wrap, filename)
      return
         
   tags = ','.join([opt.tags,'setup','notest'])

   asetup_opts = '--tags=%s --input=None' % tags         
   
   if len(lba)>0 and (opt.asetupargs==None or opt.asetupargs.find('--releasebase')==-1):
      asetup_opts += ' --releasebase=%s' % lba
      
   if cmtconfig!=None:
      asetup_opts += ' --cmtconfig=%s' % cmtconfig

   if opt.asetupargs!=None:
      asetup_opts += ' '+opt.asetupargs

   if opt.asetuphome==None:
      asetup = getAtlasSetup()
   else:
      asetup = opt.asetuphome
      
   hltenv = getHltEnv(asetup, asetup_opts)   

   # This will point one directory below the project, e.g.: /afs/cern.ch/atlas/software/builds
   hltenv['ATLAS_BASE'] = hltenv['AtlasArea'].split(hltenv['AtlasProject'])[0].rstrip('/')
   # Use tcmalloc
   hltenv['LD_LIBRARY_PATH'] += os.pathsep+hltenv['TCMALLOCDIR']
   if hltenv['CMTCONFIG'].startswith('i686'):  # need both 64 and 32 bit version on a x86 host
      hltenv['LD_LIBRARY_PATH'] += os.pathsep+hltenv['TCMALLOCDIR'].replace('i686','x86_64')

   # Preload tcmalloc and intel math libraries (ATR-14499)
   hltenv['LD_PRELOAD'] = 'libtcmalloc_minimal.so'
   hltenv['LD_PRELOAD'] += ':${ATLASMKLLIBDIR_PRELOAD}/libimf.so:${ATLASMKLLIBDIR_PRELOAD}/libintlc.so.5'

   symbolicReplace(hltenv)

   patchHltEnv(hltenv)
   
   writeHltEnv(hltenv, filename, opt.prefix)


if __name__ == "__main__":
   try:
      sys.exit(main())
   except KeyboardInterrupt:
      sys.exit(1)


