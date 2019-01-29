################################################################################
# Package: TrkAlgebraUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkAlgebraUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( BLAS )
find_package( LAPACK )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Matrix Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkAlgebraUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlgebraUtils
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} ${EIGEN_LIBRARIES} GaudiKernel ${CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES})


