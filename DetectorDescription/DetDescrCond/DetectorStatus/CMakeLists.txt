################################################################################
# Package: DetectorStatus
################################################################################
# Package is now a rump of a few Python libraries kept around for potential 
# Run 1 compatibility


# Declare the package name:
atlas_subdir( DetectorStatus )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/DetDescrCond/DetDescrConditions
			  )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( flake8_test_dir
                SCRIPT flake8 --select=ATL,F,E7,E9,W6  ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )
