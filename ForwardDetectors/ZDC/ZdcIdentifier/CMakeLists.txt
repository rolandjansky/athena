################################################################################
# Package: ZdcIdentifier
################################################################################

# Declare the package name:
atlas_subdir( ZdcIdentifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          DetectorDescription/IdDictParser
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( ZdcIdentifier
                   src/*.cxx
                   PUBLIC_HEADERS ZdcIdentifier
                   LINK_LIBRARIES AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES AthenaKernel IdDictParser GaudiKernel )

atlas_add_executable( test_zdc_id
                      test/test_zdc_id.cxx
                      LINK_LIBRARIES AtlasDetDescr IdDict Identifier AthenaKernel IdDictParser GaudiKernel ZdcIdentifier )

