# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
        super( AnaAlgorithmConfig, self ).__init__()

        # Interpret the typeAndName argument:
        pos = typeAndName.find( '/' )
        if pos == -1:
            self.setType( typeAndName )
            self.setName( typeAndName )
        else:
            self.setType( typeAndName[ 0 : pos ] )
            self.setName( typeAndName[ pos + 1 : ] )
            pass

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

    pass


def stringPropValue( value ):
    """Helper function producing a string property value"""

    stringValue = str( value )
    if isinstance( value, bool ):
        stringValue = str( int( value ) )
        pass
    return stringValue


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
