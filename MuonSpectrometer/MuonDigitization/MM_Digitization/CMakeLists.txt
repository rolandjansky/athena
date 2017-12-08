################################################################################
# Package: MM_Digitization
################################################################################

# Declare the package name:
atlas_subdir( MM_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Event/EventInfoMgt
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimData
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MM_DigitizationLib
                   src/*.cxx
                   PUBLIC_HEADERS MM_Digitization
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier xAODEventInfo GaudiKernel MagFieldInterfaces MuonSimEvent HitManagement PileUpToolsLib StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} EventInfo GeneratorObjects MuonAGDDDescription MuonReadoutGeometry MuonDigitContainer MuonSimData PathResolver TrkDetDescrUtils TrkSurfaces TrkEventPrimitives )

atlas_add_component( MM_Digitization
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib GeoPrimitives Identifier xAODEventInfo GaudiKernel MagFieldInterfaces MuonSimEvent HitManagement StoreGateLib SGtests EventInfo GeneratorObjects MuonAGDDDescription MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData PathResolver TrkDetDescrUtils TrkSurfaces TrkEventPrimitives MM_DigitizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.gas )
