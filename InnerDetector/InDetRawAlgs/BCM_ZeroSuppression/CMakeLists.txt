################################################################################
# Package: BCM_ZeroSuppression
################################################################################

# Declare the package name:
atlas_subdir( BCM_ZeroSuppression )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetBCM_RawData )

# Component(s) in the package:
atlas_add_component( BCM_ZeroSuppression
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetBCM_RawData )

# Install files from the package:
atlas_install_headers( BCM_ZeroSuppression )

