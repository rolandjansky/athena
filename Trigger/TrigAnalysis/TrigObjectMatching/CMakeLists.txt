# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigObjectMatching )

# External dependencies:
find_package( Boost COMPONENTS regex )

# Component(s) in the package:
atlas_add_library( TrigObjectMatchingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigObjectMatching
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES AnalysisTriggerEvent AthContainers AthenaBaseComps AthenaKernel CxxUtils EventKernel GaudiKernel JetEvent TrigCaloEvent TrigDecisionToolLib TrigMuonEvent TrigParticle TrigSteeringEvent egammaEvent muonEvent tauEvent
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_component( TrigObjectMatching
                     src/components/*.cxx
                     LINK_LIBRARIES TrigObjectMatchingLib )

atlas_add_dictionary( TrigObjectMatchingDict
                      TrigObjectMatching/TrigObjectMatchingDict.h
                      TrigObjectMatching/selection.xml
                      LINK_LIBRARIES TrigObjectMatchingLib )
