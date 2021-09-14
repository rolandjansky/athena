################################################################################
# Package: HGTD_TrackTimeExtension
################################################################################

# Declare the package name:
atlas_subdir( HGTD_TrackTimeExtension )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel)

# Component(s) in the package:
atlas_add_component( TrackTimeExtensionAlgorithm
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel
                     xAODTracking HGTD_TrackTimeExtensionTools HGTD_RIO_OnTrack HGTD_TruthTools GeneratorObjects
                     xAODTruth InDetSimData)

# Install files from the package:
atlas_install_headers( HGTD_TrackTimeExtension )

atlas_install_python_modules( python/*.py)

atlas_install_joboptions( share/*.py )
