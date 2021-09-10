################################################################################
# Package: HGTD_PRDFormation
################################################################################

# Declare the package name:
atlas_subdir( HGTD_PRDFormation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel)

# Component(s) in the package:
atlas_add_component( TrackTimeExtensionAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel HGTD_RecToolInterfaces
                     HGTD_RawData HGTD_Identifier)

# Install files from the package:
atlas_install_headers( HGTD_PRDFormation )

atlas_install_python_modules( python/*.py)

atlas_install_joboptions( share/*.py )
