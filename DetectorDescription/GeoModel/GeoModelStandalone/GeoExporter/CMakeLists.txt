################################################################################
# Package: GeoExporter
################################################################################

# Declare the package name:
atlas_subdir( GeoExporter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo
                          Tools/PathResolver
                          DetectorDescription/GeoModel/GeoModelStandalone/GeoExporter
                          DetectorDescription/GeoModel/GeoModelStandalone/GeoWrite
                          DetectorDescription/GeoModel/GeoModelStandalone/GeoModelDBManager
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          graphics/VP1/VP1Utils
                          )

# External dependencies:
find_package( Qt5 COMPONENTS Sql Gui PrintSupport )
find_package( Eigen ) # is it really needed here?
FIND_PACKAGE( Boost ) # is it really needed here?
find_package( CLHEP )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( GeoExporter
                   src/*.cxx
                   GeoExporter/*.h
                   PUBLIC_HEADERS GeoExporter
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES Qt5::Sql ${GeoModel_LIBRARIES} GeoModelDBManager
                   GeoModelUtilities VP1Utils GeoWrite ${CLHEP_LIBRARIES}
                   ${EIGEN_LIBRARIES} )

# Install files from the package:
atlas_install_headers( GeoExporter )
#atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/*.py )
#atlas_install_scripts( share/dump-geo )
