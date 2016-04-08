################################################################################
# Package: ISF_FatrasDetDescrModel
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasDetDescrModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasDetDescrModel
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasDetDescrModel
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} SGTools GeoPrimitives Identifier EventPrimitives InDetIdentifier InDetReadoutGeometry ISF_FatrasDetDescrInterfaces TrkDetDescrUtils TrkDetElementBase TrkGeometry TrkSurfaces TrkEventPrimitives TrkParameters StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

