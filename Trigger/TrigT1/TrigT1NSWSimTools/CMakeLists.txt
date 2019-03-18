################################################################################
# Package: TrigT1NSWSimTools
################################################################################

# Declare the package name:
atlas_subdir( TrigT1NSWSimTools )
set(CMAKE_CXX_FLAGS "-std=c++14")
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventInfo
                          DetectorDescription/RegionSelector
                          MuonSpectrometer/MuonDetDescr/MuonRegionSelector
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimEvent
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonRDO
                          Simulation/G4Sim/TrackRecord
                          )

# External dependencies:
find_package( Boost )
find_package( TBB )
find_package( HepMC )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Library in the package
atlas_add_library( TrigT1NSWSimToolsLib
                     src/*.cxx
                     PUBLIC_HEADERS TrigT1NSWSimTools
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES} ${TBB_LIBRARIES} ${HEPMC_LIBRARIES} 
                     SGTools GaudiKernel AthenaBaseComps AthenaKernel AGDDKernel GeoPrimitives Identifier EventInfo MuonAGDDDescription MuonReadoutGeometry
                     MuonDigitContainer MuonIdHelpersLib MuonSimEvent MuonSimData MuonRDO   RegionSelector )


# Component(s) in the package:
atlas_add_component( TrigT1NSWSimTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES} ${TBB_LIBRARIES} ${HEPMC_LIBRARIES} 
                     SGTools GaudiKernel AthenaBaseComps AthenaKernel AGDDKernel GeoPrimitives Identifier EventInfo MuonAGDDDescription MuonReadoutGeometry
                     MuonDigitContainer MuonIdHelpersLib MuonSimEvent MuonSimData MuonRDO RegionSelector  )






# Install files from the package:
atlas_install_headers( TrigT1NSWSimTools )
