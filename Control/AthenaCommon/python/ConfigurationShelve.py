# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: AthenaCommon/python/ConfigurationShelve.py
# @author: Wim Lavrijsen (WLavrijsen@lbl.gov)
# @author: Sebastien Binet <binet@cern.ch>

try:
   import cPickle as pickle
except ImportError:
   import pickle
import shelve

"""Shelves for writing jars of configuration. Example:

WRITING:

   shelve = ConfigurationShelve( "myjob" )
   jar = ConfigurationJar( 'myjar' )
   shelve.store( jar )

READING:
   shelve = ConfigurationShelve( "myjob" )
   jar = shelve.retrieve( 'myjar' )
"""


### data ---------------------------------------------------------------------
__version__ = '1.1.0'
__author__  = """\
Wim Lavrijsen (WLavrijsen@lbl.gov),
Sebastien Binet <binet@cern.ch>"""

__all__ = [ 'ConfigurationShelve', 'ConfigurationJar',
            'saveToAscii',  'loadFromAscii',
            'saveToPickle', 'loadFromPickle',
            'cmpConfigs',
            ]

### FIXME: --- hack to workaround bug #34752
###        http://savannah.cern.ch/bugs/?34752
def _monkeypatch_bug_34752():
   from GaudiKernel.GaudiHandles import GaudiHandleArray
   if 'typesAndNames' in GaudiHandleArray.__slots__:
      try:
         del GaudiHandleArray.__getstate__
         del GaudiHandleArray.__setstate__
      except AttributeError: pass # already done, or not relevant anymore
   return
_monkeypatch_bug_34752()
del _monkeypatch_bug_34752


### representation of job databases ==========================================
class ConfigurationShelve( object ):
   __openShelves = {}

   def __init__( self, name ):
      try:
         self._shelve = self.__openShelves[ name ]
      except KeyError:
         self._shelve = shelve.open( name, protocol = pickle.HIGHEST_PROTOCOL )

   def __getitem__( self, key, refresh = True ):
      if refresh:
       # remove current configuration first, as much as possible, to prevent
       # hysteresis; note that if the user keeps configurables around in the
       # main (or any other) space, a merge will occur
         from AlgSequence import AlgSequence

         topSeq = AlgSequence()
         topSeq.removeAll()

         from AppMgr import theApp, ServiceMgr, ToolSvc, theAuditorSvc

         theApp._streams.removeAll()
         ServiceMgr.removeAll()
         ToolSvc.removeAll()
         theAuditorSvc.removeAll()

         del topSeq, theApp, ServiceMgr, ToolSvc, theAuditorSvc

      return self._shelve[ key ]

   def __setitem__( self, key, value ):
      self._shelve[ key ] = value

   def store( self, jar ):
      return self.__setitem__( jar.getName(), jar )

   def retrieve( self, jarname, refresh = True ):
      return self.__getitem__( jarname, refresh )


### representation of individual jobs ========================================
class ConfigurationJar( object ):
   def __init__( self, name ):
      self.name = name

      from AppMgr import theApp, ServiceMgr
      from JobProperties import jobproperties

      from . import AlgSequence as _as
      self.athMasterSeq = _as.AthSequencer ("AthMasterSeq")
      self.TopSequence  = _as.AlgSequence  ("TopAlg")
      self.athAlgSeq    = _as.AthSequencer ("AthAlgSeq")
      self.athOutSeq    = _as.AthSequencer ("AthOutSeq")
      self.athRegSeq    = _as.AthSequencer ("AthRegSeq")
      
      self.AppMgr      = theApp
      self.ServiceMgr  = ServiceMgr     # takes care of AuditorSvc & ToolSvc
      self.JobProperties = jobproperties
      
   def getName( self ):
      return self.name

   def content( self ):
      return self.__getstate__()
   
   def __getstate__( self ):
      d = dict()

      d[ 'name' ] = self.name

      d[ 'AthMasterSeq' ]  = self.athMasterSeq
      d[ 'AthAlgSeq' ]     = self.athAlgSeq
      d[ 'TopSequence' ]   = self.TopSequence
      d[ 'AthOutSeq' ]     = self.athOutSeq
      d[ 'AthRegSeq' ]     = self.athRegSeq
      d[ 'AppMgr' ]        = self.AppMgr
      d[ 'Streams' ]       = self.AppMgr._streams
      d[ 'ServiceMgr' ]    = self.ServiceMgr #takes care of {Auditor,Tool}Svc
      d[ 'JobProperties' ] = self.JobProperties
      
      return d

   def __setstate__( self, d ):
    # get handles to the global objects for convenience
      self.__init__( d[ 'name' ] )

    # the mere existence of 'd' has done its work through the "magic"
    # of instances shared on name for all configurables

    # now handle jobproperties
      import JobProperties as JobProperties
      setattr( JobProperties, 'jobproperties',  d['JobProperties'] )
      pass

   def __str__(self):
      import os
      return os.linesep.join( [ "%s" % v for v in self.content().values() ] )


### store/load config of unitialized job =====================================

#----- storing to db
def storeJobOptionsCatalogue( cfg_fname ):
   """Inspect the 'configured' JobOptionsSvc and existing services,
   then dump the properties of each component into a pickle file.
   """

   from collections import defaultdict
   jocat = defaultdict( dict )
   jocfg = defaultdict( dict )

   def _fillCfg( client, props ):
     for p in props:
        n, v = p
        if hasattr( v, 'toStringProperty' ):
           v = str( v.toStringProperty() )
        elif hasattr( v, 'toString' ):
           v = str( v.toString() )
        else:
           v = str( v )
        jocfg[ client ][ n ] = v

   from AppMgr import theApp
   from AppMgr import ServiceMgr as svcMgr
   from Configurable import Configurable as C

 # tickle the Gaudi C++ side and configure the ApplicationMgr
   theApp.setup( recursive = True )

   app_props = [ (k,v.value())
                 for k,v in theApp.getHandle().properties().iteritems() ]
   _fillCfg( 'ApplicationMgr', app_props )

 # get all services that now already exist, as they require special treatment;
 # the appmgr handle talks directly to the ServiceManager and services() returns
 # a copy of the services names
   svcs = theApp.getHandle().services()

 # now assume that if these services carry configuration, then they should exist
 # on the service manager configurable
   cfgSvcs = []
   for s in svcs:
      if hasattr( svcMgr, s ):
         cfgSvcs.append( s )

 # make sure to get the values for these special cases
   for svcname in cfgSvcs:
      svc = theApp.service( svcname )
      props = []
      for k,v in svc.properties().iteritems():
         if v.value() != C.propertyNoValue:
            props.append( (k,v.value()) )
      _fillCfg( svcname, props )

 # make sure to propagate the EventLoop properties through the josvc
   try:
      evLoopName = theApp.EventLoop.split('/')[-1]
      evLoop = getattr( svcMgr, evLoopName )

      props = []
      for k,v in evLoop.properties().iteritems():
         if v != C.propertyNoValue:
            props.append( (k,v) )
      _fillCfg( evLoopName, props )

   except AttributeError:
      pass    # no properties defined for EventLoop type

 # get the values for all other components (these may contain duplicates of
 # the ones above in cfgSvcs, and there may even be conflicts)
   import AthenaPython.PyAthena as PyAthena
   josvc = PyAthena.py_svc( 'JobOptionsSvc', iface = 'IJobOptionsSvc' )

   clients = list( josvc.getClients() )
   for client in clients:
      props = josvc.getProperties( client )
      for prop in props:
         n = prop.name()
         v = prop.toString()
         jocat[ client ][ n ] = v

 # take care of some ancient history
   for k in ( 'Go', 'Exit' ):
      if k in jocfg[ 'ApplicationMgr' ]:
         del jocfg[ 'ApplicationMgr' ][k]

 # workaround for pycomps
   pycomps = []
   for c in C.allConfigurables.itervalues():
      if not isinstance( c, (PyAthena.Alg,
                             PyAthena.AlgTool,
                             PyAthena.Svc,
                             PyAthena.Aud) ):
         continue
    # FIXME: should check if the component is still 'active'
    #        ie: is it in one of the TopAlg,SvcMgr,ToolSvc and children ?
      try:
         delattr( c, 'msg' )
      except AttributeError:
         pass

      pycomps.append( c )

 # write out all parts; start with the jocat, so that it can be loaded by
 # itself only, if need be

   cfg = open( cfg_fname, 'wb' )             # output pickle file

   pickle.dump( jocat, cfg )                 # jobopt catalogue contents
   pickle.dump( jocfg, cfg )                 # special services' properties
   pickle.dump( pycomps, cfg )               # python components workaround

   cfg.close()

   return cfg_fname


#----- loading from DB
def loadJobOptionsCatalogue( cfg_fname ):
   """Load properties from a pickle file, previously dumped by
   storeConfiguration, back into the JobOptionsSvc.
   """

 # read jobopt catalogue dump and pycomps back in
   cfg = open( cfg_fname, 'rb' )

   jocat   = pickle.load( cfg )
   jocfg   = pickle.load( cfg )
   pycomps = pickle.load( cfg )

   cfg.close()

   kw = jocfg[ 'ApplicationMgr' ]
   from AppMgr import theApp
   theApp.JobOptionsSvcType = kw[ 'JobOptionsSvcType' ]
   theApp.MessageSvcType    = kw[ 'MessageSvcType' ]
   theApp.getHandle( kw )
   del jocfg[ 'ApplicationMgr' ]

 # no longer want to call setup(), since there are no Configurables to
 # setup; it would be a no-op, already, but __build_master_sequence is
 # broken, so make sure this can't be run ...
   def noop( self ):
      pass
   theApp.__class__.setup = noop

   import AthenaPython.PyAthena as PyAthena
   josvc = PyAthena.py_svc( 'JobOptionsSvc', createIf = False, iface = 'IJobOptionsSvc' )

 # restore job catalogue entries
   import GaudiPython.Bindings as gaudi
   for client in jocat:
      for n,v in jocat[ client ].iteritems():
         # In Gaudi v28, the second argument of the ctor is passed by move,
         # which pyroot doesn't handle correctly.  Do this as a workaround.
         p = gaudi.StringProperty( n, '' )
         try:
            p.fromString(v).ignore()
         except Exception:
            print "Failed to convert",n,v

         if not josvc.addPropertyToCatalogue( client, p ).isSuccess():
            raise RuntimeError( 'could not add property [%s.%s = %s]' % (client, n, v) )

 # restore special services properties
   for client in jocfg:
      svc = PyAthena.py_svc( client, createIf = False, iface='IProperty' )
      for n,v in jocfg[ client ].iteritems():
         # See comment above.
         p = gaudi.StringProperty( n, '' )
         p.fromString(v).ignore()
         svc.setProperty( p )

 # pycomps hack-around
   if pycomps:
      import AthenaPython.Configurables as _C
      _C.PyComponents.instances = dict( (p.name, p) for p in pycomps )
      for p in pycomps:
         if hasattr( p, 'setup' ):
            if callable( p.setup ):
                p.setup()


### further saving utilities =================================================
def saveToAscii(out, cfgName=None):
   """
   Helper function to store the current configuration to (ASCII) file
   """
   import os

 # temporarily set the Configurable log level to make sure that the printout
 # does not get suppressed
   import Configurable, Constants
   llevel = Configurable.log.level
   Configurable.log.setLevel( Constants.INFO )

   if cfgName is None: cfgName = "Snapshot"
   content = ConfigurationJar( cfgName ).content()

   if type(out) == str:
      out = open( out, "w" )
   for v in content.values(): out.write( str(v) + os.linesep )

   Configurable.log.setLevel( llevel )

   return

def loadFromAscii(fileName, cfgName=None):
   """
   Helper function to retrieve a configuration from an ASCII file.
   """
   raise NotImplementedError("Sorry no restoration from ASCII file (yet?)")

def saveToPickle(fileName, cfgName=None):
   """
   Helper function to store the current configuration to a pickle file
   """
   if cfgName is None: cfgName = "Snapshot"
   shelve = ConfigurationShelve( fileName )
   jar = ConfigurationJar( cfgName )
   shelve.store( jar )
   return

def loadFromPickle(fileName, cfgName=None):
   """
   Helper function to retrieve a configuration from a pickle file
   """
   if cfgName is None: cfgName = "Snapshot"
   import os
   fileName = os.path.expanduser(os.path.expandvars(fileName))
   shelve = ConfigurationShelve( fileName )
   # configuration is loaded by side-effects...
   jar = shelve.retrieve( cfgName )
   return jar

def cmpConfigs (ref, chk, refName=None, chkName=None):
   """
   Helper function to compare 2 configurations.
    @param ref the path to a pickle file where a configuration was stored
    @param chk the path to a pickle file where a configuration was stored
    @param refName the name of the configuration jar to load
    @param chkName the name of the configuration jar to load
   if `ref` and/or `chk` are joboptions (ie: their extension is '.py') a shelve
   is automatically created on the fly
   """
   import os
   _path = os.path
   ref = _path.expanduser(_path.expandvars(ref))
   chk = _path.expanduser(_path.expandvars(chk))
   
   from PyUtils.Decorators import forking
   # if we are given joboptions, create ConfigurationShelves on the fly
   @forking
   def _create_shelve (joboname):
      from tempfile import NamedTemporaryFile
      fname = os.path.basename (joboname)
      fname = os.path.splitext(fname)[0] + '.pkl'
      fname = NamedTemporaryFile(prefix=fname, suffix='.pkl').name
      job = [ "from AthenaCommon.Include import include",
              "include ('%s')" % joboname,
              "from AthenaCommon.ConfigurationShelve import saveToPickle",
              "saveToPickle ('%s')" % fname,
              # exit before actually running the job
              "raise SystemExit(0)", 
              ]
      jobofile = NamedTemporaryFile(suffix='.py')
      map (jobofile.writelines, [l+os.linesep for l in job])
      jobofile.flush()
      from commands import getstatusoutput
      sc,out = getstatusoutput ('athena.py %s' % jobofile.name)
      jobofile.close()
      if sc==0:
         return fname
      return (sc, out)

   if os.path.splitext(ref)[1]=='.py':
      print "::: creating a shelve on the fly for [%s]..."%ref
      ref = _create_shelve (ref)
      if not isinstance(ref, str):
         raise RuntimeError (
            'could not create a shelve on the fly:\n%s'%ref[1])
      import atexit
      atexit.register (os.unlink, ref)
      
   if os.path.splitext(chk)[1]=='.py':
      print "::: creating a shelve on the fly for [%s]..."%chk
      chk = _create_shelve (chk)
      if not isinstance(chk, str):
         raise RuntimeError (
            'could not create a shelve on the fly:\n%s'%chk[1])
      import atexit
      atexit.register (os.unlink, chk)
      
   for fname in (ref, chk):
      if not _path.exists (fname):
         raise RuntimeError ('file [%s] does not exist'%fname)

   @forking
   def _dict_cfg (fname, cfgName):
      #from PyUtils.odict import OrderedDict as odict
      jar = loadFromPickle (fname, cfgName)
      d = list()
      all_cfgs = dict()
      all_cfgs.setdefault (dict)
      def _visit_cfg (cfg):
         name = cfg.getJobOptName()
         values = [name]
         all_cfgs[name] = dict()
         props = cfg.getProperties()
         def _get_value (cfg, k, v):
            _no_value = cfg.propertyNoValue
            if v == _no_value:
               v = cfg.getDefaultProperty(k)
            return v

         from Configurable import Configurable
         for k,v in props.iteritems():
            if not isinstance(v, Configurable):
               all_cfgs[name][k] = _get_value(cfg,k,v)
            else:
               all_cfgs[name][k] = v.getJobOptName()
               if v not in cfg.getAllChildren():
                  values.extend (_visit_cfg(v))

         if hasattr(cfg, 'getAllChildren'):
            for c in cfg.getAllChildren():
               values.extend (_visit_cfg(c))
         return values

      for c in jar.TopSequence.getChildren():
         d.extend (_visit_cfg(c))
      
      for c in jar.ServiceMgr.getChildren():
         d.extend (_visit_cfg(c))
      return d, all_cfgs

   ref_list, ref_cfgs = _dict_cfg (ref, refName)
   chk_list, chk_cfgs = _dict_cfg (chk, chkName)

   report = []
   addNames,subNames = [],[]
   if ref_list != chk_list:
      report.append ("::: configurable list in 'ref' and in 'chk' differ !")
      addNames = sorted([ n for n in chk_cfgs if not (n in ref_cfgs) ])
      if len (addNames) > 0:
         report.append ("::: configurables in 'chk' and not in 'ref':")
         report.extend (["  + %s" % n for n in addNames])
      subNames = sorted([ n for n in ref_cfgs if not (n in chk_cfgs) ])
      if len (subNames) > 0:
         report.append ("::: configurables in 'ref' and not in 'chk':")
         report.extend (["  - %s" % n for n in subNames])

   common_cfgs = set (
      [n for n in ref_list if not (n in addNames or n in subNames)] +
      [n for n in chk_list if not (n in addNames or n in subNames)]
      )
   for n in common_cfgs:
      ref_cfg = ref_cfgs[n]
      chk_cfg = chk_cfgs[n]
      diff_props_report = []
      for k in ref_cfg.iterkeys():
         ref_val = str(ref_cfg.get(k, '**N/A**'))
         chk_val = str(chk_cfg.get(k, '**N/A**'))
         
         if ref_val != chk_val:
            diff_props_report.append (" ref: %s = %r" % (k, ref_val))
            diff_props_report.append (" chk: %s = %r" % (k, chk_val))
      if len(diff_props_report)>0:
         report.append ("::: properties differing for [%s]:"%n)
         report.extend (diff_props_report)
   return (ref_list, ref_cfgs), (chk_list, chk_cfgs), report
