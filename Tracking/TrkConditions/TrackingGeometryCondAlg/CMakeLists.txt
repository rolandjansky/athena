################################################################################
# Package: TrackingGeometryCondAlg
################################################################################

# Declare the package name:
atlas_subdir( TrackingGeometryCondAlg )

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
   Tracking/TrkDetDescr/TrkDetDescrSvc
    )

find_package( Eigen )

# Component(s) in the package:

atlas_add_component( TrackingGeometryCondAlg
   TrackingGeometryCondAlg/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} 
   AthenaBaseComps
   AthenaKernel
   GaudiKernel 
   TrkDetDescrInterfaces
   TrkDetDescrUtils
   TrkGeometry
   InDetReadoutGeometry
   TrkDetDescrSvcLib
    )

# Install files from the package:
atlas_install_python_modules( python/*.py )

