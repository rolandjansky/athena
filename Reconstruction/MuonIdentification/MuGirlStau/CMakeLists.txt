################################################################################
# Package: MuGirlStau
################################################################################

# Declare the package name:
atlas_subdir( MuGirlStau )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventInfoMgt
                          Event/xAOD/xAODTracking
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          Reconstruction/RecoEvent/ParticleCaloExtension
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MuGirlStauLib
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS MuGirlStau
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives EventInfo GaudiKernel MuonRIO_OnTrack MuonSegment MuonRecToolInterfaces MuGirlInterfaces TrkEventPrimitives TrkTrack TrkToolInterfaces StoreGateLib SGtests MdtCalibSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils Identifier xAODTracking MdtCalibData ParticleCaloExtension RecoToolInterfaces AtlasCLHEP_RandomGenerators PathResolver )

atlas_add_component( MuGirlStau
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives EventInfo GaudiKernel MuonRIO_OnTrack MuonSegment MuonRecToolInterfaces MuGirlInterfaces TrkEventPrimitives TrkTrack TrkToolInterfaces CxxUtils StoreGateLib SGtests Identifier xAODTracking MdtCalibData MdtCalibSvcLib ParticleCaloExtension RecoToolInterfaces AtlasCLHEP_RandomGenerators PathResolver MuGirlStauLib )

atlas_add_dictionary( iMuGirlStauDict
                      MuGirlStau/MuGirlStauDict.h
                      MuGirlStau/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives EventInfo GaudiKernel MuonRIO_OnTrack MuonSegment MuonRecToolInterfaces MuGirlInterfaces TrkEventPrimitives TrkTrack TrkToolInterfaces CxxUtils StoreGateLib SGtests Identifier xAODTracking MdtCalibData MdtCalibSvcLib ParticleCaloExtension RecoToolInterfaces AtlasCLHEP_RandomGenerators PathResolver MuGirlStauLib
                      DATA_LINKS MuGirlNS::StauContainer
                      ELEMENT_LINKS StauCollection )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/calo_calibration.data share/mdt_calibration.data share/rpc_calibration.data )

