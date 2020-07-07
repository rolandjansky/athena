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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
