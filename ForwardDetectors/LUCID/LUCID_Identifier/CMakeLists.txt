################################################################################
# Package: LUCID_Identifier
################################################################################

# Declare the package name:
atlas_subdir( LUCID_Identifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( LUCID_Identifier
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_Identifier
                   LINK_LIBRARIES AthenaKernel AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

