################################################################################
# Package: sTGCRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( StgcRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Graf Core Tree MathCore Hist RIO pthread Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( StgcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
 		     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib ) 

#
# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
