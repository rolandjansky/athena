# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file PyAthenaEventLoopMgr.py
#  @brief Python facade of PyAthenaEventLoopMgr
#  @author Wim Lavrijsen (WLavrijsen@lbl.gov)

import GaudiPython.Bindings as PyGaudi

# The following method is setup at ApplicationMgr creation time; this works
# because the event loop manager, which imports this module if it is the
# python one at initialization time, will instantiate the event selector in
# its base-class initialize(). Hence # the order is controlled, but all on
# the C++ side of things.
def enable_seeking(silent=False):
   """ try to install seek-stuff on the EventSelector side.
   if `silent` is True, only an attempt at installing the seeking is performed.
   otherwise an exception is raised if the seeking could not be installed.
   """

   import sys
   from AthenaCommon.Logging import log as msg
   if 'AthenaPoolCnvSvc.ReadAthenaPool' not in sys.modules:
      if silent:
         _msg = msg.debug
      else:
         _msg = msg.info
      # user did not import that module so we give up
      _msg( "Cannot enable 'seeking' b/c module "
            "[AthenaPoolCnvSvc.ReadAthenaPool] hasn't been imported..." )
      _msg( "Modify your jobOptions to import that module "+ \
            "(or just ignore this message)" )

      if not silent:
         raise RuntimeError("configuration-logic error")
      
      return

   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.Configurable import Configurable
   collectionType = svcMgr.EventSelector.properties()["CollectionType"]

   if collectionType in ( "ImplicitROOT", Configurable.propertyNoValue, ):
      msg.info   ( "=> Seeking enabled." )

   else:
      msg.warning( "Input seeking is not compatible with collection type of %s",
                   svcMgr.EventSelector.properties()["CollectionType"] )
      msg.warning( "=> Seeking disabled." )
      if not silent:
         raise RuntimeError("could not install seeking")
      
   from AthenaCommon.AppMgr import theApp
   if theApp.state() != theApp.State.OFFLINE:
      # do not bring up the whole C++ kaboodle too early in the game
      svcMgr.EventSelector.setup()
   return

def _setupEvtSelForSeekOps():
   return enable_seeking(silent=True)

_setupEvtSelForSeekOps()
del _setupEvtSelForSeekOps


## @class _PyAthenaEventLoopMgrClass
#  @brief Python facade of PyAthenaEventLoopMgr.
class PyAthenaEventLoopMgr( PyGaudi.iService ):

   def __init__( self ):
      PyGaudi.iService.__init__( self, 'PyAthenaEventLoopMgr' )

   def _installServices( self, cppself ):
    # install the interfaces onto oneself; the order is (as per the
    # cpp side multiple inheritence:
    #   IEventSeek
    #   IService              => from here ...
    #     INamedInterface
    #     IInterface
    #   IProperty             => ... up to here from the iService base class
    #   IStateful
    #   IEventProcessor
    #
    # The expectation from the C++ side it that an IEventSeek is received. Note
    # that this code can not call into the application manager, as this is run
    # during initialize, making the IService PyAthenaEventLoopMgr unavailable.
    #
    # Note that this is all a poor man's way of not needing to have dictionaries
    # for all base classes of the C++ PyAthenaEventLoopMgr.

      import cppyy

    # need to set all the following through the __dict__ b/c of iPropert.__setattr__

    # the expect IEventSeek
      self.__dict__[ '_evtSeek' ] = cppyy.bind_object( cppself, cppyy.gbl.IEventSeek )

    # the IService needed for iService._isvc and likewise iProperty._ip
      self.__dict__[ '_isvc' ] = PyGaudi.InterfaceCast( cppyy.gbl.IService )( self._evtSeek )
      self.__dict__[ '_ip' ] = PyGaudi.InterfaceCast( cppyy.gbl.IProperty )( self._evtSeek )

    # IStateful and IEventProcessor
      self.__dict__[ '_state' ] = PyGaudi.InterfaceCast( cppyy.gbl.IStateful )( self._evtSeek )
      self.__dict__[ '_evtpro' ] = PyGaudi.InterfaceCast( cppyy.gbl.IEventProcessor )( self._evtSeek )

   def __getattr__( self, attr ):
    # note the lookup order: should be as per the C++ side
      for obj in [ self._evtSeek, self._state, self._evtpro ]:
         try:
            return getattr( obj, attr )
         except Exception:
            pass

    # let properties be tried last
      return super( PyAthenaEventLoopMgr, self ).__getattr__( attr )

   def executeAlgorithms( self, cppcontext ):
      try:                   import GaudiPython.Bindings as PyGaudi
      except AttributeError: import GaudiPython          as PyGaudi
      except ImportError:    import gaudimodule          as PyGaudi
      from AthenaCommon.AppMgr import theApp

      import cppyy
      ctx = cppyy.bind_object(cppcontext, "EventContext")

      result = PyGaudi.SUCCESS

      try:
         for name in theApp.TopAlg:
            alg = theApp.algorithm( name[ name.find('/')+1 : ] )
            if not alg._ialg:
               alg.retrieveInterface()
            ialg = alg._ialg
            ialg.execState(ctx).reset()
            result = ialg.sysExecute(ctx)
            if result.isFailure():
               from AthenaCommon.Logging import log as msg
               msg.error( "Execution of algorithm %s failed", name )
               return result.getCode()
      except KeyboardInterrupt:
         from AthenaCommon.Logging import log as msg
         msg.critical( "event loop stopped by user interrupt" )
         return PyGaudi.FAILURE.getCode()

      return result.getCode()
