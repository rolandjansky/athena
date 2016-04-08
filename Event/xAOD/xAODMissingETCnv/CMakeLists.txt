################################################################################
# Package: xAODMissingETCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODMissingETCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODMissingET
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Reconstruction/MissingETEvent )

# Component(s) in the package:
atlas_add_component( xAODMissingETCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODMissingET GaudiKernel AthenaBaseComps MissingETEvent )

# Install files from the package:
atlas_install_headers( xAODMissingETCnv )
atlas_install_joboptions( share/*.py )

