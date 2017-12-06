################################################################################
# Package: EventLoopTest
################################################################################

# Declare the package name:
atlas_subdir( EventLoopTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaExamples/AthExBeginRun )

# Install files from the package:
atlas_install_joboptions( share/*.py )

