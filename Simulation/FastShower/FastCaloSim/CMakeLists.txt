
################################################################################
# Package: FastCaloSim
################################################################################

# Declare the package name:
atlas_subdir( FastCaloSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArRecEvent
                          Simulation/FastSimulation/FastSimulationEvent
                          TileCalorimeter/TileEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Generators/GeneratorObjects
                          Simulation/FastShower/FastCaloSimAthenaPool
                          PRIVATE
                          Calorimeter/CaloTrackingGeometry
                          Control/AthAllocators
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/DetDescrCond/DetDescrCondTools
                          DetectorDescription/GeoPrimitives
                          Event/EventKernel
                          Event/NavFourMom
                          Generators/TruthUtils
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Gpad Cint Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Html Postscript Gui GX11TTF GX11 )

# this line failed automatic conversion in cmt2cmake :
# macro_append FastCaloSim_cppflags " -DFastCaloSim_project_release_v1=`echo $(FastCaloSim_project_release) | awk -F. '{printf("'"'%d'"'",$$1)}'` "

# this line failed automatic conversion in cmt2cmake :
# macro_append FastCaloSim_cppflags " -DFastCaloSim_project_release_v2=`echo $(FastCaloSim_project_release) | awk -F. '{printf("'"'%d'"'",$$2)}'` "

# this line failed automatic conversion in cmt2cmake :
# macro_append FastCaloSim_cppflags " -DFastCaloSim_project_release_v3=`echo $(FastCaloSim_project_release) | awk -F. '{printf("'"'%d'"'",$$3)}'` "

# Component(s) in the package:
atlas_add_root_dictionary( FastCaloSimLib
                           FastCaloSimLibDictSource
                           ROOT_HEADERS FastCaloSim/ParticleEnergyParametrization.h FastCaloSim/ParticleEnergyParametrizationInDistbin.h FastCaloSim/TShape_Result.h FastCaloSim/TLateralShapeCorrection.h FastCaloSim/TSplineReweight.h FastCaloSim/TCellReweight.h FastCaloSim/LinkDef.h
                           EXTERNAL_PACKAGES  ROOT HepPDT CLHEP HepMC Eigen )

atlas_add_library( FastCaloSimLib
                   src/AddNoiseCellBuilderTool.cxx
                   src/BasicCellBuilderTool.cxx
                   src/EmptyCellBuilderTool.cxx
                   src/FSStoregateClean.cxx
                   src/FastShowerCellBuilderTool.cxx
                   src/ParticleEnergyParametrization.cxx
                   src/ParticleEnergyParametrizationInDistbin.cxx
                   src/TShape_Result.cxx
                   src/ParticleEnergyShape.cxx
                   src/TLateralShapeCorrection.cxx
                   src/TSplineReweight.cxx
                   src/TCellReweight.cxx
                   ${FastCaloSimLibDictSource}
                   PUBLIC_HEADERS FastCaloSim
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel Identifier GaudiKernel LArRecEvent TileEvent TrkEventPrimitives TrkParameters TrkExInterfaces CaloDetDescrLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthAllocators AthenaPoolUtilities AtlasDetDescr GeoPrimitives EventKernel NavFourMom GeneratorObjects TruthUtils FastCaloSimAthenaPool PathResolver TrkGeometry TrkSurfaces TrkMaterialOnTrack )

atlas_add_component( FastCaloSim
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel LArRecEvent TileEvent TrkEventPrimitives TrkParameters TrkExInterfaces AthAllocators AthenaPoolUtilities AtlasDetDescr GeoPrimitives EventKernel NavFourMom GeneratorObjects TruthUtils FastCaloSimAthenaPool PathResolver TrkGeometry TrkSurfaces TrkMaterialOnTrack FastCaloSimLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


atlas_add_test( FastShowerCellBuilderTool_test
  SCRIPT test/FastShowerCellBuilderTool_test.sh
  PROPERTIES TIMEOUT 600
  EXTRA_PATTERNS "Reading file|FastCaloSimFactory|FileName|default:|DATAPATH|Cache alignment|map from|Database being retired|Resolved path|^Domain|HepPDT Version|from file" )
