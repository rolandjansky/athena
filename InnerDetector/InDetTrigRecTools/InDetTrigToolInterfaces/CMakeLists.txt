################################################################################
# Package: InDetTrigToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData )

# Install files from the package:
atlas_install_headers( InDetTrigToolInterfaces )

