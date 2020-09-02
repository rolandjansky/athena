################################################################################
# Package: ISF_FatrasDetDescrModel
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasDetDescrModel )

# External dependencies:
find_package( Eigen )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasDetDescrModel
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasDetDescrModel
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoPrimitives Identifier EventPrimitives InDetIdentifier InDetReadoutGeometry ISF_FatrasDetDescrInterfaces TrkDetDescrUtils TrkDetElementBase TrkGeometry TrkSurfaces TrkEventPrimitives TrkParameters StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

