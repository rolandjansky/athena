################################################################################
# Package: TileRecEx
################################################################################

# Declare the package name:
atlas_subdir( TileRecEx )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/TileRecEx_TestConfiguration.xml share/TileRecEx_*.C share/DoTileOptimalFilteringAlgsRttHistograms.C )
atlas_install_scripts( share/TileRecEx_links.sh share/TileRecEx_links.csh )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( TileInputFiles_test
                SCRIPT python -m TileRecEx.TileInputFiles -r 239908 -d /eos/atlas/atlascerngroupdisk/det-tile/test/
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
