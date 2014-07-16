# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Utils.py 491963 2012-03-30 14:53:13Z krasznaa $
#
# Python utility functions used in multiple scripts.
#

##
# @short Function finding AtlasGPerfTools/cmt/setup.sh
#
# The script needs to source the setup script of the GPerfTools glue package
# internally. This function finds the location of the script in the filesystem
# that should be used.
#
# @returns The full path of the setup script to be used
#
def locateSetupScript():

    # Set up a logger object:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "locateSetupScript" )

    # Locate the AtlasGPerfTools package using CMT:
    import subprocess
    process = subprocess.Popen( [ "cmt", "show", "versions",
                                  "External/AtlasGPerfTools" ],
                                stdout = subprocess.PIPE,
                                stderr = subprocess.PIPE )
    ( result, errors ) = process.communicate()

    # Check that the CMT command was successful:
    if len( errors ):
        logger.error( "Couldn't execute cmt show versions command" )
        return ""

    # Select the first result:
    first_version = result.split( '\n' )[ 0 ]
    logger.verbose( "Preferred AtlasGPerfTools version: " + first_version )

    # The line should look like this:
    #  [package name] [package version] [path]
    package_info = first_version.split( ' ' )
    if len( package_info ) != 3:
        logger.error( "Couldn't interpret: " + first_version )
        return ""

    # Now, construct the path:
    path = package_info[ 2 ] + "/" + package_info[ 0 ] + "/cmt/setup.sh"
    logger.debug( "Setup script found under: " + path )

    return path
