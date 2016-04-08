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
                          DetectorDescription/AGDD/AGDDHandlers
                          Event/EventInfo
                          Event/EventInfoMgt )

# Component(s) in the package:
atlas_add_component( AGDD2GeoSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AGDDControl AGDDKernel GaudiKernel AGDDHandlers EventInfo )

# Install files from the package:
atlas_install_headers( AGDD2GeoSvc )

