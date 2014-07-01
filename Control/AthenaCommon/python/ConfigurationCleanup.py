# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/ConfigurationCleanup.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

# Forced (!) cleanup of the configuration set of Configurables. If PerfMon is
# being used, only call this function after initialize(). Part of this code has
# been experimentally defined, and so it does not cover all situations.

doCleanse = False


def _deleteChildren( sequence ):
 # removeAll() does not work if names are kept in an instance __dict__
   for c in sequence.getChildren():
      try:
         delattr( sequence, c.getName() )
      except AttributeError:       # in case of double-listing (unlikely)
         pass
   sequence.removeAll()
   sequence._Configurable__children = []


def Cleanse():
   global doCleanse
   if not doCleanse:
      return

   import gc, __main__, __builtin__, sys

   print 'There are %d python objects outstanding. Now cleansing ... ' % len(gc.get_objects())

 # to be able to run, theApp has to live
   import AthenaCommon.Include as inc
   theApp = inc.include._workspace[ 'theApp' ]
 # so do py-components and ServiceMgr ...
   svcMgr = inc.include._workspace[ 'ServiceMgr' ]
   toolSvc = inc.include._workspace[ 'ToolSvc' ]

 # pickle the python components for reading back after cleansing
   import pickle, AthenaPython.Configurables
   pycomps_dumpname = 'pycomponents_dump.pickle'
   f = open( pycomps_dumpname, 'wb' )
   pickle.dump( AthenaPython.Configurables.PyComponents.instances, f )
   f.flush(); f.close()
   del f, AthenaPython.Configurables, pickle

   def _is_special (c):
      """helper to sort on 'special' configurables and the facades"""
      try:
         return (c in (theApp, svcMgr, toolSvc)
              or c is sys.modules[ 'AthenaCommon.CfgMgr' ])
      except Exception:      # can happen if 'c' already half broken up
         return False

 # two classes to keep alive for later
   from Configurable import Configurable
   from JobProperties import JobProperty

 # make JobProperty shut up
   import Logging
   Logging.logging.getLogger('JobProperty ::').setLevel( Logging.logging.FATAL )
   del Logging

 # zap the global workspace, but return the 'special' globals and expected vars
   inc.include._workspace.clear()
   inc.include._workspace.update( __main__.__dict__ )
   inc.include._workspace[ 'theApp' ] = theApp
   del inc

 # PropertyProxy's, which are at the class level, keep a reference to instances
 # in their property setting history. This causes circles that need unwinding.
   for name, conf in Configurable.allConfigurables.items():

      if _is_special (conf):
         #print "::: skipping [%s]..." % conf.name()
         continue
      else:
         #print ">>> cleansing [%s]..." % conf.name()
         pass
      
    # delete user-side added variables (derived classes only)
      if hasattr( conf, '__dict__' ):
         if _is_special (conf):
            continue
         else:
            conf.__dict__.clear()

    # delete all properties at the (base)class level
      for b in [ conf.__class__ ] + list( conf.__class__.__bases__ ):
         if not hasattr( b, '_properties' ):
            continue

         for k, p in b._properties.items():
            try:
               delattr( b, k )
            except AttributeError:
               pass

         b._properties = {}

    # disconnect the configurable from its children and vice versa
      _deleteChildren( conf )

 # for the service manager and app manager, we're still done with children
   _deleteChildren( svcMgr )
   _deleteChildren( theApp )
   _deleteChildren( theApp._toolsvc )

 # a bit of paranoia ...
   try:
      del conf
   except NameError:
      pass

 # destroy the database of Configurables
   import ConfigurableDb
   ConfigurableDb.unloadConfigurableDb()
   del ConfigurableDb

 # perform an explicit collect here, b/c in get_referrers() in the next block
 # is slower, if more objects are being tracked
   gc.collect()

 # cleanse __main__ to the best extend possible; this is geared towards removal
 # of Configurables, but also removes variables that are hanging around
   for k, v in __main__.__dict__.items():
      if _is_special (v):
         continue

    # do not cleanse internal variables of athena.py
      if k[0] == '_':
         continue

    # do not cleanse user options variables, some modules and functions of athena.py
      if k in [ 'DbgStage',  'runBatch', 'scripts', \
                'logging', 'os', 'sys', \
                'include' ]:
         continue

    # do not touch builtins
      if k in __builtin__.__dict__:
         continue

      delattr( __main__, k )

   del v, k
   
 # perform an explicit collect here, so that the result of get_objects() is
 # smaller and the loop is subsequently faster
   gc.collect()

 # cleanse sys.modules to the best extend possible
   for key, mod in sys.modules.items():
      if hasattr(mod,'__file__') and type(mod.__file__) == str:

       # erase Atlas modules; skip core, to keep a functioning application
         if 'InstallArea' in mod.__file__:
            if not reduce( lambda x, y: x or y == key[:len(y)], \
                 [ 0, '__builtin__', '__main__',
                   'AthenaCommon.AppMgr',
#                   'ConfigurationCleanup',   # explicitly erased later
                   'GaudiKernel', 'GaudiPython', 'PerfMon' ] ):
               if key != 'AthenaCommon':
                  del sys.modules[ key ]

            elif 'Flags' in key:
               del sys.modules[ key ]

       # XML is a major consumer of objects, so force its unloading
         if 'xml' in mod.__file__ or 'pyxexpat' in mod.__file__:
            try:
               del sys.modules[ key ]
            except KeyError:      # happens on PyUtils
               pass

      elif not sys.modules[ key ]:
         del sys.modules[ key ]   # typically 'None' placeholders
   del mod

   gc.collect()

   import weakref

 # final item set to attempt to destroy configurables
   stillThere = []
   for o in gc.get_objects():
      if _is_special( o ):
         continue

      if isinstance( o, ( Configurable, JobProperty, weakref.ref ) ):
         stillThere.append( o )
         continue

      try:
         if issubclass( o, JobProperty ) and not o is JobProperty:
            stillThere.append( o )
            continue
      except TypeError:
         pass

   for o in stillThere:
      ll = gc.get_referrers( o )
      for l in ll:
         if l is stillThere:
            continue

         elif type(l) == list:
            l.remove( o )

         elif type(l) == dict:
            for k, v in l.items():
               if v is o:
                  del l[ k ]

   del o, stillThere
   del _is_special

 # remove JobProperties
   del JobProperty
   if 'AthenaCommon.JobProperties' in sys.modules:
      del sys.modules[ 'AthenaCommon.JobProperties' ]
   if 'JobProperties' in sys.modules:
      del sys.modules[ 'JobProperties' ]
   
 # remove oneself
   if 'AthenaCommon.ConfigurationCleanup' in sys.modules:
      del sys.modules[ 'AthenaCommon.ConfigurationCleanup' ]
   if 'ConfigurationCleanup' in sys.modules:
      del sys.modules[ 'ConfigurationCleanup' ]

 # all done ...
   del __builtin__, __main__

 # one final time
   gc.collect()

 # now reload the python components, and restore part of the Configurables
 # for their benefit
   import pickle, AthenaPython.Configurables
   f = open( pycomps_dumpname, 'rb' )
   AthenaPython.Configurables.PyComponents.instances = pickle.load( f )
   f.close()

 # report on success
   print 'End of cleanup, there are %d python objects left' % len(gc.get_objects())
   del gc
