################################################################################
# Package: TileCalibBlobObjs
################################################################################

# Declare the package name:
atlas_subdir( TileCalibBlobObjs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TileCalibBlobObjs
                   src/*.cxx
                   PUBLIC_HEADERS TileCalibBlobObjs
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils ${CORAL_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TileCalibBlobObjsDict
                      TileCalibBlobObjs/TileCalibBlobObjsDict.h
                      TileCalibBlobObjs/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} TileCalibBlobObjs )


# Test(s) in the package:
atlas_add_test( TileCalibDrawerOfc_test
                SOURCES test/TileCalibDrawerOfc_test.cxx
                LINK_LIBRARIES TileCalibBlobObjs TestTools ${CORAL_LIBRARIES} )

atlas_add_test( TileCalibDrawerFlt_test
                SOURCES test/TileCalibDrawerFlt_test.cxx
                LINK_LIBRARIES TileCalibBlobObjs TestTools ${CORAL_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py )

