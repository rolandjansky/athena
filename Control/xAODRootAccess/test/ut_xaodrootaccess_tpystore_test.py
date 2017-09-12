#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Unit test for the TPyStore class.
#

## C/C++ style main function
def main():

    # The name of the application:
    APP_NAME = "ut_xaodrootaccess_tpystore_test"

    # Set up a logger object:
    import logging
    logger = logging.getLogger( APP_NAME )
    logger.setLevel( logging.INFO )
    hdlr = logging.StreamHandler( sys.stdout )
    frmt = logging.Formatter( "%(name)-14s%(levelname)8s %(message)s" )
    hdlr.setFormatter( frmt )
    logger.addHandler( hdlr )

    # Set up the environment:
    import ROOT
    if not ROOT.xAOD.Init( APP_NAME ).isSuccess():
        logger.error( "Failed to call xAOD::Init(...)" )
        return 1

    # Create the object to test:
    from xAODRootAccess.TPyStore import TPyStore
    store = TPyStore()

    # Record some basic objects into it:
    auxcont = ROOT.xAOD.AuxContainerBase()
    if not store.record( auxcont,
                         "AuxContainerBase" ).isSuccess():
        logger.error( "Couldn't record xAOD::AuxContainerBase object" )
        return 1
    auxinfo = ROOT.xAOD.AuxInfoBase()
    if not store.record( auxinfo,
                         "AuxInfoBase" ).isSuccess():
        logger.error( "Couldn't record xAOD::AuxInfoBase object" )
        return 1
    logger.info( "Objects recorded into the store" )

    # Now check if the store know about them propertly:
    if not store.contains( "AuxContainerBase",
                           ROOT.xAOD.AuxContainerBase ):
        logger.error( "AuxContainerBase not available as "
                      "xAOD::AuxContainerBase" )
        return 1
    if not store.contains( "AuxContainerBase",
                           ROOT.SG.IAuxStore ):
        logger.error( "AuxContainerBase not available as "
                      "SG::IAuxStore" )
        return 1
    if not store.contains( "AuxInfoBase",
                           ROOT.xAOD.AuxInfoBase ):
        logger.error( "AuxInfoBase not available as "
                      "xAOD::AuxInfoBase" )
        return 1
    if not store.contains( "AuxInfoBase",
                           ROOT.SG.IAuxStoreIO ):
        logger.error( "AuxInfoBase not available as "
                      "SG::IAuxStoreIO" )
        return 1
    if store.contains( "AuxContainerBase",
                       ROOT.xAOD.AuxInfoBase ):
        logger.error( "AuxContainerBase available as "
                      "xAOD::AuxInfoBase?!?" )
        return 1
    logger.info( "AuxContainerBase/AuxInfoBase containment tests "
                 "succeeded" )

    # Clear the store:
    store.clear()

    # Now the objects shouldn't be there anymore:
    if( store.contains( "AuxContainerBase", ROOT.xAOD.AuxContainerBase ) or
        store.contains( "AuxInfoBase", ROOT.xAOD.AuxInfoBase ) ):
        logger.error( "The objects are still in the store?!?" )
        return 1

    # Finish with a statement:
    logger.info( "All tests succeeded" )

    # Return gracefully:
    return 0

# Run the main() function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
