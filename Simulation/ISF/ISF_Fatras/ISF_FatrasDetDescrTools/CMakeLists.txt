################################################################################
# Package: ISF_FatrasDetDescrTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasDetDescrTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrInterfaces
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasDetDescrToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasDetDescrTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel ISF_FatrasDetDescrInterfaces ISF_FatrasDetDescrModel TrkDetDescrInterfaces TrkDetDescrUtils TrkSurfaces StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES InDetIdentifier InDetReadoutGeometry TrkGeometry TrkVolumes )

atlas_add_component( ISF_FatrasDetDescrTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel ISF_FatrasDetDescrInterfaces ISF_FatrasDetDescrModel TrkDetDescrInterfaces TrkDetDescrUtils TrkSurfaces StoreGateLib SGtests InDetIdentifier InDetReadoutGeometry TrkGeometry TrkVolumes ISF_FatrasDetDescrToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

