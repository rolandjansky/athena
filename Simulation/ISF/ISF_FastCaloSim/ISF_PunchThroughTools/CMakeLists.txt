################################################################################
# Package: ISF_PunchThroughTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_PunchThroughTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Simulation/Barcode/BarcodeEvent
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimInterfaces
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimEvent
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Control/DataModel
                          GaudiKernel
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_PunchThroughTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives ISF_FastCaloSimInterfaces ISF_FastCaloSimEvent DataModel GaudiKernel ISF_Event ISF_Interfaces PathResolver )

# Install files from the package:
atlas_install_headers( ISF_PunchThroughTools )
atlas_install_runtime( Data/*.root )
