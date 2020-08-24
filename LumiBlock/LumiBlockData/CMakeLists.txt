# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LumiBlockData )

# Component(s) in the package:
atlas_add_library( LumiBlockData
                   src/*.cxx
                   PUBLIC_HEADERS LumiBlockData
                   LINK_LIBRARIES AthenaKernel AthContainers )

atlas_add_dictionary( LumiBlockDataDict
                      LumiBlockData/LumiBlockDataDict.h
                      LumiBlockData/selection.xml
                      LINK_LIBRARIES LumiBlockData )

# Tests in the package:
atlas_add_test( LBDurationCondData_test
  SOURCES test/LBDurationCondData_test.cxx
  LINK_LIBRARIES LumiBlockData )

atlas_add_test( LuminosityCondData_test
  SOURCES test/LuminosityCondData_test.cxx
  LINK_LIBRARIES LumiBlockData )

atlas_add_test( TrigLiveFractionCondData_test
  SOURCES test/TrigLiveFractionCondData_test.cxx
  LINK_LIBRARIES LumiBlockData )
