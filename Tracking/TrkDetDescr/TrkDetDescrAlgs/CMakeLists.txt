################################################################################
# Package: TrkDetDescrAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag use_trkdetdescr_memmon was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( TrkDetDescrAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrUtils TrkDetDescrInterfaces TrkGeometry TrkVolumes TrkNeutralParameters TrkExInterfaces )

# Install files from the package:
atlas_install_headers( TrkDetDescrAlgs )

