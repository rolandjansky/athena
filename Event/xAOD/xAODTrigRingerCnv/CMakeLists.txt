################################################################################
# Package: xAODTrigRingerCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigRingerCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigRinger
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigEvent/TrigCaloEvent )

# Component(s) in the package:
atlas_add_component( xAODTrigRingerCnv
                     src/*.cxx
                     LINK_LIBRARIES xAODTrigRinger GaudiKernel AthenaBaseComps AthenaKernel TrigCaloEvent )

# Install files from the package:
atlas_install_headers( xAODTrigRingerCnv )
atlas_install_joboptions( share/*.py )

