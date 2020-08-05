################################################################################
# Package: InDetConditionsSummaryService
################################################################################

# Declare the package name:
atlas_subdir( InDetConditionsSummaryService )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

atlas_add_library( InDetConditionsSummaryService
                   INTERFACE
                   PUBLIC_HEADERS InDetConditionsSummaryService
                   LINK_LIBRARIES GaudiKernel )
