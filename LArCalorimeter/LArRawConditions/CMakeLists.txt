# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRawConditions )

# Component(s) in the package:
atlas_add_library( LArRawConditions
                   src/*.cxx
                   PUBLIC_HEADERS LArRawConditions
                   LINK_LIBRARIES CaloIdentifier AthenaKernel AthContainers AthenaPoolUtilities Identifier GaudiKernel LArIdentifier StoreGateLib LArElecCalib LArCablingLib )

atlas_add_dictionary( LArRawConditions1Dict
                      LArRawConditions/LArRawConditionsDict1.h
                      LArRawConditions/selection1.xml
                      LINK_LIBRARIES LArRawConditions )

atlas_add_dictionary( LArRawConditions2Dict
                      LArRawConditions/LArRawConditionsDict2.h
                      LArRawConditions/selection2.xml
                      LINK_LIBRARIES LArRawConditions )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( LArConditionsContainerDB_test
   SOURCES test/LArConditionsContainerDB_test.cxx
   LINK_LIBRARIES LArRawConditions )

atlas_add_test( LArCompactSubset_test
   SOURCES test/LArCompactSubset_test.cxx
   LINK_LIBRARIES LArRawConditions )

atlas_add_test( LArConditionsSubset_test
   SOURCES test/LArConditionsSubset_test.cxx
   LINK_LIBRARIES LArRawConditions )

atlas_add_test( LArMCSym_test
   SOURCES test/LArMCSym_test.cxx
   LINK_LIBRARIES LArRawConditions IdDictParser )

