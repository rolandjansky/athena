# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Import(s):
import ROOT
import unittest
import copy

# This file is (mostly) a copy of AnaAlgorithmConfig.py, with
# algorithms replaced with services.  Ideally those two classes should
# be merged at some point, and the public tools be added via
# AsgToolConfig.  However, given that this class inherits from a
# type-specific Config class that is (probably) a non-trivial change
# to the class, and postponed to a future update.  Nils Krumnack 19
# Mar 21


class AsgServiceConfig( ROOT.asg.AsgServiceConfig ):
    """Standalone Analysis Service Configuration

    This class is used to describe the configuration of an analysis service
    (a C++ class inheriting from asg::AsgService) in Python. It behaves
    similar to an Athena configurable, but is implemented in a much simpler
    way.

    An example of using it in configuring an EventLoop job could look like:

       job = ROOT.asg.Job()
       ...
       from AsgServices.AsgServiceConfig import AsgServiceConfig
       service = AsgServiceConfig( "asg::UnitTestService1/TestService",
                                 property = 1.23 )
       service.string_property = "Foo"
       job.servicesAdd( service )

    Note that the python code doesn't know what properties can actually be set
    on any given C++ service. Any mistake made in the Python configuration
    (apart from syntax errors) is only discovered while initialising the
    analysis job.
    """

    # Class/static variable(s):
    printHeaderWidth = 80
    printHeaderPre   = 3

    def __init__( self, typeAndName, **kwargs ):
        """Constructor for an service configuration object

        Keyword arguments:
          typeAndName -- The type/instance name of the service

        Note that you can pass (initial) properties to the constructor like:

           service = AsgServiceConfig( "asg::UnitTestService1/TestService",
                                     property = 1.23 )
        """

        # Call the base class's constructor. Use the default constructor instead
        # of the one receiving the type and name, to avoid ROOT-10872.
        super( AsgServiceConfig, self ).__init__()
        self.setTypeAndName( typeAndName )

        # Initialise the properties of the service:
        self._props = {}

        # Set the properties on the object:
        for key, value in kwargs.items():
            self.setPropertyFromString( key, stringPropValue( value ) )
            self._props[ key ] = copy.deepcopy( value )
            pass

        pass

    def getName( self ):
        """Get the instance name of the service

        This is for compatibility with the getName() function of Athena
        configurables.
        """

        return self.name()

    def getType( self ):
        """Get the type name of the service

        This is for compatibility with the getType() function of Athena
        configurables.
        """

        return self.type()

    def __getattr__( self, name ):
        """Get a previously set property value from the configuration

        This function allows us to retrieve the value of a property that was
        already set for the service, to possibly use it in some configuration
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
        """Set an service property on an existing configuration object

        This function allows us to set/override properties on an service
        configuration object. Allowing for the following syntax:

           service = ...
           service.IntProperty = 66
           service.FloatProperty = 3.141592
           service.StringProperty = "Foo"

        Keyword arguments:
          key   -- The key/name of the property
          value -- The value to set for the property
        """

        # Private variables should be set directly:
        if key[ 0 ] == '_':
            return super( AsgServiceConfig, self ).__setattr__( key, value )

        # Set the property, and remember its value:
        super( AsgServiceConfig,
               self ).setPropertyFromString( key, stringPropValue( value ) )
        self._props[ key ] = copy.deepcopy( value )
        pass

    def __eq__( self, other ):
        """Check for equality with another object

        The implementation of this is very simple. We only check that the type
        and the name of the services would match.
        """

        # First check that the other object is also an AsgServiceConfig one:
        if not isinstance( other, AsgServiceConfig ):
            return False

        # Now check whether the type and the name of the services agree:
        return ( ( self.type() == other.type() ) and
                 ( self.name() == other.name() ) )

    def __ne__( self, other ):
        """Check for an inequality with another object

        This is just defined to make the '!=' operator of Python behave
        consistently with the '==' operator for such objects.
        """
        return not self.__eq__( other )

    def __str__( self ):
        """Print the service configuration in a user friendly way

        This is just to help with debugging configurations, allowing
        the user to get a nice printout of their job configuration.
        """

        name = 'Service %s/%s' % ( self.type(), self.name() )
        result = AsgServiceConfig._printHeader( name )
        result += '\n'
        for key, value in sorted( self._props.items() ):
            if isinstance( value, str ):
                printedValue = "'%s'" % value
            else:
                printedValue = value
                pass
            result += "|- %s: %s\n" % ( key, indentBy( printedValue, "| " ) )
            pass
        result += AsgServiceConfig._printFooter( name )
        return result

    def addPrivateTool( self, name, type ):
        """Create a private tool for the service

        This function is used in 'standalone' mode to declare a private tool
        for the service, or a private tool for an already declared private
        tool.

        Can be used like:
          config.addPrivateTool( 'tool1', 'ToolType1' )
          config.addPrivateTool( 'tool1.tool2', 'ToolType2' )

        Keyword arguments:
          name -- The full name of the private tool
          type -- The C++ type of the private tool
        """

        # And now set up the Python object that will take care of setting
        # properties on this tool.

        # Tokenize the tool's name. In case it is a subtool of a tool, or
        # something possibly even deeper.
        toolNames = name.split( '.' )

        # Look up the component that we need to set up the private tool on.
        component = self
        for tname in toolNames[ 0 : -1 ]:
            component = getattr( component, tname )
            pass

        # Check that the component doesn't have such a (tool) property yet.
        if hasattr( component, toolNames[ -1 ] ):
            raise RuntimeError( "Tool with name '%s' already exists" % name )
            pass

        # Now set up a smart object as a property on that component.
        component._props[ toolNames[ -1 ] ] = PrivateToolConfig( self, name,
                                                                 type )

        # Finally, tell the C++ code what to do.
        self.createPrivateTool( name, type ).ignore()

        pass

    @staticmethod
    def _printHeader( title ):
        """Produce a nice header when printing the configuration

        This function is used for printing the header of both services
        and tools.

        Keyword arguments:
          indentString -- String used as indentation
          title        -- The title of the service/tool
        """

        preLength = AsgServiceConfig.printHeaderPre
        postLength = AsgServiceConfig.printHeaderWidth - 3 - preLength - \
            len( title )
        return '/%s %s %s' % ( preLength * '*', title, postLength * '*' )

    @staticmethod
    def _printFooter( title ):
        """Produce a nice footer when printing the configuration

        This function is used for printing the footer of both services
        and tools.

        Keyword arguments:
          indentString -- String used as indentation
          title        -- The title of the service/tool
        """

        preLength = AsgServiceConfig.printHeaderPre
        postLength = AsgServiceConfig.printHeaderWidth - 12 - preLength - \
            len( title )
        return '\\%s (End of %s) %s' % ( preLength * '-', title,
                                         postLength * '-' )

    pass


class PrivateToolConfig( object ):
    """Standalone Private Tool Configuration

    This class is used to mimic the behaviour of Athena tool configurable
    classes. To be able to set the properties of private tools used by
    dual-use services in a way that's valid for both Athena and EventLoop.
    """

    def __init__( self, service, prefix, type ):
        """Constructor for an private tool configuration object
        """

        self._service = service
        self._prefix = prefix
        self._type = type
        self._props = {}

        pass

    def __getattr__( self, name ):
        """Get a previously set property value from the configuration

        This function allows us to retrieve the value of a tool property that
        was already set for an service's private tool, to possibly use it in
        some configuration decisions in the Python code itself.

        Keyword arguments:
          name -- The name of the property
        """

        # Fail if the property was not (yet) set:
        if not name in self._props:
            raise AttributeError( 'Property "%s" was not set on "%s/%s.%s"' %
                                  ( name, self._service.type(),
                                    self._service.name(), self._prefix ) )

        # Return the property value:
        return self._props[ name ]

    def __setattr__( self, key, value ):
        """Set a tool property on an existing configuration object

        This function allows us to set/override properties on a private tool
        of an service configuration object. Allowing for the following syntax:

           service = ...
           service.Tool.IntProperty = 66
           service.Tool.FloatProperty = 3.141592
           service.Tool.StringProperty = "Foo"

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
        self._service.setPropertyFromString( fullName,
                                               stringPropValue( value ) )
        self._props[ key ] = copy.deepcopy( value )
        pass

    def __str__( self ):
        """Print the private tool configuration in a user friendly way

        This is just to help with debugging configurations, allowing
        the user to get a nice printout of their job configuration.
        """

        name = 'Private Tool %s/%s' % ( self._type, self._prefix )
        result = ' \n'
        result += AsgServiceConfig._printHeader( name )
        result += '\n'
        for key, value in sorted( self._props.items() ):
            if isinstance( value, str ):
                printedValue = "'%s'" % value
            else:
                printedValue = value
                pass
            result += "|- %s: %s\n" % ( key, indentBy( printedValue, "| " ) )
            pass
        result += AsgServiceConfig._printFooter( name )
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

## Test case for the service type/name handling
class TestServiceTypeAndName( unittest.TestCase ):

    ## Test that the type and name are set correctly when using a single
    #  argument
    def test_singletypename( self ):
        config1 = AsgServiceConfig( "TypeName" )
        self.assertEqual( config1.type(), "TypeName" )
        self.assertEqual( config1.name(), "TypeName" )
        config2 = AsgServiceConfig( "NS::SomeType" )
        self.assertEqual( config2.type(), "NS::SomeType" )
        self.assertEqual( config2.name(), "NS::SomeType" )
        pass

    ## Test that specifying the type and name separately in the same string
    #  works as expected.
    def test_typeandname( self ):
        config1 = AsgServiceConfig( "TypeName/InstanceName" )
        self.assertEqual( config1.type(), "TypeName" )
        self.assertEqual( config1.name(), "InstanceName" )
        config2 = AsgServiceConfig( "NS::SomeType/Instance" )
        self.assertEqual( config2.type(), "NS::SomeType" )
        self.assertEqual( config2.name(), "Instance" )
        pass

## Test case for the service property handling
class TestServiceProperties( unittest.TestCase ):

    ## Common setup for the tests
    def setUp( self ):
        self.config = AsgServiceConfig( "Type/Name" )
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

## Test case for using private tools
class TestServicePrivateTool( unittest.TestCase ):

    ## Set up the main service object to test
    def setUp( self ):
        self.config = AsgServiceConfig( "ServiceType/ServiceName" )
        pass

    ## Test setting up and using one private tool
    def test_privatetool( self ):
        self.config.addPrivateTool( "Tool1", "ToolType1" )
        self.config.Tool1.Prop1 = "Value1"
        self.config.Tool1.Prop2 = [ 1, 2, 3 ]
        self.assertEqual( self.config.Tool1.Prop1, "Value1" )
        self.assertEqual( self.config.Tool1.Prop2, [ 1, 2, 3 ] )
        pass

    ## Test setting up and using a private tool of a private tool
    def test_privatetoolofprivatetool( self ):
        self.config.addPrivateTool( "Tool1", "ToolType1" )
        self.config.addPrivateTool( "Tool1.Tool2", "ToolType2" )
        self.config.Tool1.Tool2.Prop3 = "Foo"
        self.config.Tool1.Tool2.Prop4 = [ "Bar" ]
        self.assertEqual( self.config.Tool1.Tool2.Prop3, "Foo" )
        self.assertEqual( self.config.Tool1.Tool2.Prop4, [ "Bar" ] )
        pass

    ## Test that unset properties on the tools can't be used
    def test_nonexistentprop( self ):
        self.config.addPrivateTool( "Tool1", "ToolType1" )
        with self.assertRaises( AttributeError ):
            value = self.config.Tool1.BadProp
            pass
        self.config.addPrivateTool( "Tool1.Tool2", "ToolType2" )
        with self.assertRaises( AttributeError ):
            value = self.config.Tool1.Tool2.BadProp
            pass
        pass

    ## Test that private tools can't be set up on not-yet-declared tools
    def test_nonexistenttool( self ):
        with self.assertRaises( AttributeError ):
            self.config.addPrivateTool( "BadTool.Tool4", "BadToolType" )
            pass
        pass
