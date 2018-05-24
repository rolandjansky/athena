# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Import(s):
import ROOT
import unittest

class AnaAlgorithmConfig( ROOT.EL.AnaAlgorithmConfig ):
    """Standalone Analysis Algorithm Configuration

    This class is used to describe the configuration of an analysis algorithm
    (a C++ class inheriting from EL::AnaAlgorithm) in Python. It behaves
    similar to an Athena configurable, but is implemented in a much simpler
    way.

    An example of using it in configuring an EventLoop job could look like:

       job = ROOT.EL.Job() 
       ...
       from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
       alg = AnaAlgorithmConfig( "EL::UnitTestAlg2/TestAlg",
                                 property = 1.23 )
       alg.string_property = "Foo"
       job.algsAdd( alg )

    Note that the python code doesn't know what properties can actually be set
    on any given C++ algorithm. Any mistake made in the Python configuration
    (apart from syntax errors) is only discovered while initialising the
    analysis job.
    """

    def __init__( self, typeAndName, **kwargs ):
        """Constructor for an algorithm configuration object

        Keyword arguments:
          typeAndName -- The type/instance name of the algorithm

        Note that you can pass (initial) properties to the constructor like:

           alg = AnaAlgorithmConfig( "EL::UnitTestAlg2/TestAlg",
                                     property = 1.23 )
        """

        # Call the base class's constructor:
        super( AnaAlgorithmConfig, self ).__init__( typeAndName )

        # Initialise the properties of the algorihm:
        self._props = {}

        # Set the properties on the object:
        for key, value in kwargs.iteritems():
            self.setPropertyFromString( key, stringPropValue( value ) )
            self._props[ key ] = value
            pass

        pass

    def getName( self ):
        """Get the instance name of the algorithm

        This is for compatibility with the getName() function of Athena
        configurables.
        """

        return self.name()

    def getType( self ):
        """Get the type name of the algorithm

        This is for compatibility with the getType() function of Athena
        configurables.
        """

        return self.type()

    def __getattr__( self, name ):
        """Get a previously set property value from the configuration

        This function allows us to retrieve the value of a property that was
        already set for the algorithm, to possibly use it in some configuration
        decisions in the Python code itself.

        Keyword arguments:
          name -- The name of the property
        """

        # Fail if the property was not (yet) set:
        if not name in self._props:
            raise AttributeError( 'Property \'%s\' was not set on \'%s/%s\'' %
                                  ( name, self.type(), self.name() ) )

        # Return the property value:
        return self._props[ name ]

    def __setattr__( self, key, value ):
        """Set an algorithm property on an existing configuration object

        This function allows us to set/override properties on an algorithm
        configuration object. Allowing for the following syntax:

           alg = ...
           alg.IntProperty = 66
           alg.FloatProperty = 3.141592
           alg.StringProperty = "Foo"

        Keyword arguments:
          key   -- The key/name of the property
          value -- The value to set for the property
        """

        # Private variables should be set directly:
        if key[ 0 ] == '_':
            return super( AnaAlgorithmConfig, self ).__setattr__( key, value )

        # Set the property, and remember its value:
        super( AnaAlgorithmConfig,
               self ).setPropertyFromString( key, stringPropValue( value ) )
        self._props[ key ] = value
        pass

    def __eq__( self, other ):
        """Check for equality with another object

        The implementation of this is very simple. We only check that the type
        and the name of the algorithms would match.
        """

        # First check that the other object is also an AnaAlgorithmConfig one:
        if not isinstance( other, AnaAlgorithmConfig ):
            return False

        # Now check whether the type and the name of the algorithms agree:
        return ( ( self.type() == other.type() ) and
                 ( self.name() == other.name() ) )

    def __ne__( self, other ):
        """Check for an inequality with another object

        This is just defined to make the '!=' operator of Python behave
        consistently with the '==' operator for such objects.
        """
        return not self.__eq__( other )

    def __str__( self ):
        """Print the algorithm configuration in a user friendly way

        This is just to help with debugging configurations, allowing
        the user to get a nice printout of their job configuration.
        """

        result = "=== %s/%s ===\n" % ( self.type(), self.name() )
        for key, value in self._props.iteritems():
            result += "-  %s: %s\n" % \
                ( key, indentBy( value, "-" + " " * ( 4 + len( key ) ) ) )
            pass
        result += "-" * ( 9 + len( self.type() ) + len( self.name() ) )
        return result

    def addPrivateTool (self, name, type) :
        """create a private tool for the algorithm"""
        self.createPrivateTool( name, type ).ignore ()
        self._props[name] = PrivateToolConfig (self, name, type)
        pass

    pass



class PrivateToolConfig( object ):
    """Standalone Private Tool Configuration

    This class is used to mimic the behaviour of Athena tool configurable
    classes. To be able to set the properties of private tools used by
    dual-use algorithms in a way that's valid for both Athena and EventLoop.
    """

    def __init__( self, algorithm, prefix, type ):
        """Constructor for an private tool configuration object
        """

        self._algorithm = algorithm
        self._prefix = prefix
        self._type = type
        self._props = {}

        pass

    def __getattr__( self, name ):
        """Get a previously set property value from the configuration

        This function allows us to retrieve the value of a tool property that
        was already set for an algorithm's private tool, to possibly use it in
        some configuration decisions in the Python code itself.

        Keyword arguments:
          name -- The name of the property
        """

        # Fail if the property was not (yet) set:
        if not name in self._props:
            raise AttributeError( 'Property "%s" was not set on "%s/%s.%s"' %
                                  ( name, self._algorithm.type(),
                                    self._algorithm.name(), self._prefix ) )

        # Return the property value:
        return self._props[ name ]

    def __setattr__( self, key, value ):
        """Set a tool property on an existing configuration object

        This function allows us to set/override properties on a private tool
        of an algorithm configuration object. Allowing for the following syntax:

           alg = ...
           alg.Tool.IntProperty = 66
           alg.Tool.FloatProperty = 3.141592
           alg.Tool.StringProperty = "Foo"

        Keyword arguments:
          key   -- The key/name of the property
          value -- The value to set for the property
        """

        # Private variables should be set directly:
        if key[ 0 ] == '_':
            return super( PrivateToolConfig, self ).__setattr__( key, value )

        # Construct the full name, used in the C++ code:
        fullName = self._prefix + "." + key

        # Set the property, and remember its value:
        self._algorithm.setPropertyFromString( fullName,
                                               stringPropValue( value ) )
        self._props[ key ] = value
        pass

    def __str__( self ):
        """Print the private tool configuration in a user friendly way

        This is just to help with debugging configurations, allowing
        the user to get a nice printout of their job configuration.
        """

        result = "=== %s/%s ===\n" % ( self._type, self._prefix )
        for key, value in self._props.iteritems():
            result += "-  %s: %s\n" % \
                ( key, indentBy( value, "-" + " " * ( 4 + len( key ) ) ) )
            pass
        result += "-" * ( 9 + len( self._type ) + len( self._prefix ) )
        return result

    pass


def stringPropValue( value ):
    """Helper function producing a string property value"""

    stringValue = str( value )
    if isinstance( value, bool ):
        stringValue = str( int( value ) )
        pass
    return stringValue


def indentBy( propValue, indent ):
    """Helper function used in the configuration printout"""

    stringValue = str( propValue )
    result = ""
    for stringLine in stringValue.split( '\n' ):
        if len( result ):
            result += "\n" + indent
            pass
        result += stringLine
        pass
    return result


#
# Declare some unit tests for the code
#

## Test case for the algorithm type/name handling
class TestAlgTypeAndName( unittest.TestCase ):

    ## Test that the type and name are set correctly when using a single
    #  argument
    def test_singletypename( self ):
        config1 = AnaAlgorithmConfig( "TypeName" )
        self.assertEqual( config1.type(), "TypeName" )
        self.assertEqual( config1.name(), "TypeName" )
        config2 = AnaAlgorithmConfig( "NS::SomeType" )
        self.assertEqual( config2.type(), "NS::SomeType" )
        self.assertEqual( config2.name(), "NS::SomeType" )
        pass

    ## Test that specifying the type and name separately in the same string
    #  works as expected.
    def test_typeandname( self ):
        config1 = AnaAlgorithmConfig( "TypeName/InstanceName" )
        self.assertEqual( config1.type(), "TypeName" )
        self.assertEqual( config1.name(), "InstanceName" )
        config2 = AnaAlgorithmConfig( "NS::SomeType/Instance" )
        self.assertEqual( config2.type(), "NS::SomeType" )
        self.assertEqual( config2.name(), "Instance" )
        pass

## Test case for the algorithm property handling
class TestAlgProperties( unittest.TestCase ):

    ## Common setup for the tests
    def setUp( self ):
        self.config = AnaAlgorithmConfig( "Type/Name" )
        pass

    ## Test that properties that got set, can be read back
    def test_propaccess( self ):
        self.config.Prop1 = "Value1"
        self.config.Prop2 = [ "Value2" ]
        self.assertEqual( self.config.Prop1, "Value1" )
        self.assertEqual( self.config.Prop2, [ "Value2" ] )
        self.assertNotEqual( self.config.Prop1, "Foo" )
        self.assertNotEqual( self.config.Prop2, "Value2" )
        pass

    ## Test that an unset property can't be accessed
    def test_nonexistentprop( self ):
        with self.assertRaises( AttributeError ):
            value = self.config.Prop3
            pass
        pass
