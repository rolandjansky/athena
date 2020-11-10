# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package.
atlas_subdir( TopEvent )

# Externals.
find_package( Boost )
find_package( ROOT REQUIRED COMPONENTS Core MathCore )
find_package( FastJet COMPONENTS fastjetplugins fastjettools )
find_package( FastJetContrib COMPONENTS EnergyCorrelator Nsubjettiness )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( TopEvent
   TopEvent/*.h Root/*.cxx
   PUBLIC_HEADERS TopEvent
   LINK_LIBRARIES AthContainers xAODCore xAODEventInfo xAODTracking xAODEgamma
   xAODMuon xAODJet xAODTau xAODMissingET xAODTruth TopConfiguration TopPartons
   AsgTools xAODRootAccess JetReclusteringLib
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
   ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} CxxUtils xAODBase
   PATInterfaces JetSubStructureUtils FourMomUtils )

atlas_add_xaod_smart_pointer_dicts(
   INPUT TopEvent/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::KLFitterResultContainer" "xAOD::PseudoTopResultContainer"
              "xAOD::SystematicEventContainer" )

atlas_add_dictionary( TopEventDict
   TopEvent/TopEventDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore TopEvent
   EXTRA_FILES Root/dict/*.cxx )
