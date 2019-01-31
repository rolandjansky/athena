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
                          MuonSpectrometer/MuonRDO
                          Simulation/G4Sim/TrackRecord )

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
                     MuonDigitContainer MuonIdHelpersLib MuonSimEvent MuonSimData MuonRDO )


# Component(s) in the package:
atlas_add_component( TrigT1NSWSimTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES} ${TBB_LIBRARIES} ${HEPMC_LIBRARIES} 
                     SGTools GaudiKernel AthenaBaseComps AthenaKernel AGDDKernel GeoPrimitives Identifier EventInfo MuonAGDDDescription MuonReadoutGeometry
                     MuonDigitContainer MuonIdHelpersLib MuonSimEvent MuonSimData MuonRDO TrigT1NSWSimToolsLib)


#loop over each cxx file in the util directory of this package
  file (GLOB util_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/util" "${CMAKE_CURRENT_SOURCE_DIR}/util/[a-zA-Z0-9]*.cxx")
  foreach (SOURCE ${util_sources})	
    #use the name of the file as the name of the executable (e.g. myApplication.cxx will be executed with: myApplication)
    string (REGEX REPLACE ".cxx$" "" FILE ${SOURCE})
    atlas_add_executable (${FILE} SOURCES util/${FILE}.cxx LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES} ${TBB_LIBRARIES} ${HEPMC_LIBRARIES}
                          SGTools GaudiKernel AthenaBaseComps AthenaKernel AGDDKernel GeoPrimitives Identifier EventInfo MuonAGDDDescription MuonReadoutGeometry
                          MuonDigitContainer MuonIdHelpersLib MuonSimEvent MuonSimData MuonRDO TrigT1NSWSimToolsLib)
  endforeach (SOURCE ${util_sources})



# Install files from the package:
atlas_install_headers( TrigT1NSWSimTools )
