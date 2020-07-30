# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DetDescrConditions )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DetDescrConditions
                   src/AlignableTransform.cxx
                   src/DetCondFloat.cxx
                   src/DetCondCFloat.cxx
                   src/DetCondKeyTrans.cxx
                   src/DetStatus.cxx
                   src/DetStatusMap.cxx
                   PUBLIC_HEADERS DetDescrConditions
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel AthenaPoolUtilities GaudiKernel Identifier )

atlas_add_dictionary( DetDescrConditionsDict
                      DetDescrConditions/DetDescrConditionsDict.h
                      DetDescrConditions/selection.xml
                      LINK_LIBRARIES DetDescrConditions )
