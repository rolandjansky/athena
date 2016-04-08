# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/PropertyProxy.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)
# Author: Martin Woudstra (Martin.Woudstra@cern.ch)

import os, sys, weakref, copy
try:
   from GaudiKernel.GaudiHandles import *
except ImportError:
   from GaudiPython.GaudiHandles import *

# dictionary with configurable class : python module entries
import ConfigurableDb


### data ---------------------------------------------------------------------
__version__ = '2.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov), Martin Woudstra (Martin.Woudstra@cern.ch)'

__all__ = [ 'PropertyProxy',
            'GaudiHandlePropertyProxy',
            'GaudiHandleArrayPropertyProxy' ]

## for messaging
from Logging import logging
log = logging.getLogger( 'PropertyProxy' ) 


def derives_from( derived, base ):
   """A string version of isinstance().
   <derived> is either an object instance, or a type
   <base>    is a string containing the name of the base class (or <derived> class)"""
   if not isinstance( derived, type ):
      derived = type(derived)

   if derived.__name__ == base:
      return True

   for b in derived.__bases__:
      if derives_from( b,base ):
         return True

   return False
   

def _isCompatible( tp, value ):
 # Note: this function is only called for builtin types. Lists of
 # configurables (most likely GaudiHandleArrays) don't get here, since
 # their PropertyProxy types are special cases (see below). Thus, a
 # compatibility check that relies on conversion (which will always fail
 # for configurables) is acceptable.

   if ( tp == str or type(value) == str ) and not isinstance( value, tp ):
    # special case, insist on exact match for str (no conversions allowed)
      raise ValueError( "received an instance of %s, but %s expected" % (type(value),tp) )
   elif ( tp == int or tp == long ) and type(value) == float:
    # special case, insist on strict match for integer types
      raise ValueError( "received an instance of %s, but %s expected" % (type(value),tp) )
   else:
    # all other types: accept if conversion allowed
      try:
         tp( value )
      except (TypeError,ValueError):
         if isinstance( value, tp ):
          # this is a conversion mismatch, but an "ok" type (i.e. the type can
          # not be copied/constructed from itself), which is a typical case
          # for configurables being added to an ordinary list (as opposed to
          # a GaudiHandleArray), where the actual type of the list is unknown
            raise ValueError( "received non-convertible type %s, but builtin type expected" % type(value) )
         else:
          # this is a complete miss and the error message will be clear
            raise ValueError( "received an instance of %s, but %s expected" % (type(value),tp) )

   return value         # in case of e.g. classes with __int__, __iter__, etc. implemented


class PropertyProxy( object ):
   def __init__( self, descr, docString=None, default=None ):
      self.history = weakref.WeakKeyDictionary()
      self.descr   = descr
      if docString:
         self.__doc__ = docString
      if default is not None:
         self.default = default

   def setDefault( self, value ):
      self.__default = value

   def getDefault( self ):
      return self.__default

   default = property( getDefault, setDefault )

   def fullPropertyName( self, obj ):
      return (obj.getJobOptName() or obj.getName()) + '.' + self.descr.__name__

   def __get__( self, obj, type = None ):
      value = None
      try:
         value = self.descr.__get__( obj, type )
      except AttributeError:
       # special case for list: allow default to work with on +=;
       # unfortunately, that means that direct access also succeeds, even
       # as the property wasn't set yet ... (TODO: ideas??)
         if self.__default.__class__ == list:
            self.descr.__set__( obj, list(self.__default) )
            value = self.descr.__get__( obj, type )     # no history
         else:
            raise

      if value is None:
         value = self.descr.__get__( obj, type )

    # locked configurables return copies to prevent modifications
      if obj.isLocked():
         import __builtin__
         if __builtin__.type( value ) is dict:
            from ctypes import pythonapi, py_object
            from _ctypes import PyObj_FromPtr
            PyDictProxy_New = pythonapi.PyDictProxy_New
            PyDictProxy_New.argtypes = (py_object,)
            PyDictProxy_New.rettype = py_object
            value = PyObj_FromPtr( PyDictProxy_New( value ) )
         elif __builtin__.type( value ) is list:
            value = tuple( value )      # point being that tuple is read-only
         else:
            import copy
            value = copy.copy( value )

      return value

   def __set__( self, obj, value ):
    # locked configurables can no longer be changed
      if obj.isLocked():
         raise RuntimeError(
            'can not change property "%s" of locked configurable "%s"' %
            (self.descr.__name__, obj.getJobOptName()) )

    # check value/property compatibility if possible
      proptype = None
      if hasattr( self, 'default' ):
         proptype = type(self.default)
      elif obj in self.history:
         proptype = type( self.history[ obj ][ 0 ] )

    # check if type known; allow special initializer for typed instances
      if proptype and proptype != type(None):
        # check value itself
          value = _isCompatible( proptype, value )

        # check elements in case of list
          if proptype == tuple or proptype == list:
             try:
                oldvec = self.descr.__get__( obj, type )
                if oldvec:
                   tpo = type(oldvec[0])
                   for v in value:
                      _isCompatible( tpo, v )
             except AttributeError:
              # value not yet set
                pass

    # allow a property to be set if we're in non-default mode, or if it
    # simply hasn't been set before
      if not obj._isInSetDefaults() or not obj in self.history:
       # by convention, 'None' for default is used to designate objects setting
         if hasattr( self, 'default' ) and self.default == None:
            obj.__iadd__( value, self.descr )     # to establish hierarchy
         else:
            self.descr.__set__( obj, value )
         self.history.setdefault( obj, [] ).append( value )

   def __delete__( self, obj ):
      if obj in self.history:
         del self.history[ obj ]
      self.descr.__delete__( obj )



class GaudiHandlePropertyProxyBase(PropertyProxy):
   """A class with some utilities for GaudiHandles and GaudiHandleArrays"""

   def __init__(self, descr, docString, default, handleType, allowedType ):
      """<descr>: the real property in the object instance (from __slots__)
      <docString>: the documentation string of this property
      <default>: default value from C++ (via python generated by genconf)
      <handleType>: real python handle type (e.g. PublicToolHandle, PrivateToolHandle, ...)
      <allowedType>: allowed instance type for default
      """
      # check that default is of allowed type for this proxy
      if not isinstance(default,allowedType):
         raise TypeError( "%s: %s default: %r is not a %s" % \
                          ( descr.__name__, self.__class__.__name__, default, allowedType.__name__ ) )
      PropertyProxy.__init__( self, descr, docString, default )
      self._handleType = handleType
      self._confTypeName = 'Configurable' + handleType.componentType
#      print "%s: %r (%s)" % (self.__class__.__name__,self._handleType,self._confTypeName)


   def __get__( self, obj, type = None ):
      try:
         return self.descr.__get__( obj, type )
      except AttributeError:
         # Get default
         try:
            default = obj.__class__.getDefaultProperty( self.descr.__name__ )
            # if printing, just show the toolhandle. No need to auto-retrieve default configurable instance
            if obj.isPrinting(): return default
            default = self.convertDefaultToBeSet( obj, default )
            # special case if default is auto-retrieved configurable instance
            if self.isConfig(default):
               if obj.isLocked():
                  # return a locked copy to produce error in case of setting any property
                  # rather then changing the value of the default
                  default = copy.deepcopy(default)
                  default.lock()
                  return default
               else:
                  # Set to default configurable instance to allow MyTool.MySubTool.MyProperty = MyValue
                  self.__set__( obj, default )
            elif isinstance(default,GaudiHandleArray):
               if obj.isLocked():
                  # return a locked copy to produce error in case of setting any property
                  # rather then changing the value of the default
                  default = copy.deepcopy(default)
                  for c in default:
                     if self.isConfig(c):
                        c.lock()
                  return default
               else:
                  # Set array to allow to add to default with syntax: MyTool.MyArray.append()
                  self.__set__( obj, default )
            else:
               return default
         except AttributeError,e:
            import traceback
            traceback.print_exc()
            # change type of exception to avoid false error message
            raise RuntimeError("AttributeError(%s)" % e.args)

      return self.descr.__get__( obj, type )


   def __set__( self, obj, value ):
    # locked configurables can no longer be changed
      if obj.isLocked():
         raise RuntimeError(
            'can not change property "%s" of locked configurable "%s"' %
            (self.descr.__name__, obj.getJobOptName()) )

    # allow a property to be set if we're in non-default mode, or if it
    # simply hasn't been set before
      if not obj._isInSetDefaults() or not obj in self.history:
         value = self.convertValueToBeSet( obj, value )
         # assign the value
         self.descr.__set__( obj, value )
         log.verbose( "Setting %s = %r", self.fullPropertyName( obj ), value )
         self.history.setdefault( obj, [] ).append( value )


   def isHandle(self,value):
      """Check if <value> is a handle of the correct type"""
      return isinstance(value,self._handleType)


   def isConfig(self,value):
      """Check if <value> is a configurable of the correct type"""
      return derives_from(value,self._confTypeName)


   def getDefaultConfigurable(self,typeAndName,requester):
      """Return the configurable instance corresponding to the toolhandle if possible.
      Otherwise return None"""
      global log
      # find the module
      typeAndNameTuple = typeAndName.split('/')
      confType = typeAndNameTuple[0]
      confClass = ConfigurableDb.getConfigurable(confType)
      # check the type of the configurable
      if not derives_from(confClass,self._confTypeName):
         log.error( "%s: Configurable %s is not a %s",
                    requester, confType, self._confTypeName )
         return None
      try:
         confName = typeAndNameTuple[1]
      except IndexError:
         return confClass() # use default name
      else:
         return confClass(confName)


   def convertDefaultToBeSet( self, obj, default ):
      # turn string into handle
      isString = type(default) == str
      if not isString and self.isConfig(default):
#         print self.fullPropertyName(obj) + ": Setting default configurable: %r" % default
         return default
      elif isString or self.isHandle(default):
         if isString:
            # convert string into handle
            typeAndName = default
            default = self._handleType( typeAndName )
         else:
            typeAndName = default.typeAndName
         if not self._handleType.isPublic and typeAndName:
            # Find corresponding default configurable of private handles
            try:
               conf = self.getDefaultConfigurable(typeAndName, self.fullPropertyName(obj))
#               print self.fullPropertyName(obj) + ": Setting default private configurable (from default handle): %r" % conf 
            except AttributeError,e:
               # change type of exception to avoid false error message
               raise RuntimeError(*e.args)
            if conf is None:
               raise RuntimeError( "%s: Default configurable for class %s not found in ConfigurableDb.CfgDb" % \
                                   (self.fullPropertyName(obj),default.getType() ) )
            return conf
      else: # not a config, not a handle, not a string
         raise TypeError( "%s: default value %r is not of type %s or %s" % \
                          (self.fullPropertyName(obj),default,self._confTypeName,self._handleType.__name__) )

      return default

   def convertValueToBeSet( self, obj, value ):
      if value is None: value = ''
      isString = type(value) == str
      if isString:
       # create an new handle
         return self._handleType(value)
      elif self.isHandle(value):
       # make a copy of the handle
         return self._handleType(value.toStringProperty())
      elif self.isConfig(value):
         if self._handleType.isPublic:
          # A public tool must be registered to ToolSvc before assigning it
            if derives_from(value,'ConfigurableAlgTool'):
               if not value.isInToolSvc():
                  suggestion = 'You may need to add jobOptions lines something like:' + os.linesep + \
                               'from AthenaCommon.AppMgr import ToolSvc' + os.linesep + \
                               'ToolSvc += '
                  if value.getName() == value.getType(): # using default name
                     suggestion += '%s()' % value.__class__.__name__
                  else: # using user-defined name
                     suggestion += '%s(%r)' % (value.__class__.__name__,value.getName())
                  raise RuntimeError( self.fullPropertyName(obj) +
                                      ': Public tool %s is not yet in ToolSvc. %s' %
                                      (value.getJobOptName(),suggestion) )
          # make it read-only
            return self._handleType(value.toStringProperty())
         elif value.hasParent( obj.getJobOptName() ):
          # is already a child, keep as-is
            return value
         else:
          # make a copy of the configurable
            return obj.copyChildAndSetParent( value, obj.getJobOptName() )
      else:
         raise TypeError( "Property %s value %r is not a %s nor a %s nor a string" % \
                          (self.fullPropertyName(obj),value,self._confTypeName,self._handleType.__name__) )

      return value


class GaudiHandlePropertyProxy(GaudiHandlePropertyProxyBase):
   def __init__( self, descr, docString, default ):
      GaudiHandlePropertyProxyBase.__init__( self, descr, docString, default, type(default), GaudiHandle )


class GaudiHandleArrayPropertyProxy(GaudiHandlePropertyProxyBase):
   def __init__( self, descr, docString, default ):
      """<descr>: the real property in the object instance (from __slots__)
      <confTypeName>: string indicating the (base) class of allowed Configurables to be assigned.
      <handleType>: real python handle type (e.g. PublicToolHandle, PrivateToolHandle, ...)
      """
      GaudiHandlePropertyProxyBase.__init__( self, descr, docString, default, type(default).handleType, GaudiHandleArray )
      self.arrayType = type(default)

      
   def checkType( self, obj, value ):
      if not isinstance( value, list ) and not isinstance( value, self.arrayType ):
         raise TypeError( "%s: Value %r is not a list nor a %s" % \
                          ( self.fullPropertyName(obj), value, self.arrayType.__name__ ) )
      

   def convertDefaultToBeSet( self, obj, default ):
      self.checkType( obj, default )
      newDefault = self.arrayType()
      for d in default:
         cd = GaudiHandlePropertyProxyBase.convertDefaultToBeSet( self, obj, d )
         if cd: newDefault.append( cd )

      return newDefault


   def convertValueToBeSet( self, obj, value ):
      self.checkType( obj, value )
      newValue = self.arrayType()
      for v in value:
         cv = GaudiHandlePropertyProxyBase.convertValueToBeSet( self, obj, v )
         if cv: newValue.append( cv )

      return newValue



def PropertyProxyFactory( descr, doc, default ):
#   print "PropertyProxyFactory( %s, %r )" % (descr.__name__,default)
   if isinstance(default,GaudiHandleArray):
      return GaudiHandleArrayPropertyProxy( descr, doc, default )

   if isinstance(default,GaudiHandle):
      return GaudiHandlePropertyProxy( descr, doc, default )

   return PropertyProxy( descr, doc, default )
