################################################################################
# Package: AthenaPoolTools
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/PersistentDataModel
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCutFlow
                          GaudiKernel
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( AthenaPoolTools
                     src/EventCount.cxx
                     src/RequireUniqueEvent.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests PersistentDataModel EventInfo xAODEventInfo GaudiKernel xAODCutFlow )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/PoolMove )

