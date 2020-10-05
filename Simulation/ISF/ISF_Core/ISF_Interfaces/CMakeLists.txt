################################################################################
# Package: ISF_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( ISF_InterfacesLib
                   src/*.cxx
                   PUBLIC_HEADERS ISF_Interfaces
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel AtlasDetDescr GeoPrimitives GaudiKernel GeneratorObjects ISF_Event StoreGateLib SGtests TrackRecordLib )

