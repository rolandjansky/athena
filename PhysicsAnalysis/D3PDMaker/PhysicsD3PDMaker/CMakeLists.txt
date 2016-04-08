################################################################################
# Package: PhysicsD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( PhysicsD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( PhysicsD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODEgamma xAODMuon GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

