################################################################################
# Package: xAODTauCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTauCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          Event/xAOD/xAODTau
                          Reconstruction/tauEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
				 )

# Component(s) in the package:
atlas_add_component( xAODTauCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES DataModel xAODTau tauEvent AthenaBaseComps AthenaKernel GaudiKernel )

# Install files from the package:
atlas_install_headers( xAODTauCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

