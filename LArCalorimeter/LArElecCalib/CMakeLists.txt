################################################################################
# Package: LArElecCalib
################################################################################

# Declare the package name:
atlas_subdir( LArElecCalib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( LArElecCalibDict
                      LArElecCalib/LArElecCalibDict.h
                      LArElecCalib/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloIdentifier AthenaKernel SGTools Identifier GaudiKernel LArIdentifier )

# Install files from the package:
atlas_install_headers( LArElecCalib )

