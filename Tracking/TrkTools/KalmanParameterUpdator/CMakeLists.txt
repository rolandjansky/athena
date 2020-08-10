################################################################################
# Package: TrkMeasurementUpdator
################################################################################

# Declare the package name:
atlas_subdir( KalmanParameterUpdator )


# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( KalmanParameterUpdatorLib
                   src/*.cxx
                   PUBLIC_HEADERS KalmanParameterUpdator
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkEventPrimitives TrkParameters)


