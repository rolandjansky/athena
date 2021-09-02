################################################################################
# Package: HGTD_PadClusterizationTools
################################################################################

# Declare the package name:
atlas_subdir( HGTD_PadClusterizationTools )

# Component(s) in the package:
atlas_add_component( HGTD_PadClusterizationTools
                    src/*.cxx
                    src/components/*.cxx
                    LINK_LIBRARIES AthenaBaseComps GaudiKernel
                    HGTD_RecToolInterfaces HGTD_PrepRawData)

# Install files from the package:
atlas_install_headers( HGTD_PadClusterizationTools )
