################################################################################
# Package: AFP_LocReco
################################################################################

# Declare the package name:
atlas_subdir( AFP_LocReco )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          ForwardDetectors/AFP/AFP_DigiEv
                          ForwardDetectors/AFP/AFP_Geometry
                          ForwardDetectors/AFP/AFP_RecoEv/AFP_LocRecoEv
                          ForwardDetectors/AFP/AFP_Reconstruction/AFP_LocRecoInterfaces
                          ForwardDetectors/AFP/AFP_SiClusterTools
                          GaudiKernel
                          Generators/GeneratorObjects
			  Event/xAOD/xAODForward
			  PRIVATE
			  Control/AthLinks)

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( AFP_LocReco
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities EventInfo AFP_DigiEv AFP_Geometry AFP_LocRecoEv AFP_SiClusterTools GaudiKernel GeneratorObjects xAODForward AthLinks)

# Install files from the package:
atlas_install_headers( AFP_LocReco )
atlas_install_joboptions( share/*.py )

