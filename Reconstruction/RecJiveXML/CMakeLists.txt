# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecJiveXML )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( RecJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkParameters JiveXMLLib CaloEvent Navigation JetTagEvent JetEvent MissingETEvent TrkTrack )

# Install files from the package:
atlas_install_joboptions( share/*.py )
