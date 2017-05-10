################################################################################
# Package: PanTauAlgs
################################################################################

# Declare the package name:
atlas_subdir( PanTauAlgs )

# Declare the package's dependencies:
set( extra_deps_public )
set( extra_deps_private )

if( XAOD_ANALYSIS )
  set( extra_deps_public
    Control/AthToolSupport/AsgTools/AsgTools
    )
else()
  set( extra_deps_public
    GaudiKernel
    Control/AthenaBaseComps
    Event/NavFourMom
    Event/xAOD/xAODPFlow
    )
  set( extra_deps_private
    Calorimeter/CaloEvent
    Control/AthenaKernel
    Control/DataModel
    Control/StoreGate
    DetectorDescription/GeoPrimitives
    Event/FourMom
    Event/FourMomUtils
    Reconstruction/eflowEvent
    Reconstruction/MVAUtils
    Reconstruction/tauEvent
    Tracking/TrkEvent/VxVertex
    Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
    )
endif()

atlas_depends_on_subdirs( PUBLIC
  ${extra_deps_public}
  Event/xAOD/xAODTau
  Event/xAOD/xAODParticleEvent
  PRIVATE
  ${extra_deps_private}
  Event/xAOD/xAODTracking
  Reconstruction/Particle
  Reconstruction/tauRecTools
  Tools/PathResolver
  )

# External dependencies:
if( NOT XAOD_ANALYSIS )
  find_package( CLHEP )
endif()
find_package( Eigen )
find_package( ROOT COMPONENTS MathCore MathMore Matrix Core Tree Hist RIO pthread Minuit Minuit2 Physics HistPainter Rint )

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

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
if( NOT XAOD_STANDALONE )
  atlas_add_component( PanTauAlgs
    src/*.cxx
    Root/*.cxx
    src/components/*.cxx ${PanTauAlgsCintDict}
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps NavFourMom xAODPFlow xAODTau xAODParticleEvent GaudiKernel CaloEvent AthenaKernel DataModel StoreGateLib SGtests GeoPrimitives FourMom FourMomUtils xAODTracking Particle eflowEvent MVAUtils tauEvent tauRecToolsLib PathResolver VxVertex TrkVertexFitterInterfaces )
else()
  atlas_add_library( PanTauAlgs
    PanTauAlgs/*.h Root/*.cxx ${PanTauAlgsCintDict}
    PUBLIC_HEADERS PanTauAlgs
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTau xAODTracking AsgTools tauRecToolsLib xAODParticleEvent MVAUtils PathResolver
    )
endif()


# Install files from the package:
atlas_install_headers( PanTauAlgs )
atlas_install_runtime( data/weights/* )
atlas_install_python_modules( python/*.py )

