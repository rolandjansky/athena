################################################################################
# Package: xAODMetaDataCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODMetaDataCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Database/AthenaPOOL/AthenaPoolKernel
                          Event/xAOD/xAODMetaData
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_component( xAODMetaDataCnv
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODMetaData GaudiKernel AthenaKernel AthenaPoolUtilities EventInfo )

# Install files from the package:
atlas_install_headers( xAODMetaDataCnv )
atlas_install_joboptions( share/*.py )

