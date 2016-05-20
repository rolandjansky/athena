# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: pkg.py 714275 2015-12-12 13:07:36Z krasznaa $

# System import(s):
import os
import os.path
import datetime
import re
import glob
import string

## Function used in parsing Makefile.RootCore files
def getField( fileName, field ):
    result = ""
    expr = re.compile( "\s*" + field + "\s*=(.*)$" )
    ifile = open( fileName )
    for line in ifile:
        match = expr.match( line )
        if match:
            result = result + string.strip( match.group( 1 ) )
            pass
        pass
    return result

## Class describing one RootCore package in the work area
#
# This is a much lighter version of RootCore's similar class, as this
# one is only needed to make sense of the Makefile.RootCore files in
# the packages, but doesn't need to do anything.
#
class PackageInfo:

    ## Function initialising the object
    def __init__( self, srcdir ):
        # Initialise all of its variables:
        self.m_srcdir = srcdir
        pass

    ## The source directory of the package
    def srcdir( self ):
        return self.m_srcdir

    ## Name of the Makefile.RootCore file in the package
    def makefileName( self ):
        return os.path.join( self.srcdir(), "cmt", "Makefile.RootCore" )

    ## The name of the package
    def name( self ):
        return getField( self.makefileName(), "PACKAGE" )

    ## Names of the packages that this package depends on
    def dep( self ):
        return ( getField( self.makefileName(), "PACKAGE_DEP" ).split() +
                 getField( self.makefileName(), "PACKAGE_TRYDEP" ).split() )

    ## Whether the package builds a library or not
    def nocc( self ):
        value = getField( self.makefileName(), "PACKAGE_NOCC" )
        if value == "1":
            return True
        else:
            return False

    ## Whether the package builds a Reflex dictionary or not
    def reflex( self ):
        value = getField( self.makefileName(), "PACKAGE_REFLEX" )
        if value == "1":
            return True
        else:
            return False

    ## The list of libraries to load/link in addition
    def preload( self ):
        return getField( self.makefileName(), "PACKAGE_PRELOAD" ).split()

    ## Package specific linker options
    def ldflags( self ):
        return ( getField( self.makefileName(), "PACKAGE_BINFLAGS" ).split() +
                 getField( self.makefileName(), "PACKAGE_LDFLAGS" ).split() )

    ## Package specific compilation options
    def cxxflags( self ):
        flags = getField( self.makefileName(), "PACKAGE_CXXFLAGS" ).split()
        result = []
        for flag in flags:
            if flag == "#" or flag.startswith( "#" ):
                break
            result += [ flag ]
            pass
        return result

## Function used to scan the current directory for RootCore packages
#
# This function is used by the RootCore->CMake conversion code to first scan
# the current directory for RootCore packages, and create PackageInfo objects
# for each of them. Which it returns as a list.
#
# @param rootdir The directory where the scan is to be started
# @returns A list of PackageInfo objects describing the found RootCore packages
#
def findRootCorePackages( rootdir = "./" ):

    # The result list:
    result = []

    # Find all the directories that have a subdirectory called "cmt", which has
    # a file called "Makefile.RootCore" in it:
    for root, dirnames, filenames in os.walk( rootdir ):
        makefiles = glob.glob( os.path.join( root, "cmt",
                                             "Makefile.RootCore" ) )
        if len( makefiles ):
            result += [ PackageInfo( root ) ]
            pass
        pass

    # Return the packages found:
    return result

## Function generating a CMakeLists.txt file for a single RootCore package
#
# It is used by the RootCore->CMake conversion script to provide functional
# CMakeLists.txt files for each of the packages found in the current directory.
#
# @param package The package to generate a CMakeLists.txt file for
# @param packages The full list of known packages
#
def generateCMakeFile( package, packages ):

    # Open the CMake file:
    cmakeName = os.path.join( package.srcdir(), "CMakeLists.txt" )
    cmakeFile = open( cmakeName, "w" )

    # Add a header:
    cmakeFile.write( "# Auto-generated on: %s\n\n" %
                     str( datetime.datetime.now() ) )

    # Add the main package declaration:
    cmakeFile.write( "# Declare the name of this package:\n" )
    cmakeFile.write( "atlas_subdir( %s None )\n\n" % package.name() )

    # Set up the package's dependencies:
    depPackages = []
    linkLibraries = []
    includeDirectories = []
    for dpkg in package.dep():
        # Avoid self-referencing:
        if dpkg == package.name():
            continue
        # Skip all glue package dependencies:
        if dpkg.find( "Asg_" ) == 0:
            continue
        # Skip (non glue) packages that don't appear in the source directory:
        if not [ pkg_itr.name() for pkg_itr in packages ].count( dpkg ) \
          and not dpkg.startswith( "Asg_" ):
            continue

        # Add it to the dependent packages' list:
        depPackages += [ dpkg ]
        linkLibraries += [ dpkg ]
        pass

    # Known ROOT libraries:
    knownROOTLibs = [ "Core", "RIO", "Tree", "TreePlayer", "Net", "Hist",
                      "Physics", "GenVector", "Proof", "ProofPlayer", "TMVA",
                      "XMLPlayer", "XMLIO", "MathCore", "MathMore", "Graf",
                      "Gpad" ]

    # Known FastJet libraries:
    knownFastJetLibs = [ "fastjet", "fastjetplugins", "fastjettools", "siscone",
                         "siscone_spherical", "ConstituentSubtractor",
                         "EnergyCorrelator", "GenericSubtractor",
                         "JetCleanser", "JetFFMoments", "JetsWithoutJets",
                         "Nsubjettiness", "RecursiveTools", "ScJet",
                         "SubjetCounting", "VariableR" ]

    # Set up linking against the libraries specified in PACKAGE_PRELOAD:
    boostComponents = []
    useXml2 = False
    rootComponents = [ "Core", "Tree", "Hist", "RIO", "MathCore", "Graf",
                       "Gpad" ]
    fastJetComponents = []
    for preload in package.preload():
        # The Boost libraries need to be specified with a slightly different
        # formalism:
        if preload.find( "boost_" ) == 0:
            m = re.match( "boost_(.*)", preload )
            boostComponents += [ m.group( 1 ) ]
        elif preload == "xml2":
            useXml2 = True
        elif preload in knownROOTLibs:
            rootComponents += [ preload ]
        elif preload in knownFastJetLibs:
            fastJetComponents += [ preload ]
        else:
            linkLibraries += [ preload ]
            pass
        pass
    # Add the library/binary flags to the description if necessary:
    for flag in package.ldflags():
        # As we really only have "-l<bla>" flags here, let's just
        # deal with those...
        m = re.match( "-l(.*)", flag )
        if m:
            # Boost packages have to be treated differently here as well...
            depname = m.group( 1 )
            if depname.startswith( "boost_" ):
                n = re.match( "boost_(.*)", depname )
                boostComponents += [ n.group( 1 ) ]
            elif depname == "xml2":
                useXml2 = True
            elif depname in knownROOTLibs:
                rootComponents += [ depname ]
            elif depname in knownFastJetLibs:
                fastJetComponents += [ depname ]
            else:
                linkLibraries += [ depname ]
                pass
            pass
        pass

    # Declare the dependencies of the package:
    if len( depPackages ):
        cmakeFile.write( "# This package depends on other packages:\n" )
        cmakeFile.write( "atlas_depends_on_subdirs( PUBLIC\n"
                         "                          %s )\n\n" % \
                         ( "\n" + " " * 26 ).join( depPackages ) )
        pass

    # Make every single RootCore package use ROOT:
    cmakeFile.write( "# This package uses ROOT:\n" )
    cmakeFile.write( "find_package( ROOT REQUIRED " )
    rootComponents = list( set( rootComponents ) )
    if len( rootComponents ):
        cmakeFile.write( "COMPONENTS " + " ".join( rootComponents ) + " " )
        pass
    cmakeFile.write( ")\n\n" )
    linkLibraries += [ "${ROOT_LIBRARIES}" ]
    includeDirectories += [ "${ROOT_INCLUDE_DIRS}" ]

    # If the package declares a dependency on Asg_Boost, find it
    # explicitly:
    if package.dep().count( "Asg_Boost" ) or len( boostComponents ):
        cmakeFile.write( "# This package uses Boost:\n" )
        cmakeFile.write( "find_package( Boost REQUIRED " )
        if len( boostComponents ):
            cmakeFile.write( "COMPONENTS " )
            for comp in boostComponents:
                cmakeFile.write( "%s " % comp )
                pass
            pass
        cmakeFile.write( ")\n\n" )
        linkLibraries += [ "${Boost_LIBRARIES}" ]
        includeDirectories += [ "${Boost_INCLUDE_DIRS}" ]
        pass

    # If the package uses libxml2, find it explicitly:
    if useXml2:
        cmakeFile.write( "# This package uses libxml2:\n" )
        cmakeFile.write( "find_package( LibXml2 REQUIRED )\n\n" )
        linkLibraries += [ "${LIBXML2_LIBRARIES}" ]
        includeDirectories += [ "${LIBXML2_INCLUDE_DIR}" ]
        pass

    # If the package declares a dependency on Asg_FastJet, add the usual
    # FastJet libraries to the dependency.
    if package.dep().count( "Asg_FastJet" ) or len( fastJetComponents ):
        cmakeFile.write( "# This package uses FastJet:\n" )
        cmakeFile.write( "find_package( FastJet REQUIRED " )
        fastJetComponents = list( set( fastJetComponents ) )
        if len( fastJetComponents ):
            cmakeFile.write( "COMPONENTS " + " ".join( fastJetComponents ) +
                             " " )
            pass
        cmakeFile.write( ")\n\n" )
        linkLibraries += [ "${FASTJET_LIBRARIES}" ]
        includeDirectories += [ "${FASTJET_INCLUDE_DIRS}" ]
        pass

    # If the package depends on Asg_Eigen, set up the dependency for CMake
    # as well:
    if package.dep().count( "Asg_Eigen" ):
        cmakeFile.write( "# This package uses Eigen:\n" )
        cmakeFile.write( "find_package( Eigen REQUIRED )\n\n" )
        includeDirectories += [ "${EIGEN_INCLUDE_DIRS}" ]
        pass

    # In case cxx flags were specified. Sometimes RootCore picks up comments
    # as CXX flags. Which for some reason don't cause a problem in RootCore
    # compilation, but they do very much do that here. Such cases are just
    # ignored...
    cxxflags = package.cxxflags()
    if len( cxxflags ):
        cmakeFile.write( "# Custom definitions needed for this package:\n" )
        cmakeFile.write( "add_definitions( %s )\n\n" % " ".join( cxxflags ) )
        pass

    # Generate a CINT dictionary source file if needed:
    cintSource = ""
    if os.path.exists( os.path.join( package.srcdir(), "Root",
                                     "LinkDef.h" ) ):
        cintSource = "${_cintDictSource} "
        cmakeFile.write( "# Generate a CINT dictionary source file:\n" )
        cmakeFile.write( "atlas_add_root_dictionary( %s _cintDictSource\n" % \
                         package.name() )
        cmakeFile.write( ( " " * 27 ) + "ROOT_HEADERS Root/LinkDef.h\n" )
        cmakeFile.write( ( " " * 27 ) + "EXTERNAL_PACKAGES ROOT )\n\n" )
        pass

    # Check if there are any source files for the library to build.
    import glob
    sources = glob.glob( os.path.join( package.srcdir(), "Root", "*.cxx" ) )

    # Add a library. Note that it may become a virtual/interface library
    # if there are no sources in the package.
    cmakeFile.write( "# Build a library that other components can link "
                     "against:\n" )
    cmakeFile.write( ( "atlas_add_library( %s Root/*.cxx Root/*.h Root/*.icc\n" + \
                       ( " " * 19 ) + "%s/*.h %s/*.icc %s/*/*.h\n" + \
                       ( " " * 19 ) + "%s/*/*.icc %s\n" ) % \
                       ( package.name(), package.name(), package.name(),
                         package.name(), package.name(), cintSource ) )
    cmakeFile.write( ( " " * 19 ) + "PUBLIC_HEADERS %s" % \
                     package.name() )
    if len( sources ) == 0 and cintSource == "":
        cmakeFile.write( "\n" + ( " " * 19 ) + "INTERFACE" )
        pass
    if len( linkLibraries ):
        cmakeFile.write( "\n" + ( " " * 19 ) + "LINK_LIBRARIES %s" % \
                         ( "\n" + ( " " * 34 ) ).join( linkLibraries ) )
        pass
    if len( includeDirectories ):
        cmakeFile.write( "\n" + ( " " * 19 ) + "INCLUDE_DIRS %s" % \
                         ( "\n" + ( " " * 32 ) ).join( includeDirectories ) )
        pass
    cmakeFile.write( " )\n\n" )

    # Generate a reflex dictionary if needed:
    if package.reflex():
        cmakeFile.write( "# Build a (Reflex) dictionary library:\n" )
        cmakeFile.write( ( "atlas_add_dictionary( %sDict\n" +
                           ( " " * 22 ) + "%s/%sDict.h\n" +
                           ( " " * 22 ) + "%s/selection.xml" ) % \
                         ( package.name(), package.name(),
                           package.name(), package.name() ) )
        if not package.nocc() or len( linkLibraries ):
            cmakeFile.write( "\n" + ( " " * 22 ) + "LINK_LIBRARIES " )
            cmakeFile.write( ( "\n" + ( " " * 37 ) ).join( linkLibraries ) )
            if not package.nocc():
                if len( linkLibraries ):
                    cmakeFile.write( "\n" + ( " " * 37 ) )
                    pass
                cmakeFile.write( package.name() )
                pass
            pass
        extraFiles = glob.glob( os.path.join( package.srcdir(), "Root", "dict",
                                              "*.cxx" ) )
        if len( extraFiles ):
            cmakeFile.write( "\n" + ( " " * 22 ) +
                             "EXTRA_FILES Root/dict/%s" % \
                             ( "\n" + ( " " * 34 ) +
                               "Root/dict/" ).join( [ os.path.basename( f )
                                                      for f in extraFiles ] ) )
            pass
        cmakeFile.write( " )\n\n" )
        pass

    # Install the package's python modules if they exist:
    if os.path.exists( os.path.join( package.srcdir(), "python" ) ):
        cmakeFile.write( "# Install the Python modules of the package:\n" )
        cmakeFile.write( "atlas_install_python_modules( python/*.py )\n\n" )
        pass

    # Install the package's data files if they exist:
    if os.path.exists( os.path.join( package.srcdir(), "data" ) ):
        cmakeFile.write( "# Install data files from the package:\n" )
        cmakeFile.write( "atlas_install_data_files( DIRECTORY data )\n\n" )
    elif os.path.exists( os.path.join( package.srcdir(), "share" ) ):
        cmakeFile.write( "# Install data files from the package:\n" )
        cmakeFile.write( "atlas_install_data_files( DIRECTORY share )\n\n" )
        pass

    # If the package has an environment setup file, add that to the release-wide
    # environment setup file:
    """
    if os.path.exists( os.path.join( package.srcdir(), "cmt",
                                     "setup_external.RootCore.sh" ) ):
        cmakeFile.write( "# Update the CMake environment file:\n" )
        cmakeFile.write( "file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmt/"
                         "setup_external.RootCore.sh _setupContents LIMIT "
                         "50000 )\n" )
        cmakeFile.write( "atlas_add_env_setting( ${_setupContents} )\n\n" )
        pass
    """

    # Declare the executables of the package:
    utilFiles = glob.glob( os.path.join( package.srcdir(), "util", "*.cxx" ) )
    if len( utilFiles ):
        cmakeFile.write( "# Build the executables of the package:\n" )
        pass
    for utilFile in utilFiles:
        fname = os.path.basename( utilFile )
        exeName = os.path.splitext( fname )[ 0 ]
        cmakeFile.write( "atlas_add_executable( %s\n" % exeName )
        cmakeFile.write( ( " " * 22 ) + "util/%s" % fname )
        if not package.nocc() or len( linkLibraries ):
            cmakeFile.write( "\n" + ( " " * 22 ) + "LINK_LIBRARIES " )
            cmakeFile.write( ( "\n" + ( " " * 37 ) ).join( linkLibraries ) )
            if not package.nocc():
                if len( linkLibraries ):
                    cmakeFile.write( "\n" + ( " " * 37 ) )
                    pass
                cmakeFile.write( package.name() )
                pass
            pass
        cmakeFile.write( " )\n" )
        pass
    if len( utilFiles ):
        cmakeFile.write( "\n" )
        pass

    # Declare the tests of the package:
    testsDeclared = False
    testFiles = glob.glob( os.path.join( package.srcdir(), "test", "*.cxx" ) )
    if len( testFiles ):
        cmakeFile.write( "# Build/run unit tests for the package:\n" )
        testsDeclared = True
        pass
    for testFile in testFiles:
        fname = os.path.basename( testFile )
        testName = os.path.splitext( fname )[ 0 ]
        cmakeFile.write( "atlas_add_test( %s_%s\n" % ( package.name(),
                                                       testName ) )
        cmakeFile.write( "                SOURCES test/%s" % \
                         fname )
        if not package.nocc() or len( linkLibraries ):
            cmakeFile.write( "\n" + ( " " * 16 ) + "LINK_LIBRARIES " )
            cmakeFile.write( ( "\n" + ( " " * 31 ) ).join( linkLibraries ) )
            if not package.nocc():
                if len( linkLibraries ):
                    cmakeFile.write( "\n" + ( " " * 31 ) )
                    pass
                cmakeFile.write( package.name() )
                pass
            pass
        cmakeFile.write( " )\n" )
        pass

    testFiles = glob.glob( os.path.join( package.srcdir(), "test", "*.sh" ) )
    if len( testFiles ) and not testsDeclared:
        cmakeFile.write( "# Build/run unit tests for the package:\n" )
        testsDeclared = True
        pass
    for testFile in testFiles:
        fname = os.path.basename( testFile )
        cmakeFile.write( "atlas_add_test( %s_%s\n" % ( package.name(),
                                                       fname ) )
        cmakeFile.write( "                SCRIPT test/%s )\n" % \
                         fname )
        pass

    testFiles = glob.glob( os.path.join( package.srcdir(), "test", "*.py" ) )
    if len( testFiles ) and not testsDeclared:
        cmakeFile.write( "# Build/run unit tests for the package:\n" )
        testsDeclared = True
        pass
    for testFile in testFiles:
        fname = os.path.basename( testFile )
        cmakeFile.write( "atlas_add_test( %s_%s\n" % ( package.name(),
                                                       fname ) )
        cmakeFile.write( "                SCRIPT test/%s )\n" % \
                         fname )
        pass

    # Close the file:
    cmakeFile.close()

    # Return gracefully:
    return
