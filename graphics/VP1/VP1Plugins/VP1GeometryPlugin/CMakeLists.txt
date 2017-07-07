################################################################################
# Package: VP1GeometryPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1GeometryPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/VP1/VP1Systems/VP1GeometrySystems
                          graphics/VP1/VP1Systems/VP1GuideLineSystems )

# Install files from the package:
atlas_install_headers( VP1GeometryPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core OpenGL Gui Widgets )
find_package( SoQt )
find_package( Coin3D )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# get the package name into the variable 'pkgName', to be used below
atlas_get_package_name( pkgName )

### Declare the package that generates the boilercode for the plugin
# Notes:
# 1 - declare the package that builds the boilerplate code, and use it (i.e., "find" it)
# 2 - the boilerplate code is generated before starting the compilation of the plugin
#     by setting a dependency on a custom target defined in the boilerplate package
# 3- the boilerplate code also sets some variables used below (e.g. VP1_PLUGIN_SRC_FILENAME )
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../../graphics/VP1/VP1Base )
    message("path set!")
    set( VP1BasePluginBoilerplate_DIR ${CMAKE_SOURCE_DIR}/../../graphics/VP1/VP1Base/cmake )
else()
    message("ERROR! This path does not exist! ${CMAKE_SOURCE_DIR}/../../graphics/VP1/VP1Base")
endif()
find_package( VP1BasePluginBoilerplate REQUIRED )


### Build the library.
# Note:
# - The auto-generated boilerplate code is generated before the compilation of the plugin,
#   then copied to the plugin source dir, in two new folders:
#        'tmpqt_extraheaders' and 'tmpqt_extrasrc'
#   So it needs to be explicitly added to the compilation,
#   with the declarations:
#        ${VP1_PLUGIN_HEADER_FILENAME} ${VP1_PLUGIN_SRC_FILENAME}
# - Remember that the auto-generated UI files are put under CMAKE_CURRENT_BINARY_DIR,
#   so it needs to be added explicitly to the declaration of PRIVATE_INCLUDE_DIRS.
#
#
#message( ${pkgName} " - boilerplate code files: " ${VP1_PLUGIN_HEADER_FILENAME} " ; " ${VP1_PLUGIN_SRC_FILENAME} )
#atlas_add_library( ${pkgName} ${pkgName}/*.h src/*.cxx src/*.qrc ${VP1_PLUGIN_HEADER_FILENAME} ${VP1_PLUGIN_SRC_FILENAME}
atlas_add_library( ${pkgName} ${pkgName}/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS ${pkgName}
   INCLUDE_DIRS ${SOQT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS} ${QT5_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS tmpqt_extraheaders/ ${CMAKE_CURRENT_BINARY_DIR} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets ${SOQT_LIBRARIES} ${COIN3D_LIBRARIES} GeoPrimitives
   PRIVATE_LINK_LIBRARIES VP1GuideLineSystems VP1GeometrySystems
)

### Generate the plugin boilercode
# Add a dependency on a custom target, which triggers the custom command that generates the boilerplate code
# So the boilerplate code, if missing, is generated before the compilation of the plugin library
#add_custom_target( plugin_${pkgName} ALL DEPENDS ${VP1_PLUGIN_HEADERS} ${VP1_PLUGIN_SRC} ${VP1_PLUGIN_HEADERS_IN_SOURCE} ${VP1_PLUGIN_SRC_IN_SOURCE})
#add_dependencies( ${pkgName} plugin_${pkgName} )
