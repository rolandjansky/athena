# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This package is a collection of 'duel-use' tools to calculate
# high-level flavor tagging discriminants. Because it should work both
# inside and outside Athena, nothing here can use the magnetic field,
# atlas geometry, or material maps, but neural networks etc are all
# fine.

# Declare the package name:
atlas_subdir( FlavorTagDiscriminants )

# External dependencies:
find_package( Boost COMPONENTS filesystem )
find_package( lwtnn )
find_package( ROOT COMPONENTS Core MathCore )
find_package( onnxruntime )

# Build a shared library:
atlas_add_library( FlavorTagDiscriminants
  Root/BTagJetAugmenter.cxx
  Root/BTagTrackIpAccessor.cxx
  Root/BTagAugmenterTool.cxx
  Root/BTagMuonAugmenter.cxx
  Root/BTagMuonAugmenterTool.cxx
  Root/DL2.cxx
  Root/DL2HighLevel.cxx
  Root/DL2Tool.cxx
  Root/DataPrepUtilities.cxx
  Root/OnnxUtil.cxx
  Root/GNNTool.cxx
  Root/customGetter.cxx
  Root/FlipTagEnums.cxx
  Root/AssociationEnums.cxx
  Root/VRJetOverlapDecorator.cxx
  Root/VRJetOverlapDecoratorTool.cxx
  Root/HbbTag.cxx
  Root/HbbTagTool.cxx
  Root/HbbTagConfig.cxx
  Root/HbbGraphConfig.cxx
  Root/VRJetOverlapDecorator.cxx
  Root/FTagDataDependencyNames.cxx
  PUBLIC_HEADERS FlavorTagDiscriminants
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS} ${ONNXRUNTIME_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
  LINK_LIBRARIES ${Boost_LIBRARIES} ${LWTNN_LIBRARIES} ${ONNXRUNTIME_LIBRARIES} AsgTools AthContainers AthLinks JetInterface xAODBTagging xAODEventInfo xAODJet xAODMuon xAODTracking )


if (NOT XAOD_STANDALONE)
  atlas_add_component( FlavorTagDiscriminantsLib
    src/BTagDecoratorAlg.cxx
    src/JetTagDecoratorAlg.cxx
    src/BTagToJetLinkerAlg.cxx
    src/JetToBTagLinkerAlg.cxx
    src/BTagTrackLinkCopyAlg.cxx
    src/BTaggingBuilderAlg.cxx
    src/PoorMansIpAugmenterAlg.cxx
    src/JetTagConditionalDecoratorAlg.cxx
    src/components/FlavorTagDiscriminants_entries.cxx
    LINK_LIBRARIES FlavorTagDiscriminants
    )
endif()

atlas_add_dictionary( FlavorTagDiscriminantsDict
   FlavorTagDiscriminants/FlavorTagDiscriminantsDict.h
   FlavorTagDiscriminants/selection.xml
   LINK_LIBRARIES FlavorTagDiscriminants )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
