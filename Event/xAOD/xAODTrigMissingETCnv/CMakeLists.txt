################################################################################
# Package: xAODTrigMissingETCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMissingETCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigMissingET
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigEvent/TrigMissingEtEvent )

atlas_add_library( xAODTrigMissingETCnvLib
                   xAODTrigMissingETCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigMissingETCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigMissingET xAODCore )


# Component(s) in the package:
atlas_add_component( xAODTrigMissingETCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigMissingETCnvLib xAODTrigMissingET GaudiKernel AthenaBaseComps AthenaKernel TrigMissingEtEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

