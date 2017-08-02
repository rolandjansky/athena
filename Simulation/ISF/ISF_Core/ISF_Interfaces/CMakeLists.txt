################################################################################
# Package: ISF_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_Interfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
			  Generators/GeneratorObjects
                          GaudiKernel
                          Generators/GeneratorObjects
                          Simulation/Barcode/BarcodeEvent
                          Simulation/ISF/ISF_Core/ISF_Event )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( ISF_Interfaces
                   src/*.cxx
                   PUBLIC_HEADERS ISF_Interfaces
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel AtlasDetDescr GeoPrimitives GaudiKernel GeneratorObjects ISF_Event StoreGateLib SGtests )

