# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def createComponent( typeName, instanceName, componentType ):
    """Create a generic configurable

    This function is used to create an component "configurable" in a
    dual-use way, either returning an actual Athena configurable, or
    an appropriately configured PythonConfig instance.

    Keyword arguments:
      typeName      -- The C++ type name of the component
      instanceName  -- The instance name of the component to create
      componentType -- The type of component in AnalysisBase

    """

    try:
        # Try to get a configurable for this C++ class "from Athena".
        # If this succeeds, we're obviously in an Athena environment.

        # First off, construct a "python type name" for the class, replacing the
        # '::' namespace delimeters with '__'.
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable of this component:
        from AthenaCommon import CfgMgr
        componentClass = getattr( CfgMgr, pythonTypeName )

        # Return the object:
        return componentClass( instanceName )

    except ImportError:
        # If that didn't work, then apparently we're in an EventLoop
        # environment, so we need to use PythonConfig as the base class
        # for the user's class.
        from AnaAlgorithm.PythonConfig import PythonConfig
        component = PythonConfig( '%s/%s' % ( typeName, instanceName ) )
        component.setComponentType( componentType )
        return component

    pass


def createAlgorithm( typeName, instanceName ):
    """Create an algorithm configurable

    This function is used to create an algorithm "configurable" in a dual-use
    way, either returning an actual Athena configurable, or an appropriately
    configured EL::AnaAlgorithmConfig instance.

    Keyword arguments:
      typeName     -- The C++ type name of the algorithm
      instanceName -- The instance name of the algorithm to create
    """
    return createComponent( typeName, instanceName, 'AnaAlgorithm' )


def createReentrantAlgorithm( typeName, instanceName ):
    """Create a reentrant algorithm configurable

    This function is used to create an algorithm "configurable" in a dual-use
    way, either returning an actual Athena configurable, or an appropriately
    configured EL::AnaAlgorithmConfig instance.

    Keyword arguments:
      typeName     -- The C++ type name of the algorithm
      instanceName -- The instance name of the algorithm to create
    """
    return createComponent( typeName, instanceName, 'AnaReentrantAlgorithm' )


def createPublicTool( typeName, toolName ):
    """Helper function for setting up a public tool for a dual-use algorithm

    This function is meant to be used in the analysis algorithm sequence
    configurations for setting up public tools on the analysis algorithms.
    Public tools that could then be configured with a syntax shared between
    Athena and EventLoop.

    Keyword arguments:
      typeName -- The C++ type name of the private tool
      toolName -- The property name with which the tool handle was declared on
                  the algorithm. Also the instance name of the tool.
    """

    try:
        # Try to set up a public tool of this type for Athena. If this succeeds,
        # we're obviously in an Athena environment.

        # First off, construct a "python type name" for the class, replacing the
        # '::' namespace delimeters with '__'.
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable of this tool:
        from AthenaCommon import CfgMgr
        toolClass = getattr( CfgMgr, pythonTypeName )

        # Add an instance of the tool to the ToolSvc:
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr( ToolSvc, toolName ):
            ToolSvc += toolClass( toolName )
            pass

        # Return the member on the ToolSvc:
        return getattr( ToolSvc, toolName )

    except ImportError:
        # If that didn't work, then apparently we're in an EventLoop
        # environment, so let's use the EventLoop specific formalism.
        return createComponent( typeName, toolName, 'AsgTool' )


def createService( typeName, serviceName, sequence=None ):
    """Helper function for setting up a service for a dual-use algorithm

    This function is meant to be used to set up services in a dual-use
    manner, particularly for the common CP algorithms.  This allows to
    use the same syntax in EventLoop and Athena, hiding the
    differences internally.  Since in EventLoop the service gets added
    to a sequence (but in Athena does not), that sequence needs to be
    passed into this function.

    Keyword arguments:
      typeName -- The C++ type name of the service
      serviceName -- The name with which the service handle was configured on
                    the algorithm. Also the instance name of the service.
      sequence -- an optional argument of an algorithm sequence to add it to
                  in EventLoop (ignored in Athena)

    """

    try:
        # Try to set up a public tool of this type for Athena. If this succeeds,
        # we're obviously in an Athena environment.

        # First off, construct a "python type name" for the class, replacing the
        # '::' namespace delimeters with '__'.
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable of this tool:
        from AthenaCommon import CfgMgr
        serviceClass = getattr( CfgMgr, pythonTypeName )

        # Add an instance of the service to the ServiceMgr:
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr, serviceName ):
            ServiceMgr += serviceClass( serviceName )
            pass

        # Return the member on the ServiceMgr:
        return getattr( ServiceMgr, serviceName )

    except ImportError:
        # If that didn't work, then apparently we're in an EventLoop
        # environment, so let's use the EventLoop specific formalism.
        service = createComponent( typeName, serviceName, 'AsgService' )
        if sequence is not None :
            sequence += service
            pass
        return service


def addPrivateTool( alg, toolName, typeName ):
    """Helper function for declaring a private tool for a dual-use algorithm

    This function is meant to be used in the analysis algorithm sequence
    configurations for setting up private tools on the analysis algorithms.
    Private tools that could then be configured with a syntax shared between
    Athena and EventLoop.

    Keyword arguments:
      alg      -- The algorithm to set up the private tool on
      toolName -- The property name with which the tool handle was declared on
                  the algorithm. Also the instance name of the tool.
      typeName -- The C++ type name of the private tool
    """

    try:

        # First try to set up the private tool in an "Athena way".

        # Tokenize the tool's name. In case it is a subtool of a tool, or
        # something possibly even deeper.
        toolNames = toolName.split( '.' )

        # Look up the component that we need to set up the private tool on:
        component = alg
        for tname in toolNames[ 0 : -1 ]:
            component = getattr( component, tname )
            pass

        # Let's replace all '::' namespace delimeters in the type name
        # with '__'. Just because that's how the Athena code behaves...
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable describing this tool:
        from AthenaCommon import CfgMgr
        toolClass = getattr( CfgMgr, pythonTypeName )

        # Finally, set up the tool handle property:
        setattr( component, toolNames[ -1 ], toolClass( toolNames[ -1 ] ) )

    except ( ImportError, AttributeError ) as e:

        # If that failed, then we should be in an EventLoop environment. So
        # let's rely on the standalone specific formalism for setting up the
        # private tool.
        alg.addPrivateTool( toolName, typeName )
        pass

    return


def addPrivateToolInArray( alg, toolName, typeName ):
    """Helper function for declaring a private tool in an array for a
    dual-use algorithm

    This function is meant to be used in the analysis algorithm
    sequence configurations for setting up private tools in arrays on
    the analysis algorithms.  Private tools that could then be
    configured with a syntax shared between Athena and EventLoop.

    Keyword arguments:
      alg      -- The algorithm to set up the private tool on
      toolName -- The property name with which the tool handle was declared on
                  the algorithm. Also the instance name of the tool.
      typeName -- The C++ type name of the private tool

    """

    try:

        # First try to set up the private tool in an "Athena way".

        # Tokenize the tool's name. In case it is a subtool of a tool, or
        # something possibly even deeper.
        toolNames = toolName.split( '.' )

        # Look up the component that we need to set up the private tool on:
        component = alg
        for tname in toolNames[ 0 : -1 ]:
            component = getattr( component, tname )
            pass

        # Let's replace all '::' namespace delimeters in the type name
        # with '__'. Just because that's how the Athena code behaves...
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable describing this tool:
        from AthenaCommon import CfgMgr
        toolClass = getattr( CfgMgr, pythonTypeName )

        # Finally, set up the tool handle property:
        getattr( component, toolNames[ -1 ] ).append (toolClass( toolNames[ -1 ] ) )
        return getattr( component, toolNames[ -1 ] )

    except ( ImportError, AttributeError ):

        # If that failed, then we should be in an EventLoop environment. So
        # let's rely on the standalone specific formalism for setting up the
        # private tool.
        return alg.addPrivateToolInArray( toolName, typeName )

    return
