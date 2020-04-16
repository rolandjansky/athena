# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArElecCalib )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library ( LArElecCalib
                    LArElecCalib/*.h
                    INTERFACE
                    PUBLIC_HEADERS LArElecCalib
                    INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                    LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel CxxUtils GaudiKernel Identifier LArIdentifier )

atlas_add_dictionary( LArElecCalibDict
                      LArElecCalib/LArElecCalibDict.h
                      LArElecCalib/selection.xml
                      LINK_LIBRARIES LArElecCalib )
