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
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          graphics/VP1/VP1Utils
                          )
                          #DetectorDescription/GeoModel/GeoModelStandalone/GeoWrite
                          #DetectorDescription/GeoModel/GeoModelStandalone/GeoModelDBManager

# External dependencies:
find_package( Qt5 COMPONENTS Sql Gui PrintSupport )
find_package( Eigen ) # is it really needed here?
FIND_PACKAGE( Boost ) # is it really needed here?
find_package( CLHEP )
#find_package( GeoModelCore REQUIRED )
find_package( GeoModelIO REQUIRED )
#message( "GEOMODELIO_LIBRARIES: ${GEOMODELIO_LIBRARIES}")

# Component(s) in the package:
atlas_add_library( GeoExporter
                   src/*.cxx
                   GeoExporter/*.h
                   PUBLIC_HEADERS GeoExporter
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES Qt5::Sql
                      ${GEOMODELIO_LIBRARIES}
                      GeoModelUtilities
                      VP1Utils
                      ${CLHEP_LIBRARIES}
                      ${EIGEN_LIBRARIES}
                  )
                  #NEW SYNTAX TO BE USED SOON: GeoModelIO::GeoModelDBManager GeoModelIO::GeoModelWrite

# Install files from the package:
atlas_install_headers( GeoExporter )
#atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/*.py )
#atlas_install_scripts( share/dump-geo )
