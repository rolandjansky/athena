# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TruthHelper )

# External dependencies:
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TruthHelper
                   src/*.cxx
                   PUBLIC_HEADERS TruthHelper
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AtlasHepMCLib GaudiKernel GeneratorObjects StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel TruthUtils )

atlas_add_dictionary( TruthHelperDict
                      TruthHelper/TruthHelperDict.h
                      TruthHelper/selection.xml
                      INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                      LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AtlasHepMCLib StoreGateLib SGtests GaudiKernel GeneratorObjects AthenaKernel TruthUtils TruthHelper )

