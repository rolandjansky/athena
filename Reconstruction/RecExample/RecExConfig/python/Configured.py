# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: 
# Author: 

## for messaging
from AthenaCommon.Logging import logging
#log = logging.getLogger( 'Configured' )

# do not trace within ObjKeyStore 
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/RecExConfig/Configured*")

import traceback

### base class for fully configured algorithm and public tool ======
class Configured( object ):
   _nNewed = 0  
   _nInitialized = 0
   _disabled = False
   _configured = False
   _existingOutput = False   
   _ignoreExistingDataObject= False # if ignore existing data object
   _ignoreConfigError=False # if ignore python configuration error
   _output = {}


   def __new__ ( cls, *args, **kwargs ):

      cls._nNewed +=1
      mlog = logging.getLogger( 'Configured::__new__:%s:' % cls.__name__.replace( ".", '_' ) )
      if cls._nNewed ==1:
         mlog.debug("new : First instance" )
         cls.__newobject = object.__new__( cls )
      else:
         mlog.debug("new : nNewed : %s => simple access." % cls._nNewed )            
      
    

      return cls.__newobject


# constructor with optional argument
# optional argument should only be used to hack around an existing
# configuration, hence only if corresponding global variables are set
   def __init__( self, disable=False, ignoreExistingDataObject=False, ignoreConfigError=False ):
      mlog = logging.getLogger( 'Configured::__init__:%s:' % self.__class__.__name__.replace( ".", '_' )  )
      self._nInitialized +=1
      if self._nInitialized ==1:
         mlog.debug(" : First instance=>configure" )

      else:
         mlog.debug("already initialized : => simple access." )
         return



      from RecExConfig.RecConfFlags import recConfFlags
         
      self._ignoreExistingDataObject=ignoreExistingDataObject and recConfFlags.AllowIgnoreExistingDataObject()
      self._ignoreConfigError=ignoreConfigError and recConfFlags.AllowIgnoreConfigError()
         

      if disable and recConfFlags.AllowDisable :
         self._disabled=True
         mlog.info(" : Disabling" )
         return

      # initialisation
      self._configured = self.preconfigure() 
      doConfigure=False
      if not self._configured:
         mlog.debug("preconfiguration indicates this alg should not run")    
      else:   
         configuredOK = self.configure()
         if not configuredOK:
            mlog.error("configuration failed")    
 
      self._configured= self._configured and configuredOK

      mlog.info ("%s/%s" % (["Not configured","Configured"][self.configured()],
                            ["Enabled", "DISABLED"][self.disabled()]))


   def preconfigure (self):
      mlog = logging.getLogger( 'Configured::preconfigure:%s:' % self.__class__.__name__.replace( ".", '_' )  )
      mlog.debug("Output= %s" % self.output() )

      if self.checkExistingOutput ():
         return False
      
      return True

   def configure (self):
      mlog = logging.getLogger( 'Configured::configure:%s:' % self.__class__.__name__.replace( ".", '_' )  )
      mlog.info("Configured::configure not reimplemented ")
      return True


   def configured (self):
      return self._configured

   def disabled (self):
      return self._disabled

   def usable (self):
      mlog = logging.getLogger( 'Configured::usable:%s:' % self.__class__.__name__.replace( ".", '_' )  )      
      mlog.debug ("configured:%s disabled:%s" % (self.configured(),self.disabled()))
      return self.existingOutput() or ( self.configured() and not self.disabled() )


   def ignoreExistingDataObject (self):
      return self._ignoreExistingDataObject

   def ignoreConfigError (self):
      return self._ignoreConfigError

   def output (self):
      return self._output

   def existingOutput (self):
      return self._existingOutput

   def checkExistingOutput (self):
      
# check wether output is already in objKeyStore.
# If yes, unless ignore explicitly, return true
# If multiple output, all type/key should be present

      from RecExConfig.ObjKeyStore import objKeyStore

      mlog = logging.getLogger( 'Configured::existingOutput:%s:' % self.__class__.__name__.replace( ".", '_' )  )
      oneMatch=False
      oneUnmatch=False
      for atype in self.output().keys():
         allkeys=self.output()[atype]
         if type(allkeys) != type([]): allkeys    = [allkeys]
         for akey in allkeys:
            if objKeyStore.isInInput(atype,akey):
               oneMatch=True
            else:
               mlog.info('one object not in output %s:%s' % (atype,akey))
               oneUnmatch=True            


      if oneMatch and not oneUnmatch:         
         if self.ignoreExistingDataObject():
            mlog.info("Output objects already available. Continue nevertheless")
         else:
            mlog.info("Output objects already available. Do nothing")
            self._existingOutput=True
            return True

      elif oneMatch and oneUnmatch:               
         mlog.info("Some, not all, output objects already available. Continue nevertheless")
         # mlog.info("Some, not all, output objects already available. Do nothing")
         # self._existingOutput=True
         # return True

      else:
         mlog.info("No output objects already available. Continue.")      
      self._existingOutput=False
      return False   
      

   # Helper to retrieve a Getter for an input to this object.
   # IGPATH is the name of the Getter class; should be a name
   # of the form "MODULE.NAME".  Returns the Getter object
   # or None.
   def getInputGetterClass (self, igpath):
        # Make a logger.
        clsname = self.__class__.__name__.replace( ".", '_' ) 
        mlog = logging.getLogger('%s:getInputGetter' % clsname)

        # Find the input getter.
        try:
            (ig_modname, ig_name) = igpath.rsplit ('.', 1)
            ig_mod = __import__ (ig_modname, globals(), locals(), [ig_name])
            ig_cls = getattr (ig_mod, ig_name)
        except:
            mlog.error("could not get handle to %s Quit" % igpath)
            print traceback.format_exc()
            return None
        return ig_cls
     
   # Helper to retrieve a Getter for an input to this object.
   # IGPATH is the name of the Getter class; should be a name
   # of the form "MODULE.NAME".  Returns the Getter object
   # or None.
   #
   # If the input is not a string, it is just returned directly.
   def getInputGetter (self, igpath):
        if type(igpath) != type(''):
           return igpath
        
        # Make a logger.
        clsname = self.__class__.__name__.replace( ".", '_' ) 
        mlog = logging.getLogger('%s:getInputGetter' % clsname)

        # Find the input getter.
        try:
            ig_cls = self.getInputGetterClass(igpath)
            if ig_cls == None: return None
            input_getter = ig_cls()
        except:
            mlog.error("could not get handle to %s Quit" % igpath)
            print traceback.format_exc()
            return None

        if not input_getter.usable():
            ig_name = ig_cls.__name__
            if not self.ignoreConfigError():
                mlog.error("%s unusable. Quit." % ig_name)
                return None
            else:
                mlog.error("%s unusable. Continue nevertheless" % ig_name)

        return input_getter
     
