################################################################################
# Package: LArElecCalib
################################################################################

# Declare the package name:
atlas_subdir( LArElecCalib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library ( LArElecCalib
                    INTERFACE
                    PUBLIC_HEADERS LArElecCalib
                    INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                    LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloIdentifier AthenaKernel Identifier GaudiKernel LArIdentifier )

# Component(s) in the package:
atlas_add_dictionary( LArElecCalibDict
                      LArElecCalib/LArElecCalibDict.h
                      LArElecCalib/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloIdentifier AthenaKernel Identifier GaudiKernel LArIdentifier )

# Install files from the package:
atlas_install_headers( LArElecCalib )

