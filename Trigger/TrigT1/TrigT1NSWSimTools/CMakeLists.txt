################################################################################
# Package: TrigT1NSWSimTools
################################################################################

# Declare the package name:
atlas_subdir( TrigT1NSWSimTools )

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
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimEvent
                          MuonSpectrometer/MuonSimData
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( Boost )
find_package( TBB )
find_package( HepMC )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrigT1NSWSimTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES} ${TBB_LIBRARIES} ${HEPMC_LIBRARIES} 
                     SGTools GaudiKernel AthenaBaseComps AthenaKernel AGDDKernel GeoPrimitives Identifier EventInfo MuonAGDDDescription MuonReadoutGeometry
                     MuonDigitContainer MuonIdHelpers MuonSimEvent MuonSimData )

# Install files from the package:
atlas_install_headers( TrigT1NSWSimTools )

