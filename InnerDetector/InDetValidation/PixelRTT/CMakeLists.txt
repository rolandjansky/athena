################################################################################
# Package: PixelRTT
################################################################################

# Declare the package name:
atlas_subdir( PixelRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   InnerDetector/InDetCalibAlgs/PixelCalibAlgs
   PRIVATE
   InnerDetector/InDetConditions/PixelConditionsData
   Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist RIO Gpad )

# Component(s) in the package:
atlas_add_library( PixelValidation
   PixelRTT/*.h src/*.cxx
   PUBLIC_HEADERS PixelRTT
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelCalibAlgsLib
   PRIVATE_LINK_LIBRARIES PixelConditionsData PathResolver )

atlas_add_executable( doPixelValidation
   Application/doPixelValidation.cxx
   LINK_LIBRARIES PixelValidation )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.* )
