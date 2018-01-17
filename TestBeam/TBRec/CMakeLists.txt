################################################################################
# Package: TBRec
################################################################################

# Declare the package name:
atlas_subdir( TBRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArCabling
                          TestBeam/TBEvent
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloDmDetDescr
                          Calorimeter/CaloSimEvent
                          Calorimeter/CaloTrackingGeometry
                          Database/CondDBObjects
                          Event/xAOD/xAODCaloEvent
                          LArCalorimeter/LArG4TB/LArG4TBSimEvent
                          Reconstruction/Particle
                          Simulation/G4Sim/TrackRecord
                          TestBeam/TBCaloGeometry
                          TestBeam/TBConditions/TBCondRunPar
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_root_dictionary( TBRecLib
                           TBRecLibDictSource
                           ROOT_HEADERS TBRec/TBH6RunHeader.h TBRec/LinkDef.h
                           EXTERNAL_PACKAGES  ROOT Boost CLHEP Eigen )

atlas_add_library( TBRecLib
                   src/*.cxx
                   ${TBRecLibDictSource}
                   PUBLIC_HEADERS TBRec
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel AthContainers GeoPrimitives Identifier EventInfo xAODTracking GaudiKernel LArIdentifier LArRawEvent TBEvent TrkDetDescrUtils TrkParameters TrkTrack CaloRecLib CaloUtilsLib StoreGateLib SGtests LArRawUtilsLib LArCablingLib CaloDetDescrLib TBCaloGeometryLib TBCondRunParLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloDmDetDescr CaloSimEvent CondDBObjects xAODCaloEvent LArG4TBSimEvent Particle PathResolver TrkSurfaces TrkEventPrimitives TrkExInterfaces )

atlas_add_component( TBRec
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier CaloRecLib CaloUtilsLib AthenaBaseComps AthenaKernel AthContainers StoreGateLib SGtests GeoPrimitives Identifier EventInfo xAODTracking GaudiKernel LArIdentifier LArRawEvent LArRawUtilsLib LArCablingLib TBEvent TrkDetDescrUtils TrkParameters TrkTrack CaloDetDescrLib CaloDmDetDescr CaloSimEvent CondDBObjects xAODCaloEvent LArG4TBSimEvent Particle TBCaloGeometryLib TBCondRunParLib PathResolver TrkSurfaces TrkEventPrimitives TrkExInterfaces TBRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.xml )

