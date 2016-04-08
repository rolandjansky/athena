################################################################################
# Package: OnlineRecoTests
################################################################################

# Declare the package name:
atlas_subdir( OnlineRecoTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# this line failed automatic conversion in cmt2cmake :
# path_append ORTXMLFILE "../test/OnlineRecoTests_`echo $CMTPATH | sed 's/-VAL//' | sed 's/-T0//' | sed 's/-Prod//' | cut -d "/" -f 6,7 | sed 's/\//_/'`.xml"

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/ort_*.py scripts/ort_*.sh )

