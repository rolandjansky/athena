################################################################################
# Package: POOLCollectionToolsTest
################################################################################

# Declare the package name:
atlas_subdir( POOLCollectionToolsTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AtlasCollectionTools
                          Event/ByteStreamCnvSvc
                          PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( share/*.py test/POOLCollectionToolsTest_TestConfiguration.xml data/*logCheck* )

