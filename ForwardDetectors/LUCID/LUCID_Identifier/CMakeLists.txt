################################################################################
# Package: LUCID_Identifier
################################################################################

# Declare the package name:
atlas_subdir( LUCID_Identifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( LUCID_Identifier
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_Identifier
                   LINK_LIBRARIES SGTools AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel )

