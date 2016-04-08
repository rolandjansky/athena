################################################################################
# Package: InDetCosmicsFilters
################################################################################

# Declare the package name:
atlas_subdir( InDetCosmicsFilters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetCosmicsFilters
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests AtlasDetDescr Identifier GaudiKernel TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( InDetCosmicsFilters )

