# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/Configurable.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)
# Author: Martin Woudstra (Martin.Woudstra@cern.ch)

import copy, types, os, weakref
from AthenaCommon import ConfigurableMeta

# Note: load iProperty etc. from GaudiPython only as-needed
import GaudiKernel.GaudiHandles as GaudiHandles

### data ---------------------------------------------------------------------
__version__ = '3.2.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'Configurable',
            'ConfigurableAlgorithm', 'ConfigurablePyAlgorithm',
            'ConfigurableAlgTool',
            'ConfigurableAuditor',
            'ConfigurableService',
            'ConfigurableUser'
            ]


## for messaging
from Logging import logging
log = logging.getLogger( 'Configurable' )


### base class for configurable Gaudi algorithms/services/algtools/etc. ======
class Configurable( object ):
   """Base class for Gaudi components that implement the IProperty interface.
      Provides most of the boilerplate code, but the actual useful classes
      are its derived ConfigurableAlgorithm, ConfigurableService, and
      ConfigurableAlgTool."""

 ## for detecting the default name
   class DefaultName:
      pass

   propertyNoValue = '<no value>'
   indentUnit = '| '
   printHeaderWidth=100
   printHeaderPre=5

   _fInSetDefaults = 0x01      # currently setting default values
   _fIsLocked      = 0x02      # track configurable to be locking
   _fIsPrinting    = 0x04      # inside __str__ to avoid setting of private AlgTools
   _fInitOk        = 0x08      # used to enforce base class init
   _fSetupOk       = 0x10      # for debugging purposes (temporary (?))

   __metaclass__ = ConfigurableMeta.ConfigurableMeta

   __slots__ = (
      '__weakref__',          # required for dealing with weak references
      '__children',           # controlled components, e.g. private AlgTools
      '_name',                # the (unqualified) component name
      '_flags',               # see list of flags above
   )

   allConfigurables = weakref.WeakValueDictionary()    # just names would do,
                              # but currently refs to the actual configurables
                              # are used for debugging purposes

   _printOnce = 0

   configurableRun3Behavior=0

   def __new__ ( cls, *args, **kwargs ):
      """To Gaudi, any object with the same type/name is the same object. Hence,
         this is mimicked in the configuration: instantiating a new Configurable
         of a type with the same name will return the same instance."""

    # try to get the name of the Configurable (having a name is compulsary)
      if 'name' in kwargs:
       # simple keyword (by far the easiest)
         name = kwargs[ 'name' ]
      elif 'name' in cls.__init__.func_code.co_varnames:
       # either positional in args, or default
         index =  list(cls.__init__.func_code.co_varnames).index( 'name' )
         try:
          # var names index is offset by one as __init__ is to be called with self
            name = args[ index - 1 ]
         except IndexError:
          # retrieve default value, then
            name = cls.__init__.func_defaults[ index - (len(args)+1) ]
      else:
       # positional index is assumed (will work most of the time)
         try:
            name = args[1]    # '0' is for self
         except (IndexError,TypeError):
            raise TypeError( 'no "name" argument while instantiating "%s"' % cls.__name__ )

      if name == Configurable.DefaultName:
       # select either conventional name, or the type of the class
         if hasattr( cls, 'DefaultedName' ):
            name = cls.DefaultedName
         else:
            name = cls.getType()
      elif not name or type(name) != str:
       # unnamed, highly specialized user code, etc. ... unacceptable
         raise TypeError( 'could not retrieve name from %s.__init__ arguments' % cls.__name__ )

    # close backdoor access to otherwise private subalgs/tools
      if 0 <= name.find( '.' ):
         raise NameError( '"%s": Gaudi name indexing with "." to private configurables not '
                          'allowed, as it leads to uncheckable backdoors' % name )

      if 0 <= name.find( '/' ):
         raise NameError( '"%s": type separator "/" no allowed in component name, '
                          'typename is derived from configurable instead' % name )

      #Uncomment the following line for debugging:
      #print "cls.configurableRun3Behavior=",cls.configurableRun3Behavior
      if cls.configurableRun3Behavior==0:
         # ordinary recycle case
         try:
            conf = cls.configurables[ name ]

          # initialize additional properties, if any
            for n,v in kwargs.items():
               try:
                  setattr( conf, n, v )
               except AttributeError as originalAttributeError:
                # rather annoying that we have to be somewhat silent here (the
                # most common cases are 'name' and user kw args to be supplied
                # to an overridden __init__)
                  log.debug( 'not accepting keyword "%s" as a property', n )

                # now for a completely different can of worms ...
                  acceptableKeyWord = False

                # little helper
                  def flat( classtree ):
                     if isinstance( classtree, list ) or isinstance( classtree, tuple ):
                        return [ j for i in classtree for j in flat( i ) ]
                     else:
                        return [ classtree ]

                # the following attempts to figure out if the missing attribute
                # is in fact an acceptable formal function argument to __init__,
                  import inspect
                  allclasses = flat( inspect.getclasstree( [ conf.__class__ ] ) )
                  for confklass in allclasses:
                     try:
                      # the following may result in the same init tried several
                      # times, but we shouldn't be in this loop too often anyway
                        confinit = getattr( confklass, '__init__' )
                        if n in confinit.func_code.co_varnames:
                           log.debug( 'accepting keyword "%s" as an argument for %s.__init__' % (n,confklass.__name__) )
                           acceptableKeyWord = True
                           break
                     except AttributeError:
                        pass

                # raising here will break the usage of keywords in __init__, but
                # there don't appear to be such cases in ATLAS yet ...
                  if not acceptableKeyWord:
                     raise originalAttributeError
            return conf
         except KeyError:
            pass

       # the following is purely for debugging support and should realistically bomb
         try:
            conf = cls.allConfigurables[ name ]
            raise TypeError( 'attempt to redefine type of "%s" (was: %s, new: %s)' %
                             (name,conf.__class__.__name__,cls.__name__) )
         except KeyError:
            pass
      else:
         #Run 3 style config
         #Uncomment this line to verify that RecExCommon doesn't use that
         #print "Run 3 style config" 
         pass #end if not new configuration approach
    # still here: create a new instance ...
      conf = object.__new__( cls )

    # ... python convention says to silently return, if __new__ fails ...
      if conf is None:
         return

    # ... initialize it
      cls.__init__( conf, *args, **kwargs )

    # update normal, per-class cache
      if cls.configurableRun3Behavior==0:
         cls.configurables[ name ] = conf

    # update generics super-cache
      if cls.configurableRun3Behavior==0:
         cls.allConfigurables[ name ] = conf

      return conf

 # constructor
   def __init__( self, name = DefaultName ):
    # check class readiness, all required overloads should be there now
      klass = self.__class__

    # this is an abstract class
      if klass == Configurable:
         raise TypeError( "%s is an ABC and can not be instantiated" % str(Configurable))

    # for using this Configurable as a (Gaudi) sequence
      self.__children = []

    # know who we are
      if name == Configurable.DefaultName:
         if hasattr( self.__class__, 'DefaultedName' ): 
            self._name = self.__class__.DefaultedName
         else:
            self._name = self.getType()
      else: 
         self._name = name

      self._flags = 0

    # set to True when collecting defaults, False otherwise
      self._flags &= ~self._fInSetDefaults

    # set to True when locked, False otherwise
      self._flags &= ~self._fIsLocked

    # set to True when inside __str__ (volatile - not stored on file)
      self._flags &= ~self._fIsPrinting

    # for later, in case __init__ itself is overridden
      self._flags |= self._fInitOk

    # for debugging purposes (temporary (?))
      self._flags &= ~self._fSetupOk

 # pickle support
   def __getstate__( self ):
      d = {}
      for name, proxy in self._properties.items():
         try:
            d[ name ] = proxy.__get__( self )
         except AttributeError:
            pass

      d[ '_Configurable__children' ] = self.__children
      d[ '_name' ] = self._name
      if hasattr(self, '_jobOptName'):
         d[ '_jobOptName' ] = self.getJobOptName()
      d[ '_fIsLocked' ] = self.isLocked()

      return d

   def __setstate__( self, dct ):
    # flags are set to neutral, not from pickle except for lockedness
      self._flags = 0
      self._flags |= self._fInitOk
      self._flags &= ~self._fInSetDefaults
      if dct[ '_fIsLocked' ]:
          self._flags != self._fIsLocked
      else:
          self._flags &= ~self._fIsLocked
      del dct['_fIsLocked']
      self._flags &= ~self._fIsPrinting
      for (n, v) in dct.items():
         setattr( self, n, v )

      return

   def __getnewargs__( self ):
      return (self._name,)

 # to allow a few basic sanity checks, as well as nice syntax
   def __len__( self ):
      return len( self.__children )

   def __iter__( self ):
      return iter( self.__children )

 # ownership rules of self through copying
   def __deepcopy__( self, memo ):
      newconf = object.__new__( self.__class__ )
      self.__class__.__init__( newconf, self.getName() )

      for proxy in self._properties.values():
         try:
            proxy.__set__( newconf, proxy.__get__( self ) )
         except AttributeError:
            pass                   # means property was not set for self

      for c in self.__children:
         newconf += c              # processes proper copy semantics

      return newconf

 # hierarchy building, and ownership rules of children
   def __iadd__( self, configs, descr = None, index = None ):
      if not type(configs) in (list,tuple):
         configs = ( configs, )

      joname = self.getJobOptName()

      for cfg in configs:
       # prevent type mismatches
         if not isinstance( cfg, Configurable ):
            raise TypeError( "'%s' is not a Configurable" % str(cfg) )

         cc = self.copyChildAndSetParent( cfg, joname )
         
       # filters dupes; usually "ok" (backdoor should catch them)
         ccjo = cc.getJobOptName()
         for c in self.__children:
            if c.getJobOptName() == ccjo:
               log.error( 'attempt to add a duplicate (%s.%s) ... dupe ignored' % (joname or self.name(),ccjo) )
               break
         else:
            if index is None:
               self.__children.append( cc )
            else:
               self.__children.insert( index, cc )

         try:
            if descr:         # support for tool properties
               descr.__set__( self, cc )
            else:
               setattr( self, cc.getName(), cc )
         except AttributeError:
            pass              # to allow free addition of tools/subalgorithms

      return self

   def __getattr__( self, attr ):  # until ToolProperties exist ...
      if attr[0] == "_":
         return super( Configurable, self ).__getattribute__(attr)
      for c in self.__children:
         if c.getName() == attr:
            return c

      raise AttributeError( "'%s' object has no attribute '%s'" % (self.__class__,attr) )

   def __delattr__( self, attr ):
    # remove as property, otherwise try as child
      try:
       # remove history etc., then reset to default (in case set before)
         prop = self._properties[ attr ]
         prop.__delete__( self )
         prop.__set__( self, prop.default )
         return               # reaches here? was property: done now
      except KeyError:
         pass

    # otherwise, remove child, if one is so named
      for c in self.__children:
         if c.getName() == attr:
            self.__children.remove( c )

    # potentially, there are left over caches (certain user derived classes)
      try:
         del self.__dict__[ attr ]
      except (AttributeError,KeyError):
         pass

   def __nonzero__(self):
      return True

   def remove( self, items ):
      if type(items) != list and type(items) != tuple:
         items = [ items ]

      self.__children = [ e for e in self.__children if e not in items ]

   def removeAll( self ):
      self.remove( self.__children )

 # to copy a full configuration
   def clone( self, newname ):
      if newname == self.getName():
         log.error( 'not cloning %s, because new name given is same as old name', self.getName() )
         return None

      return self.__class__( newname, **self.getValuedProperties() )

 # called by __iadd__; determines child copy semantics
   def copyChild( self, child ):
      return copy.deepcopy( child )

   def setParent( self, parentName ):
      pass

   def getParent( self ):
      return ""

   def hasParent( self, parent ):
      return False

   def copyChildAndSetParent(self,cfg,parent):
      cc = self.copyChild( cfg )

      if hasattr( cc, 'setParent' ) and parent:
         cc.setParent( parent )

      return cc

   def getChildren( self ):
      return self.__children[:]    # read only

   def getAllChildren( self ):
      """Get all (private) configurable children, both explicit ones (added with +=)
      and the ones in the private GaudiHandle properties"""
      childs = []
    # add private configurable properties (also inside handles)
      for proxy in self._properties.values():
         try:
            c = proxy.__get__( self )
         except AttributeError:
            pass
         else:
            if isinstance(c,Configurable) and not c.isPublic():
               childs.append(c)
            elif isinstance(c,GaudiHandles.GaudiHandle):
               try:
                  conf = c.configurable
               except AttributeError:
                  pass
               else:
                  if not conf.isPublic():
                     childs.append(conf)
            elif isinstance(c,GaudiHandles.GaudiHandleArray):
               # only setup private arrays
               if not c.isPublic():
                  for ci in c:
                     if isinstance(ci,Configurable):
                        childs.append(ci)
                     else:
                        try:
                           conf = ci.configurable
                        except AttributeError:
                           pass
                        else:
                           childs.append(conf)
                        
    # add explicit children
      childs += self.__children
      return childs

   def getSequence( self ):
      elems = []
      for c in self.__children:
         elems.append( c.getFullName() )
      return elems

   def setup( self ):
    # make sure base class init has been called
      if not hasattr(self,'_fInitOk') or not self._fInitOk:
       # could check more, but this is the only explanation
         raise TypeError("Configurable.__init__ not called in %s override" % self.__class__.__name__)

    # setup self: this collects all values on the python side
      self.__setupServices()
      self.__setupDlls()
      self.__setupDefaults()

    # setup children
      for c in self.getAllChildren():
         c.setup()

    # now get handle to work with for moving properties into the catalogue
      handle = self.getHandle()
      if not handle: 	 
         log.debug( 'no handle for %s: not transporting properties', self._name ) 	 
         return                    # allowed, done early 	 

    # pass final set of properties on to handle on the C++ side or JobOptSvc
      for name in self._properties.keys():
         if hasattr( self, name ): # means property has python-side value/default
            setattr( handle, name, getattr(self,name) )

    # for debugging purposes
      self._flags |= self._fSetupOk

   def lock( self ):
    # prevent any further changes, unless unlocked
      self._flags |= self._fIsLocked

    # same for any of its private ToolHandle properties
      for k, v in self.getProperties().items():
         if isinstance( v, Configurable ):
            v.lock()

    # leave children alone ... most likely are public (and can't check otherwise)

   def unlock( self ):
    # allow again changes to be made
      import sys, traceback
      stack = traceback.extract_stack( sys._getframe(1), 1 )
      log.warning( 'unlock() called on configurable "%s" in %s', self.getJobOptName(), stack[0][0] )
      self._flags &= ~self._fIsLocked

    # note that unlock() does not unlock the children; do that individually

   def isLocked( self ):
    # check whether this configurable is currently locked
      return self._flags & self._fIsLocked

   def isPrinting( self ):
    # check whether this configurable is currently being printed (see PropertyProxy.py)
      return self._flags & self._fIsPrinting

   def getProperties( self ):
      props = {}
      for name, proxy in self._properties.items():
         try:
            props[ name ] = proxy.__get__( self )
         except AttributeError:
            props[ name ] = Configurable.propertyNoValue

      return props

   def getValuedProperties( self ):
      props = {}
      for name, proxy in self._properties.items():
         try:
            value = proxy.__get__( self )
            if hasattr(value, 'getFullName') :
               value = value.getFullName()
            elif type(value) is list and len(value) > 0 and hasattr(value[0], 'getFullName'):
               value = [ i.getFullName() for i in value ]
            props[ name ] = value               
         except AttributeError:
            pass

      return props

   def properties( self ):
      return self.getProperties()           # compatibility

   @classmethod
   def getDefaultProperties( cls ):
      class collector:
         pass

    # user provided defaults
      c = collector()
      cls.setDefaults( c )

    # defaults from C++
      for k,v in cls._properties.items():
         if k not in c.__dict__ and hasattr( v, 'default' ):
            c.__dict__[ k ] = v.default

      return c.__dict__

   @classmethod
   def getDefaultProperty( cls, name ):
      class collector:
         pass

    # user provided defaults
      c = collector()
      cls.setDefaults( c )

      if name in c.__dict__:
         return c.__dict__[ name ]

    # defaults from C++
      try:
         v = cls._properties[name]
         if hasattr( v, 'default' ):
            return v.default
      except KeyError:
         pass

      return None

   def getType( cls ):
      return cls.__name__

   def getName( self ):
      return self._name

   def name( self ):
      return self.getName()

   def getJobOptName( self ):               # full hierachical name
      return self.getName()

   def isPublic( self ):
      return True

 # for a couple of existing uses out there
   def getFullName( self ) :
      return str( self.getType() + '/' + self.getName() )

   def getFullJobOptName( self ):
      return "%s/%s" % (self.getType(),self.getJobOptName() or self.getName())

   def getPrintTitle(self):
      return self.getGaudiType() + ' ' + self.getTitleName()

   def getTitleName( self ):
      if log.isEnabledFor( logging.DEBUG ):
         return self.getFullJobOptName()
      else:
         return self.getFullName()

   def setDefaults( cls, handle ):
      pass

   def _isInSetDefaults( self ):
      return self._flags & self._fInSetDefaults

   def __setupServices( self ):
      #svcs = self.getServices()
      #if not svcs:
      svcs = []
      #elif type(svcs) == types.StringType:
      #   svcs = [ svcs ]

      import OldStyleConfig
      for svc in svcs:
         handle = OldStyleConfig.Service( svc )  # noqa: F841
       # services should be configurables as well, but aren't for now
       # handle.setup()

       # allow Configurable to make some changes
         if hasattr( self, 'configure' + svc ):
            eval( 'self.configure' + svc + '( handle )' )

   def __setupDlls( self ):
      dlls = self.getDlls()
      if not dlls:
         dlls = []
      elif isinstance(dlls, types.StringType):
         dlls = [ dlls ]

      from AppMgr import theApp
      dlls = filter( lambda d: d not in theApp.Dlls, dlls )
      if dlls: theApp.Dlls += dlls

   def __setupDefaults( self ):
    # set handle defaults flags to inform __setattr__ that it is being
    # called during setDefaults of the concrete Configurable
      self._flags |= self._fInSetDefaults
      self.setDefaults( self )
      self._flags &= ~self._fInSetDefaults

   @staticmethod
   def _printHeader( indentStr, title ):
      preLen  = Configurable.printHeaderPre
      postLen = Configurable.printHeaderWidth - preLen - 3 - len(title)# - len(indentStr)
      postLen = max(preLen,postLen)
      return indentStr + '/%s %s %s' % (preLen*'*',title,postLen*'*')

   @staticmethod
   def _printFooter( indentStr, title ):
      preLen  = Configurable.printHeaderPre
      postLen = Configurable.printHeaderWidth - preLen - 12 - len(title)# - len(indentStr)
      postLen = max(preLen,postLen)
      return indentStr + '\\%s (End of %s) %s' % (preLen*'-',title,postLen*'-')

   def __repr__( self ):
      return '<%s at %s>' % (self.getFullJobOptName(),hex(id(self)))

   def __str__( self, indent = 0, headerLastIndentUnit=indentUnit ):
      global log  # to print some info depending on output level

      indentStr = indent*Configurable.indentUnit
    # print header
      title = self.getPrintTitle()

    # if run under WARNING and hight only, stick with title
      if logging.WARNING <= log.level:
         if not Configurable._printOnce:
            Configurable._printOnce = 1
            return "Reduced Configurable printout; change log level for more details, e.g.:\n   import AthenaCommon.Configurable as Configurable\n   Configurable.log.setLevel( INFO )\n" + title
         else:
            return title

    # avoid auto-setting private AlgTools while doing printout
      self._flags |= self._fIsPrinting

    # print line to easily see start-of-configurable
      if indent > 0:
         headerIndent = (indent-1)*Configurable.indentUnit + headerLastIndentUnit
      else:
         headerIndent = ''
      rep = Configurable._printHeader( headerIndent, title )
      rep += os.linesep
    # print own properties
      props = self.getProperties()
      defs = self.getDefaultProperties()
      if not props:
         rep += indentStr + '|-<no properties>' + os.linesep
      else:
       # get property name with
         nameWidth = 0
         for p in props.keys():
            nameWidth=max(nameWidth,len(p))
         propNames = sorted(props.keys())
         for p in propNames:
            v = props[p]
          # start with indent and property name
            prefix = indentStr + '|-%-*s' % (nameWidth,p)
          # add memory address for debugging (not for defaults)
            if log.isEnabledFor( logging.DEBUG ):
               if v != Configurable.propertyNoValue:
                  address = ' @%11s' % hex(id(v))
               else:
                  address = 13*' '
               prefix += address
          # add value and default
            default = defs.get(p)
            if v == Configurable.propertyNoValue:
               # show default value as value, and no extra 'default'
               strVal = repr(default)
               strDef = None
            else:
               # convert configurable to handle
               if isinstance(v,Configurable):
                  vv = v.getGaudiHandle()
               else:
                  vv = v
               if isinstance(vv,(GaudiHandles.GaudiHandle,GaudiHandles.GaudiHandleArray)):
                  strVal = repr(vv)
                  strDef = repr(default.toStringProperty())
                  if strDef == repr(vv.toStringProperty()):
                     strDef = None
               else:
                  strVal = repr(vv)
                  strDef = repr(default)
          # add the value
            line = prefix + ' = ' + strVal
          # add default if present
            if strDef is not None:
               # put default on new line if too big
               if len(line) + len(strDef) > Configurable.printHeaderWidth:
                  line += os.linesep + indentStr + '| ' + (len(prefix)-len(indentStr)-3)*' '
               line += '  (default: %s)' % (strDef,)
          # add the line to the total string
            rep += line + os.linesep

    # print configurables + their properties, or loop over sequence
      childNotToSort = []
      childToSort    = []
      for c in self.getAllChildren():
         if isinstance(c, ConfigurableAlgorithm): childNotToSort.append(c)
         else:                                    childToSort.append(c)
    # sort the non-algs
      childToSort.sort(key=lambda x : x.getName())
      for cfg in childToSort + childNotToSort:
         rep += cfg.__str__( indent + 1, '|=' ) + os.linesep

    # print line to easily see end-of-configurable. Note: No linesep!
      rep += Configurable._printFooter( indentStr, title )

      self._flags &= ~self._fIsPrinting
      return rep

    # hash method for set/dict operations
    # first attempt, assuming need to recurse into child properties
    # if too much overhead, could attempt to cache with python
    # properties, but hard to propagate changes upwards to parents

   def getFlattenedProperties(self):
      self._flags |= self._fIsPrinting
      properties = self.getValuedProperties()
      propstr = ""
      for key,val in sorted(properties.iteritems()):
         if isinstance(val,GaudiHandles.PublicToolHandle) or isinstance(val,GaudiHandles.PrivateToolHandle):
            propstr += val.getFullName()
         elif isinstance(val,Configurable):
            propstr += "({0}:{1})".format(key,val.getFlattenedProperties())
         elif isinstance(val,GaudiHandles.PublicToolHandleArray) or isinstance(val,GaudiHandles.PrivateToolHandleArray):
            for th in val:
               # Handle ToolHandles that have just been set as strings(?)
               if isinstance(th,Configurable):
                  propstr += "({0}:{1}".format(th.getFullName(), th.getFlattenedProperties())
               else:
                  propstr += th.getFullName()
         else:
            propstr += "({0}:{1})".format(key,str(val))
      self._flags &= ~self._fIsPrinting
      return propstr

   def getStrDescriptor(self):

      descr = ""
      if hasattr( self,"_name" ):
         propstr = self.getFlattenedProperties()
         descr = (self.getFullName(), propstr)
      else: # Not yet initialised?
         descr = self.getType()
      
      return descr

   #  # (in)equality operators, based on hash
   def __eq__(self,rhs):
      # Check identity first
      if self is rhs: return True
      # Avoid comparing against None...
      if not rhs: return False
      # Class check
      if not isinstance(rhs,Configurable): return False
      # Type/Name check
      if self.getFullName() != rhs.getFullName(): return False
      # If identical types and names, then go the whole hog and test children
      # Could be sped up by testing property by property...
      return self.getStrDescriptor() == rhs.getStrDescriptor()
   def __ne__(self,rhs):
      return (not self.__eq__(rhs))

### base classes for individual Gaudi algorithms/services/algtools ===========

### Configurable base class for Algorithms -----------------------------------
class ConfigurableAlgorithm( Configurable ):
   __slots__ = { '_jobOptName' : 0, 'OutputLevel' : 0, \
      'Enable' : 1, 'ErrorMax' : 1, 'ErrorCounter' : 0, 'AuditAlgorithms' : 0, \
      'AuditInitialize' : 0, 'AuditReinitialize' : 0, 'AuditExecute' : 0, \
      'AuditFinalize' : 0, 'AuditBeginRun' : 0, 'AuditEndRun' : 0 }

   def __init__( self, name = Configurable.DefaultName ):
      super( ConfigurableAlgorithm, self ).__init__( name )
      name = self.getName()
      self._jobOptName = name[ name.find('/')+1 : ]   # strips class

   def __deepcopy__( self, memo ):
      return self                 # algorithms are always shared

   def getHandle( self ):
      try:
         from GaudiPython.Bindings import iAlgorithm
      except ImportError:
         from gaudimodule import iAlgorithm
      return iAlgorithm( self.getJobOptName() )

   def getGaudiType( self ):
      return 'Algorithm'

   def getJobOptName( self ):
      return self._jobOptName


### Configurable base class for Services -------------------------------------
class ConfigurableService( Configurable ):
   __slots__ = { 'OutputLevel' : 0, \
      'AuditServices' : 0, 'AuditInitialize' : 0, 'AuditFinalize' : 0 }

   def __deepcopy__( self, memo ):
      return self                 # services are always shared

   def copyChild( self, child ):
      return child                # full sharing

   def getHandle( self ):
      try:
         from GaudiPython.Bindings import iService
      except ImportError:
         from gaudimodule import iService
      return iService( self._name )

   def getGaudiType( self ):
      return 'Service'

   def getGaudiHandle( self ):
      return GaudiHandles.ServiceHandle( self.toStringProperty() )

   def toStringProperty( self ):
    # called on conversion to a string property for the jocat
      return self.getName()


### Configurable base class for AlgTools -------------------------------------
class ConfigurableAlgTool( Configurable ):
   __slots__ = { '_jobOptName' : '', 'OutputLevel' : 0, \
      'AuditTools' : 0, 'AuditInitialize' : 0, 'AuditFinalize' : 0 }

   def __init__( self, name = Configurable.DefaultName ):
      super( ConfigurableAlgTool, self ).__init__( name )
      name = self.getName()
      name = name[ name.find('/')+1 : ]   # strips class, if any
      self._jobOptName = name

   def getHandle( self ):
    # iAlgTool isn't useful, unless one knows for sure that the tool exists
      try:
         from GaudiPython.Bindings import iProperty
      except ImportError:
         from gaudimodule import iProperty
      return iProperty( self.getJobOptName() )

   def getGaudiType( self ):
      return 'AlgTool'
         
   def getGaudiHandle( self ):
      if self.isPublic():
         return GaudiHandles.PublicToolHandle( self.toStringProperty() )
      else:
         return GaudiHandles.PrivateToolHandle( self.toStringProperty() )
      
   def getPrintTitle(self):
      if self.isPublic():
         pop = 'Public '
      else:
         pop = 'Private '
      return pop + Configurable.getPrintTitle(self)

   def setParent( self, parentName ):
#      print "ConfigurableAlgTool.setParent(%s@%x,%r)" % (self.getName(),id(self),parentName)
#      print "Calling stack:"
#      import traceback
#      traceback.print_stack()
    # propagate parent to AlgTools in children
      for c in self.getAllChildren():
         if isinstance(c,ConfigurableAlgTool): c.setParent( parentName )

    # update my own parent
      self._jobOptName = parentName + '.' + self._jobOptName


   def getParent( self ):
      dot = self._jobOptName.rfind('.')
      if dot != -1:
         return self._jobOptName[:dot]
      else:
         return ""

   def getDirectParent( self ):
      parent = self.getParent()
      dot = parent.rfind('.')
      if dot != -1:
         return parent[dot+1:]
      else:
         return parent

   def hasParent( self, parent ):
      return self._jobOptName.startswith( parent + '.' )

   def getJobOptName( self ):
      return self._jobOptName

   def isPublic( self ):
      return self.isInToolSvc()

   def isInToolSvc( self ):
      return self.getDirectParent() == 'ToolSvc'

   def toStringProperty( self ):
    # called on conversion to a string property for the jocat
      return self.getType() + '/' + self.getName()


### Configurable base class for Auditors -------------------------------------
class ConfigurableAuditor( Configurable ):
   __slots__ = { '_jobOptName' : 0, 'OutputLevel' : 0, \
                 'Enable' : 1 }

   def __init__( self, name = Configurable.DefaultName ):
      super( ConfigurableAuditor, self ).__init__( name )
      name = self.getName()
      name = name[ name.find('/')+1 : ]   # strips class, if any
      self._jobOptName = name

   def getHandle( self ):
    # Auditor handles don't exist ... use iProperty
      try:
         from GaudiPython.Bindings import iProperty
      except ImportError:
         from gaudimodule import iProperty
      return iProperty( self.getJobOptName() )

   def getGaudiType( self ):
      return 'Auditor'
         
   def getJobOptName( self ):
      return self._jobOptName

   def toStringProperty( self ):
    # called on conversion to a string property for the jocat
      return self.getType() + '/' + self.getName()

   pass # class ConfigurableAuditor


### Configurable base class for PyAlgorithms ---------------------------------
class ConfigurablePyAlgorithm( ConfigurableAlgorithm ):
   def __init__( self, name, **kwargs ):
      super( ConfigurableAlgorithm, self ).__init__( name )
      self._jobOptName = name

      for n, v in kwargs.items():
         setattr(self, n, v)

   def getDlls( self ):
      return None

   def getHandle( self ):
      return None


### Configurable that is disjoint from any Gaudi component -------------------
class ConfigurableUser( Configurable ):
   __slots__ = {}

   def __init__( self, name = Configurable.DefaultName, **kwargs ):
      super( ConfigurableUser, self ).__init__( name )
      for n, v in kwargs.items():
         setattr(self, n, v)

   def getGaudiType( self ):
      return 'User'

   def getDlls( self ):
      return None

   def getHandle( self ):
      return None
