################################################################################
# Package: xAODBTaggingCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODBTagging
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Trigger/TrigEvent/TrigParticle )

atlas_add_library( xAODBTaggingCnvLib
                   xAODBTaggingCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODBTaggingCnv
                   LINK_LIBRARIES GaudiKernel xAODBTagging )


# Component(s) in the package:
atlas_add_component( xAODBTaggingCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODBTaggingCnvLib xAODBTagging AthenaBaseComps AthenaKernel GaudiKernel TrigParticle )

