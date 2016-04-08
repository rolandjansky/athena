################################################################################
# Package: TileSimEx
################################################################################

# Declare the package name:
atlas_subdir( TileSimEx )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/TileSimEx_TestConfig.xml share/*.C )
atlas_install_scripts( share/TileSimEx_links.sh share/TileSimEx_links.csh )

