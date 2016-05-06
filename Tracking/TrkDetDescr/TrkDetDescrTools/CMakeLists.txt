################################################################################
# Package: TrkDetDescrTools
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkVolumes
                          PRIVATE
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkDetDescrTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry TrkVolumes EventPrimitives TrkSurfaces )

# Install files from the package:
atlas_install_headers( TrkDetDescrTools )

