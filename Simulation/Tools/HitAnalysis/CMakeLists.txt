################################################################################
# Package: HitAnalysis
################################################################################

# Declare the package name:
atlas_subdir( HitAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          DetectorDescription/GeoModel/GeoAdaptors
                          Event/EventInfo
                          ForwardDetectors/AFP/AFP_SimEv
                          ForwardDetectors/ALFA/ALFA_SimEv
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          Generators/GeneratorObjects
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/G4Sim/TrackRecord
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( HitAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel CaloDetDescrLib CaloIdentifier CaloSimEvent AthenaBaseComps GeoAdaptors EventInfo AFP_SimEv ALFA_SimEv LUCID_SimEvent ZDC_SimEvent GeneratorObjects InDetSimEvent LArSimEvent MuonSimEvent TileDetDescr TileSimEvent )

# Install files from the package:
atlas_install_headers( HitAnalysis )
atlas_install_joboptions( share/*.py )

