# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############
### The code below sets a custom command to create and build the Qt Plugin boilerplate code

# set args for the 'generate-vp1-factory-plugin-cmake' script
set( VP1_BUILDDIR "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}" )
set( VP1_SOURCEDIR ${CMAKE_CURRENT_SOURCE_DIR} )

set( VP1_PLUGIN_HEADER_FILENAME "tmpqt_extraheaders/${pkgName}_VP1AutoFactory.h" )
set( VP1_PLUGIN_SRC_FILENAME "tmpqt_extrasrc/${pkgName}_VP1AutoFactory.cxx" )

set( VP1_PLUGIN_HEADERS "${VP1_BUILDDIR}/tmpqt_extraheaders/${VP1_PLUGIN_HEADER_FILENAME}")
set( VP1_PLUGIN_SRC "${VP1_BUILDDIR}/tmpqt_extrasrc/${VP1_PLUGIN_SRC_FILENAME}")
set( VP1_PLUGIN_SRC_DIR "${VP1_BUILDDIR}/tmpqt_extrasrc")
set( VP1_PLUGIN_HEADERS_DIR "${VP1_BUILDDIR}/tmpqt_extraheaders")

set( VP1_PLUGIN_HEADERS_IN_SOURCE "${VP1_SOURCEDIR}/tmpqt_extraheaders/${VP1_PLUGIN_HEADER_FILENAME}" )
set( VP1_PLUGIN_SRC_IN_SOURCE "${VP1_SOURCEDIR}/tmpqt_extraheaders/${VP1_PLUGIN_HEADER_FILENAME}" )

message( "package ${pkgName} - boiler-plate files to generate:" ${VP1_PLUGIN_HEADERS} " , " ${VP1_PLUGIN_SRC} )

add_custom_command(
    OUTPUT  ${VP1_PLUGIN_HEADERS} ${VP1_PLUGIN_SRC} ${VP1_PLUGIN_HEADERS_IN_SOURCE} ${VP1_PLUGIN_SRC_IN_SOURCE}
    PRE_BUILD
    COMMAND "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/bin/generate-vp1-factory-code-cmake" ${VP1_BUILDDIR} ${VP1_SOURCEDIR} ${pkgName}
    COMMENT "Launching 'generate-vp1-factory-code' to build the plugin boiler-code for package ${pkgName}"
)

####
# NOTE!!
# The custom_target CANNOT BE USED here, because we want to run it for all plugin packages, while targets are built only once.
# So we just add a dependency on teh output of the custom_command, so it is run for each plugin package
###
# the custom target has to be defined only by the first plugin using it, 
# then the other plugins will use that definition. 
# If not enclosed by "if...else", the target would be defined by all plugins, 
# making CMake throw errors.
#
#if( NOT TARGET generatepluginboilercode )
#    add_custom_target( generatepluginboilercode ALL DEPENDS ${VP1_PLUGIN_HEADERS} ${VP1_PLUGIN_SRC}  )
#endif()
###########

