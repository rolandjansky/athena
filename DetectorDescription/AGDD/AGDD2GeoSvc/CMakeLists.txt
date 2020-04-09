################################################################################
# Package: AGDD2GeoSvc
################################################################################

# Declare the package name:
atlas_subdir( AGDD2GeoSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/AGDD/AGDDControl
                          DetectorDescription/AGDD/AGDDKernel
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/AGDD/AGDDHandlers )

atlas_add_library ( AGDD2GeoSvcLib
                    INTERFACE
                    PUBLIC_HEADERS AGDD2GeoSvc
                    LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( AGDD2GeoSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AGDDControl AGDDKernel GaudiKernel AGDDHandlers )

# Install files from the package:
atlas_install_headers( AGDD2GeoSvc )

