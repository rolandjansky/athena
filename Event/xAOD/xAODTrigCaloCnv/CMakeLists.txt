################################################################################
# Package: xAODTrigCaloCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigCaloCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigCalo
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigEvent/TrigCaloEvent )

atlas_add_library( xAODTrigCaloCnvLib
                   xAODTrigCaloCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigCaloCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigCalo )


# Component(s) in the package:
atlas_add_component( xAODTrigCaloCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigCaloCnvLib xAODTrigCalo GaudiKernel AthenaBaseComps AthenaKernel TrigCaloEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

