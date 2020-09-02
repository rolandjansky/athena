################################################################################
# Package: MuonHoughPatternTools
################################################################################

# Declare the package name:
atlas_subdir( MuonHoughPatternTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/MuonClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonHoughPatternEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventPrimitives
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTruth
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTruthData 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonHoughPatternToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonHoughPatternTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS 
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MuonDetDescrUtils MuonHoughPatternEvent MuonPattern MuonPrepRawData MuonSegment MuonRecToolInterfaces MuonLayerHough TrkDriftCircleMath MuonIdHelpersLib MuonClusterizationLib MuonRecHelperToolsLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib CxxUtils EventPrimitives xAODMuon xAODTruth MuonReadoutGeometry MuonRIO_OnTrack TrkSurfaces TrkTruthData )

atlas_add_component( MuonHoughPatternTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AtlasHepMCLib AthenaBaseComps GeoPrimitives GaudiKernel MuonDetDescrUtils MuonIdHelpersLib MuonClusterizationLib MuonHoughPatternEvent MuonPattern MuonPrepRawData MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLayerHough TrkDriftCircleMath CxxUtils StoreGateLib SGtests EventPrimitives xAODMuon xAODTruth MuonReadoutGeometry MuonRIO_OnTrack TrkSurfaces TrkTruthData MuonHoughPatternToolsLib )

