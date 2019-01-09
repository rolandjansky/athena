################################################################################
# Package: SCT_ModuleDistortions
################################################################################

# Declare the package name:
atlas_subdir( SCT_ModuleDistortions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_ModuleDistortions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives Identifier GaudiKernel InDetReadoutGeometry TrkEventPrimitives TrkParameters AthenaBaseComps StoreGateLib SGtests InDetIdentifier PathResolver )

atlas_add_test( TestSCT_DistortionsTool
                SCRIPT athena.py --threads=1 SCT_ModuleDistortions/TestSCT_DistortionsTool.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=1 )

# Install files from the package:
atlas_install_headers( SCT_ModuleDistortions )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt )
