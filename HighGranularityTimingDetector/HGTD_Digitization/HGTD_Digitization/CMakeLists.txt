# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_Digitization )

# External dependencies:
find_package( CLHEP )

atlas_add_component( HGTD_Digitization
                  src/*.cxx
                  src/components/*.cxx
                  INCLUDE_DIRS HGTD_Digitization ${CLHEP_INCLUDE_DIRS}
                  LINK_LIBRARIES ${CLHEP_LIBRARIES} PileUpToolsLib HGTD_RawData
                  HitManagement AthenaBaseComps ReadoutGeometryBase SiDigitization
                  HGTD_Identifier HGTD_ReadoutGeometry GeneratorObjects GaudiKernel
                  InDetSimData InDetSimEvent AthenaKernel AtlasCLHEP_RandomGenerators)

atlas_install_headers( HGTD_Digitization )

atlas_install_python_modules( python/*.py )
