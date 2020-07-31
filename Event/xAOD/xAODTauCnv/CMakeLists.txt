################################################################################
# Package: xAODTauCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTauCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODTau
                          Reconstruction/tauEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
				 )

atlas_add_library( xAODTauCnvLib
                   xAODTauCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTauCnv
                   LINK_LIBRARIES GaudiKernel AthContainers xAODTau tauEvent )


# Component(s) in the package:
atlas_add_component( xAODTauCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTauCnvLib xAODTau tauEvent AthenaBaseComps AthenaKernel GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

