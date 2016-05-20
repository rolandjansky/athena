# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: main.py 716299 2016-01-04 17:18:23Z krasznaa $
#
# Module holding the C/C++ style main function of the CMake configuration
# generator.

# System import(s):
import subprocess
import os
import optparse

# Local import(s):
import project
import pkg

## C/C++ style main function
def main():

    # Parse the command line argument(s):
    parser = \
      optparse.OptionParser( usage="generateRootCoreCMakeProject.sh [options]",
         description="RootCore->CMake conversion script. " \
                     "Use this script to generate CMakeLists.txt files for " \
                     "all the RootCore packages under the current directory.")
    parser.add_option( "-n", "--no-project", dest="no_project",
                       action="store_true",
                       help="When specified, no project file is generated" )
    ( options, files ) = parser.parse_args()

    # Greet the user:
    print( "  --- RootCore -> CMake Conversion ---" )

    # Find all RootCore packages in the current directory:
    print( "-- Searching current directory for RootCore packages..." )
    localpackages = pkg.findRootCorePackages()
    print( "-- Found %i package(s) in total" % len( localpackages ) )

    # If the script is run from an installed release, scan the release for
    # available packages as well.
    prefix_path = os.environ.get( "CMAKE_PREFIX_PATH" )
    releasepackages = []
    if prefix_path:
        releases = prefix_path.split( ":" )
        for release_dir in releases:
            if os.path.isdir( "%s/src" % release_dir ):
                releasepackages += \
                  pkg.findRootCorePackages( "%s/src/" % release_dir )
                pass
            pass
        pass

    # The list of all packages, with possible overlaps:
    allpackages = localpackages + releasepackages

    # Collect which packages need CMakeLists.txt file generation:
    packages = []
    for package in localpackages:
        # Skip the RootCore and Asg_<Bla> glue packages:
        if ( package.name() == "RootCore" ) or \
           package.name().startswith( "Asg_" ):
            continue
        # Skip packages that have a CMakeLists.txt file in them,
        # which is known to svn.
        cmakeName = os.path.join( package.srcdir(), "CMakeLists.txt" )
        try:
            FNULL = open( os.devnull, "w" )
            subprocess.check_call( [ "svn", "info", cmakeName ],
                                   stdout = FNULL, stderr = FNULL )
        except subprocess.CalledProcessError:
            # Okay, there's no such file. So we'll need to create one:
            packages += [ package ]
            pass
        pass
    print( "-- Found %i package(s) needing CMakeLists.txt generation" % \
           len( packages ) )
    print( "-- Package(s) to generate CMakeLists.txt for: %s" % \
           str( [ package.name() for package in packages ] ) )

    # Loop over the packages:
    for package in packages:
        pkg.generateCMakeFile( package, allpackages )
        pass

    # If we don't need to generate the project files, stop now:
    if options.no_project:
        return 0

    # Create the top-level project file:
    project.generateProjectFile( "CMakeLists.txt", "AnalysisBase", "3.0.0" )
    # Create the main CTest configuration file:
    project.generateCTestFile( "CTestConfig.cmake", "AnalysisBase" )

    # Return gracefully:
    return 0

# Execute the main function
if __name__ == "__main__":
    import sys
    sys.exit( main() )
