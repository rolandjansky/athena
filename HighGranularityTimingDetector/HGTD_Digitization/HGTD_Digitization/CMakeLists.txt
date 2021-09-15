################################################################################
# Package: HGTD_Digitization
################################################################################

# Declare the package name:
atlas_subdir( HGTD_Digitization )

# External dependencies:
# find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
# find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_component( HGTD_Digitization
                  src/*.cxx
                  src/components/*.cxx
                  INCLUDE_DIRS HGTD_Digitization ${CLHEP_INCLUDE_DIRS}
                  LINK_LIBRARIES ${CLHEP_LIBRARIES} PileUpToolsLib HGTD_RawData
                  HitManagement AthenaBaseComps ReadoutGeometryBase SiDigitization
                  HGTD_Identifier HGTD_ReadoutGeometry GeneratorObjects GaudiKernel
                  InDetSimData InDetSimEvent AthenaKernel AtlasCLHEP_RandomGenerators)

atlas_install_headers( HGTD_Digitization )
