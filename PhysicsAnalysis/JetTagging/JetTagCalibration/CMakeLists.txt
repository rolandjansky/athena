################################################################################
# Package: JetTagCalibration
################################################################################

# Declare the package name:
atlas_subdir( JetTagCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Database/APR/FileCatalog
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/PoolSvc
                          Reconstruction/MVAUtils
                          DetectorDescription/DetDescrCond/DetDescrCondTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Eigen )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library( JetTagCalibrationLib
                   src/*.cxx
                   PUBLIC_HEADERS JetTagCalibration
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${LWTNN_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests FileCatalog MVAUtils
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities PoolSvcLib )

atlas_add_component( JetTagCalibration
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel FileCatalog AthenaPoolUtilities JetTagCalibrationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
