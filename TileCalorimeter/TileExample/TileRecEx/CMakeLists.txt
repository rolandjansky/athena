################################################################################
# Package: TileRecEx
################################################################################

# Declare the package name:
atlas_subdir( TileRecEx )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/TileRecEx_TestConfiguration.xml share/TileRecEx_*.C share/DoTileOptimalFilteringAlgsRttHistograms.C )
atlas_install_scripts( share/TileRecEx_links.sh share/TileRecEx_links.csh )

