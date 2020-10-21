# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBDetDescrCnv )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( TBDetDescrCnv
                     src/TB*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} DetDescrCnvSvcLib GaudiKernel StoreGateLib GeoPrimitives TBDetDescr )

# Install files from the package:
atlas_install_joboptions( share/*.py )
