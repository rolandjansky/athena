################################################################################
# Package: RPC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( RPC_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/PileUpTools
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement
                          PRIVATE
                          Control/AthenaKernel
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc
                          Event/EventInfo
                          Event/EventInfoMgt
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          Tools/PathResolver )

# External dependencies:
find_package( AIDA )
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( RPC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps PileUpToolsLib xAODEventInfo GaudiKernel MuonSimData MuonSimEvent HitManagement AthenaKernel EventInfo GeneratorObjects MuonCondInterface MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib PathResolver )

# Install files from the package:
atlas_install_headers( RPC_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/RPC_Digitization_jobOptions.txt share/*.py )
atlas_install_runtime( share/G4RPC_Digitizer.txt share/PermanentDeadPanels.txt share/PermanentGoodPanels.txt )

