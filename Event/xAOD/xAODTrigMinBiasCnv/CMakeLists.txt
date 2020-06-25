################################################################################
# Package: xAODTrigMinBiasCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMinBiasCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigMinBias
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent )

atlas_add_library( xAODTrigMinBiasCnvLib
                   xAODTrigMinBiasCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigMinBiasCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigMinBias )


# Component(s) in the package:
atlas_add_component( xAODTrigMinBiasCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigMinBiasCnvLib xAODTrigMinBias GaudiKernel AthenaBaseComps AthenaKernel TrigCaloEvent TrigInDetEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

