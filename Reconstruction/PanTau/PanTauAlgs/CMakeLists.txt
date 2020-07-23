################################################################################
# Package: PanTauAlgs
################################################################################

# Declare the package name:
atlas_subdir( PanTauAlgs )

# External dependencies:
if( NOT XAOD_ANALYSIS )
  find_package( CLHEP )
endif()
find_package( Eigen )
find_package( ROOT COMPONENTS MathMore Matrix Core Tree Hist RIO pthread Minuit Minuit2 Physics HistPainter Rint )

atlas_add_root_dictionary( PanTauAlgs PanTauAlgsCintDict
  ROOT_HEADERS
  PanTauAlgs/PanTauProcessor.h
  PanTauAlgs/Tool_DecayModeDeterminator.h
  PanTauAlgs/Tool_DetailsArranger.h
  PanTauAlgs/Tool_FeatureExtractor.h
  PanTauAlgs/Tool_InformationStore.h
  PanTauAlgs/Tool_InputConverter.h
  PanTauAlgs/Tool_ModeDiscriminator.h
  PanTauAlgs/Tool_TauConstituentGetter.h
  PanTauAlgs/Tool_TauConstituentSelector.h
  PanTauAlgs/PanTauSeed.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES ROOT
  )

if ( XAOD_STANDALONE )
  atlas_add_library( PanTauAlgs
    PanTauAlgs/*.h Root/*.cxx ${PanTauAlgsCintDict}
    PUBLIC_HEADERS PanTauAlgs
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTau xAODTracking AsgTools tauRecToolsLib xAODParticleEvent MVAUtils PathResolver
    )
endif()

# Component(s) in the package:
if( NOT XAOD_STANDALONE )
  atlas_add_component( PanTauAlgs
    src/*.cxx
    Root/*.cxx
    src/components/*.cxx ${PanTauAlgsCintDict}
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps NavFourMom xAODPFlow xAODTau xAODParticleEvent GaudiKernel CaloEvent AthenaKernel StoreGateLib SGtests GeoPrimitives FourMom FourMomUtils xAODTracking Particle eflowEvent MVAUtils tauEvent tauRecToolsLib PathResolver VxVertex TrkVertexFitterInterfaces )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
