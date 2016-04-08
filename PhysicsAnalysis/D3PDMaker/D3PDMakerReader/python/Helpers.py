# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Helpers.py 600807 2014-06-08 15:26:51Z krasznaa $
#
# This module collects some functions used by the helper scripts of
# the package.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

##
# @short Separate the source and header files based on their names
#
# This function can be used to separate a list of file names into header
# and source files just based on their names.
#
# @param filename A simple list of the names of all the files
# @returns The a <code>(headers,sources)</code> pair of file name lists
def separateSources( filenames ):

    # Loop over the file names, and create the header and source lists:
    header_files = []
    source_files = []
    for file in filenames:
        # Skip the lingering linkdef and dictionary files:
        if file.lower().find( "linkdef" ) != -1 or file.lower().find( "dict" ) != -1:
            continue
        # Find the header files:
        if file.endswith( ".h" ) or file.endswith( ".icc" ):
            header_files += [ file ]
            pass
        # Find the source files:
        if file.endswith( ".cxx" ) or file.endswith( ".cpp" ) or file.endswith( ".C" ):
            source_files += [ file ]
            pass
        pass

    # Return the result:
    return ( header_files, source_files )

##
# @short Find the headers needed for dictionary generation
#
# This function can be used to find the headers from the specified
# file names that should be given to rootcint for dictionary
# generation.
#
# @param filenames A list of file names
# @returns A list of header file names that should be given to rootcint
def dictionaryHeaders( filenames ):

    # Loop over the file names, and collect the "dictionary headers":
    header_files = []
    for file in filenames:
        # Skip the lingering linkdef and dictionary files:
        if file.lower().find( "linkdef" ) != -1 or file.lower().find( "dict" ) != -1:
            continue
        # Skip the D3PDReader template classes:
        if ( file.find( "VarHandle.h" ) != -1 or
             file.find( "VarHandle.icc" ) != -1 or
             file.find( "VarProxy.h" ) != -1 or
             file.find( "VarProxy.icc" ) != -1 ):
            continue
        # Find the header files:
        if file.endswith( ".h" ):
            header_files += [ file ]
            pass
        pass

    # Return the final list:
    return header_files

##
# @short Function collecting the class names to create a dictionary for
#
# This function can parse all headers of the generated sources to collect
# the class names for which a CINT dictionary should be generated.
#
# @param filenames Header file names (as returned by
#        <code>dictionaryHeaders(...)</code>)
# @returns A list with the class names
def dictionaryClasses( filenames ):

    # Create a logger object first of all:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "dictionaryClasses" )

    # Extract the class names from the header files:
    classnames = [ "D3PDReader::VarHandleBase",
                   "D3PDReader::VarProxyBase",
                   "D3PDReader::VariableStats",
                   "D3PDReader::D3PDReadStats",
                   "map<TString,D3PDReader::VariableStats>",
                   "pair<TString,D3PDReader::VariableStats>",
                   "map<TString,D3PDReader::VarHandleBase*>",
                   "pair<TString,D3PDReader::VarHandleBase*>",
                   "D3PDReader::UserD3PDObjectElement",
                   "D3PDReader::UserD3PDObject" ]
    for header in filenames:
        # Open the header file:
        hfile = open( header, "rU" )
        if not hfile:
            logger.error( "Couldn't open header file: " + hfile )
            return 255
        for line in hfile:
            import re
            m1 = re.match( ".*ClassDef\( (\w+), 0 \)", line )
            m2 = re.match( ".*(VarHandle< .* >).*", line )
            m3 = re.match( ".*(VarProxy< .* >).*", line )
            if m1:
                if not ( "D3PDReader::" + m1.group( 1 ) ) in classnames:
                    logger.verbose( "Found class: " + m1.group( 1 ) )
                    classnames += [ "D3PDReader::" + m1.group( 1 ) ]
                    pass
                continue
            if m2:
                if m2.group( 1 ) == "VarHandle< T >": continue
                if not ( "D3PDReader::" + m2.group( 1 ) ) in classnames:
                    logger.verbose( "Found class: " + m2.group( 1 ) )
                    classnames += [ "D3PDReader::" + m2.group( 1 ) ]
                    pass
                continue
            if m3:
                if m3.group( 1 ) == "VarProxy< T >": continue
                if not ( "D3PDReader::" + m3.group( 1 ) ) in classnames:
                    logger.verbose( "Found class: " + m3.group( 1 ) )
                    classnames += [ "D3PDReader::" + m3.group( 1 ) ]
                    pass
                continue
            pass
        pass

    # Now "clean up" these class names a little, so that ROOT would
    # like them better:
    import re
    finalnames = []
    for cname in classnames:
        # First, let's remove all mentions of the std namespace:
        cname = re.sub( r"std::", "", cname )
        # Now remove all the whitespaces:
        cname = re.sub( r"\s", "", cname )
        # Remove the allocator class names:
        cname = re.sub( r",allocator<\w*>", "", cname )
        cname = re.sub( r",allocator<\w*<\w*>>", "", cname )
        cname = re.sub( r",allocator<\w*<\w*<\w*>>>", "", cname )
        # Make sure whitespaces are re-inserted into the unsigned primitive
        # names:
        cname = re.sub( r"unsigned", "unsigned ", cname )
        # Also take care of adding back the needed whitespace before the
        # "long" modifier. But don't add a whitespace after the last long in
        # the name. For instance, "longlong" should become "long long" and not
        # "long long ".
        cname = re.sub( r"long(?!>)", "long ", cname )
        # Finally, make sure that there is a space between the > operators:
        while cname != re.sub( r">>", "> >", cname ):
            cname = re.sub( r">>", "> >", cname )
            pass
        finalnames += [ cname ]
        pass

    return finalnames

##
# @short Function writing a LinkDef file
#
# This function can be used to generate a LinkDef file based on the class
# names for which a dictionary needs to be generated.
#
# @param filename The name of the output LinkDef file
# @param classname A list of the class names to generate a dictionary for
# @param headers Header file names that are to be included
# @returns <code>0</code> in case of success, something else in case
#          of failure
def writeLinkDefFile( filename, classnames, headers = [] ):

    # Create a logger object first of all:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "writeLinkDefFile" )

    # Open the linkdef file:
    linkdef = open( filename, "w" )
    if not linkdef:
        logger.error( "Couldn't open %s for writing!" % filename )
        return 255
    else:
        logger.info( "Writing linkdef file to: %s" % filename )
        pass

    # Write the file's header:
    linkdef.write( "// Dear emacs, this is -*- c++ -*-\n" )
    linkdef.write( "// $Id: Helpers.py 600807 2014-06-08 15:26:51Z krasznaa $\n" );
    linkdef.write( "#ifndef D3PDREADER_LINKDEF_H\n" )
    linkdef.write( "#define D3PDREADER_LINKDEF_H\n\n" )

    # Write the required includes:
    for header in headers:
        linkdef.write( "#include \"%s\"\n" % header )
        pass

    # Write the rest of the constant part:
    linkdef.write( "\n#ifdef __CINT__\n\n" )
    linkdef.write( "#pragma link off all globals;\n" )
    linkdef.write( "#pragma link off all classes;\n" )
    linkdef.write( "#pragma link off all functions;\n\n" )
    linkdef.write( "#pragma link C++ nestedclass;\n\n" )

    # Write which files to generate a dictionary for:
    for classname in classnames:
        linkdef.write( "#pragma link C++ class %s+;\n" % classname )
        pass

    # Write which template functions to generate a dictionary for:
    linkdef.write( "\n" )
    linkdef.write( "// You can disable the remaining lines if you don't\n" )
    linkdef.write( "// plan to use the library in CINT or PyROOT.\n" )
    functions = [ "D3PDReader::UserD3PDObject::DeclareVariable<bool>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<short>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<unsigned short>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<int>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<unsigned int>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<long long>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<unsigned long long>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<float>",
                  "D3PDReader::UserD3PDObject::DeclareVariable<double>",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<short>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<unsigned short>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<int>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<unsigned int>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<long long>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<unsigned long long>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<float>* >",
#                  "D3PDReader::UserD3PDObject::DeclareVariable<vector<double>* >",
                  "D3PDReader::UserD3PDObject::Variable<bool>",
                  "D3PDReader::UserD3PDObject::Variable<short>",
                  "D3PDReader::UserD3PDObject::Variable<unsigned short>",
                  "D3PDReader::UserD3PDObject::Variable<int>",
                  "D3PDReader::UserD3PDObject::Variable<unsigned int>",
                  "D3PDReader::UserD3PDObject::Variable<long long>",
                  "D3PDReader::UserD3PDObject::Variable<unsigned long long>",
                  "D3PDReader::UserD3PDObject::Variable<float>",
                  "D3PDReader::UserD3PDObject::Variable<double>",
#                  "D3PDReader::UserD3PDObject::Variable<vector<short>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<unsigned short>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<int>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<unsigned int>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<long long>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<unsigned long long>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<float>* >",
#                  "D3PDReader::UserD3PDObject::Variable<vector<double>* >",
                  "D3PDReader::UserD3PDObjectElement::Variable<short>",
                  "D3PDReader::UserD3PDObjectElement::Variable<unsigned short>",
                  "D3PDReader::UserD3PDObjectElement::Variable<int>",
                  "D3PDReader::UserD3PDObjectElement::Variable<unsigned int>",
                  "D3PDReader::UserD3PDObjectElement::Variable<long long>",
                  "D3PDReader::UserD3PDObjectElement::Variable<unsigned long long>",
                  "D3PDReader::UserD3PDObjectElement::Variable<float>",
                  "D3PDReader::UserD3PDObjectElement::Variable<double>" ]
    for function in functions:
        linkdef.write( "#pragma link C++ function %s;\n" % function )
        pass

    # Close the file:
    linkdef.write( "\n" )
    linkdef.write( "#endif // __CINT__\n" )
    linkdef.write( "#endif // D3PDREADER_LINKDEF_H\n" )
    linkdef.close()

    # Signal that the function was successful:
    return 0

##
# @short Create the directory structure for a RootCore package
#
# This function can be used to create the directory structure for
# a RootCore package.
#
# @param directory The directory where the package is to be created
# @param name The name for the RootCore package
# @returns <code>0</code> in case of success, something else in case
#          of failure
def makeRootCorePackageSkeleton( directory, name ):

    # Create a logger object first of all:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "makeRootCorePackageSkeleton" )

    # Check whether the output directory exists:
    import os.path
    if not os.path.exists( directory ):
        logger.error( "The output directory (%s) doesn't exist!" % directory )
        return 255

    # Check that the package's directory doesn't exist yet:
    if os.path.exists( directory + "/" + name ):
        logger.error( "The directory for the package (%s) already exists!" % \
                      directory + "/" + name )
        return 255

    # Create the directory structure:
    import os
    os.mkdir( directory + "/" + name, 0755 )
    os.mkdir( directory + "/" + name + "/" + name, 0755 )
    os.mkdir( directory + "/" + name + "/Root", 0755 )
    os.mkdir( directory + "/" + name + "/cmt", 0755 )

    # Create the RootCore Makefile:
    makefile = open( directory + "/" + name + "/cmt/Makefile.RootCore", "w" )
    makefile.write( "# $Id: Helpers.py 600807 2014-06-08 15:26:51Z krasznaa $\n\n" )
    makefile.write( "PACKAGE          = %s\n" % name )
    makefile.write( "PACKAGE_PRELOAD  = Tree\n" )
    makefile.write( "# Add the option -DACTIVATE_BRANCHES if your analysis framework\n" )
    makefile.write( "# disables all branches by default.\n" )
    makefile.write( "# Remove the COLLECT_D3PD_READING_STATISTICS option to gain a bit\n" )
    makefile.write( "# of performance...\n" )
    makefile.write( "PACKAGE_CXXFLAGS = -DCOLLECT_D3PD_READING_STATISTICS\n" )
    makefile.write( "PACKAGE_LDFLAGS  =\n" )
    makefile.write( "PACKAGE_DEP      =\n" )
    makefile.write( "PACKAGE_PEDANTIC = 1\n" )
    makefile.write( "PACKAGE_NOOPT    = dict\n\n" )
    makefile.write( "include $(ROOTCOREDIR)/Makefile-common\n" )
    makefile.close()

    # Create the RootCore requirements file:
    requirements = open( directory + "/" + name + "/cmt/requirements", "w" )
    requirements.write( "package %s\n\n" % name )
    requirements.write( "use AtlasPolicy  AtlasPolicy-*\n" )
    requirements.write( "use AtlasROOT    AtlasROOT-*    External\n\n" )
    requirements.write( "library %s ../Root/*.cxx\n" % name )
    requirements.write( "apply_pattern installed_library\n\n" )
    requirements.write( "apply_pattern have_root_headers root_headers=\"*.h " \
                        "../Root/LinkDef.h\" headers_lib=%s\n" % name )
    requirements.close()

    # Signal that the function was successful:
    return 0

##
# @short Create an SFrame package skeleton
#
# This function can be used to create the directory structure for
# an SFrame package. It does as similar job as SFrame's sframe_new_package.sh
# script.
#
# @param directory The directory where the package is to be created
# @param name The name for the SFrame package
# @returns <code>0</code> in case of success, something else in case
#          of failure
def makeSFramePackageSkeleton( directory, name ):

    # Create a logger object first of all:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "makeSFramePackageSkeleton" )

    # Check whether the output directory exists:
    import os.path
    if not os.path.exists( directory ):
        logger.error( "The output directory (%s) doesn't exist!" % directory )
        return 255

    # Check that the package's directory doesn't exist yet:
    if os.path.exists( directory + "/" + name ):
        logger.error( "The directory for the package (%s) already exists!" % \
                      directory + "/" + name )
        return 255

    # Create the directory structure:
    import os
    os.mkdir( directory + "/" + name, 0755 )
    os.mkdir( directory + "/" + name + "/include", 0755 )
    os.mkdir( directory + "/" + name + "/src", 0755 )
    os.mkdir( directory + "/" + name + "/proof", 0755 )

    # Create the SFrame Makefile:
    makefile = open( directory + "/" + name + "/Makefile", "w" )
    makefile.write( "# $Id: Helpers.py 600807 2014-06-08 15:26:51Z krasznaa $\n\n" )
    makefile.write( "# Package information\n" )
    makefile.write( "LIBRARY = %s\n" % name )
    makefile.write( "OBJDIR  = obj\n" )
    makefile.write( "DEPDIR  = $(OBJDIR)/dep\n" )
    makefile.write( "SRCDIR  = src\n" )
    makefile.write( "INCDIR  = include\n\n" )
    makefile.write( "# Enable collecting D3PD reading statistics\n" )
    makefile.write( "INCLUDES += -DCOLLECT_D3PD_READING_STATISTICS\n\n" )
    makefile.write( "# Include the generic compilation rules\n" )
    makefile.write( "include $(SFRAME_DIR)/Makefile.common\n" )
    makefile.close()

    # Create the PROOF related files:
    setup = open( directory + "/" + name + "/proof/SETUP.C", "w" )
    setup.write( "// $Id: Helpers.py 600807 2014-06-08 15:26:51Z krasznaa $\n\n" )
    setup.write( "int SETUP() {\n\n" )
    setup.write( "   if( gSystem->Load( \"libTree\" ) == -1 ) return -1;\n" )
    setup.write( "   if( gSystem->Load( \"lib%s\" ) == -1 ) return -1;\n\n" % name )
    setup.write( "   return 0;\n" )
    setup.write( "}\n" )
    setup.close()
    # Notice that BUILD.sh has to be executable:
    build = open( directory + "/" + name + "/proof/BUILD.sh", "w" )
    build.write( "# $Id: Helpers.py 600807 2014-06-08 15:26:51Z krasznaa $\n\n" )
    build.write( "if [ \"$1\" = \"clean\" ]; then\n" )
    build.write( "   make distclean\n" )
    build.write( "   exit 0\n" )
    build.write( "fi\n\n" )
    build.write( "make default\n" )
    build.close()
    os.chmod( directory + "/" + name + "/proof/BUILD.sh", 0755 )

    # Signal that the function was successful:
    return 0
