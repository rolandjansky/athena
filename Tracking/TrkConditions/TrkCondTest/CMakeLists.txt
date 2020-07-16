################################################################################
# Package: TrkCondTest
################################################################################

# Declare the package name:
atlas_subdir( TrkCondTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   GaudiKernel
   Tracking/TrkDetDescr/TrkDetDescrInterfaces
   Tracking/TrkDetDescr/TrkDetDescrUtils
   Tracking/TrkDetDescr/TrkGeometry
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   PRIVATE
    )

find_package( Eigen )

# Component(s) in the package:

atlas_add_component( TrackingGeometryCondAlgTest
   TrkCondTest/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} 
   AthenaBaseComps
   AthenaKernel
   GaudiKernel 
   TrkDetDescrInterfaces
   TrkDetDescrUtils
   TrkGeometry
   InDetReadoutGeometry
    )

# Install files from the package:
atlas_install_joboptions( share/*.py )
