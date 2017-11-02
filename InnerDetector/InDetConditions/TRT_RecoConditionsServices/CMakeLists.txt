################################################################################
# Package: TRT_RecoConditionsServices
################################################################################

# Declare the package name:
atlas_subdir( TRT_RecoConditionsServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TRT_RecoConditionsServices
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} GaudiKernel TrkParameters AthenaBaseComps StoreGateLib SGtests InDetIdentifier InDetReadoutGeometry TRT_ConditionsServicesLib )
