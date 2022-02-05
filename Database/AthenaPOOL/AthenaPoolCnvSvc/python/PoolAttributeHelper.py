# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" A basic module that helps with setting various common Pool Attributes """

def setPoolAttribute( **kwargs ):
    """ The low-level function that builds the requested Pool Attribure string. """

    # Resolve the variables for convenience
    fileName  = kwargs.get('fileName')
    contName  = kwargs.get('contName')
    attrName  = kwargs.get('attrName')
    attrValue = kwargs.get('attrValue')

    # Now let's build the string
    result = ""

    # First set the domain attribute
    if not attrName or not attrValue:
        return result
    result += f"{attrName} = '{attrValue}';"

    # Then set database and container level attributes
    # These are optional: can set both, only one, or neither 
    if contName:
        result = f"ContainerName = '{contName}'; {result}"
    if fileName:
        result = f"DatabaseName = '{fileName}'; {result}"

    # Finally return the result
    return result

def setFileCompAlg( fileName = None, compAlg = None ):
    """ Convenience method for setting the compression algorithm for a given file. """

    return setPoolAttribute( fileName  = fileName, 
                             attrName  = "COMPRESSION_ALGORITHM",
                             attrValue = compAlg )

def setFileCompLvl( fileName = None, compLvl = None ):
    """ Convenience method for setting the compression level for a given file. """

    return setPoolAttribute( fileName  = fileName, 
                             attrName  = "COMPRESSION_LEVEL",
                             attrValue = compLvl )

def setMaxBufferSize( fileName = None, bufferSize = None ):
    """ Convenience method for setting the maximum basket buffer size """

    return setPoolAttribute( fileName = fileName,
                             attrName = "MAXIMUM_BUFFERSIZE",
                             attrValue = bufferSize )

def setMinBufferEntries( fileName = None, nEntries = None ):
    """ Convenience method for setting the minimum basket buffer entries """

    return setPoolAttribute( fileName = fileName,
                             attrName = "MINIMUM_BUFFERENTRIES",
                             attrValue = nEntries )

def setTreeAutoFlush( fileName = None, treeName = None, autoFlush = None ):
    """ Convenience method for setting the AutoFlush for a tree in a given file. """

    return setPoolAttribute( fileName  = fileName,
                             contName  = f"TTree={treeName}",
                             attrName  = "TREE_AUTO_FLUSH",
                             attrValue = autoFlush )

def setContainerSplitLevel( fileName = None, treeName = None, splitLvl = None ):
    """ Convenience method for setting the split level for a tree in a given file. """

    return setPoolAttribute( fileName  = fileName,
                             contName  = f"TTree={treeName}",
                             attrName  = "CONTAINER_SPLITLEVEL",
                             attrValue = splitLvl )

def setBranchBasketSize( fileName = None, treeName = None, basketSize = None ):
    """ Convenience method for setting the branch basket size for a tree in a given file. """

    return setPoolAttribute( fileName  = fileName,
                             contName  = f"TTree={treeName}",
                             attrName  = "BRANCH_BASKET_SIZE",
                             attrValue = basketSize )

# Main Function: Only to check the basic functionality
# Can be run via python PoolAttributeHelper.py
if "__main__" in __name__:

    # Test Basic Functionality
    attrs = []

    # High-level
    attrs += [ setFileCompAlg( "AOD.pool.root", 2 ) ]
    attrs += [ setFileCompLvl( "AOD.pool.root", 1 ) ]
    attrs += [ setMaxBufferSize( "*", 131072 ) ]
    attrs += [ setMinBufferEntries( "*", 10 ) ]
    attrs += [ setTreeAutoFlush( "AOD.pool.root", "CollectionTree", 10 ) ]
    attrs += [ setContainerSplitLevel( None, "POOLContainerForm(DataHeaderForm)", 99 ) ]

    # Low-level
    attrs += [ setPoolAttribute( attrName = "DEFAULT_SPLITLEVEL", attrValue = 0) ]

    # Print the configuration
    for attr in attrs:
        print( f"{attr}" )
