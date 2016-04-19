################################################################################
# Package: Tier0ChainTests
################################################################################

# Declare the package name:
atlas_subdir( Tier0ChainTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# this line failed automatic conversion in cmt2cmake :
# path_append TCTXMLFILE "../test/Tier0ChainTests_`echo $CMTPATH | awk -F "nightlies" '{ print $2 }' | sed 's/-VAL//' | sed 's/-T0//' | sed 's/-Prod//' | cut -d "/" -f 2,3 | sed 's/\//_/'`.xml"

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/tct_*.py scripts/tct_*.sh )

