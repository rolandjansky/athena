################################################################################
# Package: xAODBTaggingCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODBTagging
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   GaudiKernel
   Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( xAODBTaggingCnv
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES xAODBTagging AthenaBaseComps AthenaKernel GaudiKernel
   TrigParticle )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_headers( xAODBTaggingCnv )
