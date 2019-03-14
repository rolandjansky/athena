# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CVSutil
CVSkeywords = CVSutil.CVSkeywords( ["$Id: TransformConfig.py,v 1.21 2009-02-13 21:15:41 ctan Exp $" ,
                                    "$Name: not supported by cvs2svn $" ,
                                    "$Revision: 285339 $"] )

__version__ = CVSkeywords["Revision"]
__author__ = "clat@hep.ph.bham.ac.uk"

import os,sys
from PyJobTransformsCore.trferr import TransformConfigError


class Descriptor(object):
    """Base class for all fixed types, implemented as descriptors. Each instance of a specific fixed type
    (a derived class) can be set either to None (to indicate no value has been set), or to a type specified
    by the derived class. The type is checked by the derived class by implementing the member function
    _checkType()"""

    def __init__(self,doc,default,allowedValues=None):
        self.__name = '\"%s\"' % doc  # Temporary name overwritten by JobConfigMetaClass
        self.__doc = doc
        self.__allowed = []
        if allowedValues is not None:
            try:
                allowedValues.__iter__
            except AttributeError:
                allowedValues = [ allowedValues ]
            varName = "%s.allowedValues" % self.__name
            self.__allowed = self._checkAllowedValues( varName, allowedValues )
        # allow None as a value for all types
        if default is not None:
            # check type and value of default
            varName = "%s default" % self.__name
            default = self._checkType( varName, default )
            default = self._checkValue( varName, default )
        self.__default = default

    def __get__(self,instance,owner):
        if instance is None:
            return self
        return instance._attributeDictionary().get(self._attributeName(instance),self.__default)


    def __set__(self,instance,value):
        varName = "%s.%s" % (instance.name(), self.__name) # for error/debug printout
        # allow None as a value for all types
        if value is not None:
            # do value and/or type checking and possibly change type and/or value
            value = self._checkType(varName,value)
            value = self._checkValue(varName,value)
        # Do possible additional actions
        self._setValue(varName,value)
        instance._attributeDictionary()[self._attributeName(instance)] = value  # store the value


    def _attributeName(self,instance):
        return '_%s__%s' % (instance.__class__.__name__,self.__name)


    def name(self):
        return self.__name


    def default(self):
        return self.__default


    def doc(self):
        """The documentation string"""
        return self.__doc
    

    def help(self):
        """The help string: type, documentation and possible values"""
        hlp = '(%s) %s' % (self.__class__.__name__, self.__doc)
        if self.__allowed:
            hlp += '. Possible values: %s' % self.__allowed
            
        return hlp


    def allowedValues(self):
        return self.__allowed


    def _checkType(self,variableName,value):
        """Private helper functin to check the type of <value>. May convert to another type.
        This implementation does nothing, it just returns value.
        This function can be overridden in derived class to do type checking.
        It should return the value (possible with new type), or raise a TransformConfigError
        exception in case of problems. """
        return value


    def _checkValue(self,variableName,value):
        """Private helper function to check the value of <value>. This function is
        called after calling _checkType. <value> can therefore be considered to be
        of the correct type.
        This implementation checks that the value is one of the allowed values (if defined).
        This function can be overridden in derived class to do type & additional value checking.
        It has to return the value (adapted if needed) if all is OK. It has to raise
        a TransformConfigError exception in case of problems.
        <variableName> is the name of the variable that is being set and is typically
        only used for error messages."""
        if self.__allowed and value not in self.__allowed:
            raise TransformConfigError( '%s value %r is not in %s' % \
                                        (variableName, value, self.__allowed) )

        return value


    def _setValue(self,variableName,value):
        """Private helper function which is called when the value of the object is set.
        It is called after _checkType() and _checkValue(), so the value can be
        assumed to be correct.
        This function can be overridden in a derived class, typically to trigger additional action
        when the value is set.
        In case of error, raise a TransformConfigError exception, otherwise just return.
        This implementation does nothing.
        <variableName> is the name of the variable that is being set and is typically
        only used for error messages."""
        pass


    def _checkAllowedValues(self,variableName,allowedValues):
        # Check all allowed values
        for v in allowedValues:
            self._checkType(variableName,v)
        return allowedValues

        

class String(Descriptor):
    def __init__(self,doc,default='',allowedValues=None):
        Descriptor.__init__(self,doc,default,allowedValues)


    def _checkValue(self,variableName,value):
        valType = type(value).__name__
        if valType != 'str':
            raise TransformConfigError( '%s should be a string. Got %s instead.' % (variableName, valType) )

        # check the value against list of possible values
        return Descriptor._checkValue(self,variableName,value)
         


class Boolean(Descriptor):
    def __init__(self,doc,default):
        Descriptor.__init__(self,doc,default,[True,False])



class Float(Descriptor):
    def __init__(self,doc,default,allowedValues=None):
        Descriptor.__init__(self,doc,default,allowedValues)


    def _checkValue(self,variableName,value):
        try:
            value = float(value)
        except:
            raise TransformConfigError( '%s value %s is not a float' % (variableName, value) )

        # check the value against list of possible values
        return Descriptor._checkValue(self,variableName,value)



class Integer(Descriptor):
    def __init__(self,doc,default,allowedValues=None):
        Descriptor.__init__(self,doc,default,allowedValues)


    def _checkValue(self,variableName,value):
        inValue = value
        try:
            value = int(value)
        except:
            raise TransformConfigError( '%s value %s is not an int' % (variableName, value) )
    
        if value != float(inValue):
            raise TransformConfigError( '%s value %s is not an int' % (variableName, value) )

        # check the value against list of possible values
        return Descriptor._checkValue(self,variableName,value)


        
class UniqueList(Descriptor):
    """List with a unique set of entries (duplicates are removed).
    List of allowed values are the entries that are allowed in the list."""
    def __init__(self,doc,default=None,allowedValues=None):
        if default is None: default = [] # trick needed to avoid sharing among instances
        Descriptor.__init__(self,doc,default,allowedValues)


    def _checkType(self,variableName,value):
        """Check that <value> is of type list of tuple, and convert to a list if it is a tuple."""
#        # check types
#        valType = type(value).__name__
#        if valType != 'list' and valType != 'tuple':
#            raise TransformConfigError( '%s should be a list or tuple. Got %s instead.' % \
#                                        (variableName, valType) )
#        if valType == 'tuple':
#            # convert to a list
#            value = list(value)
#        return value
        try:
            value.__iter__
            return list( value )
        except:
            raise TransformConfigError( '%s should be a list or tuple. Got %s instead.' % \
                                        ( variableName, type( value ).__name__ ) )


    def _checkValue(self,variableName,value):
        # check that entries are allowed
        allowed = self.allowedValues()
        if allowed:
            for v in value:
                if v not in allowed:
                    raise TransformConfigError( '%s value %r is not one of %s' % \
                                                (variableName, value, allowed) )
        # make entries unique
        from collections import OrderedDict
        return list(OrderedDict.fromkeys(value))


    def _checkAllowedValues(self,variableName,allowedValues):
        # Convert values to list before calling _checkType, and back to entry
        newAllowed = set() # []
        for v in allowedValues:
            newAllowed.add( self._checkType(variableName,[v])[0] )
        return list( newAllowed )
   


class ListOfStrings(UniqueList):
    """List with a unique set of strings (duplicates are removed)"""
    def __init__(self,doc,default=None,allowedValues=None):
        UniqueList.__init__(self,doc,default,allowedValues)


    def _checkType(self,variableName,value):
        """Add check that all entries are strings"""
        # check that <value> is a list or tuple
        value = UniqueList._checkType(self,variableName,value)
        # check that all entries are strings
        for v in value:
            if not isinstance( v, str ):
                raise TransformConfigError("Entry %r in %s is not a string (but an %s)" % \
                                           ( v, variableName, type( v ).__name__ ) )                                           
        return value
   


class AllowedExpression:
    def __init__(self,expression):
        """Any python (string) boolean expression where 'value' stands for the value.
        For example, for a number range check: \"0 < value < 100\" """
        self.expression = expression


    def __contains__(self,value):
        return eval(self.expression)


    def __str__(self):
        return self.expression


    def __iter__(self):
        return self
    

    def next(self):
        """No iteration"""
        raise StopIteration
            



class JobConfigMetaClass(type):
    def __init__( self, name, bases, dict ):
##        print "JobConfigMetaClass( self=%r, name=%r, bases=%r, dict=%r )" % (self,name,bases,dict)
        if '__slots__' not in dict:
            raise TransformConfigError('Class %s does not have member __slots__. Please add __slots__ = ()'
                                 ' to the class definition' % (name,) )
        #
        # add list of properties
        #
        # first add it to __slots__
        slots = self.__slots__
##        print "%s.__slots__ before: = %r" % (self.__name__,slots)
        descrName = '__properties'
        if descrName not in slots:
            # add variable
            slots += ( descrName, )
            # synchronise dict
            dict['__slots__'] = slots
##            print "%s.__slots__ after: = %r" % (self.__name__,slots)
        # then add the list itself
        setattr(self,descrName,[])
        descrList = getattr(self,descrName)
        # set names of properties and add them to the list of properties
        for n,attrib in dict.items():
#            attrib = getattr(self,n)
            if isinstance(attrib,Descriptor):
##                print "Setting name of %s.%s" % (self.__name__,n)
                setattr(attrib,'_Descriptor__name',n)
##                print "Adding %s.%s to %s.%s" % (self.__name__,n,self.__name__,descrName)
                descrList.append(attrib)

        type.__init__(self,name,bases,dict)
    


class JobConfig(object):
    __slots__ = ( '__name', '__attributes' )

    __metaclass__ = JobConfigMetaClass

    def __init__(self,name=None):
        """name is used in printout. The default name is derived from the filename of the python file
        where constructor is called"""

        if name is None:
            filename = os.path.basename(sys._getframe(1).f_code.co_filename)
            name = os.path.splitext(filename)[0]
        self.__name = name
        self.__attributes = {}  # dictionary to store values of attributes (managed by Descriptors)


    def __str__(self,prefix=''):
        prefix += self.__name
        me = []
        for descr in self.properties():
            name = descr.name()
            value = getattr(self,name)
            me.append( "%s.%s = %r   \t# %s" % (prefix,name,value,descr.help()) )
            prefix = ' '*len(prefix)
        return os.linesep.join(me)


    def __len__(self):
        """Return the total number of properties (this class plus all base classes)"""
        return len(self.properties())


    def __iter__(self):
        """Provide iteration over the full list of properties (the Descriptors, not the values)"""
        return iter(self.properties())
        

    def _attributeDictionary(self):
        """Dictionary to store the values. Used in the class Descriptor."""
        return self.__attributes


    def properties(self):
        """Return the full list of properties (the descriptors, not the values)"""
##        print "%s.properties()" % self.__name__
        descr = []
        for cl in self.__class__.__bases__ + (self.__class__,):
            descr += getattr(cl,'__properties')
        return descr
    

    def getProperty(self,name):
        """Get the property (the descriptor, not the value) named <name>.
        It raises an TransformConfigError if the class does not have proporty <name>."""
        for d in self.properties():
            if d.name() == name: return d

        raise TransformConfigError('class %s does not have property %s' % (self.__class__.__name__, name))


    def hasProperty(self,name):
        """Return bool indicating if this class has property <name>"""
        try:
            self.getProperty(name)
            return True
        except TransformConfigError:
            return False


    def attributes(self):
        """Dictionary with the attibutes (name:value). It is a copy to make it read-only."""
        return self.__attributes.copy()
    

    def name(self):
        """The name of the configuration instance (mainly for printout)"""
        return self.__name



class TransformConfig(JobConfig):
    """Configuration class for JobTransforms.
    It has a number of properties shared among all jobtransforms. Those properties are
    processed by class JobTransform."""
    __slots__ = ( '__metadata', )

    # add basic configuration properties
    auxfiles = ListOfStrings("List of needed auxiliary files (to be found in DATAPATH)")
    minevents = Integer("minimum number of events in output file",0, AllowedExpression("value >= 0"))
    maxeventsfactor = Float("maximum number of output events = minevents * maxeventsfactor",2.0,
                            AllowedExpression("value >= 1.0"))
    maxeventsstrategy = String("what to do if number of input events is less than maxEvents",'INPUTEVENTS',
                               ['IGNORE','INPUTEVENTS','ABORT'])
    efficiency = Float("Event filtering efficiency (acceptance factor)", 1.0, AllowedExpression("0.0 < value <= 1.0") )


    def __init__(self,name=None,metaData=None):
        """name is used in printout. The default name is derived from the filename of the python file
        where constructor is called"""
        JobConfig.__init__(self,name)
        self.__metadata = []
        if metaData is not None:
            propertyNames = [ d.name() for d in self.properties() ]
            for m in metaData:
                if m not in propertyNames:
                    raise TransformConfigError('Requested metadata %s is not an attribute of %s' % (m,self.name()) )
                self.__metadata.append(m)


    def __str__(self,prefix=''):
        me = JobConfig.__str__(self,prefix)
        #
        # add list of metadata
        #
        prefix += ' '*len(self.name())
        me += os.linesep + prefix + 'MetaData attributes: '
        if self.__metadata:
            me += ','.join(self.__metadata)
        else:
            me += '(None)'
        return me

       
    def metaData(self):
        """A dictionary of metadata to be added to the metadata of the output files.
        Values set to None will not be added to the list. Values will be converted to strings.
        Python lists will be transformed to a string with comma separated entries."""
        meta = {}
        for name in self.__metadata:
            value = getattr(self,name)
            if value is not None:
                # turn a python list into a string with comma separated entries
                if type(value).__name__ == 'list': value = ','.join([str(v) for v in value])
                meta[name] = str(value)
        return meta

