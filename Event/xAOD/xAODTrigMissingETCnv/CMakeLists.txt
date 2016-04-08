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

# Component(s) in the package:
atlas_add_component( xAODTrigMissingETCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigMissingET GaudiKernel AthenaBaseComps AthenaKernel TrigMissingEtEvent )

# Install files from the package:
atlas_install_headers( xAODTrigMissingETCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

