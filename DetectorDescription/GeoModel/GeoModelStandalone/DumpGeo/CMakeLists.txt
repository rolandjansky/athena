################################################################################
# Package: DumpGeo
################################################################################

# Declare the package name:
atlas_subdir( DumpGeo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/EventInfo
                          Tools/PathResolver
                          DetectorDescription/GeoModel/GeoModelStandalone/GeoExporter
                          )

# Component(s) in the package:
atlas_add_component( DumpGeo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel EventInfo PathResolver GeoExporter )

# Install files from the package:
atlas_install_headers( DumpGeo )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/dump-geo )
