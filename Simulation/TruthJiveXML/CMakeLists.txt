#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TruthJiveXML )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel JiveXMLLib EventPrimitives GeneratorObjects TrackRecordLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
