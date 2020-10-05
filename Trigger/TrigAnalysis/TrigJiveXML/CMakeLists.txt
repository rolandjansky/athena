# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigJiveXML )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AnalysisTriggerEvent AthenaBaseComps EventInfo InDetIdentifier InDetPrepRawData JiveXMLLib TrigDecisionToolLib TrigInDetEvent TrigSteeringEvent TrigT1CaloEventLib TrigT1Interfaces TrkPrepRawData xAODTrigL1Calo xAODTrigger )

# Install files from the package:
atlas_install_joboptions( share/*.py )
