################################################################################
# Package: xAODTrigL1CaloCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigL1CaloCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigT1/TrigT1CaloEvent )

# Component(s) in the package:
atlas_add_component( xAODTrigL1CaloCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigL1Calo GaudiKernel AthenaBaseComps AthenaKernel TrigT1CaloEventLib )

# Install files from the package:
atlas_install_headers( xAODTrigL1CaloCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

