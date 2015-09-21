# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/ConfigurableDb.py
# Author: Sebastien Binet (binet@cern.ch)
# Modified: Wim Lavrijsen (WLavrijsen@lbl.gov)
# Modified: Marco Clemencic

"""Provides cfgDb, a Singleton that holds the information on the whereabouts
of all the accessible, automatically generated, Configurables.
This repository of (informations on) Configurables is used by the PropertyProxy
class to locate Configurables and feed the JobOptionsSvc. It could also be used
to feed the AthenaEmacs module..."""

import string


### data ---------------------------------------------------------------------
__version__ = '3.0.0'
__author__  = 'Sebastien Binet (binet@cern.ch)'

__all__ = [ 'CfgDb', 'cfgDb',
            'loadConfigurableDb', 'unloadConfigurableDb',
            'getConfigurable' ]

_transtable = string.maketrans('<>&*,: ().', '__rp__s___')

### helpers ------------------------------------------------------------------
def _fillConfDict():
   global cfgDb

   nFiles = loadConfigurableDb()
   cfgDb.msg.info( "Read module info for %d configurables from %d genConfDb files",
             len(cfgDb), nFiles )

   if 0 < len( cfgDb.duplicates() ):
      cfgDb.msg.warning( "Found %d duplicates among the %d genConfDb files :",
                   len(cfgDb.duplicates()), nFiles )
      cfgDb.msg.warning( "--------------------------------------------------" )
      cfgDb.msg.warning( "  -%s: %s - %s",
                   "<component name>", "<module>", "[ <duplicates> ]" )
      cfgDb.msg.warning( "--------------------------------------------------" )

      dups = cfgDb.duplicates()
      for cfgName in dups.keys():
         cfgDb.msg.warning( "  -%s: %s - %s",
                      cfgName,
                      cfgDb[cfgName]['module'],
                      str([ d['module'] for d in dups[cfgName]]) )
      del dups

      cfgDb.msg.warning( "Fix your cmt/requirements file !!" )

   else:
      cfgDb.msg.info( "No duplicates have been found: that's good !" )


### singleton holding configurable and loadable information ------------------
class _CfgDb( dict ):
   """
   A singleton class holding informations about automatically generated
   Configurables.
    --> package holding that Configurable
    --> library holding the components related to that Configurable
    --> python module holding the Configurable
    --> a dictionary of duplicates
   """

   __slots__ = {
      '_duplicates' : { },
      '_msg' : None,
   }
    
   def __init__(self):
      object.__init__( self )
      self._duplicates = {}
      self._msg = None
      
   @property
   def msg(self):
      if self._msg is None:
         ## for messaging
         from . import Logging
         self._msg = Logging.logging.getLogger( 'ConfigurableDb' )
      return self._msg
      
   def add( self, configurable, package, module, lib ):
      """Method to populate the Db.
      It is called from the auto-generated Xyz_confDb.py files (genconf.cpp)
      @param configurable: the name of the configurable being added
      @param package: the name of the package which holds this Configurable
      @param module: the name of the python module holding the Configurable
      @param lib: the name of the library holding the component(s) (ie: the
                  C++ Gaudi component which is mapped by the Configurable)
      """

      cfg = { 'package' : package,
              'module'  : module,
              'lib'     : lib }

      if self.has_key( configurable ):
       # check if it comes from the same library...
         if cfg['lib'] != self[configurable]['lib']:
            self.msg.verbose( "dup!! [%s] p=%s m=%s lib=%s",
                          configurable, package, module, lib )
            if self._duplicates.has_key(configurable):
               self._duplicates[configurable] += [ cfg ]
            else:
               self._duplicates[configurable]  = [ cfg ]

      else:
         self.msg.verbose( "added [%s] p=%s m=%s lib=%s", 
                       configurable, package, module, lib )
         self[configurable] = cfg

   def duplicates(self):
      return self._duplicates


   def _loadModule(self, fname):
      f = open(fname)
      for i,ll in enumerate(f):
         l = ll.strip()
         if l.startswith('#') or len(l) <= 0:
            continue
         try:
            line = l.split()
            cname = line[2]
            pkg = line[0].split('.')[0]
            module = line[0]
            lib = line[1]
            self.add(cname, pkg, module, lib)
         except Exception:
            f.close()
            raise Exception(
               "invalid line format: %s:%d: %r" %
               (fname, i+1, ll)
               )
      f.close()

### function to load all ConfigurableDb files holding informations -----------
def loadConfigurableDb():
   """Load all ConfigurableDb files (modules) holding informations about
   Configurables available in the release
   """

   import os, sys
   from glob import glob
   from os.path import join as path_join
   from fnmatch import fnmatch as _fnmatch
   
   global cfgDb

   cfgDb.msg.debug( "loading confDb files..." )
   nFiles = 0
   pathlist = os.getenv("LD_LIBRARY_PATH", "").split(os.pathsep)
   for path in pathlist:
      if not os.path.isdir(path):
         continue
      cfgDb.msg.verbose( "walking in [%s]..." % path )
      confDbFiles = [f for f in [path_join(path, f) for f in os.listdir(path)
                                 if f.endswith('.confdb')]
                     if os.path.isfile(f)]

      # check if we use "*_merged.confdb"
      mergedConfDbFiles = [f for f in confDbFiles
                           if f.endswith('_merged.confdb')]
      if mergedConfDbFiles:
         # use only the merged ones
         confDbFiles = mergedConfDbFiles

      for confDb in confDbFiles:
         cfgDb.msg.debug( "\t-loading [%s]...", confDb )
         try:
            cfgDb._loadModule( confDb )
         except Exception, err:
            import traceback
            traceback.print_exc()
            log.warning( "Could not load file [%s] !", confDb )
            log.warning( "Reason: %s", err )
         nFiles += 1
   cfgDb.msg.debug( "loading confDb files... [DONE]" )
   nPkgs = len( set([k['package'] for k in cfgDb.values()]) )
   cfgDb.msg.debug( "loaded %i confDb packages" % nPkgs )
   return nFiles


### function to UNload all ConfigurableDb informations -----------------------
def unloadConfigurableDb():
   """UNload all ConfigurableDb files (modules) holding informations about
   Configurables available in the release:
    - remove imported modules from sys.modules
    - clear the configurable Db
   """

   global cfgDb

   confModules = set( [ cfgDb.get(k).get('module')  for k in cfgDb ] +
                       [ cfgDb.get(k).get('package') for k in cfgDb ] )
   cfgDb.clear()
   import sys
   for m in sys.modules.keys():
      if m in confModules or \
         m.count("_merged_confDb") > 0 or \
         m.count(".GaudiHandles" ) > 0 or \
         m.count("ConfigurableDb") > 0 or \
         m.count("AthenaPython")   > 0:

         if hasattr(sys.modules[m], '__dict__'):
            for i in sys.modules[m].__dict__.keys():
               del sys.modules[m].__dict__[i]
         del sys.modules[m]
   del confModules


def getConfigurable( className, requester = '', assumeCxxClass = True ):
   """Localize and load a Configurable class based on the (C++) class name.
   """

   global cfgDb

 # fill the configurable dict, first time only
   try:
      global _fillConfDict
      _fillConfDict()
      del _fillConfDict
   except NameError:
      pass

   confClass = className
   if assumeCxxClass:
    # assume className is C++: --> translate to python
      confClass = string.translate( confClass, _transtable )

 # attempt to retrieve existing information
   confClassInfo = cfgDb.get( confClass )

 # get the python module, if info available
   confMod = confClassInfo and confClassInfo.get( 'module' )
   if not confMod:
      cfgDb.msg.warning( "%s: Class %s not in database", requester, className )
      return None

 # load the module
   try:
      mod = __import__( confMod, globals(), locals(), confClass )
   except ImportError:
      cfgDb.msg.warning( "%s: Module %s not found (needed for configurable %s)",
                   requester, confMod, className )
      return None

 # localize class
   try:
      confClass = getattr( mod, confClass )
   except AttributeError:
      cfgDb.msg.warning( "%s: Configurable %s not found in module %s",
                   requester, confClass, confMod )
      return None

 # got this far, assume confClass to be valid
   cfgDb.msg.debug( "%s: Found configurable %s in module %s",
              requester, confClass, confMod )

   return confClass


### create configurable DB, and pre-fill it, then clean-up remnants ----------
class _Singleton( object ):
 # the object this singleton is holding
 # No other object will be created...
   __obj = _CfgDb()

   def __call__( self ):
      return self.__obj

CfgDb = _Singleton()
cfgDb = CfgDb()          # default name for CfgDb instance

del _CfgDb, _Singleton


### basic tester code; minimally assumes AtlasCore environment ---------------
if __name__ == '__main__':
 # we'll get a recursive import, so point to ourselves
   import sys, __main__
   sys.modules[ 'AthenaCommon.ConfigurableDb' ] = __main__  # for clients
   sys.modules[ 'ConfigurableDb' ]              = __main__  # for siblings

 # retrieve a configurable
   conf = getConfigurable( 'StoreGateSvc' )

 # now make sure it is what we think it is
   from StoreGate.StoreGateConf import StoreGateSvc
   assert( conf is StoreGateSvc )
