################################################################################
# Package: sTGC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( sTGC_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Event/EventInfoMgt
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonSimData
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( sTGC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib Identifier xAODEventInfo GaudiKernel MuonSimEvent HitManagement AtlasCLHEP_RandomGenerators StoreGateLib SGtests EventInfo GeneratorObjects MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData PathResolver TrkDetDescrUtils TrkSurfaces TrkEventPrimitives )

# Install files from the package:
atlas_install_headers( sTGC_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )
atlas_install_runtime( share/*.config )

