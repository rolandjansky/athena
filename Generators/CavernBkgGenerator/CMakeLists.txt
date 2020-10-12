# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CavernBkgGenerator )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS minicern )

# tag $(as_needed_tag) was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( CavernBkgGeneratorLib
                   src/GenerateCavernBkg.cxx
                   src/MuonBackgroundEventParser.cxx
                   src/MuonBackgroundConverter.cxx
                   src/gbkgout.F
                   src/cavbkg.F
                   src/atlrgeom.F
                   src/vdist.F
                   PUBLIC_HEADERS CavernBkgGenerator
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GaudiKernel ${ROOT_LIBRARIES} GeneratorModulesLib )

atlas_add_component( CavernBkgGenerator
                     src/components/*.cxx
                     LINK_LIBRARIES CavernBkgGeneratorLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

