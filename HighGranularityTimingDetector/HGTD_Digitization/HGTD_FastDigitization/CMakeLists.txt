# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_FastDigitization )

# External(s).
find_package( CLHEP )
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package.
atlas_add_component( HGTD_FastDigitization
   HGTS_FastDigitization/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel AthenaKernel
   HGTD_PrepRawData HitManagement InDetSimEvent PileUpToolsLib EventPrimitives
   HGTD_Identifier HGTD_ReadoutGeometry TrkTruthData )

# Install files from the package.
atlas_install_python_modules( python/*.py )
